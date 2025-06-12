#include "MIDIContentViewer.h"
#include "../../misc/Tools.h"
#include "../../misc/CoreActions.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

#define NOTE_VELOCITY_INIT 127

class MIDIChannelListener final : public juce::ChangeListener {
public:
	MIDIChannelListener() = delete;
	MIDIChannelListener(MIDIContentViewer* parent)
		: parent(parent) {};

	void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) override {
		this->parent->midiChannelChanged();
	}

private:
	MIDIContentViewer* const parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIChannelListener)
};

MIDIContentViewer::MIDIContentViewer(
	const ScrollFunc& scrollFunc,
	const WheelFunc& wheelFunc,
	const WheelAltFunc& wheelAltFunc,
	const MouseYPosFunc& mouseYPosFunc,
	const MouseLeaveFunc& mouseLeaveFunc,
	const DragStartFunc& dragStartFunc,
	const DragProcessFunc& dragProcessFunc,
	const DragEndFunc& dragEndFunc)
	: scrollFunc(scrollFunc), wheelFunc(wheelFunc), wheelAltFunc(wheelAltFunc),
	mouseYPosFunc(mouseYPosFunc), mouseLeaveFunc(mouseLeaveFunc),
	dragStartFunc(dragStartFunc), dragProcessFunc(dragProcessFunc), dragEndFunc(dragEndFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::MidiContent));

	/** Init Temp */
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
	this->keyTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
	this->blockTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
	this->noteTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);

	/** Listeners */
	this->midiChannelListener = std::make_unique<MIDIChannelListener>(this);
	Tools::getInstance()->addMIDIChannelChangedListener(this->midiChannelListener.get());
}

void MIDIContentViewer::update(int index, uint64_t ref) {
	this->index = index;
	this->ref = ref;

	if (index >= 0) {
		this->trackColor = quickAPI::getTrackColor(
			{ quickAPI::TrackType::Track, index });
		//this->trackColorIsLight = utils::isLightColor(this->trackColor);

		/** Note Color Gradient */
		auto& laf = this->getLookAndFeel();
		juce::Colour highChannelNoteColor = laf.findColour(
			juce::Label::ColourIds::backgroundWhenEditingColourId);
		this->noteColorGradient = utils::generateBezierColorGradient(
			this->trackColor, highChannelNoteColor, 16);

		/** Note Name Color Gradient */
		juce::Colour lightNoteLabelColor = laf.findColour(
			juce::MidiKeyboardComponent::ColourIds::textLabelColourId);
		juce::Colour darkNoteLabelColor = laf.findColour(
			juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 2);
		this->noteLabelColorGradient.clearQuick();
		for (auto& backColor : this->noteColorGradient) {
			this->noteLabelColorGradient.add(utils::chooseTextColor(
				backColor, lightNoteLabelColor, darkNoteLabelColor));
		}
	}

	this->updateBlocks();
}

