#include <JuceHeader.h>
#include "audioCore/AC_API.h"
#include <FlowUI.h>
#include <google/protobuf/message_lite.h>
#include "ui/misc/ColorMap.h"
#include "ui/misc/InitTaskList.h"
#include "ui/misc/RCManager.h"
#include "ui/misc/MainThreadPool.h"
#include "ui/misc/ConfigManager.h"
#include "ui/misc/SysStatus.h"
#include "ui/debug/AudioDebuggerComponent.h"
#include "ui/debug/MidiDebuggerComponent.h"
#include "ui/lookAndFeel/LookAndFeelFactory.h"
#include "ui/component/ToolBar.h"
#include "ui/component/Splash.h"
#include "ui/component/CompManager.h"
#include "ui/menuAndCommand/CommandManager.h"
#include "ui/menuAndCommand/CoreCommandTarget.h"
#include "ui/menuAndCommand/GUICommandTarget.h"
#include "ui/Utils.h"

class MainApplication : public juce::JUCEApplication {
private:
	std::unique_ptr<Splash> splash = nullptr;

	void loadConfig() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Load Configs..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				ConfigManager::getInstance()->loadConfigs();
			}
		);
	};

	void setAudioConfig() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set Audio Configs..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				/** Plugin Path */
				quickAPI::setPluginSearchPathListFilePath("./data/audio/pluginPaths.txt");
				quickAPI::setPluginListTemporaryFilePath("./data/audio/plugins.xml");
				quickAPI::setPluginBlackListFilePath("./data/audio/blackPlugins.txt");
				quickAPI::setDeadPluginListPath("./data/audio/deadPlugins");

				/** Functions */
				auto& funcVar = ConfigManager::getInstance()->get("function");
				quickAPI::setReturnToStartOnStop(funcVar["return-on-stop"]);
			}
		);
	};

	void loadTheme() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Load Theme Colors..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				/** Get Config */
				auto& conf = ConfigManager::getInstance()->get("startup");
				juce::String themeName = conf["theme"].toString();

				/** Get Color Map */
				juce::File colorMapFile = utils::getThemeColorFile(themeName);
				auto colorMapData = juce::JSON::parse(colorMapFile);
				if (!colorMapData.isObject()) {
					juce::AlertWindow::showMessageBox(
						juce::MessageBoxIconType::WarningIcon, "VocalShaper Fatal Error",
						"Can't load theme!");
					juce::JUCEApplication::quit();
				}

				/** Set Global Color Map */
				if (auto pColorObj = colorMapData.getDynamicObject()) {
					auto& colorSet = pColorObj->getProperties();
					for (auto& i : colorSet) {
						ColorMap::getInstance()->set(
							i.name.toString(), ColorMap::fromString(i.value.toString()));
					}
				}
			}
		);
	};

	void initLookAndFeel() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Init Default LookAndFeel..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				LookAndFeelFactory::getInstance()->initialise();
			}
		);
	};

	void setFlowUIIcon() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set FlowUI Button Icons..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				flowUI::FlowStyle::setButtonLeftIcon("./RemixIcon/Design/layout-left-2-line.svg");
				flowUI::FlowStyle::setButtonRightIcon("./RemixIcon/Design/layout-right-2-line.svg");
				flowUI::FlowStyle::setButtonTopIcon("./RemixIcon/Design/layout-top-2-line.svg");
				flowUI::FlowStyle::setButtonBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg");
				flowUI::FlowStyle::setButtonAdsorbCenterIcon("./RemixIcon/Editor/link.svg");
				flowUI::FlowStyle::setButtonAdsorbLeftIcon("./RemixIcon/Design/layout-left-2-line.svg");
				flowUI::FlowStyle::setButtonAdsorbRightIcon("./RemixIcon/Design/layout-right-2-line.svg");
				flowUI::FlowStyle::setButtonAdsorbTopIcon("./RemixIcon/Design/layout-top-2-line.svg");
				flowUI::FlowStyle::setButtonAdsorbBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg");
			}
		);
	};

	void configFlowUI() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Config FlowUI Window..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				flowUI::FlowWindowHub::setTitle(utils::getAudioPlatformName());
				flowUI::FlowWindowHub::setIcon("./rc/logo.png");
				flowUI::FlowWindowHub::setOpenGL(true);
			}
		);
	};

	void loadUITranslate() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Load UI Translations..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				/** Get Config */
				auto& conf = ConfigManager::getInstance()->get("startup");
				juce::String transName = conf["language"].toString();

				/** Load Translates */
				juce::File transDir = utils::getTransRootDir(transName);
				auto transList = transDir.findChildFiles(
					juce::File::findFiles, true, "*.txt", juce::File::FollowSymlinks::noCycles);

				if (transList.size() > 0) {
					auto trans = new juce::LocalisedStrings(transList.getReference(0), true);

					for (int i = 1; i < transList.size(); i++) {
						trans->addStrings(juce::LocalisedStrings(transList.getReference(i), true));
					}

					juce::LocalisedStrings::setCurrentMappings(trans);
				}
			}
		);
	};

	void setUIFont() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set UI Fonts..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				/** Get Config */
				auto& conf = ConfigManager::getInstance()->get("startup");
				juce::String fontName = conf["font"].toString();

				/** Load Font */
				juce::File fontFile = utils::getFontFile(fontName);

				auto fontSize = fontFile.getSize();
				auto ptrFontData = std::unique_ptr<char[]>(new char[fontSize]);

				auto fontStream = fontFile.createInputStream();
				fontStream->read(ptrFontData.get(), fontSize);

				auto ptrTypeface = juce::Typeface::createSystemTypefaceFor(ptrFontData.get(), fontSize);
				LookAndFeelFactory::getInstance()->setDefaultSansSerifTypeface(ptrTypeface);
			}
		);
	};

	void createComponents() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Create Components..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				CompManager::getInstance()->set(CompManager::CompType::StartMenu,
				std::make_unique<flowUI::FlowComponent>(TRANS("Start Menu")));
		CompManager::getInstance()->set(CompManager::CompType::ToolBar,
			std::unique_ptr<flowUI::FlowComponent>(new ToolBar));
		CompManager::getInstance()->set(CompManager::CompType::PluginView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Plugin")));
		CompManager::getInstance()->set(CompManager::CompType::SourceView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Source")));
		CompManager::getInstance()->set(CompManager::CompType::TrackView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Track")));
		CompManager::getInstance()->set(CompManager::CompType::InstrView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Instrument")));
		CompManager::getInstance()->set(CompManager::CompType::MixerView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Mixer")));
		CompManager::getInstance()->set(CompManager::CompType::SourceEditView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Source Editor")));
		CompManager::getInstance()->set(CompManager::CompType::SourceRecordView,
			std::make_unique<flowUI::FlowComponent>(TRANS("Source Recorder")));
		CompManager::getInstance()->set(CompManager::CompType::AudioDebugger,
			std::unique_ptr<flowUI::FlowComponent>(new AudioDebuggerComponent));
		CompManager::getInstance()->set(CompManager::CompType::MidiDebugger,
			std::unique_ptr<flowUI::FlowComponent>(new MidiDebuggerComponent));
			}
		);
	};

	void initCommands() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Init Command Manager..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				CommandManager::getInstance()->init();
				flowUI::FlowWindowHub::addKeyListener(CommandManager::getInstance()->getKeyMappings());
			}
		);
	};

	void initCoreHooks() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Init Core Hooks..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				flowUI::FlowWindowHub::setAppExitHook([]() -> bool {
					if (quickAPI::checkProjectSaved() && quickAPI::checkSourcesSaved()) {
						return true;
					}

					return juce::AlertWindow::showOkCancelBox(
						juce::MessageBoxIconType::QuestionIcon, TRANS("Close Editor"),
						TRANS("Discard unsaved changes and exit?"));
					});
			}
		);
	};

	void autoLayout() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Auto Layout Components..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				/** Get Config */
				auto& conf = ConfigManager::getInstance()->get("startup");
				juce::String layoutName = conf["layout"].toString();

				/** Layout */
				CompManager::getInstance()->autoLayout(
					utils::getLayoutFile(layoutName).getFullPathName());
			}
		);
	};

	void prepareMainWindow() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set Main Window Size..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				CompManager::getInstance()->maxMainWindow();
			}
		);
	};

	void hideSplash() {
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) {
					splash->showMessage("Ready.");
					splash->ready();
				}
			}
		);
	};

	void loadProject(const juce::String& commandLineParameters) {
		InitTaskList::getInstance()->add(
			[commandLineParameters] {
				if (commandLineParameters.isNotEmpty()) {
					auto params = utils::parseCommand(commandLineParameters);

					/** Remove Quote */
					for (auto& s : params) {
						s = s.removeCharacters("\"");
					}
					/** Check First Arg And Remove Execute Path */
					{
						juce::File firstArgFile(params[0]);
						juce::File execFile = utils::getAppExecFile();
						if (firstArgFile == execFile) {
							params.remove(0);
						}
					}

					/** Load Project */
					if (params.size() >= 1) {
						juce::File projFile = utils::getAppRootDir().getChildFile(params[0]);
						if (projFile.existsAsFile() && projFile.getFileExtension() == ".vsp4") {
							CoreCommandTarget::getInstance()->systemRequestOpen(params[0]);
						}
					}
				}
			}
		);
	};

