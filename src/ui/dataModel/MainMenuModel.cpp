#include "MainMenuModel.h"
#include "../menuAndCommand/CommandManager.h"
#include "../menuAndCommand/CommandTypes.h"

MainMenuModel::MainMenuModel()
	: MenuBarModel() {}

const juce::StringArray MainMenuModel::getMenuBarNames() {
	return { TRANS("File"), TRANS("Edit"), TRANS("View"),
		TRANS("Project"), TRANS("Control"), TRANS("Config"), TRANS("Misc") };
}

juce::PopupMenu MainMenuModel::getMenuForIndex(
	int topLevelMenuIndex, const juce::String& /*menuName*/) {
	switch (topLevelMenuIndex) {
	case 0:
		return this->createFile();
	case 1:
		return this->createEdit();
	case 2:
		return this->createView();
	case 3:
		return this->createProject();
	case 4:
		return this->createControl();
	case 5:
		return this->createConfig();
	case 6:
		return this->createMisc();
	}
	return juce::PopupMenu{};
}

juce::PopupMenu MainMenuModel::createFile() const {
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

juce::PopupMenu MainMenuModel::createEdit() const {
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

juce::PopupMenu MainMenuModel::createView() const {
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
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::SourceRecordView);

	return menu;
}

juce::PopupMenu MainMenuModel::createProject() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenuModel::createControl() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenuModel::createConfig() const {
	juce::PopupMenu menu;

	/** TODO */

	return menu;
}

juce::PopupMenu MainMenuModel::createMisc() const {
	juce::PopupMenu linksMenu;
	linksMenu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Bilibili);
	linksMenu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Github);

	juce::PopupMenu devMenu;
	devMenu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::AudioDebugger);
	devMenu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::MidiDebugger);
	devMenu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::RegProj);
	devMenu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::UnregProj);

	juce::PopupMenu menu;

	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Help);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Update);
	menu.addSubMenu(TRANS("Links"), linksMenu);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::Website);
	menu.addSubMenu(TRANS("Developer Tool"), devMenu);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::License);
	menu.addCommandItem(CommandManager::getInstance(), (juce::CommandID)GUICommandType::About);

	return menu;
}
