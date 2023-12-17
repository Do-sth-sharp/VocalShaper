#include "MetaDataModel.h"

MetaDataModel::MetaDataModel(const juce::StringPairArray& data)
	: data(data) {}

int MetaDataModel::getNumRows() {
	return this->data.size();
}

void MetaDataModel::paintRowBackground(juce::Graphics& g, int rowNumber,
	int width, int height, bool rowIsSelected) {
	/** TODO */
}

void MetaDataModel::paintCell(juce::Graphics&, int rowNumber, int columnId,
	int width, int height, bool rowIsSelected) {
	/** TODO */
}

void MetaDataModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event) {
	/** TODO */
}

void MetaDataModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) {
	/** TODO */
}

void MetaDataModel::backgroundClicked(const juce::MouseEvent& event) {
	/** TODO */
}

int MetaDataModel::getColumnAutoSizeWidth(int columnId) {
	/** TODO */
	return 0;
}

void MetaDataModel::deleteKeyPressed(int lastRowSelected) {
	/** TODO */
}