void MIDIContentViewer::updateTempoLabel() {
	/** Update Ruler Temp */
	this->lineTemp = this->createRulerLine(this->hPos, this->hItemSize);
	this->updateRulerImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateBlocks() {
	/** Clear Temp */
	this->blockItemTemp.clear();

	/** Update Block Temp */
	auto list = quickAPI::getBlockList(
		{ quickAPI::TrackType::Track, this->index });
	for (auto [startTime, endTime, offset] : list) {
		this->blockItemTemp.add({ startTime, endTime, startTime + offset });
	}

	/** Sort by Source Start Time to Optimize Note Drawing Time */
	class BlockItemComparator {
	public:
		static int compareElements(const BlockItem& first, const BlockItem& second) {
			auto& firstSourceStartTime = std::get<2>(first);
			auto& secondSourceStartTime = std::get<2>(second);
			return (firstSourceStartTime < secondSourceStartTime) ? -1
				: ((secondSourceStartTime < firstSourceStartTime) ? 1 : 0);
		}
	} blockItemComp{};
	this->blockItemTemp.sort(blockItemComp);

	/** Update UI */
	this->updateBlockImageTemp();
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateData() {
	/** Clear Temp */
	this->midiDataTemp.clear();

	/** Update Note Temp */
	if (this->index >= 0 && this->ref != 0) {
		this->currentMIDITrack = quickAPI::getTrackCurrentMIDITrack(
			{ quickAPI::TrackType::Track, this->index });
		auto midiNoteList = quickAPI::getMIDISourceNotes(this->ref, this->currentMIDITrack);

		/** Add Each Note */
		this->midiDataTemp.ensureStorageAllocated(midiNoteList.size());
		for (auto& note : midiNoteList) {
			/** Set Temp */
			Note noteTemp{};
			noteTemp.startSec = note.timeSec;
			noteTemp.endSec = note.endSec;
			noteTemp.num = note.pitch;
			noteTemp.vel = note.vel;
			noteTemp.channel = note.channel;
			noteTemp.lyrics = note.lyrics;
			this->midiDataTemp.add(noteTemp);
		}
	}

	/** Update Note Zone Temp */
	{
		uint8_t minNote = 127, maxNote = 0;
		for (auto& note : this->midiDataTemp) {
			minNote = std::min(minNote, note.num);
			maxNote = std::max(maxNote, note.num);
		}
		if (maxNote < minNote) { maxNote = minNote = 0; }
		this->midiMinNote = minNote;
		this->midiMaxNote = maxNote;
	}

	/** Update UI */
	this->updateBlockImageTemp();
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateLevelMeter() {
	/** Get Play Position */
	this->playPosSec = quickAPI::getTimeInSecond();

	/** Get Loop Time */
	std::tie(this->loopStartSec, this->loopEndSec) = quickAPI::getLoopTimeSec();

	/** Repaint */
	this->repaint();
}

void MIDIContentViewer::updateHPos(double pos, double itemSize) {
	if (this->getWidth() <= 0) { return; }

	this->hPos = pos;
	this->hItemSize = itemSize;

	std::tie(this->secStart, this->secEnd) = this->getHViewArea(pos, itemSize);
	this->lineTemp = this->createRulerLine(pos, itemSize);

	this->updateRulerImageTemp();
	this->updateBlockImageTemp();
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateVPos(double pos, double itemSize) {
	if (this->getHeight() <= 0) { return; }

	this->vPos = pos;
	this->vItemSize = itemSize;

	std::tie(this->keyTop, this->keyBottom) = this->getVViewArea(pos, itemSize);

	this->updateKeyImageTemp();
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::resized() {
	/** Update Line Temp */
	std::tie(this->secStart, this->secEnd) = this->getHViewArea(this->hPos, this->hItemSize);
	std::tie(this->keyTop, this->keyBottom) = this->getVViewArea(this->vPos, this->vItemSize);
	this->lineTemp = this->createRulerLine(this->hPos, this->hItemSize);

	/** Update Ruler Temp */
	int width = this->getWidth(), height = this->getHeight();
	width = std::max(width, 1);
	height = std::max(height, 1);
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateRulerImageTemp();

	/** Update Key Temp */
	this->keyTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateKeyImageTemp();

	/** Update Block Temp */
	this->blockTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateBlockImageTemp();

	/** Update Note Temp */
	this->noteTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateNoteImageTemp();
}

void MIDIContentViewer::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float noteCornerSize = screenSize.getHeight() * 0.003;
	float noteOutlineThickness = screenSize.getHeight() * 0.001;

	float notePaddingWidth = screenSize.getWidth() * 0.003;
	float notePaddingHeight = screenSize.getHeight() * 0.0025;
	float noteFontHeight = screenSize.getHeight() * 0.0135;

	int width = this->getWidth(), height = this->getHeight();

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour noteOutlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);
	juce::Colour noteLyricsColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 3);
	juce::Colour noteBaseColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour noteEditColor = juce::Colours::white.withAlpha(0.6f);
	juce::Colour noteRemoveColor = juce::Colours::black.withAlpha(0.6f);

	/** Font */
	juce::Font noteLabelFont(juce::FontOptions{ noteFontHeight });

	/** Key Temp */
	if (this->keyTemp) {
		g.drawImageAt(*(this->keyTemp.get()), 0, 0);
	}

	/** Ruler Temp */
	if (this->rulerTemp) {
		g.drawImageAt(*(this->rulerTemp.get()), 0, 0);
	}

	/** Block Temp */
	if (this->blockTemp) {
		g.drawImageAt(*(this->blockTemp.get()), 0, 0);
	}

	/** Note Temp */
	if (this->noteTemp) {
		g.drawImageAt(*(this->noteTemp.get()), 0, 0);
	}

	/** Add Note */
	constexpr float editingNoteOpaque = 0.5f;
	if (this->noteInsertTime >= 0 && this->noteInsertLength > 0) {
		int minNoteNum = std::floor(this->keyBottom), maxNoteNum = std::floor(this->keyTop);
		double insertNoteStartSec = this->noteInsertTime,
			insertNoteEndSec = this->noteInsertTime + this->noteInsertLength;
		if (insertNoteStartSec <= this->secEnd &&
			this->secStart <= insertNoteEndSec) {
			if (this->noteInsertPitch >= (minNoteNum - 1) &&
				this->noteInsertPitch <= maxNoteNum) {
				/** Note Rect */
				float startXPos = (insertNoteStartSec - this->secStart) / (this->secEnd - this->secStart) * width;
				float endXPos = (insertNoteEndSec - this->secStart) / (this->secEnd - this->secStart) * width;
				float noteYPos = ((this->noteInsertPitch + 1) - this->keyTop) / (this->keyBottom - this->keyTop) * height;
				juce::Rectangle<float> noteRect(
					startXPos, noteYPos,
					endXPos - startXPos, (float)this->vItemSize);
				g.setColour(noteBaseColor);
				g.fillRoundedRectangle(noteRect, noteCornerSize);
				g.setColour(this->noteColorGradient[this->noteInsertChannel - 1].withAlpha(editingNoteOpaque));
				g.fillRoundedRectangle(noteRect, noteCornerSize);
				g.setColour(noteOutlineColor.withAlpha(editingNoteOpaque));
				g.drawRoundedRectangle(noteRect, noteCornerSize, noteOutlineThickness);

				/** Note Name */
				juce::String noteName = this->keyNames[this->noteInsertPitch % this->keyMasks.size()] + juce::String{ this->noteInsertPitch / this->keyMasks.size() };
				float noteNameWidth = juce::TextLayout::getStringWidth(noteLabelFont, noteName);
				if ((noteNameWidth + notePaddingWidth * 2) <= noteRect.getWidth()
					&& (noteFontHeight + notePaddingHeight * 2) <= noteRect.getHeight()) {
					juce::Rectangle<float> noteLabelRect = noteRect.withWidth(noteNameWidth + notePaddingWidth * 2);
					g.setFont(noteLabelFont);
					g.setColour(this->noteLabelColorGradient[this->noteInsertChannel - 1].withAlpha(editingNoteOpaque));
					g.drawFittedText(noteName, noteLabelRect.toNearestInt(),
						juce::Justification::centred, 1, 0.75f);
				}
			}
		}
	}

	/** Edit Note */
	else if (this->noteEditStatus == NoteControllerType::Left
		|| this->noteEditStatus == NoteControllerType::Right
		|| this->noteEditStatus == NoteControllerType::Inside) {
		/** Edit Note Time */
		if (this->noteEditIndex > -1) {
			/** Get Note */
			auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(this->noteEditIndex);
			auto& note = this->midiDataTemp.getReference(noteIndex);
			double noteStartTime = note.startSec;
			double noteEndTime = note.endSec;
			uint8_t notePitch = note.num;

			/** Note Rect */
			float startXPos = (noteStartTime - this->secStart) / (this->secEnd - this->secStart) * width;
			float endXPos = (noteEndTime - this->secStart) / (this->secEnd - this->secStart) * width;
			float noteYPos = ((notePitch + 1) - this->keyTop) / (this->keyBottom - this->keyTop) * height;
			juce::Rectangle<float> noteRect(
				startXPos, noteYPos,
				endXPos - startXPos, (float)this->vItemSize);

			/** Cover Note */
			g.setColour(noteEditColor);
			g.fillRoundedRectangle(noteRect, noteCornerSize);

			/** New Note Rect */
			float newStartXPos = startXPos;
			if (this->noteEditStatus == NoteControllerType::Left) {
				newStartXPos = (this->noteEditTime - this->secStart) / (this->secEnd - this->secStart) * width;
			}
			else if (this->noteEditStatus == NoteControllerType::Inside) {
				newStartXPos = (this->noteEditTime - this->secStart) / (this->secEnd - this->secStart) * width;
			}
			float newEndXPos = endXPos;
			if (this->noteEditStatus == NoteControllerType::Right) {
				newEndXPos = (this->noteEditTime - this->secStart) / (this->secEnd - this->secStart) * width;
			}
			else if (this->noteEditStatus == NoteControllerType::Inside) {
				newEndXPos = ((this->noteEditTime + (noteEndTime - noteStartTime)) - this->secStart) / (this->secEnd - this->secStart) * width;
			}
			uint8_t newNotePitch = notePitch;
			if (this->noteEditStatus == NoteControllerType::Inside) {
				newNotePitch = this->noteEditPitch;
			}
			float newNoteYPos = ((newNotePitch + 1) - this->keyTop) / (this->keyBottom - this->keyTop) * height;
			
			juce::Rectangle<float> newNoteRect(
				newStartXPos, newNoteYPos,
				newEndXPos - newStartXPos, (float)this->vItemSize);

			/** Draw New Note */
			g.setColour(noteBaseColor);
			g.fillRoundedRectangle(newNoteRect, noteCornerSize);
			g.setColour(this->noteColorGradient[channel - 1].withAlpha(editingNoteOpaque));
			g.fillRoundedRectangle(newNoteRect, noteCornerSize);
			g.setColour(noteOutlineColor.withAlpha(editingNoteOpaque));
			g.drawRoundedRectangle(newNoteRect, noteCornerSize, noteOutlineThickness);

			/** Note Name */
			juce::String noteName = this->keyNames[newNotePitch % this->keyMasks.size()] + juce::String{ newNotePitch / this->keyMasks.size() };
			float noteNameWidth = juce::TextLayout::getStringWidth(noteLabelFont, noteName);
			if ((noteNameWidth + notePaddingWidth * 2) <= newNoteRect.getWidth()
				&& (noteFontHeight + notePaddingHeight * 2) <= newNoteRect.getHeight()) {
				juce::Rectangle<float> noteLabelRect = newNoteRect.withWidth(noteNameWidth + notePaddingWidth * 2);
				g.setFont(noteLabelFont);
				g.setColour(this->noteLabelColorGradient[channel - 1].withAlpha(editingNoteOpaque));
				g.drawFittedText(noteName, noteLabelRect.toNearestInt(),
					juce::Justification::centred, 1, 0.75f);
			}
		}
		/** Remove Note */
		else if (this->noteRemoveIndex > -1) {
			/** Get Note */
			auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(this->noteRemoveIndex);
			auto& note = this->midiDataTemp.getReference(noteIndex);
			double noteStartTime = note.startSec;
			double noteEndTime = note.endSec;
			uint8_t notePitch = note.num;

			/** Note Rect */
			float startXPos = (noteStartTime - this->secStart) / (this->secEnd - this->secStart) * width;
			float endXPos = (noteEndTime - this->secStart) / (this->secEnd - this->secStart) * width;
			float noteYPos = ((notePitch + 1) - this->keyTop) / (this->keyBottom - this->keyTop) * height;
			juce::Rectangle<float> noteRect(
				startXPos, noteYPos,
				endXPos - startXPos, (float)this->vItemSize);

			/** Cover Note */
			g.setColour(noteRemoveColor);
			g.fillRoundedRectangle(noteRect, noteCornerSize);
		}
	}
}

void MIDIContentViewer::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float cursorThickness = screenSize.getWidth() * 0.00075;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour cursorColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	/*juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);*/

	/** Cursor */
	int width = this->getWidth(), height = this->getHeight();
	float cursorPosX = ((this->playPosSec - this->secStart) / (this->secEnd - this->secStart)) * width;
	juce::Rectangle<float> cursorRect(
		cursorPosX - cursorThickness / 2, 0,
		cursorThickness, height);

	if (cursorPosX >= 0 && cursorPosX <= width) {
		g.setColour(cursorColor);
		g.fillRect(cursorRect);
	}

	/** Time Off */
	//if (this->loopEndSec > this->loopStartSec) {
	//	/** Left */
	//	if (this->loopStartSec > this->secStart) {
	//		float xPos = (this->loopStartSec - this->secStart) / (this->secEnd - this->secStart) * width;
	//		juce::Rectangle<float> offRect(0, 0, xPos, height);

	//		g.setColour(offColor);
	//		g.fillRect(offRect);
	//	}

	//	/** Right */
	//	if (this->loopEndSec < this->secEnd) {
	//		float xPos = (this->loopEndSec - this->secStart) / (this->secEnd - this->secStart) * width;
	//		juce::Rectangle<float> offRect(xPos, 0, width - xPos, height);

	//		g.setColour(offColor);
	//		g.fillRect(offRect);
	//	}
	//}
}

void MIDIContentViewer::mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		/** Move View Area */
		if (event.mods.isAltDown()) {
			this->viewMoving = true;
			this->setMouseCursor(juce::MouseCursor::DraggingHandCursor);
			this->dragStartFunc();
		}
		
		/** Edit Note */
		else if (Tools::getInstance()->getType() == Tools::Type::Pencil) {
			auto& pos = event.position;

			auto [type, index] = this->getNoteController(pos);
			
			/** Add Note */
			if (type == NoteControllerType::None) {
				/** Get Time */
				double time = this->secStart + (pos.x / this->getWidth()) * (this->secEnd - this->secStart);
				time = quickAPI::limitTimeSec(time, Tools::getInstance()->getAdsorb());

				/** Get Pitch */
				uint8_t pitch = (uint8_t)std::floor(this->keyTop - (pos.y / this->getHeight()) * (this->keyTop - this->keyBottom));

				/** Get Length */
				double length = Tools::getInstance()->getLastNoteLength();
				if (length <= 0) {
					/** Init Length */
					int tempoIndex = quickAPI::getTempoTempIndexBySec(time);
					auto tempo = quickAPI::getTempoData(tempoIndex);
					length = std::get<3>(tempo);

					Tools::getInstance()->setLastNoteLength(length);
				}

				/** Set Temp */
				this->noteInsertTime = time;
				this->noteInsertLength = length;
				this->noteInsertPitch = pitch;
				this->noteInsertChannel = Tools::getInstance()->getMIDIChannel();
				this->repaint();
			}
			/** Edit Note */
			else if (type == NoteControllerType::Left || type == NoteControllerType::Right) {
				/** Get Time */
				double time = this->secStart + (pos.x / this->getWidth()) * (this->secEnd - this->secStart);
				time = quickAPI::limitTimeSec(time, Tools::getInstance()->getAdsorb());

				/** Limit Note Length */
				int tempoIndex = quickAPI::getTempoTempIndexBySec(time);
				auto tempo = quickAPI::getTempoData(tempoIndex);
				double minLength = std::get<3>(tempo) / 64;

				auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(index);
				auto& note = this->midiDataTemp.getReference(noteIndex);
				double noteStartTime = note.startSec;
				double noteEndTime = note.endSec;
				if (type == NoteControllerType::Right && time < noteStartTime + minLength) {
					time = noteStartTime + minLength;
				}
				else if (type == NoteControllerType::Left && time > noteEndTime - minLength) {
					time = noteEndTime - minLength;
				}

				/** Set Temp */
				this->noteEditStatus = type;
				this->noteEditIndex = index;
				this->noteEditTime = time;
				this->noteEditMinLength = minLength;
				this->repaint();
			}
			/** Move Note */
			else if (type == NoteControllerType::Inside) {
				/** Get Start Time */
				double time = this->secStart + (pos.x / this->getWidth()) * (this->secEnd - this->secStart);

				/** Get Note Index */
				auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(index);
				auto& note = this->midiDataTemp.getReference(noteIndex);
				double noteStartTime = note.startSec;

				/** Set Temp */
				this->noteEditStatus = type;
				this->noteEditIndex = index;
				this->noteEditTime = noteStartTime;
				this->noteDownTime = time;
				this->noteEditPitch = note.num;
				this->repaint();
			}
		}
	}
	else if (event.mods.isRightButtonDown()) {
		/** Remove Note */
		if (Tools::getInstance()->getType() == Tools::Type::Pencil) {
			auto& pos = event.position;

			auto [type, index] = this->getNoteControllerWithoutEdge(pos);

			if (type == NoteControllerType::Inside) {
				/** Set Temp */
				this->noteEditStatus = type;
				this->noteRemoveIndex = index;
				this->repaint();
			}
		}
	}
}

