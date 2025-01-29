#include "SeqTrackInputMonitoringComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SeqTrackInputMonitoringComponent::SeqTrackInputMonitoringComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::InputMonitoringButton));
}

void SeqTrackInputMonitoringComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;

	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;

	float textFontHeight = buttonWidth * 0.7;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->inputMonitoring
		? juce::TextButton::ColourIds::buttonOnColourId
		: juce::TextButton::ColourIds::buttonColourId);
	juce::Colour textColor = laf.findColour(this->inputMonitoring
		? juce::TextButton::ColourIds::textColourOnId
		: juce::TextButton::ColourIds::textColourOffId);

	/** Font */
	juce::Font textFont(juce::FontOptions{ textFontHeight });

	/** Button */
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);
	g.setColour(backgroundColor);
	g.fillRect(buttonRect);

	g.setColour(textColor);
	g.drawRect(buttonRect, lineThickness);

	g.setFont(textFont);
	g.drawFittedText("I", buttonRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void SeqTrackInputMonitoringComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void SeqTrackInputMonitoringComponent::mouseMove(const juce::MouseEvent& event) {
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

void SeqTrackInputMonitoringComponent::mouseUp(const juce::MouseEvent& event) {
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
			this->changeInputMonitoring();
		}
		else if (event.mods.isRightButtonDown()) {
			this->showMenu();
		}
	}
}

void SeqTrackInputMonitoringComponent::updateIndex(int index) {
	this->index = index;
}

void SeqTrackInputMonitoringComponent::update() {
	/** Get Input Monitoring State */
	this->inputMonitoring = quickAPI::getTrackInputMonitoring(
		{ quickAPI::TrackType::Track, this->index });

	/** Get Input Connections */
	this->midiInput = quickAPI::isTrackMIDIInputConnected(
		{ quickAPI::TrackType::Track, this->index });
	this->audioInput = quickAPI::getTrackAudioInputChannels(
		{ quickAPI::TrackType::Track, this->index });

	/** Repaint */
	this->repaint();
}

void SeqTrackInputMonitoringComponent::changeInputMonitoring() {
	CoreActions::setTrackInputMonitoring(this->index, !(this->inputMonitoring));
}

enum SeqInputMonitoringButtonActionType {
	MIDIInput = 1, AudioInput
};

void SeqTrackInputMonitoringComponent::showMenu() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case SeqInputMonitoringButtonActionType::MIDIInput:
		this->changeMIDIInput();
		break;
	case SeqInputMonitoringButtonActionType::AudioInput:
		this->changeAudioInput();
		break;
	}
}

void SeqTrackInputMonitoringComponent::changeMIDIInput() {
	if (this->midiInput) {
		CoreActions::removeTrackMIDIInput(
			quickAPI::TrackType::Track, this->index);
	}
	else {
		CoreActions::addTrackMIDIInput(
			quickAPI::TrackType::Track, this->index);
	}
}

void SeqTrackInputMonitoringComponent::changeAudioInput() {
	CoreActions::setTrackAudioInputGUI(
		quickAPI::TrackType::Track, this->index);
}

juce::PopupMenu SeqTrackInputMonitoringComponent::createMenu() {
	juce::PopupMenu menu;

	menu.addItem(SeqInputMonitoringButtonActionType::MIDIInput,
		TRANS("MIDI Input"), true, this->midiInput);
	menu.addItem(SeqInputMonitoringButtonActionType::AudioInput,
		TRANS("Audio Input"), true, (this->audioInput.size() > 0));

	return menu;
}
