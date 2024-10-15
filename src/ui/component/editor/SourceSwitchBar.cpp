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

	/** Sync Name Button */
	this->syncButtonName();
}

void SourceSwitchBar::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonPaddingHeight = screenSize.getHeight() * 0.003;
	int buttonPaddingWidth = screenSize.getWidth() * 0.003;
	int buttonSplitWidth = screenSize.getWidth() * 0.003;

	int buttonHeight = this->getHeight() - buttonPaddingHeight * 2;
	int nameButtonWidth = screenSize.getWidth() * 0.03;

	/** Switch Button */
	juce::Rectangle<int> switchRect(
		buttonPaddingWidth, buttonPaddingHeight,
		buttonHeight, buttonHeight);
	this->switchButton->setBounds(switchRect);

	/** Name Button */
	juce::Rectangle<int> nameRect(
		switchRect.getRight(), buttonPaddingHeight,
		nameButtonWidth, buttonHeight);
	this->nameButton->setBounds(nameRect);

	/** Fit Name Button Width */
	this->nameButton->changeWidthToFitText();
	int rightLimit = this->getWidth() - buttonPaddingWidth;
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
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.fillAll(backgroundColor);
}

void SourceSwitchBar::update(int index, uint64_t audioRef, uint64_t midiRef) {
	/** Set Source Info Temp */
	this->index = index;
	this->audioRef = audioRef;
	this->midiRef = midiRef;
	this->audioName = quickAPI::getAudioSourceName(audioRef);
	this->midiName = quickAPI::getMIDISourceName(midiRef);

	/** Current Source Invalid */
	if (((this->current == SwitchState::Audio) && (audioRef == 0))
		|| ((this->current == SwitchState::MIDI) && (midiRef == 0))) {
		this->switchInternal(SwitchState::Off);
		return;
	}

	/** Update Name */
	this->syncButtonName();
}

void SourceSwitchBar::switchTo(SwitchState state) {
	this->current = state;
	this->syncButtonName();
}

void SourceSwitchBar::showSwitchMenu() {
	auto menu = this->createSwitchMenu();
	int result = menu.showAt(this->switchButton.get());

	if (result > 0) {
		this->switchInternal((SourceSwitchBar::SwitchState)result);
	}
}

void SourceSwitchBar::switchInternal(SwitchState state) {
	this->stateCallback(state);
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
	}

	this->nameButton->setButtonText(buttonText);

	/** Update Button Size */
	this->resized();
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
