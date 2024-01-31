#include "MixerTrackComponent.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

MixerTrackComponent::MixerTrackComponent() {}

void MixerTrackComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int colorHeight = screenSize.getHeight() * 0.015;
	int titleBorderWidth = screenSize.getWidth() * 0.00125;

	float outlineThickness = screenSize.getHeight() * 0.00125;

	float titleFontHeight = screenSize.getHeight() * 0.0125;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Font */
	juce::Font titleFont(titleFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Outline */
	auto totalRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(totalRect, outlineThickness);

	/** Track Color */
	auto colorRect = totalRect.withHeight(colorHeight);
	g.setColour(this->trackColor);
	g.fillRect(colorRect);

	/** Title */
	juce::String title = juce::String{ this->index };
	if (this->name.isNotEmpty()) {
		title += (" - " + this->name);
	}
	g.setColour(this->nameColor);
	g.setFont(titleFont);
	g.drawFittedText(title,
		colorRect.withTrimmedLeft(titleBorderWidth).withTrimmedRight(titleBorderWidth),
		juce::Justification::centred, 1, 1.f);
}

void MixerTrackComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->name = quickAPI::getMixerTrackName(index);
		this->trackColor = quickAPI::getMixerTrackColor(index);

		auto& laf = this->getLookAndFeel();
		if (utils::isLightColor(this->trackColor)) {
			this->nameColor = laf.findColour(
				juce::Label::ColourIds::textWhenEditingColourId);
		}
		else {
			this->nameColor = laf.findColour(
				juce::Label::ColourIds::textColourId);
		}
	}
}
