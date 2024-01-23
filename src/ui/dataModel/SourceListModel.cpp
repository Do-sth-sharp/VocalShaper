#include "SourceListModel.h"
#include "../component/SourceComponent.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../../audioCore/AC_API.h"

SourceListModel::SourceListModel(
	const std::function<void(int)>& selectCallback)
	: selectCallback(selectCallback) {}

int SourceListModel::getNumRows() {
	return quickAPI::getSourceNum();
}

void SourceListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** Nothing To Do */
}

juce::Component* SourceListModel::refreshComponentForRow(
	int rowNumber, bool isRowSelected,
	juce::Component* existingComponentToUpdate) {
	if (rowNumber >= this->getNumRows()) { return existingComponentToUpdate; }

	SourceComponent* comp = nullptr;
	if (existingComponentToUpdate) {
		comp = dynamic_cast<SourceComponent*>(existingComponentToUpdate);
		if (!comp) { delete existingComponentToUpdate; }
	}
	if (!comp) { comp = new SourceComponent(this->selectCallback); }

	comp->update(rowNumber, isRowSelected);

	return comp;
}

juce::String SourceListModel::getNameForRow(int rowNumber) {
	return quickAPI::getSourceName(rowNumber);
}

void SourceListModel::backgroundClicked(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->showBackgroundMenu();
	}
}

enum SourceListActionType {
	NewAudio = 1,
	NewMIDI,
	Load
};

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
	case SourceListActionType::Load:
		CoreActions::loadSourceGUI();
		break;
	}
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

	return menu;
}
