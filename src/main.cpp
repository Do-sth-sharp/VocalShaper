#include <JuceHeader.h>
#include "audioCore/AC_API.h"
#include <FlowUI.h>
#include <google/protobuf/message_lite.h>
#include "ui/misc/ColorMap.h"
#include "ui/debug/AudioDebuggerComponent.h"
#include "ui/debug/MidiDebuggerComponent.h"

class MainApplication : public juce::JUCEApplication {
private:
	std::unique_ptr<flowUI::FlowComponent>
		startMenu = nullptr,
		toolBar = nullptr,
		resourceView = nullptr,
		patternView = nullptr,
		trackView = nullptr;
	std::unique_ptr<AudioDebuggerComponent> audioDebugger = nullptr;
	std::unique_ptr<MidiDebuggerComponent> midiDebugger = nullptr;

public:
	const juce::String getApplicationName() override {
		return utils::getAudioPlatformName(); };
	const juce::String getApplicationVersion() override {
		return utils::getAudioPlatformVersionString(); };
	bool moreThanOneInstanceAllowed() override { return false; };

	void initialise(const juce::String& commandLineParameters) override {
		/** Get Config */
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
		quickAPI::setPluginSearchPathListFilePath("./data/audio/pluginPaths.txt");
		quickAPI::setPluginListTemporaryFilePath("./data/audio/plugins.xml");
		quickAPI::setPluginBlackListFilePath("./data/audio/blackPlugins.txt");
		quickAPI::setDeadPluginListPath("./data/audio/deadPlugins");

		{
			/** Get Color Map */
			juce::String themeName = configData["theme"].toString();
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

		/** Set FlowUI Color */
		flowUI::FlowStyle::setTitleBackgroundColor(
			ColorMap::getInstance()->get("ThemeColorB3"));
		flowUI::FlowStyle::setTitleHighlightColor(
			ColorMap::getInstance()->get("ThemeColorB4"));
		flowUI::FlowStyle::setTitleBorderColor(
			ColorMap::getInstance()->get("ThemeColorA2"));
		flowUI::FlowStyle::setTitleSplitColor(
			ColorMap::getInstance()->get("ThemeColorD4"));
		flowUI::FlowStyle::setTitleTextColor(
			ColorMap::getInstance()->get("ThemeColorD2"));
		flowUI::FlowStyle::setTitleTextHighlightColor(
			ColorMap::getInstance()->get("ThemeColorD1"));

		flowUI::FlowStyle::setResizerColor(
			ColorMap::getInstance()->get("ThemeColorA1"));

		flowUI::FlowStyle::setContainerBorderColor(
			ColorMap::getInstance()->get("ThemeColorB1"));

		flowUI::FlowStyle::setButtonIconColor(
			ColorMap::getInstance()->get("ThemeColorD2"));
		flowUI::FlowStyle::setButtonIconBackgroundColor(
			ColorMap::getInstance()->get("ThemeColorD4"));
		flowUI::FlowStyle::setAdsorbColor(
			ColorMap::getInstance()->get("ThemeColorE3"));

		/** Set Menu Color */
		auto& laf = juce::Desktop::getInstance().getDefaultLookAndFeel();
		laf.setColour(juce::PopupMenu::ColourIds::backgroundColourId,
			ColorMap::getInstance()->get("ThemeColorB4"));
		laf.setColour(juce::PopupMenu::ColourIds::textColourId,
			ColorMap::getInstance()->get("ThemeColorD2"));
		laf.setColour(juce::PopupMenu::ColourIds::headerTextColourId,
			ColorMap::getInstance()->get("ThemeColorD2"));
		laf.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId,
			ColorMap::getInstance()->get("ThemeColorB3"));
		laf.setColour(juce::PopupMenu::ColourIds::highlightedTextColourId,
			ColorMap::getInstance()->get("ThemeColorD2"));

		/** Set Alert Color */
		laf.setColour(juce::AlertWindow::ColourIds::backgroundColourId,
			ColorMap::getInstance()->get("ThemeColorB3"));
		laf.setColour(juce::AlertWindow::ColourIds::textColourId,
			ColorMap::getInstance()->get("ThemeColorD2"));
		laf.setColour(juce::AlertWindow::ColourIds::outlineColourId,
			ColorMap::getInstance()->get("ThemeColorB3"));
		laf.setColour(juce::TextButton::ColourIds::buttonColourId,
			ColorMap::getInstance()->get("ThemeColorA2"));
		laf.setColour(juce::TextButton::ColourIds::buttonOnColourId,
			ColorMap::getInstance()->get("ThemeColorA2"));
		laf.setColour(juce::TextButton::ColourIds::textColourOffId,
			ColorMap::getInstance()->get("ThemeColorD1"));
		laf.setColour(juce::TextButton::ColourIds::textColourOnId,
			ColorMap::getInstance()->get("ThemeColorD1"));
		laf.setColour(juce::ComboBox::ColourIds::outlineColourId,
			juce::Colour::fromRGBA(0, 0, 0, 0));

		/** Set Window Background Color */
		laf.setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
			ColorMap::getInstance()->get("ThemeColorB2"));

