#include "MixerTrackSoloComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

MixerTrackSoloComponent::MixerTrackSoloComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::SoloButton));
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void MixerTrackSoloComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float lineThickness = screenSize.getHeight() * 0.001;
	float textFontHeight = this->getHeight() * 0.7;

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
	juce::Rectangle<float> buttonRect = this->getLocalBounds().toFloat();
	g.setColour(backgroundColor);
	g.fillRect(buttonRect);

	g.setColour(textColor);
	g.drawRect(buttonRect, lineThickness);

	g.setFont(textFont);
	g.drawFittedText("S", buttonRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void MixerTrackSoloComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->changeSolo();
	}
	else if (event.mods.isRightButtonDown()) {
		this->changeSolo();
	}
}

void MixerTrackSoloComponent::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}

void MixerTrackSoloComponent::update() {
	this->solo = quickAPI::getTrackSolo(
		{ (quickAPI::TrackType)this->type, this->index });

	this->repaint();
}

void MixerTrackSoloComponent::changeSolo() {
	bool solo = quickAPI::getTrackSolo(
		{ (quickAPI::TrackType)this->type, this->index });
	CoreActions::setTrackSolo(
		(quickAPI::TrackType)this->type, this->index, !solo);
}
