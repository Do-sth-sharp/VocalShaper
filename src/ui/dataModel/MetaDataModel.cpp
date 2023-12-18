#include "MetaDataModel.h"

MetaDataModel::MetaDataModel(
	const juce::StringPairArray& data,
	const AddCallback& addCallback,
	const EditCallback& editCallback,
	const RemoveCallback& removeCallback)
	: data(data), lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()),
	addCallback(addCallback), editCallback(editCallback), removeCallback(removeCallback) {}

int MetaDataModel::getNumRows() {
	return this->data.size();
}

void MetaDataModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/,
	int /*width*/, int /*height*/, bool rowIsSelected) {
	juce::Colour backgroundColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId);
	g.setColour(backgroundColor);
	g.fillAll();
}

void MetaDataModel::paintCell(juce::Graphics& g, int rowNumber, int columnId,
	int width, int height, bool rowIsSelected) {
	/** Data */
	juce::String text = (columnId==1)
		? this->data.getAllKeys()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1]
		: this->data.getAllValues()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1];

	/** Color */
	juce::Colour textColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::textColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::textColourId);

	/** Font */
	juce::Font font(height * 0.6);

	/** Text */
	juce::Rectangle<int> textRect(
		0.5 * height, 0, width - height, height);
	g.setColour(textColor);
	g.setFont(font);
	g.drawFittedText(text, textRect, juce::Justification::centredLeft, 1, 1.f);
}

void MetaDataModel::cellClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent& event) {
	if (!event.mods.isRightButtonDown()) { return; }

	auto menu = this->createMenu(true);
	int result = menu.show();

	switch (result) {
	case 1:
		this->addCallback();
		break;
	case 2:
		this->editCallback(
			this->data.getAllKeys()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1],
			this->data.getAllValues()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1]);
		break;
	case 3:
		this->removeCallback(
			this->data.getAllKeys()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1]);
		break;
	}
}

void MetaDataModel::cellDoubleClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent& event) {
	if (!event.mods.isLeftButtonDown()) { return; }

	this->editCallback(
		this->data.getAllKeys()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1],
		this->data.getAllValues()[this->sortOrder ? rowNumber : this->data.size() - rowNumber - 1]);
}

void MetaDataModel::backgroundClicked(const juce::MouseEvent& event) {
	if (!event.mods.isRightButtonDown()) { return; }
	
	auto menu = this->createMenu(false);
	int result = menu.show();

	switch (result) {
	case 1:
		this->addCallback();
		break;
	}
}

void MetaDataModel::sortOrderChanged(int newSortColumnId, bool isForwards) {
	if (newSortColumnId != 1) { return; }
	this->sortOrder = isForwards;
}

void MetaDataModel::deleteKeyPressed(int lastRowSelected) {
	this->removeCallback(
		this->data.getAllKeys()[this->sortOrder ? lastRowSelected : this->data.size() - lastRowSelected - 1]);
}

void MetaDataModel::set(const juce::String& key, const juce::String& value) {
	this->data.set(key, value);
}

void MetaDataModel::remove(const juce::String& key) {
	this->data.remove(juce::StringRef{ key });
}

const juce::StringPairArray MetaDataModel::getData() const {
	return this->data;
}

juce::PopupMenu MetaDataModel::createMenu(bool onRow) const{
	juce::PopupMenu menu;

	menu.addItem(1, TRANS("Add"), true, false);
	menu.addSeparator();
	menu.addItem(2, TRANS("Edit"), onRow, false);
	menu.addItem(3, TRANS("Remove"), onRow, false);

	return menu;
}