void MIDIContentViewer::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		/** Move View */
		if (this->viewMoving) {
			this->viewMoving = false;
			this->setMouseCursor(juce::MouseCursor::NormalCursor);
			this->dragEndFunc();
		}

		/** Add Note */
		if (this->noteInsertTime >= 0) {
			/** Insert Note */
			this->insertNote(
				this->noteInsertTime, this->noteInsertLength,
				this->noteInsertPitch, this->noteInsertChannel);

			/** Reset Temp */
			this->noteInsertTime = -1;
			this->noteInsertLength = -1;
			this->noteInsertPitch = 0;
			this->noteInsertChannel = 0;
			this->repaint();
		}

		/** Edit Time */
		if (this->noteEditStatus == NoteControllerType::Left || this->noteEditStatus == NoteControllerType::Right) {
			/** Set Note Time */
			if (this->noteEditStatus == NoteControllerType::Left) {
				this->setNoteStartTime(this->noteEditIndex, this->noteEditTime);
			}
			else if (this->noteEditStatus == NoteControllerType::Right) {
				this->setNoteEndTime(this->noteEditIndex, this->noteEditTime);
			}

			/** Reset Temp */
			this->noteEditStatus = NoteControllerType::None;
			this->noteEditIndex = -1;
			this->noteEditTime = -1;
			this->noteEditMinLength = -1;
			this->repaint();
		}

		/** Move Note */
		if (this->noteEditStatus == NoteControllerType::Inside) {
			/** Get Note Index */
			auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(this->noteEditIndex);
			auto& note = this->midiDataTemp.getReference(noteIndex);
			double noteStartTime = note.startSec;
			double noteEndTime = note.endSec;

			/** Set Note Time */
			this->setNoteTimeAndPitch(this->noteEditIndex, this->noteEditTime,
				this->noteEditTime + (noteEndTime - noteStartTime), this->noteEditPitch);

			/** Reset Temp */
			this->noteEditStatus = NoteControllerType::None;
			this->noteEditIndex = -1;
			this->noteEditTime = -1;
			this->noteDownTime = -1;
			this->noteEditPitch = 0;
			this->repaint();
		}
	}
	else if (event.mods.isRightButtonDown()) {
		if (this->noteEditStatus == NoteControllerType::Inside) {
			/** Remove Note */
			if (this->noteRemoveIndex > -1) {
				this->removeNote(this->noteRemoveIndex);
			}

			/** Reset Temp */
			this->noteEditStatus = NoteControllerType::None;
			this->noteRemoveIndex = -1;
			this->repaint();
		}
	}
}

