#include "InstrListModel.h"
#include "../component/InstrComponent.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

int InstrListModel::getNumRows() {
	return quickAPI::getInstrNum();
}

void InstrListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
	int width, int height, bool rowIsSelected) {
	/** Nothing To Do */
}

juce::Component* InstrListModel::refreshComponentForRow(int rowNumber, bool /*isRowSelected*/,
	juce::Component* existingComponentToUpdate) {
	if (rowNumber >= this->getNumRows()) { return existingComponentToUpdate; }

	InstrComponent* comp = nullptr;
	if (existingComponentToUpdate) {
		comp = dynamic_cast<InstrComponent*>(existingComponentToUpdate);
		if (!comp) { delete existingComponentToUpdate; }
	}
	if (!comp) { comp = new InstrComponent; }

	comp->update(rowNumber);

	return comp;
}

juce::String InstrListModel::getNameForRow(int rowNumber) {
	return quickAPI::getInstrName(rowNumber);
}

void InstrListModel::backgroundClicked(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		auto callback = [this](const juce::PluginDescription& plugin) {
			CoreActions::insertInstrGUI(
				this->getNumRows(), plugin.createIdentifierString());
			};

		auto menu = this->createBackgroundMenu(callback);
		menu.show();
	}
}

juce::PopupMenu InstrListModel::createBackgroundMenu(
	const std::function<void(const juce::PluginDescription&)>& callback) {
	/** Create Menu */
	auto [valid, list] = quickAPI::getPluginList(true, true);
	if (!valid) { return juce::PopupMenu{}; }
	auto groups = utils::groupPlugin(list, utils::PluginGroupType::Category);
	return utils::createPluginMenu(groups, callback);
}
