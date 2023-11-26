#include "MainMenu.h"
#include "../../audioCore/AC_API.h"
#include "CommandManager.h"
#include "CommandTypes.h"

juce::PopupMenu MainMenu::create() const {
	juce::PopupMenu menu;

	menu.addSectionHeader(utils::getAudioPlatformName());
	menu.addSubMenu(TRANS("File"), this->createFile());
	menu.addSubMenu(TRANS("Edit"), this->createEdit());
	menu.addSubMenu(TRANS("View"), this->createView());
	menu.addSubMenu(TRANS("Project"), this->createProject());
	menu.addSubMenu(TRANS("Control"), this->createControl());
	menu.addSubMenu(TRANS("Config"), this->createConfig());
	menu.addSubMenu(TRANS("Misc"), this->createMisc());

	return menu;
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

	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Undo);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)CoreCommandType::Redo);
	menu.addSeparator();
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Copy);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Cut);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Paste);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Clipboard);
	menu.addSeparator();
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::SelectAll);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Delete);

	return menu;
}

juce::PopupMenu MainMenu::createView() const {
	juce::PopupMenu menu;

	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::LoadLayout);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::SaveLayout);
	menu.addSeparator();
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::PluginView);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::SourceView);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::TrackView);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::InstrView);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::MixerView);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::SourceEditView);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::AudioDebugger);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::MidiDebugger);

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