public:
	const juce::String getApplicationName() override {
		return utils::getAudioPlatformName(); };
	const juce::String getApplicationVersion() override {
		return utils::getAudioPlatformVersionString(); };
	bool moreThanOneInstanceAllowed() override { return false; };

	void initialise(const juce::String& commandLineParameters) override {
		/** Show Splash */
		this->splash = std::make_unique<Splash>();
		this->splash->setVisible(true);

		/** Load Config */
		this->loadConfig();

		/** Set Audio Config */
		this->setAudioConfig();
		
		/** Load Theme Colors */
		this->loadTheme();

		/** Init Default LookAndFeel */
		this->initLookAndFeel();

		/** Set FlowUI Button Icon */
		this->setFlowUIIcon();

		/** Config Flow Window */
		this->configFlowUI();

		/** Load UI Translate */
		this->loadUITranslate();

		/** Set UI Font */
		this->setUIFont();

		/** Create Components */
		this->createComponents();

		/** Init Commands */
		this->initCommands();

		/** Init Core Hooks */
		this->initCoreHooks();

		/** Auto Layout */
		this->autoLayout();

		/** Set Main Window Size */
		this->prepareMainWindow();

		/** Hide Splash */
		this->hideSplash();

		/** Load Project */
		this->loadProject(commandLineParameters);

		/** Run Init */
		InitTaskList::getInstance()->runNow();
	};

	void shutdown() override {
		/** ShutDown FlowUI */
		flowUI::FlowWindowHub::shutdown();

		/** Release Commands */
		CommandManager::releaseInstance();
		CoreCommandTarget::releaseInstance();
		GUICommandTarget::releaseInstance();

		/** Release Components */
		CompManager::releaseInstance();
		LookAndFeelFactory::releaseInstance();

		/** ShutDown Threads */
		MainThreadPool::releaseInstance();

		/** ShutDown Backend */
		shutdownAudioCore();
		google::protobuf::ShutdownProtobufLibrary();

		/** ShutDown Sigar */
		SysStatus::releaseInstance();

		/** Release Resources */
		RCManager::releaseInstance();
	};

	void anotherInstanceStarted(const juce::String& commandLine) override {
		if (commandLine.isNotEmpty()) {
			auto params = utils::parseCommand(commandLine);

			/** Remove Quote */
			for (auto& s : params) {
				s = s.removeCharacters("\"");
			}
			/** Check First Arg And Remove Execute Path */
			{
				juce::File firstArgFile(params[0]);
				juce::File execFile = utils::getAppExecFile();
				if (firstArgFile == execFile) {
					params.remove(0);
				}
			}

			/** Load Project */
			if (params.size() >= 1) {
				juce::File projFile = utils::getAppRootDir().getChildFile(params[0]);
				if (projFile.existsAsFile() && projFile.getFileExtension() == ".vsp4") {
					CoreCommandTarget::getInstance()->systemRequestOpen(params[0]);
				}
			}
		}
	}
};

START_JUCE_APPLICATION(MainApplication)
