#include "EffectListModel.h"
#include "../../component/mixer/EffectComponent.h"
#include "../../../audioCore/AC_API.h"

int EffectListModel::getNumRows() {
	return quickAPI::getEffectSlotNum();
}

void EffectListModel::paintListBoxItem(
	int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** Nothing To Do */
}
juce::Component* EffectListModel::refreshComponentForRow(int rowNumber, bool isRowSelected,
	juce::Component* existingComponentToUpdate) {
	if (rowNumber >= this->getNumRows()) { return existingComponentToUpdate; }

	EffectComponent* comp = nullptr;
	if (existingComponentToUpdate) {
		comp = dynamic_cast<EffectComponent*>(existingComponentToUpdate);
		if (!comp) { delete existingComponentToUpdate; }
	}
	if (!comp) { comp = new EffectComponent; }

	comp->update(this->type, this->index, rowNumber);

	return comp;
}

juce::String EffectListModel::getNameForRow(int rowNumber) {
	return quickAPI::getEffectName({ (quickAPI::TrackType)this->type, this->index }, rowNumber);
}

void EffectListModel::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}
