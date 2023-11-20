#include "MainMenu.h"
#include "../../audioCore/AC_API.h"
#include "CommandManager.h"
#include "CommandTypes.h"

juce::PopupMenu MainMenu::create() const {
	if (this->menuTemp) { return *(this->menuTemp.get()); }

	this->menuTemp = std::make_unique<juce::PopupMenu>();

	this->menuTemp->addSectionHeader(utils::getAudioPlatformName());
	this->menuTemp->addSubMenu(TRANS("File"), this->createFile());
	this->menuTemp->addSubMenu(TRANS("Edit"), this->createEdit());
	this->menuTemp->addSubMenu(TRANS("View"), this->createView());
	this->menuTemp->addSubMenu(TRANS("Project"), this->createProject());
	this->menuTemp->addSubMenu(TRANS("Control"), this->createControl());
	this->menuTemp->addSubMenu(TRANS("Config"), this->createConfig());
	this->menuTemp->addSubMenu(TRANS("Misc"), this->createMisc());

	return *(this->menuTemp.get());
}

juce::PopupMenu MainMenu::createFile() const {
	juce::PopupMenu menu;

	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::NewProject);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::OpenProject);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::SaveProject);
	menu.addSeparator();
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::LoadSource);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::LoadSynthSource);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::SaveSource);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::ExportSource);
	menu.addSeparator();
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Render);
	menu.addSeparator();
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::CloseEditor);

	return menu;
}

juce::PopupMenu MainMenu::createEdit() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenu::createView() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenu::createProject() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenu::createControl() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenu::createConfig() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenu::createMisc() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

MainMenu* MainMenu::getInstance() {
	return MainMenu::instance ? MainMenu::instance 
		: (MainMenu::instance = new MainMenu{});
}

void MainMenu::releaseInstance() {
	if (MainMenu::instance) {
		delete MainMenu::instance;
		MainMenu::instance = nullptr;
	}
}

MainMenu* MainMenu::instance = nullptr;
