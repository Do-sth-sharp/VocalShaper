#include "SourceListModel.h"
#include "../component/SourceComponent.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
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
	SourceComponent* comp = nullptr;
	if (existingComponentToUpdate) {
		comp = dynamic_cast<SourceComponent*>(existingComponentToUpdate);
		if (!comp) { delete existingComponentToUpdate; }
	}
	if (!comp) { comp = new SourceComponent; }

	comp->update(rowNumber, isRowSelected);

	return comp;
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

enum SourceListActionType {
	NewAudio = 1,
	NewMIDI,
	NewSynth,
	Clone,
	Load,
	Save,
	Export,
	Remove,
	Replace,
	SetName,
	SetSynthesizer,
	Synth
};

void SourceListModel::showItemMenu(int index) {
	auto menu = this->createItemMenu(index);
	auto result = (SourceListActionType)(menu.show());

	switch (result) {
	case SourceListActionType::NewAudio:
		CoreActions::newAudioSourceGUI();
		break;
	case SourceListActionType::NewMIDI:
		CoreActions::newMIDISourceGUI();
		break;
	case SourceListActionType::NewSynth:
		CoreActions::newSynthSourceGUI();
		break;
	case SourceListActionType::Clone:
		CoreActions::cloneSource(index);
		break;
	case SourceListActionType::Load:
		CoreActions::loadSourceGUI();
		break;
	case SourceListActionType::Save:
		CoreActions::saveSourceGUI(index);
		break;
	case SourceListActionType::Export:
		CoreActions::exportSourceGUI(index);
		break;
	case SourceListActionType::Remove:
		CoreActions::removeSourceGUI(index);
		break;
	case SourceListActionType::Replace:
		CoreActions::reloadSourceGUI(index);
		break;
	case SourceListActionType::SetName:
		CoreActions::setSourceNameGUI(index);
		break;
	case SourceListActionType::SetSynthesizer:
		CoreActions::setSourceSynthesizerGUI(index);
		break;
	case SourceListActionType::Synth:
		CoreActions::synthSourceGUI(index);
		break;
	}
}

void SourceListModel::showBackgroundMenu() {
	auto menu = this->createBackgroundMenu();
	auto result = (SourceListActionType)(menu.show());

	switch (result) {
	case SourceListActionType::NewAudio:
		CoreActions::newAudioSourceGUI();
		break;
	case SourceListActionType::NewMIDI:
		CoreActions::newMIDISourceGUI();
		break;
	case SourceListActionType::NewSynth:
		CoreActions::newSynthSourceGUI();
		break;
	case SourceListActionType::Load:
		CoreActions::loadSourceGUI();
		break;
	}
}

void SourceListModel::deleteItem(int index) {
	CoreActions::removeSourceGUI(index);
}

juce::PopupMenu SourceListModel::createItemMenu(int index) const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("New"), this->createNewMenu());
	menu.addItem(SourceListActionType::Clone, TRANS("Clone"));
	menu.addItem(SourceListActionType::Load, TRANS("Load"));
	menu.addItem(SourceListActionType::Save, TRANS("Save"));
	menu.addItem(SourceListActionType::Export, TRANS("Export"),
		quickAPI::getSourceType(index) == quickAPI::SourceType::SynthSource);
	menu.addItem(SourceListActionType::Remove, TRANS("Remove"));
	menu.addItem(SourceListActionType::Replace, TRANS("Replace"));
	menu.addSeparator();
	menu.addItem(SourceListActionType::SetName, TRANS("Set Name"));
	menu.addItem(SourceListActionType::SetSynthesizer, TRANS("Set Synthesizer"),
		quickAPI::getSourceType(index) == quickAPI::SourceType::SynthSource);
	menu.addItem(SourceListActionType::Synth, TRANS("Synth"),
		quickAPI::getSourceType(index) == quickAPI::SourceType::SynthSource);

	return menu;
}

juce::PopupMenu SourceListModel::createBackgroundMenu() const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("New"), this->createNewMenu());
	menu.addItem(SourceListActionType::Load, TRANS("Load"));

	return menu;
}

juce::PopupMenu SourceListModel::createNewMenu() const {
	juce::PopupMenu menu;

	menu.addItem(SourceListActionType::NewAudio, TRANS("Audio"));
	menu.addItem(SourceListActionType::NewMIDI, TRANS("MIDI"));
	menu.addItem(SourceListActionType::NewSynth, TRANS("Synth"));

	return menu;
}