void MIDIContentViewer::mouseMove(const juce::MouseEvent& event) {
	/** Send Y Pos */
	this->mouseYPosFunc(event.position.getY());

	/** Update Mouse Cursor */
	this->updateMouseCursor(event.position);
}

void MIDIContentViewer::mouseDrag(const juce::MouseEvent& event) {
	/** Send Y Pos */
	this->mouseYPosFunc(event.position.getY());

	/** Auto Scroll */
	float xPos = event.position.getX();
	if (!this->viewMoving) {
		double delta = 0;
		if (xPos > this->getWidth()) {
			delta = xPos - this->getWidth();
		}
		else if (xPos < 0) {
			delta = xPos;
		}

		if (delta != 0) {
			this->scrollFunc(delta / 4);
		}
	}

	if (event.mods.isLeftButtonDown()) {
		/** Move View */
		if (this->viewMoving) {
			int distanceX = event.getDistanceFromDragStartX();
			int distanceY = event.getDistanceFromDragStartY();
			this->dragProcessFunc(distanceX, distanceY, true, true);
		}

		/** Add Note */
		auto& pos = event.position;
		if (this->noteInsertTime >= 0) {
			/** Get Time */
			double time = this->secStart + (pos.x / this->getWidth()) * (this->secEnd - this->secStart);
			time = quickAPI::limitTimeSec(time, Tools::getInstance()->getAdsorb());

			/** Get Pitch */
			uint8_t pitch = (uint8_t)std::floor(this->keyTop - (pos.y / this->getHeight()) * (this->keyTop - this->keyBottom));

			/** Set Temp */
			this->noteInsertTime = time;
			this->noteInsertPitch = pitch;
			this->repaint();
		}

		/** Edit Time */
		if (this->noteEditStatus == NoteControllerType::Left || this->noteEditStatus == NoteControllerType::Right) {
			/** Get Time */
			double time = this->secStart + (pos.x / this->getWidth()) * (this->secEnd - this->secStart);
			time = quickAPI::limitTimeSec(time, Tools::getInstance()->getAdsorb());

			/** Limit Note Length */
			auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(this->noteEditIndex);
			auto& note = this->midiDataTemp.getReference(noteIndex);
			double noteStartTime = note.startSec;
			double noteEndTime = note.endSec;
			if (this->noteEditStatus == NoteControllerType::Right && time < noteStartTime + this->noteEditMinLength) {
				time = noteStartTime + this->noteEditMinLength;
			}
			else if (this->noteEditStatus == NoteControllerType::Left && time > noteEndTime - this->noteEditMinLength) {
				time = noteEndTime - this->noteEditMinLength;
			}

			/** Set Temp */
			this->noteEditTime = time;
			this->repaint();
		}

		/** Move Note */
		if (this->noteEditStatus == NoteControllerType::Inside) {
			/** Get Time */
			double time = this->secStart + (pos.x / this->getWidth()) * (this->secEnd - this->secStart);
			double delta = time - this->noteDownTime;

			/** Get Pitch */
			uint8_t pitch = this->keyTop - (pos.y / this->getHeight()) * (this->keyTop - this->keyBottom);

			/** Get Note Index */
			auto& [noteIndex, rect, channel] = this->noteRectTempList.getReference(this->noteEditIndex);
			auto& note = this->midiDataTemp.getReference(noteIndex);
			double noteStartTime = note.startSec;
			double noteEndTime = note.endSec;
			double newTime = quickAPI::limitTimeSec(noteStartTime + delta, Tools::getInstance()->getAdsorb());

			/** Set Temp */
			this->noteEditTime = newTime;
			this->noteEditPitch = pitch;
			this->repaint();
		}
	}
	else if (event.mods.isRightButtonDown()) {
		/** Remove Note */
		if (this->noteEditStatus == NoteControllerType::Inside) {
			auto& pos = event.position;

			auto [type, index] = this->getNoteControllerWithoutEdge(pos);

			/** Set Temp */
			this->noteRemoveIndex = (type == NoteControllerType::Inside)
				? index : -1;
			this->repaint();
		}
	}
}