		/** Set FlowUI Button Icon */
		flowUI::FlowStyle::setButtonLeftIcon("./RemixIcon/Design/layout-left-2-line.svg");
		flowUI::FlowStyle::setButtonRightIcon("./RemixIcon/Design/layout-right-2-line.svg");
		flowUI::FlowStyle::setButtonTopIcon("./RemixIcon/Design/layout-top-2-line.svg");
		flowUI::FlowStyle::setButtonBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg");
		flowUI::FlowStyle::setButtonAdsorbCenterIcon("./RemixIcon/Editor/link.svg");
		flowUI::FlowStyle::setButtonAdsorbLeftIcon("./RemixIcon/Design/layout-left-2-line.svg");
		flowUI::FlowStyle::setButtonAdsorbRightIcon("./RemixIcon/Design/layout-right-2-line.svg");
		flowUI::FlowStyle::setButtonAdsorbTopIcon("./RemixIcon/Design/layout-top-2-line.svg");
		flowUI::FlowStyle::setButtonAdsorbBottomIcon("./RemixIcon/Design/layout-bottom-2-line.svg");

		/** Set Flow Window Icon */
		flowUI::FlowWindowHub::setIcon("./rc/logo.png");
		flowUI::FlowWindowHub::setOpenGL(true);

		/** Load UI Translate */
		{
			juce::File transDir = juce::File::getSpecialLocation(
				juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
				.getChildFile("./translates/" + configData["language"].toString() + "/");
			auto transList = transDir.findChildFiles(juce::File::findFiles, true, "*.txt");

			if (transList.size() > 0) {
				auto trans = new juce::LocalisedStrings(transList.getReference(0), true);

				for (int i = 1; i < transList.size(); i++) {
					trans->addStrings(juce::LocalisedStrings(transList.getReference(i), true));
				}

				juce::LocalisedStrings::setCurrentMappings(trans);
			}
		}

		/** Set UI Font */
		{
			juce::File fontFile = juce::File::getSpecialLocation(
				juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
				.getChildFile("./fonts/" + configData["font"].toString() + ".ttf");

			auto fontSize = fontFile.getSize();
			auto ptrFontData = std::unique_ptr<char[]>(new char[fontSize]);

			auto fontStream = fontFile.createInputStream();
			fontStream->read(ptrFontData.get(), fontSize);

			auto ptrTypeface = juce::Typeface::createSystemTypefaceFor(ptrFontData.get(), fontSize);
			laf.setDefaultSansSerifTypeface(ptrTypeface);
		}

		/** Create Components */
		this->startMenu = std::make_unique<flowUI::FlowComponent>(TRANS("StartMenu"));
		this->toolBar = std::make_unique<flowUI::FlowComponent>(TRANS("ToolBar"));
		this->resourceView = std::make_unique<flowUI::FlowComponent>(TRANS("Resource"));
		this->patternView = std::make_unique<flowUI::FlowComponent>(TRANS("Pattern"));
		this->trackView = std::make_unique<flowUI::FlowComponent>(TRANS("Track"));
		this->audioDebugger = std::make_unique<AudioDebuggerComponent>();
		this->midiDebugger = std::make_unique<MidiDebuggerComponent>();

		/** Auto Layout */
		flowUI::FlowWindowHub::autoLayout(
			"./layouts/" + configData["layout"].toString() + ".json",
			juce::Array<flowUI::FlowComponent*>{
				this->toolBar.get(), this->resourceView.get(),
				this->patternView.get(), this->trackView.get(),
				this->audioDebugger.get(), this->midiDebugger.get()});

		/** Set Main Window Size */
		if (flowUI::FlowWindowHub::getSize() > 0) {
			if (auto window = flowUI::FlowWindowHub::getWindow(0)) {
				window->setFullScreen(true);
			}
		}
	};

	void shutdown() override {
		flowUI::FlowWindowHub::shutdown();
		shutdownAudioCore();
		google::protobuf::ShutdownProtobufLibrary();
	};
};

START_JUCE_APPLICATION(MainApplication)
