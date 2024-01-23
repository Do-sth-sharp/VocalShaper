#include "InstrListModel.h"
#include "../../audioCore/AC_API.h"

int InstrListModel::getNumRows() {
	return quickAPI::getInstrNum();
}

void InstrListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** Nothing To Do */
	g.setColour(juce::Colours::white);
	g.setFont(juce::Font{ (float)height });
	juce::Rectangle<int> rect(0, 0, width, height);
	juce::String str = "[" + juce::String{ rowNumber } + "] " + this->getNameForRow(rowNumber);
	g.drawFittedText(str, rect,
		juce::Justification::centredLeft, 1, 1.f);
}

juce::Component* InstrListModel::refreshComponentForRow(int rowNumber, bool isRowSelected,
	juce::Component* existingComponentToUpdate) {
	/** TODO */
	return existingComponentToUpdate;
}

juce::String InstrListModel::getNameForRow(int rowNumber) {
	return quickAPI::getInstrName(rowNumber);
}

void InstrListModel::backgroundClicked(const juce::MouseEvent&) {
	/** TODO */
}