void MIDIContentViewer::mouseExit(const juce::MouseEvent& event) {
	/** Send Mouse Exit */
	this->mouseLeaveFunc();

	/** Move View */
	if (this->viewMoving) {
		this->viewMoving = false;
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
		this->dragEndFunc();
	}

	/** Add Note */
	if (this->noteInsertTime >= 0) {
		/** Reset Temp */
		this->noteInsertTime = -1;
		this->noteInsertLength = -1;
		this->noteInsertPitch = 0;
		this->noteInsertChannel = 0;
		this->repaint();
	}

	/** Edit Time */
	if (this->noteEditStatus == NoteControllerType::Left || this->noteEditStatus == NoteControllerType::Right) {
		/** Reset Temp */
		this->noteEditStatus = NoteControllerType::None;
		this->noteEditIndex = -1;
		this->noteEditTime = -1;
		this->noteEditMinLength = -1;
		this->repaint();
	}

	/** Move Note */
	if (this->noteEditStatus == NoteControllerType::Inside) {
		/** Reset Temp */
		this->noteEditStatus = NoteControllerType::None;
		this->noteEditIndex = -1;
		this->noteEditTime = -1;
		this->noteDownTime = -1;
		this->noteEditPitch = 0;
		this->noteRemoveIndex = -1;
		this->repaint();
	}
}

void MIDIContentViewer::mouseWheelMove(
	const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel) {
	if (event.mods.isAltDown()) {
		double thumbPer = event.position.getX() / (double)this->getWidth();
		double centerNum = this->secStart + (this->secEnd - this->secStart) * thumbPer;

		this->wheelAltFunc(centerNum, thumbPer, wheel.deltaY, wheel.isReversed);
	}
	else {
		this->wheelFunc(wheel.deltaY, wheel.isReversed);
	}
}

void MIDIContentViewer::midiChannelChanged() {
	/** Update Note Image */
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::insertNote(
	double startTime, double length,
	uint8_t pitch, uint8_t channel) {
	/** Match Block */
	double endTime = startTime + length;
	for (int i = 0; i < this->blockItemTemp.size(); i++) {
		auto [blockStartTime, blockEndTime, sourceStartTime] = this->blockItemTemp.getUnchecked(i);
		if (blockStartTime <= startTime &&
			blockEndTime >= endTime) {
			double noteMappedStartTime = sourceStartTime + (startTime - blockStartTime);
			CoreActions::midiAddNote(this->ref, this->currentMIDITrack,
				noteMappedStartTime, noteMappedStartTime + length, channel, pitch, NOTE_VELOCITY_INIT);
			break;
		}
	}
}

void MIDIContentViewer::setNoteStartTime(int tempIndex, double time) {
	/** Get Note */
	if (tempIndex < 0 || tempIndex >= this->noteRectTempList.size()) { return; }
	auto [index, rect, channel] = this->noteRectTempList[tempIndex];

	if (index < 0 || index >= this->midiDataTemp.size()) { return; }
	auto note = this->midiDataTemp[index];

	/** Set Time */
	CoreActions::midiSetNoteTime(
		this->ref, this->currentMIDITrack,
		index, time, note.endSec);
}

void MIDIContentViewer::setNoteEndTime(int tempIndex, double time) {
	/** Get Note */
	if (tempIndex < 0 || tempIndex >= this->noteRectTempList.size()) { return; }
	auto [index, rect, channel] = this->noteRectTempList[tempIndex];

	if (index < 0 || index >= this->midiDataTemp.size()) { return; }
	auto note = this->midiDataTemp[index];

	/** Set Time */
	CoreActions::midiSetNoteTime(
		this->ref, this->currentMIDITrack,
		index, note.startSec, time);
}

void MIDIContentViewer::setNoteTimeAndPitch(int tempIndex,
	double startTime, double endTime, uint8_t pitch) {
	/** Get Note */
	if (tempIndex < 0 || tempIndex >= this->noteRectTempList.size()) { return; }
	auto [index, rect, channel] = this->noteRectTempList[tempIndex];

	if (index < 0 || index >= this->midiDataTemp.size()) { return; }

	/** Set Pitch */
	CoreActions::midiSetNotePitch(
		this->ref, this->currentMIDITrack,
		index, pitch);

	/** Set Time */
	CoreActions::midiSetNoteTime(
		this->ref, this->currentMIDITrack,
		index, startTime, endTime);
}

void MIDIContentViewer::removeNote(int tempIndex) {
	/** Get Note */
	if (tempIndex < 0 || tempIndex >= this->noteRectTempList.size()) { return; }
	auto [index, rect, channel] = this->noteRectTempList[tempIndex];

	if (index < 0 || index >= this->midiDataTemp.size()) { return; }

	/** Remove Note */
	CoreActions::midiRemoveNote(
		this->ref, this->currentMIDITrack, index);
}

void MIDIContentViewer::updateKeyImageTemp() {
	/** Clear Temp */
	juce::Graphics g(*(this->keyTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.0005;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour whiteKeyColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId);
	juce::Colour blackKeyColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::blackNoteColourId);
	juce::Colour lineColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::keySeparatorLineColourId);

	/** Background */
	g.fillAll(backgroundColor);

	/** Keys */
	int keyStart = std::floor(this->keyBottom);
	int keyEnd = std::floor(this->keyTop);

	float keyStartPos = this->getHeight() + (this->keyBottom - keyStart) * this->vItemSize;
	for (int i = keyStart; i >= 0 && i <= keyEnd; i++) {
		bool isBlackKey = this->keyMasks[i % 12];
		float keyPos = keyStartPos - (i - keyStart) * this->vItemSize;

		juce::Rectangle<float> keyRect(
			0, keyPos - this->vItemSize,
			this->keyTemp->getWidth(), this->vItemSize);
		g.setColour(isBlackKey ? blackKeyColor : whiteKeyColor);
		g.fillRect(keyRect);

		juce::Rectangle<float> lineRect(
			0, keyPos - lineThickness / 2,
			keyRect.getWidth(), lineThickness);
		g.setColour(lineColor);
		g.fillRect(lineRect);
	}
}

