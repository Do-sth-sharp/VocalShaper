#include "LicenseListModel.h"

LicenseListModel::LicenseListModel()
	: lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()) {}

int LicenseListModel::getNumRows() {
	return this->fileList.size();
}

void LicenseListModel::paintListBoxItem(
	int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	if (rowNumber < 0 || rowNumber >= this->fileList.size()) { return; }

	/** Data */
	juce::String text = this->getNameForRow(rowNumber);

	/** Color */
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

juce::String LicenseListModel::getNameForRow(int rowNumber) {
	return this->fileList[rowNumber].getFileNameWithoutExtension();
}

void LicenseListModel::selectedRowsChanged(int lastRowSelected) {
	if (!this->callback) { return; }
	this->callback(this->fileList[lastRowSelected]);
}

juce::String LicenseListModel::getTooltipForRow(int row) {
	return this->fileList[row].getFullPathName();
}

void LicenseListModel::setList(const juce::Array<juce::File>& list) {
	this->fileList = list;
}

void LicenseListModel::setCallback(const RowCallback& callback) {
	this->callback = callback;
}
