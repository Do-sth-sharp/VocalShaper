#include "SourceListModel.h"
#include "../misc/DragSourceType.h"
#include "../../audioCore/AC_API.h"

SourceListModel::SourceListModel(const juce::Component* parent)
	: parent(parent) {}

int SourceListModel::getNumRows() {
	return quickAPI::getSourceNum();
}

void SourceListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** TODO */
}

juce::Component* SourceListModel::refreshComponentForRow(
	int rowNumber, bool isRowSelected,
	juce::Component* existingComponentToUpdate) {
	/** TODO */
	return existingComponentToUpdate;
}

juce::String SourceListModel::getNameForRow(int rowNumber) {
	return quickAPI::getSourceName(rowNumber);
}

void SourceListModel::listBoxItemClicked(int row, const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->showItemMenu(row);
	}
}

void SourceListModel::backgroundClicked(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->showBackgroundMenu();
	}
}

void SourceListModel::deleteKeyPressed(int lastRowSelected) {
	this->deleteItem(lastRowSelected);
}

juce::var SourceListModel::getDragSourceDescription(
	const juce::SparseSet<int>& rowsToDescribe) {
	/** Get First Row */
	auto range = rowsToDescribe.getRange(0);
	int index = range.getStart();

	/** Var */
	auto object = std::make_unique<juce::DynamicObject>();

	object->setProperty("type", (int)DragSourceType::Source);
	object->setProperty("name", quickAPI::getSourceName(index));
	object->setProperty("srcType", quickAPI::getSourceType(index));
	object->setProperty("length", quickAPI::getSourceLength(index));

	return juce::var{ object.release() };
}

bool SourceListModel::mayDragToExternalWindows() const {
	return true;
}

juce::String SourceListModel::getTooltipForRow(int row) {
	auto type = quickAPI::getSourceType(row);

	juce::String result = 
		"#" + juce::String{ quickAPI::getSourceId(row) } + " " + quickAPI::getSourceName(row) + "\n"
		+ TRANS("Type:") + " " + quickAPI::getSourceTypeName(row) + "\n"
		+ TRANS("Length:") + " " + juce::String{ quickAPI::getSourceLength(row) } + "s\n";

	if (type == quickAPI::SourceType::AudioSource || type == quickAPI::SourceType::SynthSource) {
		result += (TRANS("Channels:") + " " + juce::String{ quickAPI::getSourceChannelNum(row) } + "\n");
	}
	if (type == quickAPI::SourceType::MIDISource || type == quickAPI::SourceType::SynthSource) {
		result += (TRANS("Tracks:") + " " + juce::String{ quickAPI::getSourceTrackNum(row) } + "\n");
	}
	if (type == quickAPI::SourceType::MIDISource || type == quickAPI::SourceType::SynthSource) {
		result += (TRANS("Events:") + " " + juce::String{ quickAPI::getSourceEventNum(row) } + "\n");
	}
	if (type == quickAPI::SourceType::SynthSource) {
		result += (TRANS("Synthesizer:") + " " + juce::String{ quickAPI::getSourceSynthesizerName(row) } + "\n");
	}
	if (type == quickAPI::SourceType::AudioSource || type == quickAPI::SourceType::SynthSource) {
		result += (TRANS("Sample Rate:") + " " + juce::String{ quickAPI::getSourceSampleRate(row) } + "\n");
	}

	return result;
}

juce::MouseCursor SourceListModel::getMouseCursorForRow(int row) {
	return juce::MouseCursor::PointingHandCursor;
}

void SourceListModel::showItemMenu(int index) {
	/** TODO */
}

void SourceListModel::showBackgroundMenu() {
	/** TODO */
}

void SourceListModel::deleteItem(int index) {
	/** TODO */
}

juce::PopupMenu SourceListModel::createItemMenu(int index) const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("New"), this->createNewMenu());
	menu.addItem(1, TRANS("Clone"));
	menu.addItem(2, TRANS("Load"));
	menu.addItem(3, TRANS("Save"));
	menu.addItem(4, TRANS("Export"),
		quickAPI::getSourceType(index) == quickAPI::SourceType::SynthSource);
	menu.addItem(5, TRANS("Remove"));
	menu.addItem(6, TRANS("Replace"));
	menu.addSeparator();
	menu.addItem(7, TRANS("Set Synthesizer"),
		quickAPI::getSourceType(index) == quickAPI::SourceType::SynthSource);
	menu.addItem(8, TRANS("Synth"),
		quickAPI::getSourceType(index) == quickAPI::SourceType::SynthSource);

	return menu;
}

juce::PopupMenu SourceListModel::createBackgroundMenu() const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("New"), this->createNewMenu());
	menu.addItem(1, TRANS("Load"));

	return menu;
}

juce::PopupMenu SourceListModel::createNewMenu() const {
	juce::PopupMenu menu;

	menu.addItem(0x1001, TRANS("Audio"));
	menu.addItem(0x1002, TRANS("MIDI"));
	menu.addItem(0x1003, TRANS("Synth"));

	return menu;
}