void MIDIContentViewer::updateRulerImageTemp() {
	/** Clear Temp */
	this->rulerTemp->clear(this->rulerTemp->getBounds());
	juce::Graphics g(*(this->rulerTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float longLineThickness = screenSize.getWidth() * 0.00075;
	float shortLineThickness = screenSize.getWidth() * 0.0005;

	float shortLineIntervalMin = screenSize.getWidth() * 0.01 / 8;
	float dashLineIntervalMin = screenSize.getWidth() * 0.01;

	float dashLineThickness = screenSize.getWidth() * 0.0005;
	float dashLineDashLength = screenSize.getHeight() * 0.005;
	float dashLineSkipLength = dashLineDashLength;
	float dashLineArray[2] = { dashLineDashLength, dashLineSkipLength };

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour lineColor = laf.findColour(
		juce::TableListBox::ColourIds::outlineColourId);

	/** Lines */
	for (int i = 0; i < this->lineTemp.size(); i++) {
		auto [xPos, type, barId] = this->lineTemp.getUnchecked(i);

		/** Check Interval */
		if (type == LineItemType::Dashed) {
			if (i > 0 && (xPos - std::get<0>(this->lineTemp.getUnchecked(i - 1))) < dashLineIntervalMin) {
				continue;
			}
			if (i < this->lineTemp.size() - 1 && (std::get<0>(this->lineTemp.getUnchecked(i + 1)) - xPos) < dashLineIntervalMin) {
				continue;
			}
		}
		if (type == LineItemType::Beat) {
			if (i > 0 && (xPos - std::get<0>(this->lineTemp.getUnchecked(i - 1))) < shortLineIntervalMin) {
				continue;
			}
			if (i < this->lineTemp.size() - 1 && (std::get<0>(this->lineTemp.getUnchecked(i + 1)) - xPos) < shortLineIntervalMin) {
				continue;
			}
		}

		/** Line */
		if (type != LineItemType::Dashed) {
			float lineThickness = type == LineItemType::Bar ? longLineThickness : shortLineThickness;
			juce::Rectangle<float> lineRect(
				xPos - lineThickness / 2, 0,
				lineThickness, this->rulerTemp->getHeight());

			g.setColour(lineColor);
			g.fillRect(lineRect);
		}
		else {
			juce::Line<float> line(
				xPos, 0, xPos, this->rulerTemp->getHeight());

			g.setColour(lineColor);
			g.drawDashedLine(line, dashLineArray,
				sizeof(dashLineArray) / sizeof(float), dashLineThickness, 0);
		}
	}
}

void MIDIContentViewer::updateBlockImageTemp() {
	/** Clear Temp */
	this->blockTemp->clear(this->blockTemp->getBounds());
	juce::Graphics g(*(this->blockTemp.get()));

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);

	/** Paint Each Area */
	if (this->currentMIDITrack >= 0) {
		double startSec = 0;
		for (int i = 0; i < this->blockItemTemp.size() + 1; i++) {
			double endSec = (i < this->blockItemTemp.size())
				? std::get<0>(this->blockItemTemp.getUnchecked(i))
				: std::max(startSec, this->secEnd);

			if (startSec < this->secEnd && endSec > this->secStart) {
				float startPos = (startSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
				float endPos = (endSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();

				juce::Rectangle<float> areaRect(
					startPos, 0, endPos - startPos, this->blockTemp->getHeight());
				g.setColour(offColor);
				g.fillRect(areaRect);
			}

			if (i < this->blockItemTemp.size()) {
				startSec = std::get<1>(this->blockItemTemp.getUnchecked(i));
			}
		}
	}
	/** No Track */
	else {
		g.setColour(offColor);
		g.fillAll();
	}
}

void MIDIContentViewer::updateNoteImageTemp() {
	/** Clear Temp */
	this->noteTemp->clear(this->noteTemp->getBounds());
	juce::Graphics g(*(this->noteTemp.get()));
	int width = this->noteTemp->getWidth(), height = this->noteTemp->getHeight();

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float noteCornerSize = screenSize.getHeight() * 0.003;
	float noteOutlineThickness = screenSize.getHeight() * 0.001;

	float notePaddingWidth = screenSize.getWidth() * 0.003;
	float notePaddingHeight = screenSize.getHeight() * 0.0025;
	float noteFontHeight = screenSize.getHeight() * 0.0135;
	float noteLyricsFontHeight = screenSize.getHeight() * 0.0135;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour noteOutlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);
	juce::Colour noteLyricsColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 3);
	juce::Colour noteBaseColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);

	/** Font */
	juce::Font noteLabelFont(juce::FontOptions{ noteFontHeight });
	juce::Font noteLyricsFont(juce::FontOptions{ noteLyricsFontHeight });

	/** Temp */
	this->noteRectTempList.clear();
	uint8_t midiChannel = Tools::getInstance()->getMIDIChannel();

	/** Blocks */
	int minNoteNum = std::floor(this->keyBottom), maxNoteNum = std::floor(this->keyTop);
	for (int i = 0; i < this->blockItemTemp.size(); i++) {
		auto [blockStartTime, blockEndTime, sourceStartTime] = this->blockItemTemp.getUnchecked(i);
		if ((blockStartTime <= this->secEnd)
			&& (blockEndTime >= this->secStart)) {
			double sourceEndTime = sourceStartTime + (blockEndTime - blockStartTime);
			
			/** For Each Notes */
			for (int j = 0; j < this->midiDataTemp.size(); j++) {
				auto& note = this->midiDataTemp.getReference(j);
				if (note.startSec <= sourceEndTime &&
					sourceStartTime <= note.endSec) {
					double noteMappedStartSec = blockStartTime + (note.startSec - sourceStartTime);
					double noteMappedEndSec = blockStartTime + (note.endSec - sourceStartTime);
					if (noteMappedStartSec <= this->secEnd &&
						this->secStart <= noteMappedEndSec) {
						if (note.num >= (minNoteNum - 1) &&
							note.num <= maxNoteNum) {
							/** Opaque */
							float opaque = (note.channel == midiChannel) ? 1.0f : 0.4f;

							/** Note Rect */
							float startXPos = (noteMappedStartSec - this->secStart) / (this->secEnd - this->secStart) * width;
							float endXPos = (noteMappedEndSec - this->secStart) / (this->secEnd - this->secStart) * width;
							float noteYPos = ((note.num + 1) - this->keyTop) / (this->keyBottom - this->keyTop) * height;
							juce::Rectangle<float> noteRect(
								startXPos, noteYPos,
								endXPos - startXPos, (float)this->vItemSize);
							g.setColour(noteBaseColor);
							g.fillRoundedRectangle(noteRect, noteCornerSize);
							g.setColour(this->noteColorGradient[note.channel - 1].withAlpha(opaque));
							g.fillRoundedRectangle(noteRect, noteCornerSize);
							g.setColour(noteOutlineColor.withAlpha(opaque));
							g.drawRoundedRectangle(noteRect, noteCornerSize, noteOutlineThickness);

							/** Note Name */
							juce::String noteName = this->keyNames[note.num % this->keyMasks.size()] + juce::String{ note.num / this->keyMasks.size() };
							float noteNameWidth = juce::TextLayout::getStringWidth(noteLabelFont, noteName);
							if ((noteNameWidth + notePaddingWidth * 2) <= noteRect.getWidth()
								&& (noteFontHeight + notePaddingHeight * 2) <= noteRect.getHeight()) {
								juce::Rectangle<float> noteLabelRect = noteRect.withWidth(noteNameWidth + notePaddingWidth * 2);
								g.setFont(noteLabelFont);
								g.setColour(this->noteLabelColorGradient[note.channel - 1].withAlpha(opaque));
								g.drawFittedText(noteName, noteLabelRect.toNearestInt(),
									juce::Justification::centred, 1, 0.75f);
							}

							/** Lyrics */
							float noteLyricsWidth = juce::TextLayout::getStringWidth(noteLyricsFont, note.lyrics);
							if (notePaddingWidth * 2 <= noteRect.getWidth()) {
								juce::Rectangle<float> noteLyricsRect(
									noteRect.getX() + notePaddingWidth, noteRect.getY() - (float)this->vItemSize,
									noteRect.getWidth() - notePaddingWidth * 2, (float)this->vItemSize);
								g.setFont(noteLyricsFont);
								g.setColour(noteLyricsColor.withAlpha(opaque));
								g.drawFittedText(note.lyrics, noteLyricsRect.toNearestInt(),
									juce::Justification::left, 1, 1.0f);
							}

							/** Add Temp */
							this->noteRectTempList.add({ j, noteRect, note.channel });
						}
					}
				}
			}
		}
	}
}

