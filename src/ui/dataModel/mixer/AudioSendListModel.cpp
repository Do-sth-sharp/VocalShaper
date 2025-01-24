#include "AudioSendListModel.h"
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
	/** TODO */
}

juce::String AudioSendListModel::getNameForRow(int rowNumber) {
	/** Get Dst */
	auto dst = quickAPI::getTrackAudioSendDst(
		{ (quickAPI::TrackType)this->type, this->index }, rowNumber);
	if (dst.second < 0) { return ""; }

	/** Device */
	if (dst.first == quickAPI::SendDstType::ToDevice) {
		return quickAPI::getAudioDeviceName(false);
	}

	/** Master Track */
	if (dst.first == quickAPI::SendDstType::ToMaster) {
		return quickAPI::getTrackName(
			{ quickAPI::TrackType::MasterTrack, dst.second });
	}

	/** Aux Track */
	return quickAPI::getTrackName(
		{ quickAPI::TrackType::AuxTrack, dst.second });
}

void AudioSendListModel::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}
