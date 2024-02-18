#include "EffectListModel.h"
#include "../component/EffectComponent.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

int EffectListModel::getNumRows() {
	return quickAPI::getEffectNum(this->index);
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

	comp->update(this->index, rowNumber);

	return comp;
}

juce::String EffectListModel::getNameForRow(int rowNumber) {
	return quickAPI::getEffectName(this->index, rowNumber);
}

void EffectListModel::backgroundClicked(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		auto callback = [this](const juce::PluginDescription& plugin) {
			CoreActions::insertEffect(this->index,
				this->getNumRows(), plugin.createIdentifierString());
			};

		auto menu = this->createBackgroundMenu(callback);
		menu.show();
	}
}

void EffectListModel::update(int index) {
	this->index = index;
}

juce::PopupMenu EffectListModel::createBackgroundMenu(
	const std::function<void(const juce::PluginDescription&)>& callback) {
	/** Create Menu */
	auto [valid, list] = quickAPI::getPluginList(true, false);
	if (!valid) { return juce::PopupMenu{}; }
	auto groups = utils::groupPlugin(list, utils::PluginGroupType::Category);
	return utils::createPluginMenu(groups, callback);
}
