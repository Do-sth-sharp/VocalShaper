#include "EditableStringListModel.h"

EditableStringListModel::EditableStringListModel(
	const juce::StringArray& list,
	const AddCallback& addCallback,
	const EditCallback& editCallback,
	const RemoveCallback& removeCallback,
	MenuType menuType)
	: lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()),
	list(list), addCallback(addCallback), editCallback(editCallback), removeCallback(removeCallback),
	menuType(menuType) {}

int EditableStringListModel::getNumRows() {
	return this->list.size();
}

void EditableStringListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
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
	juce::Font font(height * 0.8);

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

juce::String EditableStringListModel::getNameForRow(int rowNumber) {
	return this->list[rowNumber];
}

void EditableStringListModel::listBoxItemClicked(int row, const juce::MouseEvent& event) {
	if (!event.mods.isRightButtonDown()) { return; }

	auto menu = this->createMenu(true);
	int result = menu.show();

	switch (result) {
	case 1:
		this->addCallback(row);
		break;
	case 2:
		this->editCallback(
			row, this->getNameForRow(row));
		break;
	case 3:
		this->removeCallback(
			row, this->getNameForRow(row));
		break;
	}
}

void EditableStringListModel::listBoxItemDoubleClicked(int row, const juce::MouseEvent& event) {
	if (!event.mods.isLeftButtonDown()) { return; }

	this->editCallback(
		row, this->getNameForRow(row));
}

void EditableStringListModel::backgroundClicked(const juce::MouseEvent& event) {
	if (!event.mods.isRightButtonDown()) { return; }

	auto menu = this->createMenu(false);
	int result = menu.show();

	switch (result) {
	case 1:
		this->addCallback(this->getNumRows());
		break;
	}
}

void EditableStringListModel::deleteKeyPressed(int lastRowSelected) {
	this->removeCallback(
		lastRowSelected, this->getNameForRow(lastRowSelected));
}

juce::MouseCursor EditableStringListModel::getMouseCursorForRow(int row) {
	return juce::MouseCursor::PointingHandCursor;
}

void EditableStringListModel::insert(int index, const juce::String& value) {
	this->list.insert(index, value);
}

void EditableStringListModel::set(int index, const juce::String& value) {
	this->list.set(index, value);
}

void EditableStringListModel::remove(int index) {
	this->list.remove(index);
}

const juce::StringArray EditableStringListModel::getData() const {
	return this->list;
}

bool EditableStringListModel::contains(const juce::String& value) const {
	return this->list.contains(value);
}

juce::PopupMenu EditableStringListModel::createMenu(bool onRow) const {
	juce::PopupMenu menu;

	auto testFlag = [](MenuType value, MenuType flag)->bool {
		return (value & flag) == flag;
		};

	menu.addItem(1, TRANS("Add"),
		true && testFlag(this->menuType, MenuType::MenuAddItem), false);
	menu.addSeparator();
	menu.addItem(2, TRANS("Edit"),
		onRow && testFlag(this->menuType, MenuType::MenuEditItem), false);
	menu.addItem(3, TRANS("Remove"),
		onRow && testFlag(this->menuType, MenuType::MenuRemoveItem), false);

	return menu;
}
