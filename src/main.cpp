#include <JuceHeader.h>
#include "audioCore/AC_API.h"
#include <FlowUI.h>
#include <google/protobuf/message_lite.h>
#include "ui/misc/ColorMap.h"
#include "ui/misc/InitTaskList.h"
#include "ui/debug/AudioDebuggerComponent.h"
#include "ui/debug/MidiDebuggerComponent.h"
#include "ui/lookAndFeel/LookAndFeelFactory.h"
#include "ui/component/ToolBar.h"
#include "ui/component/Splash.h"
#include "ui/component/CompManager.h"
#include "ui/menuAndCommand/CommandManager.h"

class MainApplication : public juce::JUCEApplication {
private:
	std::unique_ptr<Splash> splash = nullptr;

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

		/** Get Config */
		this->splash->showMessage("Load StartUp Configs...");

		juce::File configFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
			.getParentDirectory().getChildFile("./data/config/startup.json");
		auto configData = juce::JSON::parse(configFile);
		if (!configData.isObject()) {
			juce::AlertWindow::showMessageBox(
				juce::MessageBoxIconType::WarningIcon, "VocalShaper Fatal Error",
				"Can't load startup configs!");
			juce::JUCEApplication::quit();
		}

		/** Set Audio Config */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set Audio Configs..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				quickAPI::setPluginSearchPathListFilePath("./data/audio/pluginPaths.txt");
				quickAPI::setPluginListTemporaryFilePath("./data/audio/plugins.xml");
				quickAPI::setPluginBlackListFilePath("./data/audio/blackPlugins.txt");
				quickAPI::setDeadPluginListPath("./data/audio/deadPlugins");
			}
		);
		
		/** Load Theme Colors */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Load Theme Colors..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[themeName = configData["theme"].toString()] {
				/** Get Color Map */
				juce::File colorMapFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
					.getParentDirectory().getChildFile("./themes/" + themeName + "/colors.json");
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

		/** Init Default LookAndFeel */
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

		/** Set FlowUI Button Icon */
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

		/** Set Flow Window Icon */
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

		/** Load UI Translate */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Load UI Translations..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[transName = configData["language"].toString()] {
				juce::File transDir = juce::File::getSpecialLocation(
					juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
					.getChildFile("./translates/" + transName + "/");
				auto transList = transDir.findChildFiles(juce::File::findFiles, true, "*.txt");

				if (transList.size() > 0) {
					auto trans = new juce::LocalisedStrings(transList.getReference(0), true);

					for (int i = 1; i < transList.size(); i++) {
						trans->addStrings(juce::LocalisedStrings(transList.getReference(i), true));
					}

					juce::LocalisedStrings::setCurrentMappings(trans);
				}
			}
		);

		/** Set UI Font */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set UI Fonts..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[fontName = configData["font"].toString()] {
				juce::File fontFile = juce::File::getSpecialLocation(
					juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
					.getChildFile("./fonts/" + fontName + ".ttf");

				auto fontSize = fontFile.getSize();
				auto ptrFontData = std::unique_ptr<char[]>(new char[fontSize]);

				auto fontStream = fontFile.createInputStream();
				fontStream->read(ptrFontData.get(), fontSize);

				auto ptrTypeface = juce::Typeface::createSystemTypefaceFor(ptrFontData.get(), fontSize);
				LookAndFeelFactory::getInstance()->setDefaultSansSerifTypeface(ptrTypeface);
			}
		);

		/** Create Components */
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
				CompManager::getInstance()->set(CompManager::CompType::AudioDebugger,
					std::unique_ptr<flowUI::FlowComponent>(new AudioDebuggerComponent));
				CompManager::getInstance()->set(CompManager::CompType::MidiDebugger,
					std::unique_ptr<flowUI::FlowComponent>(new MidiDebuggerComponent));
			}
		);

		/** Init Commands */
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

		/** Init Core Hooks */
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

		/** Auto Layout */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Auto Layout Components..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[layoutName = configData["layout"].toString()] {
				flowUI::FlowWindowHub::autoLayout(
					"./layouts/" + layoutName + ".json",
					juce::Array<flowUI::FlowComponent*>{
						CompManager::getInstance()->get(CompManager::CompType::ToolBar),
						CompManager::getInstance()->get(CompManager::CompType::PluginView),
						CompManager::getInstance()->get(CompManager::CompType::SourceView),
						CompManager::getInstance()->get(CompManager::CompType::TrackView),
						CompManager::getInstance()->get(CompManager::CompType::InstrView),
						CompManager::getInstance()->get(CompManager::CompType::MixerView),
						CompManager::getInstance()->get(CompManager::CompType::SourceEditView),
						CompManager::getInstance()->get(CompManager::CompType::AudioDebugger),
						CompManager::getInstance()->get(CompManager::CompType::MidiDebugger),
				});
			}
		);

		/** Set Main Window Size */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) { splash->showMessage("Set Main Window Size..."); }
			}
		);
		InitTaskList::getInstance()->add(
			[] {
				if (flowUI::FlowWindowHub::getSize() > 0) {
					if (auto window = flowUI::FlowWindowHub::getWindow(0)) {
						window->setFullScreen(true);
					}
				}
			}
		);

		/** Hide Splash */
		InitTaskList::getInstance()->add(
			[splash = Splash::SafePointer<Splash>(this->splash.get())] {
				if (splash) {
					splash->showMessage("Ready.");
					splash->ready();
				}
			}
		);

		/** Run Init */
		InitTaskList::getInstance()->runNow();
	};

	void shutdown() override {
		/** ShutDown FlowUI */
		flowUI::FlowWindowHub::shutdown();

		/** Release Commands */
		CommandManager::releaseInstance();

		/** Release Components */
		CompManager::releaseInstance();

		/** ShutDown Backend */
		shutdownAudioCore();
		google::protobuf::ShutdownProtobufLibrary();
	};
};

START_JUCE_APPLICATION(MainApplication)
