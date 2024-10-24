#include "SourceSwitchBar.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

SourceSwitchBar::SourceSwitchBar(
	const StateChangedCallback& stateCallback)
	: stateCallback(stateCallback) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forEditorSwitchBar());

	/** Icons */
	this->switchIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Arrows", "arrow-down-s-fill").getFullPathName());
	this->switchIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Buttons */
	this->switchButton = std::make_unique<juce::DrawableButton>(
		TRANS("Switch Editor Source"), juce::DrawableButton::ImageOnButtonBackground);
	this->switchButton->setImages(this->switchIcon.get());
	this->switchButton->setWantsKeyboardFocus(false);
	this->switchButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->switchButton->setConnectedEdges(juce::Button::ConnectedOnRight);
	this->switchButton->onClick = [this] { this->showSwitchMenu(); };
	this->addAndMakeVisible(this->switchButton.get());

	this->nameButton = std::make_unique<juce::TextButton>(
		TRANS("Switch Editor Source"), TRANS("Switch Editor Source"));
	this->nameButton->setWantsKeyboardFocus(false);
	this->nameButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->nameButton->setConnectedEdges(juce::Button::ConnectedOnLeft);
	this->nameButton->onClick = [this] { this->showSwitchMenu(); };
	this->addAndMakeVisible(this->nameButton.get());

	/** Source Create String */
	this->audioCreateStr = TRANS("Create Audio Source");
	this->midiCreateStr = TRANS("Create MIDI Source");

	/** Track Empty Name */
	this->trackEmptyName = TRANS("Untitled");

	/** Sync Name Button */
	this->syncButtonName();
}

void SourceSwitchBar::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonPaddingHeight = screenSize.getHeight() * 0.003;
	int buttonPaddingWidth = screenSize.getWidth() * 0.005;
	int buttonSplitWidth = screenSize.getWidth() * 0.003;

	int buttonHeight = this->getHeight() - buttonPaddingHeight * 2;
	int nameButtonWidth = screenSize.getWidth() * 0.03;

	float trackNameFontHeight = screenSize.getHeight() * 0.02;
	int trackNameMaxWidth = screenSize.getWidth() * 0.05;

	/** Font */
	juce::Font trackNameFont(juce::FontOptions{ trackNameFontHeight });

	/** Switch Button */
	juce::Rectangle<int> switchRect(
		buttonPaddingWidth, buttonPaddingHeight,
		buttonHeight, buttonHeight);
	this->switchButton->setBounds(switchRect);

	/** Track Name */
	int trackNameWidth = std::min((float)trackNameMaxWidth, std::ceil(juce::TextLayout::getStringWidth(trackNameFont, this->trackName)));
	int rightLimit = this->getWidth() - buttonPaddingWidth - trackNameWidth - buttonSplitWidth;

	/** Name Button */
	juce::Rectangle<int> nameRect(
		switchRect.getRight(), buttonPaddingHeight,
		nameButtonWidth, buttonHeight);
	this->nameButton->setBounds(nameRect);

	/** Fit Name Button Width */
	this->nameButton->changeWidthToFitText();
	if (this->nameButton->getWidth() < nameButtonWidth) {
		nameRect.setWidth(nameButtonWidth);
		this->nameButton->setBounds(nameRect);
	}
	if (this->nameButton->getRight() > rightLimit) {
		nameRect.setRight(rightLimit);
		this->nameButton->setBounds(nameRect);
	}
}

