#include "SeqTrackRecComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SeqTrackRecComponent::SeqTrackRecComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::RecButton));
}

void SeqTrackRecComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;

	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;

	float iconHeight = buttonWidth * 0.4;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour((this->rec > 0)
		? juce::TextButton::ColourIds::buttonOnColourId
		: juce::TextButton::ColourIds::buttonColourId);
	juce::Colour iconColor = laf.findColour((this->rec > 0)
		? juce::TextButton::ColourIds::textColourOnId
		: juce::TextButton::ColourIds::textColourOffId);

	/** Button */
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);
	g.setColour(backgroundColor);
	g.fillRect(buttonRect);

	g.setColour(iconColor);
	g.drawRect(buttonRect, lineThickness);

	juce::Rectangle<float> iconRect(
		buttonRect.getWidth() / 2.f - iconHeight / 2.f,
		buttonRect.getHeight() / 2.f - iconHeight / 2.f,
		iconHeight, iconHeight);
	g.setColour(iconColor);
	g.fillEllipse(iconRect);
}

void SeqTrackRecComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void SeqTrackRecComponent::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;
	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);

	/** Cursor */
	this->setMouseCursor(buttonRect.contains(event.position)
		? juce::MouseCursor::PointingHandCursor
		: juce::MouseCursor::NormalCursor);
}

void SeqTrackRecComponent::mouseUp(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;
	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);

	if (buttonRect.contains(event.position)) {
		if (event.mods.isLeftButtonDown()) {
			this->changeRecQuick();
		}
		else if (event.mods.isRightButtonDown()) {
			this->changeRec();
		}
	}
}

void SeqTrackRecComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->rec = quickAPI::getSeqTrackRecording(index);

		this->repaint();
	}
}

enum SeqRecordMenuActionType {
	NoMIDI = 1, MIDINewTrack, MIDIInsert, MIDICover,
	NoAudio, AudioInsert, AudioCover
};

void SeqTrackRecComponent::changeRecQuick() {
	CoreActions::setSeqRec(this->index, static_cast<quickAPI::RecordState>(
		(this->rec == quickAPI::RecordState::NotRecording)
		? quickAPI::RecordState::MIDICoverMode | quickAPI::RecordState::AudioCoverMode
		: quickAPI::RecordState::NotRecording));
}

void SeqTrackRecComponent::changeRec() {
	auto menu = this->createMenu();
	int result = menu.show();

	int state = this->rec;
	switch (result) {
	case SeqRecordMenuActionType::NoMIDI:
		state = (state & 0xF0) | quickAPI::RecordState::NotRecording;
		break;
	case SeqRecordMenuActionType::MIDINewTrack:
		state = (state & 0xF0) | quickAPI::RecordState::MIDINewTrackMode;
		break;
	case SeqRecordMenuActionType::MIDIInsert:
		state = (state & 0xF0) | quickAPI::RecordState::MIDIInsertMode;
		break;
	case SeqRecordMenuActionType::MIDICover:
		state = (state & 0xF0) | quickAPI::RecordState::MIDICoverMode;
		break;
	case SeqRecordMenuActionType::NoAudio:
		state = (state & 0x0F) | quickAPI::RecordState::NotRecording;
		break;
	case SeqRecordMenuActionType::AudioInsert:
		state = (state & 0x0F) | quickAPI::RecordState::AudioInsertMode;
		break;
	case SeqRecordMenuActionType::AudioCover:
		state = (state & 0x0F) | quickAPI::RecordState::AudioCoverMode;
		break;
	default:
		break;
	}

	if (state != this->rec) {
		CoreActions::setSeqRec(this->index, static_cast<quickAPI::RecordState>(state));
	}
}

juce::PopupMenu SeqTrackRecComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(SeqRecordMenuActionType::NoMIDI, TRANS("No MIDI Recording"), true,
		(this->rec & 0x0F) == quickAPI::RecordState::NotRecording);
	menu.addItem(SeqRecordMenuActionType::MIDINewTrack, TRANS("MIDI New Track Mode"), true,
		(this->rec & 0x0F) == quickAPI::RecordState::MIDINewTrackMode);
	menu.addItem(SeqRecordMenuActionType::MIDIInsert, TRANS("MIDI Insert Mode"), true,
		(this->rec & 0x0F) == quickAPI::RecordState::MIDIInsertMode);
	menu.addItem(SeqRecordMenuActionType::MIDICover, TRANS("MIDI Cover Mode"), true,
		(this->rec & 0x0F) == quickAPI::RecordState::MIDICoverMode);

	menu.addSeparator();

	menu.addItem(SeqRecordMenuActionType::NoAudio, TRANS("No Audio Recording"), true,
		(this->rec & 0xF0) == quickAPI::RecordState::NotRecording);
	menu.addItem(SeqRecordMenuActionType::AudioInsert, TRANS("Audio Insert Mode"), true,
		(this->rec & 0xF0) == quickAPI::RecordState::AudioInsertMode);
	menu.addItem(SeqRecordMenuActionType::AudioCover, TRANS("Audio Cover Mode"), true,
		(this->rec & 0xF0) == quickAPI::RecordState::AudioCoverMode);

	return menu;
}
