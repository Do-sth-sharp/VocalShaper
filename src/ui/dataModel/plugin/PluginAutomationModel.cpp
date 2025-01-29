#include "PluginAutomationModel.h"
#include "../../misc/CoreActions.h"
#include "../../../audioCore/AC_API.h"

PluginAutomationModel::PluginAutomationModel(PluginType type)
	: lookAndFeel(juce::LookAndFeel::getDefaultLookAndFeel()), type(type) {}

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
	juce::Font font(juce::FontOptions{ height * 0.6f });

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

void PluginAutomationModel::update(int type, int track, int index) {
	this->trackType = type;
	this->track = track;
	this->index = index;

	/** Clear Current */
	this->listTemp.clear();

	/** Get Temp List */
	switch (this->type) {
	case PluginType::Instr: {
		auto links = quickAPI::getInstrParamCCLinks(track);
		for (auto& [param, cc] : links) {
			this->listTemp.add({ param,
				quickAPI::getInstrParamName(track, param),
				cc, quickAPI::getMIDICCChannelName(cc) });
		}
		break;
	}
	case PluginType::Effect: {
		auto links = quickAPI::getEffectParamCCLinks({ (quickAPI::TrackType)type, track }, index);
		for (auto& [param, cc] : links) {
			this->listTemp.add({ param,
				quickAPI::getEffectParamName({ (quickAPI::TrackType)type, track }, index, param),
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
	switch (this->type) {
	case PluginType::Instr: {
		CoreActions::addInstrParamCCLinkGUI(this->track);
		break;
	}
	case PluginType::Effect: {
		CoreActions::addEffectParamCCLinkGUI(
			(quickAPI::TrackType)this->trackType, this->track, this->index);
		break;
	}
	}
}

void PluginAutomationModel::editItem(int itemIndex) {
	auto& [param, paramName, cc, ccName] = this->listTemp.getReference(itemIndex);

	switch (this->type) {
	case PluginType::Instr: {
		CoreActions::editInstrParamCCLinkGUI(this->track, param, cc);
		break;
	}
	case PluginType::Effect: {
		CoreActions::editEffectParamCCLinkGUI(
			(quickAPI::TrackType)this->trackType, this->track, this->index, param, cc);
		break;
	}
	}
}

void PluginAutomationModel::removeItem(int itemIndex) {
	auto& [param, paramName, cc, ccName] = this->listTemp.getReference(itemIndex);

	switch (this->type) {
	case PluginType::Instr: {
		CoreActions::removeInstrParamCCLink(this->track, cc);
		break;
	}
	case PluginType::Effect: {
		CoreActions::removeEffectParamCCLink(
			(quickAPI::TrackType)this->trackType, this->track, this->index, cc);
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