void SourceSwitchBar::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonPaddingHeight = screenSize.getHeight() * 0.003;
	int buttonPaddingWidth = screenSize.getWidth() * 0.005;
	int buttonSplitWidth = screenSize.getWidth() * 0.003;

	int buttonHeight = this->getHeight() - buttonPaddingHeight * 2;

	float trackNameFontHeight = screenSize.getHeight() * 0.02;
	int trackNameMaxWidth = screenSize.getWidth() * 0.05;

	/** Font */
	juce::Font trackNameFont(juce::FontOptions{ trackNameFontHeight });

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour trackNameColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Background */
	g.fillAll(backgroundColor);

	/** Switch Button */
	juce::Rectangle<int> switchRect(
		buttonPaddingWidth, buttonPaddingHeight,
		buttonHeight, buttonHeight);

	/** Track Name */
	int trackNameWidth = std::min((float)trackNameMaxWidth, std::ceil(juce::TextLayout::getStringWidth(trackNameFont, this->trackName)));
	int trackNamePosX = this->getWidth() - buttonPaddingWidth - trackNameWidth - buttonSplitWidth;
	if (trackNamePosX < switchRect.getRight()) {
		trackNamePosX = switchRect.getRight();
		trackNameWidth = this->getWidth() - buttonPaddingWidth - trackNamePosX;
	}
	juce::Rectangle<int> trackNameRect(
		trackNamePosX, 0, trackNameWidth, this->getHeight());
	g.setFont(trackNameFont);
	g.setColour(trackNameColor);
	g.drawFittedText(this->trackName, trackNameRect,
		juce::Justification::centredRight, 1, 0.75f);
}

void SourceSwitchBar::update(int index, uint64_t audioRef, uint64_t midiRef) {
	/** Set Source Info Temp */
	this->index = index;
	if (index >= 0) {
		auto name = quickAPI::getSeqTrackName(index);
		this->trackName = name.isEmpty() ? this->trackEmptyName : name;
	}
	else {
		this->trackName = "";
	}

	this->audioRef = audioRef;
	this->midiRef = midiRef;
	this->audioName = quickAPI::getAudioSourceName(audioRef);
	this->midiName = quickAPI::getMIDISourceName(midiRef);

	/** Auto Switch Editors */
	if ((midiRef != 0) && (this->current != SwitchState::MIDI)) {
		this->switchTo(SwitchState::MIDI, true);
		return;
	}
	if ((audioRef != 0) && (this->current != SwitchState::Audio)) {
		this->switchTo(SwitchState::Audio, true);
		return;
	}
	if ((audioRef == 0) && (midiRef == 0) && (this->current != SwitchState::Off)) {
		this->switchTo(SwitchState::Off, true);
		return;
	}

	/** Update Name */
	this->syncButtonName();
}

void SourceSwitchBar::switchTo(SwitchState state, bool invokeCallback) {
	this->current = state;
	this->syncButtonName();

	if (invokeCallback) {
		this->stateCallback(state);
	}
}

void SourceSwitchBar::showSwitchMenu() {
	auto menu = this->createSwitchMenu();
	int result = menu.showAt(this->switchButton.get());

	if (result > 0) {
		this->switchTo((SourceSwitchBar::SwitchState)result, true);
	}
}

void SourceSwitchBar::syncButtonName() {
	juce::String buttonText = TRANS("Empty");

	switch (this->current) {
	case SwitchState::Audio: {
		buttonText = this->audioName;
		break;
	}
	case SwitchState::MIDI: {
		buttonText = this->midiName;
		break;
	}
	default:
		break;
	}

	this->nameButton->setButtonText(buttonText);

	/** Update Button Size */
	this->resized();

	/** Update Track Name */
	this->repaint();
}

juce::PopupMenu SourceSwitchBar::createSwitchMenu() const {
	juce::PopupMenu menu;

	menu.addItem((int)SwitchState::Off, TRANS("Empty"),
		(this->audioRef == 0) && (this->midiRef == 0), this->current == SwitchState::Off);
	menu.addItem((int)SwitchState::Audio, (this->audioRef == 0) ? this->audioCreateStr : this->audioName,
		(this->index >= 0), this->current == SwitchState::Audio);
	menu.addItem((int)SwitchState::MIDI, (this->midiRef == 0) ? this->midiCreateStr : this->midiName,
		(this->index >= 0), this->current == SwitchState::MIDI);

	return menu;
}
