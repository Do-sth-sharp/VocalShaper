#include "MixerTrackMuteComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

MixerTrackMuteComponent::MixerTrackMuteComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::MuteButton));
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void MixerTrackMuteComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float lineThickness = screenSize.getHeight() * 0.001;
	float textFontHeight = this->getHeight() * 0.7;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->mute
		? juce::TextButton::ColourIds::buttonOnColourId
		: juce::TextButton::ColourIds::buttonColourId);
	juce::Colour textColor = laf.findColour(this->mute
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
	g.drawFittedText("M", buttonRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void MixerTrackMuteComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->changeMute();
	}
	else if (event.mods.isRightButtonDown()) {
		this->changeMute();
	}
}

void MixerTrackMuteComponent::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}

void MixerTrackMuteComponent::update() {
	this->mute = quickAPI::getTrackEquivalentMute(
		{ (quickAPI::TrackType)this->type, this->index });

	this->repaint();
}

void MixerTrackMuteComponent::changeMute() {
	bool mute = quickAPI::getTrackMute(
		{ (quickAPI::TrackType)this->type, this->index });
	CoreActions::setTrackMute(
		(quickAPI::TrackType)this->type, this->index, !mute);
}
