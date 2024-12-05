#include "SeqTrackSoloComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SeqTrackSoloComponent::SeqTrackSoloComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::SoloButton));
}

void SeqTrackSoloComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;

	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;

	float textFontHeight = buttonWidth * 0.7;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->solo
		? juce::TextButton::ColourIds::buttonOnColourId
		: juce::TextButton::ColourIds::buttonColourId);
	juce::Colour textColor = laf.findColour(this->solo
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
	g.drawFittedText("S", buttonRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void SeqTrackSoloComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void SeqTrackSoloComponent::mouseMove(const juce::MouseEvent& event) {
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

void SeqTrackSoloComponent::mouseUp(const juce::MouseEvent& event) {
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
			this->changeSolo();
		}
		else if (event.mods.isRightButtonDown()) {
			this->changeSolo();
		}
	}
}

void SeqTrackSoloComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->solo = quickAPI::getSeqTrackSolo(index);

		this->repaint();
	}
}

void SeqTrackSoloComponent::changeSolo() {
	CoreActions::setSeqSolo(this->index, !(this->solo));
}
