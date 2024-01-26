#include "PluginAutomationModel.h"
#include "../misc/CoreActions.h"

PluginAutomationModel::PluginAutomationModel(
	quickAPI::PluginHolder plugin, PluginType type)
	: lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()),
	plugin(plugin), type(type) {}

int PluginAutomationModel::getNumRows() {
	return this->listTemp.size();
}

void PluginAutomationModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/,
	int /*width*/, int /*height*/, bool rowIsSelected) {
	juce::Colour backgroundColor = rowIsSelected
		? this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId + 1)
		: this->lookAndFeel.findColour(juce::ListBox::ColourIds::backgroundColourId);
	g.setColour(backgroundColor);
	g.fillAll();
}

void PluginAutomationModel::paintCell(juce::Graphics& g, int rowNumber, int columnId,
	int width, int height, bool rowIsSelected) {
	/** Data */
	juce::String text;
	{
		auto& [param, paramName, cc, ccName] = this->listTemp.getReference(rowNumber);
		switch (columnId) {
		case 1: {
			text += ("#" + juce::String{ param });
			if (paramName.isNotEmpty()) {
				text += (" " + paramName);
			}
			break;
		}
		case 2: {
			text += ("MIDI CC #" + juce::String{ cc });
			if (ccName.isNotEmpty()) {
				text += (" (" + ccName + ")");
			}
			break;
		}
		}
	}

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

void PluginAutomationModel::cellClicked(int rowNumber, int /*columnId*/,
	const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->showItemMenu(rowNumber);
	}
}

void PluginAutomationModel::backgroundClicked(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->showBackgroundMenu();
	}
}

void PluginAutomationModel::sortOrderChanged(int newSortColumnId, bool isForwards) {
	/** Set Prop */
	this->sortColumn = newSortColumnId;
	this->sortForwards = isForwards;

	/** Comparator */
	SortComparator comparator(newSortColumnId, isForwards);

	/** Sort Temp */
	this->listTemp.sort(comparator);
}

void PluginAutomationModel::deleteKeyPressed(int lastRowSelected) {
	this->removeItem(lastRowSelected);
}

void PluginAutomationModel::update() {
	/** Clear Current */
	this->listTemp.clear();
	
	/** Check Plugin Valid */
	if (!this->plugin) { return; }

	/** Get Temp List */
	switch (this->type) {
	case PluginType::Instr: {
		auto links = quickAPI::getInstrParamCCLink(this->plugin);
		for (auto& [param, cc] : links) {
			this->listTemp.add({ param,
				quickAPI::getInstrParamName(this->plugin, param),
				cc, quickAPI::getMIDICCChannelName(cc) });
		}
		break;
	}
	case PluginType::Effect: {
		auto links = quickAPI::getEffectParamCCLink(this->plugin);
		for (auto& [param, cc] : links) {
			this->listTemp.add({ param,
				quickAPI::getEffectParamName(this->plugin, param),
				cc, quickAPI::getMIDICCChannelName(cc) });
		}
		break;
	}
	}

	/** Comparator */
	SortComparator comparator(this->sortColumn, this->sortForwards);

	/** Sort Temp */
	this->listTemp.sort(comparator);
}

void PluginAutomationModel::addItem() {
	if (!this->plugin) { return; }

	switch (this->type) {
	case PluginType::Instr: {
		CoreActions::addInstrParamCCLinkGUI(this->plugin);
		break;
	}
	case PluginType::Effect: {
		CoreActions::addEffectParamCCLinkGUI(this->plugin);
		break;
	}
	}
}

void PluginAutomationModel::editItem(int itemIndex) {
	if (!this->plugin) { return; }
	auto& [param, paramName, cc, ccName] = this->listTemp.getReference(itemIndex);

	switch (this->type) {
	case PluginType::Instr: {
		CoreActions::editInstrParamCCLinkGUI(this->plugin, param, cc);
		break;
	}
	case PluginType::Effect: {
		CoreActions::editEffectParamCCLinkGUI(this->plugin, param, cc);
		break;
	}
	}
}

void PluginAutomationModel::removeItem(int itemIndex) {
	if (!this->plugin) { return; }
	auto& [param, paramName, cc, ccName] = this->listTemp.getReference(itemIndex);

	switch (this->type) {
	case PluginType::Instr: {
		CoreActions::removeInstrParamCCLink(this->plugin, cc);
		break;
	}
	case PluginType::Effect: {
		CoreActions::removeEffectParamCCLink(this->plugin, cc);
		break;
	}
	}
}

enum PluginAutoActionType {
	Add = 1, Edit, Remove
};

void PluginAutomationModel::showItemMenu(int itemIndex) {
	auto menu = this->createMenu(true);
	int result = menu.show();

	switch (result) {
	case PluginAutoActionType::Add: {
		this->addItem();
		break;
	}
	case PluginAutoActionType::Edit: {
		this->editItem(itemIndex);
		break;
	}
	case PluginAutoActionType::Remove: {
		this->removeItem(itemIndex);
		break;
	}
	}
}

void PluginAutomationModel::showBackgroundMenu() {
	auto menu = this->createMenu(false);
	int result = menu.show();

	switch (result) {
	case PluginAutoActionType::Add: {
		this->addItem();
		break;
	}
	}
}

juce::PopupMenu PluginAutomationModel::createMenu(bool isItem) {
	juce::PopupMenu menu;

	menu.addItem(PluginAutoActionType::Add, TRANS("Add"), true);
	menu.addSeparator();
	menu.addItem(PluginAutoActionType::Edit, TRANS("Edit"), isItem);
	menu.addItem(PluginAutoActionType::Remove, TRANS("Remove"), isItem);

	return menu;
}
