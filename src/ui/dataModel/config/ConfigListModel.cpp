#include "ConfigListModel.h"

ConfigListModel::ConfigListModel()
	: lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()),
	list(juce::StringArray{ TRANS("Startup Config"), TRANS("Function Config"),
		TRANS("Audio Config"), TRANS("Output Config"),
		TRANS("Plugin Config"), TRANS("Key Mapping Config") }) {}

int ConfigListModel::getNumRows() {
	return this->list.size();
}

void ConfigListModel::paintListBoxItem(
	int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	if (rowNumber < 0 || rowNumber >= this->list.size()) { return; }

	/** Data */
	juce::String text = this->getNameForRow(rowNumber);

	/** Size */
	juce::Colour backgroundColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId);
	juce::Colour textColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::textColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::textColourId);

	/** Font */
	juce::Font font(juce::FontOptions{ height * 0.5f });

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

juce::String ConfigListModel::getNameForRow(int rowNumber) {
	return this->list[rowNumber];
}

void ConfigListModel::selectedRowsChanged(int lastRowSelected) {
	if (lastRowSelected < 0 || lastRowSelected >= this->list.size()) { return; }
	if (!this->callback) { return; }
	this->callback(lastRowSelected);
}

juce::MouseCursor ConfigListModel::getMouseCursorForRow(int row) {
	return juce::MouseCursor::PointingHandCursor;
}

void ConfigListModel::setCallback(const RowCallback& callback) {
	this->callback = callback;
}
