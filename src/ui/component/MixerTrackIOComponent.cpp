#include "MixerTrackIOComponent.h"
#include "../misc/DragSourceType.h"
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
	/** TODO */
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
