#include "MIDISendListModel.h"
#include "../../component/mixer/MIDISendComponent.h"
#include "../../../audioCore/AC_API.h"

int MIDISendListModel::getNumRows() {
	return quickAPI::getTrackMIDISendSlotNum();
}

void MIDISendListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** Nothing To Do */
}

juce::Component* MIDISendListModel::refreshComponentForRow(int rowNumber, bool isRowSelected,
	juce::Component* existingComponentToUpdate) {
	if (rowNumber >= this->getNumRows()) { return existingComponentToUpdate; }

	MIDISendComponent* comp = nullptr;
	if (existingComponentToUpdate) {
		comp = dynamic_cast<MIDISendComponent*>(existingComponentToUpdate);
		if (!comp) { delete existingComponentToUpdate; }
	}
	if (!comp) { comp = new MIDISendComponent; }

	comp->update(this->type, this->index, rowNumber);

	return comp;
}

juce::String MIDISendListModel::getNameForRow(int rowNumber) {
	return quickAPI::getTrackMIDISendDstShortName(
		{ (quickAPI::TrackType)this->type, this->index }, rowNumber);
}

void MIDISendListModel::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}
