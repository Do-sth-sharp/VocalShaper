#include "MainMenuModel.h"
#include "../menuAndCommand/MainMenu.h"
#include "../menuAndCommand/CommandManager.h"

MainMenuModel::MainMenuModel()
	: MenuBarModel() {
	this->setApplicationCommandManagerToWatch(
		CommandManager::getInstance());
}

juce::StringArray MainMenuModel::getMenuBarNames() {
	return MainMenu::getInstance()->getNames();
}

juce::PopupMenu MainMenuModel::getMenuForIndex(
	int topLevelMenuIndex, const juce::String& /*menuName*/) {
	switch (topLevelMenuIndex) {
	case 1:
		return MainMenu::getInstance()->createFile();
	case 2:
		return MainMenu::getInstance()->createEdit();
	case 3:
		return MainMenu::getInstance()->createView();
	case 4:
		return MainMenu::getInstance()->createProject();
	case 5:
		return MainMenu::getInstance()->createControl();
	case 6:
		return MainMenu::getInstance()->createConfig();
	case 7:
		return MainMenu::getInstance()->createMisc();
	}
	return juce::PopupMenu{};
}

void MainMenuModel::menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) {
	/** Nothing To Do */
}

void MainMenuModel::menuBarActivated(bool /*isActive*/) {
	/** Nothing To Do */
}
