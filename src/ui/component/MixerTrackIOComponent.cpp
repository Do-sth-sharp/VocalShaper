#include "MixerTrackIOComponent.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

MixerTrackIOComponent::MixerTrackIOComponent(
	bool isInput, bool isMidi)
	: isInput(isInput), isMidi(isMidi) {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void MixerTrackIOComponent::paint(juce::Graphics& g) {
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

void MixerTrackIOComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->name = quickAPI::getMixerTrackName(index);

		if (this->isInput) {
			if (this->isMidi) {
				/** Get Connections */
				this->midiInputFromDevice = quickAPI::getMixerTrackMIDIInputFromDevice(index);
				this->midiInputFromSource = quickAPI::getMixerTrackMIDIInputFromSource(index);

				/** Create Temp */
				this->midiInputSourceTemp.clear();
				for (auto& [src, dst] : this->midiInputFromSource) {
					this->midiInputSourceTemp.insert(src);
				}

				/** Set Flag */
				this->linked = this->midiInputFromDevice || (!(this->midiInputSourceTemp.empty()));
			}
			else {
				/** Get Connections */
				this->audioInputFromDevice = quickAPI::getMixerTrackAudioInputFromDevice(index);
				this->audioInputFromSource = quickAPI::getMixerTrackAudioInputFromSource(index);
				this->audioInputFromInstr = quickAPI::getMixerTrackAudioInputFromInstr(index);
				this->audioInputFromSend = quickAPI::getMixerTrackAudioInputFromSend(index);

				/** Create Temp */
				this->audioInputSourceTemp.clear();
				for (auto& [src, srcc, dst, dstc] : this->audioInputFromSource) {
					this->audioInputSourceTemp.insert(src);
				}

				this->audioInputInstrTemp.clear();
				for (auto& [src, srcc, dst, dstc] : this->audioInputFromInstr) {
					this->audioInputInstrTemp.insert(src);
				}

				this->audioInputSendTemp.clear();
				for (auto& [src, srcc, dst, dstc] : this->audioInputFromSend) {
					this->audioInputSendTemp.insert(src);
				}

				/** Set Flag */
				this->linked = (!(this->audioInputFromDevice.isEmpty()))
					|| (!(this->audioInputSourceTemp.empty()))
					|| (!(this->audioInputInstrTemp.empty()))
					|| (!(this->audioInputSendTemp.empty()));
			}
		}
		else {
			if (this->isMidi) {
				/** Get Connections */
				this->midiOutputToDevice = quickAPI::getMixerTrackMIDIOutputToDevice(index);

				/** Set Flag */
				this->linked = this->midiOutputToDevice;
			}
			else {
				/** Get Connections */
				this->audioOutputToDevice = quickAPI::getMixerTrackAudioOutputToDevice(index);
				this->audioOutputToSend = quickAPI::getMixerTrackAudioOutputToSend(index);

				/** Create Temp */
				this->audioOutputSendTemp.clear();
				for (auto& [src, srcc, dst, dstc] : this->audioOutputToSend) {
					this->audioOutputSendTemp.insert(dst);
				}

				/** Set Flag */
				this->linked = (!(this->audioOutputToDevice.isEmpty()))
					|| (!(this->audioOutputSendTemp.empty()));
			}
		}

		this->repaint();

		this->setTooltip(this->createToolTipString());
	}
}

void MixerTrackIOComponent::mouseDrag(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (auto dragSource = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
			dragSource->startDragging(this->getDragSourceDescription(),
				this, juce::ScaledImage{}, true);
		}
	}
}

void MixerTrackIOComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (!event.mouseWasDraggedSinceMouseDown()) {
			this->showLinkMenu(true);
		}
	}
	else if (event.mods.isRightButtonDown()) {
		this->showLinkMenu(false);
	}
}

enum MixerTrackIOAction {
	Device = 1,
	NumBase0 = 2,
	NumBase1 = 0x4000,
	NumBase2 = 0x8000
};

