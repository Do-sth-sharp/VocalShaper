#include "MixerTrackMuteComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

MixerTrackMuteComponent::MixerTrackMuteComponent(
	const RightButtonCallback& rightCallback)
	: rightCallback(rightCallback) {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMuteButton());
}

void MixerTrackMuteComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonWidth = screenSize.getWidth() * 0.01;
	int buttonHeight = buttonWidth;

	float lineThickness = screenSize.getHeight() * 0.001;
	float textFontHeight = screenSize.getHeight() * 0.011;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->mute
		? juce::TextButton::ColourIds::buttonOnColourId
		: juce::TextButton::ColourIds::buttonColourId);
	juce::Colour textColor = laf.findColour(this->mute
		? juce::TextButton::ColourIds::textColourOnId
		: juce::TextButton::ColourIds::textColourOffId);

	/** Font */
	juce::Font textFont(textFontHeight);

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
	g.drawFittedText("M", buttonRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void MixerTrackMuteComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void MixerTrackMuteComponent::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonWidth = screenSize.getWidth() * 0.01;
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

void MixerTrackMuteComponent::mouseUp(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int buttonWidth = screenSize.getWidth() * 0.01;
	int buttonHeight = buttonWidth;
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);

	if (buttonRect.contains(event.position)) {
		if (event.mods.isLeftButtonDown()) {
			this->changeMute();
		}
		else if (event.mods.isRightButtonDown()) {
			this->showMenu();
		}
	}
	else {
		if (event.mods.isRightButtonDown()) {
			this->rightCallback();
		}
	}
}

void MixerTrackMuteComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->mute = quickAPI::getMixerTrackMute(index);

		this->repaint();
	}
}

void MixerTrackMuteComponent::changeMute() {
	CoreActions::setTrackMute(this->index, !(this->mute));
}

enum MixerMuteActionType {
	Mute = 1, Solo, MuteAll, UnmuteAll
};

void MixerTrackMuteComponent::showMenu() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case MixerMuteActionType::Mute:
		this->changeMute();
		break;
	case MixerMuteActionType::Solo:
		CoreActions::setTrackSolo(this->index);
		break;
	case MixerMuteActionType::MuteAll:
		CoreActions::setTrackMuteAll(true);
		break;
	case MixerMuteActionType::UnmuteAll:
		CoreActions::setTrackMuteAll(false);
		break;
	}
}

juce::PopupMenu MixerTrackMuteComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(MixerMuteActionType::Mute, TRANS("Mute"), true, this->mute);
	menu.addItem(MixerMuteActionType::Solo, TRANS("Solo"));
	menu.addSeparator();
	menu.addItem(MixerMuteActionType::MuteAll, TRANS("Mute All"));
	menu.addItem(MixerMuteActionType::UnmuteAll, TRANS("Unmute All"));

	return menu;
}