void MIDIContentViewer::updateMouseCursor(const juce::Point<float>& pos) {
	std::tuple<MIDIContentViewer::NoteControllerType, int> noteController;
	switch (Tools::getInstance()->getType()) {
	case Tools::Type::Arrow:
		noteController = this->getNoteControllerWithoutEdge(pos);
		break;
	case Tools::Type::Pencil:
		noteController = this->getNoteController(pos);
		break;
	}

	switch (std::get<0>(noteController)) {
	case NoteControllerType::Left:
		this->setMouseCursor(juce::MouseCursor::LeftEdgeResizeCursor);
		break;
	case NoteControllerType::Right:
		this->setMouseCursor(juce::MouseCursor::RightEdgeResizeCursor);
		break;
	case NoteControllerType::Inside:
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
		break;
	default:
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
		break;
	}
}

std::tuple<MIDIContentViewer::NoteControllerType, int>
MIDIContentViewer::getNoteController(const juce::Point<float>& pos) const {
	/** MIDI Channel */
	uint8_t midiChannel = Tools::getInstance()->getMIDIChannel();

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int noteJudgeWidth = screenSize.getWidth() * 0.005;

	int width = this->noteTemp->getWidth(), height = this->noteTemp->getHeight();

	/** Check Pos Inside Note */
	auto [type, index] = this->getNoteControllerWithoutEdge(pos);
	if (index >= 0) {
		auto& note = this->noteRectTempList.getReference(index);
		float startXPos = std::get<1>(note).getX();
		float endXPos = std::get<1>(note).getRight();

		/** Judge Area */
		float judgeSSX = startXPos - noteJudgeWidth, judgeSEX = startXPos + noteJudgeWidth;
		float judgeESX = endXPos - noteJudgeWidth, judgeEEX = endXPos + noteJudgeWidth;
		if (endXPos - startXPos < noteJudgeWidth * 2) {
			judgeSEX = startXPos + (endXPos - startXPos) / 2;
			judgeESX = endXPos - (endXPos - startXPos) / 2;
		}

		/** Get Controller */
		if (pos.getX() >= judgeSSX && pos.getX() < judgeSEX) {
			return { NoteControllerType::Left, index };
		}
		else if (pos.getX() >= judgeESX && pos.getX() < judgeEEX) {
			return { NoteControllerType::Right, index };
		}
		else if (pos.getX() >= judgeSEX && pos.getX() < judgeESX) {
			return { NoteControllerType::Inside, index };
		}
	}

	/** Get Each Note */
	for (int i = 0; i < this->noteRectTempList.size(); i++) {
		auto& [index, rect, channel] = this->noteRectTempList.getReference(i);
		if (channel == midiChannel) {
			if (pos.getY() >= rect.getY() && pos.getY() < rect.getBottom()) {
				/** Judge Area */
				float judgeSSX = rect.getX() - noteJudgeWidth, judgeSEX = rect.getX() + noteJudgeWidth;
				float judgeESX = rect.getRight() - noteJudgeWidth, judgeEEX = rect.getRight() + noteJudgeWidth;
				if (rect.getRight() - rect.getX() < noteJudgeWidth * 2) {
					judgeSEX = rect.getX() + (rect.getRight() - rect.getX()) / 2;
					judgeESX = rect.getRight() - (rect.getRight() - rect.getX()) / 2;
				}

				/** Get Controller */
				if (pos.getX() >= judgeSSX && pos.getX() < judgeSEX) {
					return { NoteControllerType::Left, i };
				}
				else if (pos.getX() >= judgeESX && pos.getX() < judgeEEX) {
					return { NoteControllerType::Right, i };
				}
				else if (pos.getX() >= judgeSEX && pos.getX() < judgeESX) {
					return { NoteControllerType::Inside, i };
				}
			}
		}
	}

	/** None */
	return { NoteControllerType::None, -1 };
}