void MixerTrackIOComponent::showLinkMenu(bool link) {
	auto menu = link ? this->createLinkMenu() : this->createUnlinkMenu();
	int result = menu.showAt(this);

	if (this->isInput) {
		if (this->isMidi) {
			if (result == MixerTrackIOAction::Device) {
				CoreActions::setTrackMIDIInputFromDevice(this->index, link);
			}
			else if (result >= MixerTrackIOAction::NumBase0) {
				int src = result - MixerTrackIOAction::NumBase0;
				CoreActions::setTrackMIDIInputFromSeqTrack(this->index, src, link);
			}
		}
		else {
			if (result == MixerTrackIOAction::Device) {
				auto links = this->getInputFromDeviceChannelLinks();
				CoreActions::setTrackAudioInputFromDeviceGUI(
					this->index, link, links);
			}
			else if (result >= MixerTrackIOAction::NumBase0
				&& result < MixerTrackIOAction::NumBase1) {
				int src = result - MixerTrackIOAction::NumBase0;

				auto links = this->getInputFromSourceChannelLinks(src);
				CoreActions::setTrackAudioInputFromSourceGUI(
					this->index, src, link, links);
			}
			else if (result >= MixerTrackIOAction::NumBase1
				&& result < MixerTrackIOAction::NumBase2) {
				int src = result - MixerTrackIOAction::NumBase1;

				auto links = this->getInputFromInstrChannelLinks(src);
				CoreActions::setTrackAudioInputFromInstrGUI(
					this->index, src, link, links);
			}
			else if (result >= MixerTrackIOAction::NumBase2) {
				int src = result - MixerTrackIOAction::NumBase2;

				auto links = this->getInputFromSendChannelLinks(src);
				CoreActions::setTrackAudioInputFromSendGUI(
					this->index, src, link, links);
			}
		}
	}
	else {
		if (this->isMidi) {
			if (result == MixerTrackIOAction::Device) {
				CoreActions::setTrackMIDIOutputToDevice(this->index, link);
			}
		}
		else {
			if (result == MixerTrackIOAction::Device) {
				auto links = this->getOutputToDeviceChannelLinks();
				CoreActions::setTrackAudioOutputToDeviceGUI(
					this->index, link, links);
			}
			else if (result >= MixerTrackIOAction::NumBase0
				&& result < MixerTrackIOAction::NumBase1) {
				int dst = result - MixerTrackIOAction::NumBase0;

				auto links = this->getOutputToSendChannelLinks(dst);
				CoreActions::setTrackAudioOutputToSendGUI(
					this->index, dst, link, links);
			}
		}
	}
}

const juce::Array<std::tuple<int, int>> 
MixerTrackIOComponent::getInputFromDeviceChannelLinks() const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioInputFromDevice) {
		if (dst == this->index) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

const juce::Array<std::tuple<int, int>>
MixerTrackIOComponent::getInputFromSourceChannelLinks(int seqIndex) const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioInputFromSource) {
		if ((src == seqIndex) && (dst == this->index)) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

const juce::Array<std::tuple<int, int>>
MixerTrackIOComponent::getInputFromInstrChannelLinks(int instrIndex) const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioInputFromInstr) {
		if ((src == instrIndex) && (dst == this->index)) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

const juce::Array<std::tuple<int, int>>
MixerTrackIOComponent::getInputFromSendChannelLinks(int trackIndex) const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioInputFromSend) {
		if ((src == trackIndex) && (dst == this->index)) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

const juce::Array<std::tuple<int, int>>
MixerTrackIOComponent::getOutputToDeviceChannelLinks() const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioOutputToDevice) {
		if (src == this->index) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

const juce::Array<std::tuple<int, int>>
MixerTrackIOComponent::getOutputToSendChannelLinks(int trackIndex) const {
	juce::Array<std::tuple<int, int>> result;
	for (auto& [src, srcc, dst, dstc] : this->audioOutputToSend) {
		if ((src == this->index) && (dst == trackIndex)) {
			result.add({ srcc, dstc });
		}
	}
	return result;
}

juce::var MixerTrackIOComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	if (this->isInput) {
		if (this->isMidi) {
			object->setProperty("type", (int)DragSourceType::TrackMidiInput);
		}
		else {
			object->setProperty("type", (int)DragSourceType::TrackAudioInput);
		}
	}
	else {
		if (this->isMidi) {
			object->setProperty("type", (int)DragSourceType::TrackMidiOutput);
		}
		else {
			object->setProperty("type", (int)DragSourceType::TrackAudioOutput);
		}
	}
	object->setProperty("track", this->index);
	object->setProperty("name", this->name);

	return juce::var{ object.release() };
}

