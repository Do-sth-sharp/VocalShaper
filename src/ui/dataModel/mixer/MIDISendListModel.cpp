#include "MIDISendListModel.h"
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
	/** TODO */
}

juce::String MIDISendListModel::getNameForRow(int rowNumber) {
	/** Get Dst */
	auto dst = quickAPI::getTrackMIDISendDst(
		{ (quickAPI::TrackType)this->type, this->index }, rowNumber);
	if (dst.second < 0) { return ""; }

	/** Device */
	if (dst.first == quickAPI::SendDstType::ToDevice) {
		return quickAPI::getMIDIOutputDeviceName();
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

void MIDISendListModel::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;
}
