#include "SeqTrackIOComponent.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTrackIOComponent::SeqTrackIOComponent(bool isMidi)
	: isMidi(isMidi) {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void SeqTrackIOComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float pointHeight = screenSize.getHeight() * 0.0075;

	/** Color */
	juce::Colour colorOn = this->isMidi
		? juce::Colours::skyblue
		: juce::Colours::lightgreen;
	juce::Colour colorOff = this->isMidi
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

void SeqTrackIOComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->name = quickAPI::getSeqTrackName(index);

		if (this->isMidi) {
			/** Get Connections */
			this->midiOutputToMixer = quickAPI::getSeqTrackMIDIOutputToMixer(index);

			/** Create Temp */
			this->midiOutputMixerTemp.clear();
			for (auto& [src, dst] : this->midiOutputToMixer) {
				this->midiOutputMixerTemp.insert(dst);
			}

			/** Set Flag */
			this->linked = !(this->midiOutputToMixer.isEmpty());
		}
		else {
			/** Get Connections */
			this->audioOutputToMixer = quickAPI::getSeqTrackAudioOutputToMixer(index);

			/** Create Temp */
			this->audioOutputMixerTemp.clear();
			for (auto& [src, srcc, dst, dstc] : this->audioOutputToMixer) {
				this->audioOutputMixerTemp.insert(dst);
			}

			/** Set Flag */
			this->linked = !(this->audioOutputToMixer.isEmpty());
		}

		this->repaint();

		this->setTooltip(this->createToolTipString());
	}
}

void SeqTrackIOComponent::mouseDrag(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (auto dragSource = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
			dragSource->startDragging(this->getDragSourceDescription(),
				this, juce::ScaledImage{}, true);
		}
	}
}

void SeqTrackIOComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (!event.mouseWasDraggedSinceMouseDown()) {
			this->showLinkMenu(true);
		}
	}
	else if (event.mods.isRightButtonDown()) {
		this->showLinkMenu(false);
	}
}

void SeqTrackIOComponent::showLinkMenu(bool link) {
	auto menu = link ? this->createLinkMenu() : this->createUnlinkMenu();
	int result = menu.showAt(this);
	if (result <= 0) { return; }

	if (this->isMidi) {
		int dst = result - 1;
		CoreActions::setSeqMIDIOutputToMixer(this->index, dst, link);
	}
	else {

		/*if (result == MixerTrackIOAction::Device) {
			auto links = this->getOutputToDeviceChannelLinks();
			CoreActions::setTrackAudioOutputToDeviceGUI(
				this->index, link, links);
		}
		else if (result >= MixerTrackIOAction::NumBase0
			&& result < MixerTrackIOAction::NumBase1) {
			int dst = result - MixerTrackIOAction::NumBase0;
			this->setAudioOutputToSend(dst, link);
		}*/
	}
}

const juce::Array<std::tuple<int, int>>
SeqTrackIOComponent::getOutputToMixerChannelLinks(int trackIndex) const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioOutputToMixer) {
		if ((src == this->index) && (dst == trackIndex)) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

juce::var SeqTrackIOComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	if (this->isMidi) {
		object->setProperty("type", (int)DragSourceType::SourceMidiOutput);
	}
	else {
		object->setProperty("type", (int)DragSourceType::SourceAudioOutput);
	}
	object->setProperty("track", this->index);
	object->setProperty("name", this->name);

	return juce::var{ object.release() };
}

juce::String SeqTrackIOComponent::createToolTipString() const {
	juce::String result;

	if (this->isMidi) {
		for (auto i : this->midiOutputMixerTemp) {
			auto trackName = quickAPI::getMixerTrackName(i);
			juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
			result += name + "\n";
		}
	}
	else {
		for (auto i : this->audioOutputMixerTemp) {
			auto trackName = quickAPI::getMixerTrackName(i);
			juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
			result += name + "\n";
		}
	}

	if (result.isEmpty()) {
		result = TRANS("Unlinked");
	}

	return result;
}

juce::PopupMenu SeqTrackIOComponent::createLinkMenu() const {
	juce::PopupMenu menu;

	auto mixerTracks = quickAPI::getMixerTrackNameList();
	for (int i = 0; i < mixerTracks.size(); i++) {
		juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + mixerTracks[i];
		menu.addItem(i + 1, name, i != this->index,
			this->isMidi ? this->midiOutputMixerTemp.contains(i)
			: this->audioOutputMixerTemp.contains(i));
	}

	return menu;
}

juce::PopupMenu SeqTrackIOComponent::createUnlinkMenu() const {
	juce::PopupMenu menu;

	if (this->isMidi) {
		for (auto i : this->midiOutputMixerTemp) {
			auto trackName = quickAPI::getMixerTrackName(i);
			juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
			menu.addItem(i + 1, name, true, true);
		}
	}
	else {
		for (auto i : this->audioOutputMixerTemp) {
			auto trackName = quickAPI::getMixerTrackName(i);
			juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
			menu.addItem(i + 1, name, true, true);
		}
	}

	return menu;
}