std::tuple<MIDIContentViewer::NoteControllerType, int>
MIDIContentViewer::getNoteControllerWithoutEdge(const juce::Point<float>& pos) const {
	/** MIDI Channel */
	uint8_t midiChannel = Tools::getInstance()->getMIDIChannel();

	/** Get Each Note */
	for (int i = 0; i < this->noteRectTempList.size(); i++) {
		auto& [index, rect, channel] = this->noteRectTempList.getReference(i);
		/** Inside Note */
		if (channel == midiChannel) {
			if (pos.getX() >= rect.getX() && pos.getX() < rect.getRight()
				&& pos.getY() >= rect.getY() && pos.getY() < rect.getBottom()) {
				return { NoteControllerType::Inside, i };
			}
		}
	}

	/** None */
	return { NoteControllerType::None, -1 };
}

std::tuple<double, double> MIDIContentViewer::getHViewArea(double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getWidth() / itemSize;
	return { secStart, secStart + secLength };
}

std::tuple<double, double> MIDIContentViewer::getVViewArea(double pos, double itemSize) const {
	double keyTop = this->totalKeys - pos / itemSize;
	double keyNum = this->getHeight() / itemSize;
	return { keyTop, keyTop - keyNum };
}

const MIDIContentViewer::LineItemList
MIDIContentViewer::createRulerLine(double pos, double itemSize) const {
	/** Get View Area */
	auto [secStart, secEnd] = this->getHViewArea(pos, itemSize);
	double width = this->getWidth();

	/** Get Each Line */
	LineItemList result;

	/** Get Temp */
	int tempStartIndex = quickAPI::getTempoTempIndexBySec(secStart);
	int tempEndIndex = quickAPI::getTempoTempIndexBySec(secEnd);
	juce::Array<quickAPI::TempoData> tempoTempList;
	for (int i = tempStartIndex; i <= tempEndIndex; i++) {
		tempoTempList.add(quickAPI::getTempoData(i));
	}
	if (tempoTempList.size() <= 0) { return result; }

	/** Line Start */
	constexpr int dashLineNum = 8;
	double realSecStart = secStart;
	int tempIndex = 0;
	auto [timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator] = tempoTempList.getUnchecked(tempIndex);
	{
		/** Get Real Quarter */
		double quarterStart = timeInQuarter + (secStart - timeInSec) / secPerQuarter;
		double realQuarterStart = std::floor(quarterStart * (denominator / 4.0) * dashLineNum) / ((denominator / 4.0) * dashLineNum);
		if (!juce::approximatelyEqual(quarterStart, realQuarterStart)) {
			realQuarterStart += ((4.0 / denominator) / dashLineNum);
		}

		/** Next Temp */
		while ((tempoTempList.size() > (tempIndex + 1)) &&
			(realQuarterStart > std::get<1>(tempoTempList.getUnchecked(tempIndex + 1)))) {
			realQuarterStart -= (4.0 / denominator) / dashLineNum;
			std::tie(timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator) = tempoTempList.getUnchecked(++tempIndex);
			realQuarterStart += (4.0 / denominator) / dashLineNum;
		}

		/** Get Real Sec */
		realSecStart = timeInSec + (realQuarterStart - timeInQuarter) * secPerQuarter;
	}

	/** Build Line Temp */
	for (double currentSec = realSecStart; currentSec < secEnd;) {
		/** Check Current Is Bar */
		double currentQuarter = timeInQuarter + (currentSec - timeInSec) / secPerQuarter;
		double currentBar = timeInBar + (currentQuarter - timeInQuarter) * (denominator / 4.0) / numerator;
		constexpr double epsilon = 1.0 / 480.0;
		//bool isBar = juce::approximatelyEqual(currentBar, std::round(currentBar));
		LineItemType type = LineItemType::Dashed;
		if (std::abs(std::round(currentQuarter * (denominator / 4.0)) - (currentQuarter * (denominator / 4.0))) < epsilon) {
			type = LineItemType::Beat;
		}
		if (std::abs(std::round(currentBar) - currentBar) < epsilon) {
			type = LineItemType::Bar;
		}

		/** Get Current X Pos */
		double XPos = (currentSec - secStart) / (secEnd - secStart) * width;

		/** Add Into Result */
		result.add({ XPos, type, (int)std::round(currentBar) });

		/** Next Line */
		double nextQuarter = currentQuarter + (4.0 / denominator) / dashLineNum;
		while ((tempoTempList.size() > (tempIndex + 1)) &&
			(nextQuarter > std::get<1>(tempoTempList.getUnchecked(tempIndex + 1)))) {
			nextQuarter -= (4.0 / denominator) / dashLineNum;
			std::tie(timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator) = tempoTempList.getUnchecked(++tempIndex);
			nextQuarter += (4.0 / denominator)/ dashLineNum;
		}

		/** Update Current Sec */
		currentSec = timeInSec + (nextQuarter - timeInQuarter) * secPerQuarter;
	}

	/** Result */
	return result;
}