juce::String MixerTrackIOComponent::createToolTipString() const {
	juce::String result;

	if (this->isInput) {
		if (this->isMidi) {
			if (this->midiInputFromDevice) {
				result += TRANS("MIDI Input") + "\n";
			}

			for (auto i : this->midiInputSourceTemp) {
				auto trackName = quickAPI::getSeqTrackName(i);
				juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + trackName;
				result += name + "\n";
			}
		}
		else {
			if (this->audioInputFromDevice.size() > 0) {
				result += TRANS("Audio Input") + "\n";
			}

			for (auto i : this->audioInputSourceTemp) {
				auto trackName = quickAPI::getSeqTrackName(i);
				juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + trackName;
				result += name + "\n";
			}

			for (auto i : this->audioInputInstrTemp) {
				auto instrName = quickAPI::getInstrName(i);
				juce::String name = TRANS("Instrument") + " #" + juce::String{ i } + " " + instrName;
				result += name + "\n";
			}

			for (auto i : this->audioInputSendTemp) {
				auto trackName = quickAPI::getMixerTrackName(i);
				juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
				result += name + "\n";
			}
		}
	}
	else {
		if (this->isMidi) {
			if (this->midiOutputToDevice) {
				result += TRANS("MIDI Output") + "\n";
			}
		}
		else {
			if (this->audioOutputToDevice.size() > 0) {
				result += TRANS("Audio Output") + "\n";
			}

			for (auto i : this->audioOutputSendTemp) {
				auto trackName = quickAPI::getMixerTrackName(i);
				juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
				result += name + "\n";
			}
		}
	}

	if (result.isEmpty()) {
		result = TRANS("Unlinked");
	}

	return result;
}

