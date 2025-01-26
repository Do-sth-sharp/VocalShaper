#include "AudioSendListModel.h"
#include "../../component/mixer/AudioSendComponent.h"
#include "../../../audioCore/AC_API.h"

int AudioSendListModel::getNumRows() {
	return quickAPI::getTrackAudioSendSlotNum();
}

void AudioSendListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** Nothing To Do */
}

juce::Component* AudioSendListModel::refreshComponentForRow(int rowNumber, bool isRowSelected,
	juce::Component* existingComponentToUpdate) {
	if (rowNumber >= this->getNumRows()) { return existingComponentToUpdate; }

	AudioSendComponent* comp = nullptr;
	if (existingComponentToUpdate) {
		comp = dynamic_cast<AudioSendComponent*>(existingComponentToUpdate);
		if (!comp) { delete existingComponentToUpdate; }
	}
	if (!comp) { comp = new AudioSendComponent; }

	comp->update(this->type, this->index, rowNumber);

	return comp;
}

juce::String AudioSendListModel::getNameForRow(int rowNumber) {
	return quickAPI::getTrackAudioSendDstName(
		{ (quickAPI::TrackType)this->type, this->index }, rowNumber);
}

void AudioSendListModel::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}
