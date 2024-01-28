#include "InstrIOComponent.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

InstrIOComponent::InstrIOComponent(bool isInput)
	: isInput(isInput) {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void InstrIOComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float pointHeight = screenSize.getHeight() * 0.01;

	/** Color */
	juce::Colour colorOn = this->isInput
		? juce::Colours::skyblue
		:juce::Colours::lightgreen;
	juce::Colour colorOff = this->isInput
		? juce::Colours::indianred
		: juce::Colours::yellow;

	/** Center */
	auto centerPoint = this->getLocalBounds().getCentre();

	/** Draw Point */
	if (this->linked) {
		juce::Path path;
		path.startNewSubPath(centerPoint.getX(), centerPoint.getY() - pointHeight / 2);
		path.lineTo(centerPoint.getX() + pointHeight / 2, centerPoint.getY());
		path.lineTo(centerPoint.getX(), centerPoint.getY() + pointHeight / 2);
		path.lineTo(centerPoint.getX() - pointHeight / 2, centerPoint.getY());
		path.closeSubPath();

		g.setColour(colorOn);
		g.fillPath(path);
	}
	else {
		juce::Rectangle<float> pointRect(
			centerPoint.getX() - pointHeight / 2, centerPoint.getY() - pointHeight / 2,
			pointHeight, pointHeight);

		g.setColour(colorOff);
		g.fillEllipse(pointRect);
	}
}

void InstrIOComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		if (this->isInput) {
			/** Get Input Connections */
			this->midiInputFromDevice = quickAPI::getInstrMIDIInputFromDevice(index);
			this->midiInputFromSource = quickAPI::getInstrMIDIInputFromSource(index);

			/** Create Temp */
			this->inputSourceTemp.clear();
			for (auto& [src, dst] : this->midiInputFromSource) {
				this->inputSourceTemp.insert(src);
			}

			/** Set Flag */
			this->linked = this->midiInputFromDevice || (!(this->inputSourceTemp.empty()));
		}
		else {
			/** Get Output Connections */
			this->audioOutputToMixer = quickAPI::getInstrAudioOutputToMixer(index);

			/** Create Temp */
			this->outputMixerTemp.clear();
			for (auto& [src, srcc, dst, dstc] : this->audioOutputToMixer) {
				this->outputMixerTemp.insert(dst);
			}

			/** Set Flag */
			this->linked = !(this->outputMixerTemp.empty());
		}
		this->repaint();
	}
}

void InstrIOComponent::mouseDrag(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (auto dragSource = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
			dragSource->startDragging(this->getDragSourceDescription(),
				this, juce::ScaledImage{}, true);
		}
	}
}

void InstrIOComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (!event.mouseWasDraggedSinceMouseDown()) {
			this->showLinkMenu();
		}
	}
	else if (event.mods.isRightButtonDown()) {
		this->showUnlinkMenu();
	}
}

enum InstrIOMenuType {
	Device = 1, NumBase
};

void InstrIOComponent::showLinkMenu() {
	auto menu = this->createLinkMenu();
	int result = menu.showAt(this);

	if (this->isInput) {
		if (result == InstrIOMenuType::Device) {
			CoreActions::setInstrMIDIInputFromDevice(this->index, true);
		}
		else if (result >= InstrIOMenuType::NumBase) {
			int src = result - InstrIOMenuType::NumBase;
			CoreActions::setInstrMIDIInputFromSeqTrack(this->index, src, true);
		}
	}
	else {
		if (result >= InstrIOMenuType::NumBase) {
			int track = result - InstrIOMenuType::NumBase;

			auto links = this->getOutputChannelLinks(track);
			CoreActions::setInstrAudioOutputToMixerGUI(
				this->index, track, true, links);
		}
	}
}

void InstrIOComponent::showUnlinkMenu() {
	auto menu = this->createUnlinkMenu();
	int result = menu.showAt(this);

	if (this->isInput) {
		if (result == InstrIOMenuType::Device) {
			CoreActions::setInstrMIDIInputFromDevice(this->index, false);
		}
		else if (result >= InstrIOMenuType::NumBase) {
			int src = result - InstrIOMenuType::NumBase;
			CoreActions::setInstrMIDIInputFromSeqTrack(this->index, src, false);
		}
	}
	else {
		if (result >= InstrIOMenuType::NumBase) {
			int track = result - InstrIOMenuType::NumBase;
			
			auto links = this->getOutputChannelLinks(track);
			CoreActions::setInstrAudioOutputToMixerGUI(
				this->index, track, false, links);
		}
	}
}

juce::var InstrIOComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	if (this->isInput) {
		object->setProperty("type", (int)DragSourceType::InstrInput);
	}
	else {
		object->setProperty("type", (int)DragSourceType::InstrOutput);
	}
	object->setProperty("instr", this->index);
	object->setProperty("name", this->name);

	return juce::var{ object.release() };
}

juce::PopupMenu InstrIOComponent::createLinkMenu() {
	juce::PopupMenu menu;

	if (this->isInput) {
		menu.addItem(InstrIOMenuType::Device, TRANS("MIDI Input"),
			!this->midiInputFromDevice, this->midiInputFromDevice);
		menu.addSeparator();

		auto seqTracks = quickAPI::getSeqTrackNameList();
		for (int i = 0; i < seqTracks.size(); i++) {
			juce::String name = TRANS("Sequencer Track") + " #" + juce::String{i} + " " + seqTracks[i];
			menu.addItem(InstrIOMenuType::NumBase + i, name, true, this->inputSourceTemp.contains(i));
		}
	}
	else {
		auto mixerTracks = quickAPI::getMixerTrackNameList();
		for (int i = 0; i < mixerTracks.size(); i++) {
			juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + mixerTracks[i];
			menu.addItem(InstrIOMenuType::NumBase + i, name, true, this->outputMixerTemp.contains(i));
		}
	}

	return menu;
}

juce::PopupMenu InstrIOComponent::createUnlinkMenu() {
	juce::PopupMenu menu;

	if (this->isInput) {
		menu.addItem(InstrIOMenuType::Device, TRANS("MIDI Input"),
			this->midiInputFromDevice, this->midiInputFromDevice);
		menu.addSeparator();

		for (auto i : this->inputSourceTemp) {
			auto trackName = quickAPI::getSeqTrackName(i);
			juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + trackName;
			menu.addItem(InstrIOMenuType::NumBase + i, name, true, true);
		}
	}
	else {
		for (auto i : this->outputMixerTemp) {
			auto trackName = quickAPI::getMixerTrackName(i);
			juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
			menu.addItem(InstrIOMenuType::NumBase + i, name, true, true);
		}
	}

	return menu;
}

const juce::Array<std::tuple<int, int>> InstrIOComponent::getOutputChannelLinks(int track) const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioOutputToMixer) {
		if ((src == this->index) && (dst == track)) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}
