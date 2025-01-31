#include "TrackListBoxModel.h"
#include "../../../audioCore/AC_API.h"

TrackListBoxModel::TrackListBoxModel() 
	: lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()) {}

int TrackListBoxModel::getNumRows() {
	return this->trackItemList.size();
}

void TrackListBoxModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	if (rowNumber < 0 || rowNumber >= this->trackItemList.size()) { return; }

	/** Data */
	auto& [type, index, name, bus] = this->trackItemList.getReference(rowNumber);
	juce::String prefix;
	if (type == (int)quickAPI::TrackType::MasterTrack) {
		prefix = "M";
	}
	else if (type == (int)quickAPI::TrackType::AuxTrack) {
		prefix = "A";
	}
	juce::String text = "[" + prefix + juce::String{ index } + "] "
		+ name + " - " + bus;

	/** Size */
	juce::Colour backgroundColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId);
	juce::Colour textColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::textColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::textColourId);

	/** Font */
	juce::Font font(juce::FontOptions{ height * 0.8f });

	/** Background */
	g.setColour(backgroundColor);
	g.fillRect(0, 0, width, height);

	/** Text */
	juce::Rectangle<int> textRect(
		0.5 * height, 0, width - height, height);
	g.setColour(textColor);
	g.setFont(font);
	g.drawFittedText(text, textRect,
		juce::Justification::centredLeft, 1, 1.f);
}

void TrackListBoxModel::setItems(
	const juce::Array<TrackListBoxModel::TrackInfo>& items) {
	this->trackItemList = items;
}
