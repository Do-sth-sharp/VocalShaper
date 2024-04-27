#include "SeqTrackMuteComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTrackMuteComponent::SeqTrackMuteComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMuteButton());
}

void SeqTrackMuteComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;
	float textFontHeight = screenSize.getHeight() * 0.012;

	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;

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

void SeqTrackMuteComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void SeqTrackMuteComponent::mouseMove(const juce::MouseEvent& event) {
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

void SeqTrackMuteComponent::mouseUp(const juce::MouseEvent& event) {
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
			this->changeMute();
		}
		else if (event.mods.isRightButtonDown()) {
			this->showMenu();
		}
	}
}

void SeqTrackMuteComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->mute = quickAPI::getSeqTrackMute(index);

		this->repaint();
	}
}

void SeqTrackMuteComponent::changeMute() {
	CoreActions::setSeqMute(this->index, !(this->mute));
}

enum MixerMuteActionType {
	Mute = 1, Solo, MuteAll, UnmuteAll
};

void SeqTrackMuteComponent::showMenu() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case MixerMuteActionType::Mute:
		this->changeMute();
		break;
	case MixerMuteActionType::Solo:
		CoreActions::setSeqSolo(this->index);
		break;
	case MixerMuteActionType::MuteAll:
		CoreActions::setSeqMuteAll(true);
		break;
	case MixerMuteActionType::UnmuteAll:
		CoreActions::setSeqMuteAll(false);
		break;
	}
}

juce::PopupMenu SeqTrackMuteComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(MixerMuteActionType::Mute, TRANS("Mute"), true, this->mute);
	menu.addItem(MixerMuteActionType::Solo, TRANS("Solo"));
	menu.addSeparator();
	menu.addItem(MixerMuteActionType::MuteAll, TRANS("Mute All"));
	menu.addItem(MixerMuteActionType::UnmuteAll, TRANS("Unmute All"));

	return menu;
}

