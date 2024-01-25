#include "PluginAutomationModel.h"

PluginAutomationModel::PluginAutomationModel(
	quickAPI::PluginHolder plugin, PluginType type)
	: plugin(plugin), type(type) {}

int PluginAutomationModel::getNumRows() {
	return this->listTemp.size();
}

void PluginAutomationModel::paintRowBackground(juce::Graphics&, int rowNumber,
	int width, int height, bool rowIsSelected) {
	/** TODO */
}

void PluginAutomationModel::paintCell(juce::Graphics&, int rowNumber, int columnId,
	int width, int height, bool rowIsSelected) {
	/** TODO */
}

void PluginAutomationModel::cellClicked(int rowNumber, int columnId,
	const juce::MouseEvent&) {
	/** TODO */
}

void PluginAutomationModel::backgroundClicked(const juce::MouseEvent&) {
	/** TODO */
}

void PluginAutomationModel::sortOrderChanged(int newSortColumnId, bool isForwards) {
	/** TODO */
}

void PluginAutomationModel::deleteKeyPressed(int lastRowSelected) {
	/** TODO */
}

void PluginAutomationModel::update() {
	this->listTemp.clear();
	/** TODO Create Temp */
}