juce::PopupMenu MixerTrackIOComponent::createLinkMenu() const {
	juce::PopupMenu menu;

	if (this->isInput) {
		if (this->isMidi) {
			menu.addItem(MixerTrackIOAction::Device, TRANS("MIDI Input"),
				!this->midiInputFromDevice, this->midiInputFromDevice);
			menu.addSeparator();

			auto seqTracks = quickAPI::getSeqTrackNameList();
			for (int i = 0; i < seqTracks.size(); i++) {
				juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + seqTracks[i];
				menu.addItem(MixerTrackIOAction::NumBase0 + i, name, true, this->midiInputSourceTemp.contains(i));
			}
		}
		else {
			menu.addItem(MixerTrackIOAction::Device, TRANS("Audio Input"),
				true, this->audioInputFromDevice.size() > 0);
			menu.addSeparator();

			auto seqTracks = quickAPI::getSeqTrackNameList();
			for (int i = 0; i < seqTracks.size(); i++) {
				juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + seqTracks[i];
				menu.addItem(MixerTrackIOAction::NumBase0 + i, name, true, this->audioInputSourceTemp.contains(i));
			}
			menu.addSeparator();

			auto instrs = quickAPI::getInstrNameList();
			for (int i = 0; i < instrs.size(); i++) {
				juce::String name = TRANS("Instrument") + " #" + juce::String{ i } + " " + instrs[i];
				menu.addItem(MixerTrackIOAction::NumBase1 + i, name, true, this->audioInputInstrTemp.contains(i));
			}
			menu.addSeparator();

			auto mixerTracks = quickAPI::getMixerTrackNameList();
			for (int i = 0; i < mixerTracks.size(); i++) {
				juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + mixerTracks[i];
				menu.addItem(MixerTrackIOAction::NumBase2 + i, name, i != this->index, this->audioInputSendTemp.contains(i));
			}
		}
		
	}
	else {
		if (this->isMidi) {
			menu.addItem(MixerTrackIOAction::Device, TRANS("MIDI Output"),
				!this->midiOutputToDevice, this->midiOutputToDevice);
		}
		else {
			menu.addItem(MixerTrackIOAction::Device, TRANS("Audio Output"),
				true, this->audioOutputToDevice.size() > 0);
			menu.addSeparator();

			auto mixerTracks = quickAPI::getMixerTrackNameList();
			for (int i = 0; i < mixerTracks.size(); i++) {
				juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + mixerTracks[i];
				menu.addItem(MixerTrackIOAction::NumBase0 + i, name, i != this->index, this->audioOutputSendTemp.contains(i));
			}
		}
	}

	return menu;
}

juce::PopupMenu MixerTrackIOComponent::createUnlinkMenu() const {
	juce::PopupMenu menu;

	if (this->isInput) {
		if (this->isMidi) {
			menu.addItem(MixerTrackIOAction::Device, TRANS("MIDI Input"),
				this->midiInputFromDevice, this->midiInputFromDevice);
			menu.addSeparator();

			for (auto i : this->midiInputSourceTemp) {
				auto trackName = quickAPI::getSeqTrackName(i);
				juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + trackName;
				menu.addItem(MixerTrackIOAction::NumBase0 + i, name, true, true);
			}
		}
		else {
			menu.addItem(MixerTrackIOAction::Device, TRANS("Audio Input"),
				this->audioInputFromDevice.size() > 0, this->audioInputFromDevice.size() > 0);
			menu.addSeparator();

			for (auto i : this->audioInputSourceTemp) {
				auto trackName = quickAPI::getSeqTrackName(i);
				juce::String name = TRANS("Sequencer Track") + " #" + juce::String{ i } + " " + trackName;
				menu.addItem(MixerTrackIOAction::NumBase0 + i, name, true, true);
			}
			menu.addSeparator();

			for (auto i : this->audioInputInstrTemp) {
				auto instrName = quickAPI::getInstrName(i);
				juce::String name = TRANS("Instrument") + " #" + juce::String{ i } + " " + instrName;
				menu.addItem(MixerTrackIOAction::NumBase1 + i, name, true, true);
			}
			menu.addSeparator();

			for (auto i : this->audioInputSendTemp) {
				auto trackName = quickAPI::getMixerTrackName(i);
				juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
				menu.addItem(MixerTrackIOAction::NumBase2 + i, name, true, true);
			}
		}
	}
	else {
		if (this->isMidi) {
			menu.addItem(MixerTrackIOAction::Device, TRANS("MIDI Output"),
				this->midiOutputToDevice, this->midiOutputToDevice);
		}
		else {
			menu.addItem(MixerTrackIOAction::Device, TRANS("Audio Output"),
				this->audioOutputToDevice.size() > 0, this->audioOutputToDevice.size() > 0);
			menu.addSeparator();

			for (auto i : this->audioOutputSendTemp) {
				auto trackName = quickAPI::getMixerTrackName(i);
				juce::String name = TRANS("Mixer Track") + " #" + juce::String{ i } + " " + trackName;
				menu.addItem(MixerTrackIOAction::NumBase0 + i, name, true, true);
			}
		}
	}

	return menu;
}
