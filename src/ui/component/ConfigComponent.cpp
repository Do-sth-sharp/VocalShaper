#include "ConfigComponent.h"
#include "ConfigPropertyComponents.h"
#include "OutputConfigComponent.h"
#include "PluginConfigComponent.h"
#include "../misc/MainThreadPool.h"
#include "../menuAndCommand/CommandManager.h"
#include "../menuAndCommand/CommandTypes.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"
#include <FlowUI.h>

ConfigComponent::ConfigComponent() {
	/** Text */
	this->scanningMes = TRANS("Waiting for a moment...");

	/** Config List Model */
	this->listModel = std::make_unique<ConfigListModel>();
	this->listModel->setCallback(
		[this](int index) {
			this->showPage(index);
		});

	/** Config List */
	this->list = std::make_unique<juce::ListBox>(
		TRANS("Config List"), this->listModel.get());
	this->list->updateContent();
	this->addAndMakeVisible(this->list.get());

	/** Scan For Options */
	this->scan();
}

void ConfigComponent::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Size */
	int listWidth = screenSize.getWidth() * 0.1;
	int audioItemHeight = screenSize.getHeight() * 0.04;

	/** List */
	this->list->setBounds(
		this->getLocalBounds().withWidth(listWidth));

	/** List Height */
	this->list->setRowHeight(screenSize.getHeight() * 0.05);

	/** Audio Page */
	auto audioViewPort = dynamic_cast<juce::Viewport*>(this->pageList[2]);
	auto audioSelector = dynamic_cast<juce::AudioDeviceSelectorComponent*>(
		audioViewPort ? audioViewPort->getViewedComponent() : nullptr);

	/** Output Page */
	auto outputViewPort = dynamic_cast<juce::Viewport*>(this->pageList[3]);
	auto outputEditor = dynamic_cast<OutputConfigComponent*>(
		outputViewPort ? outputViewPort->getViewedComponent() : nullptr);

	/** Plugin Page */
	auto pluginViewPort = dynamic_cast<juce::Viewport*>(this->pageList[4]);
	auto pluginEditor = dynamic_cast<PluginConfigComponent*>(
		pluginViewPort ? pluginViewPort->getViewedComponent() : nullptr);

	/** Audio Selector Item Height */
	if (audioSelector) {
		audioSelector->setItemHeight(audioItemHeight);
	}

	/** Audio Viewport Pos */
	auto audioViewPos = audioSelector
		? audioViewPort->getViewPosition() : juce::Point<int>{};

	/** Output Viewport Pos */
	auto outputViewPos = outputEditor
		? outputViewPort->getViewPosition() : juce::Point<int>{};

	/** Plugin Viewport Pos */
	auto pluginViewPos = pluginEditor
		? pluginViewPort->getViewPosition() : juce::Point<int>{};

	/** Page */
	auto pageRect = this->getLocalBounds().withTrimmedLeft(listWidth);
	for (auto i : this->pageList) {
		i->setBounds(pageRect);
	}

	/** Audio Selector Size */
	if (audioSelector) {
		int contentWidth = audioViewPort->getMaximumVisibleWidth();
		int contentHeight = audioSelector->getHeight();
		audioSelector->setBounds(0, 0,
			contentWidth, std::max(contentHeight, pageRect.getHeight()));
	}

	/** Output Editor Size */
	if (outputEditor) {
		int contentWidth = outputViewPort->getMaximumVisibleWidth();
		int contentHeight = outputEditor->getHeightPrefered();
		outputEditor->setBounds(0, 0,
			contentWidth, std::max(contentHeight, pageRect.getHeight()));
	}

	/** Plugin Editor Size */
	if (pluginEditor) {
		int contentWidth = pluginViewPort->getMaximumVisibleWidth();
		int contentHeight = pluginEditor->getHeightPrefered();
		pluginEditor->setBounds(0, 0,
			contentWidth, std::max(contentHeight, pageRect.getHeight()));
	}

	/** Set Audio Viewport Pos */
	if (audioSelector) {
		audioViewPort->setViewPosition(audioViewPos);
	}

	/** Set Output Viewport Pos */
	if (outputEditor) {
		outputViewPort->setViewPosition(outputViewPos);
	}

	/** Set Plugin Viewport Pos */
	if (pluginEditor) {
		pluginViewPort->setViewPosition(pluginViewPos);
	}
}

void ConfigComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();
	auto screenSize = utils::getScreenSize(this);

	/** Size */
	int listWidth = screenSize.getWidth() * 0.1;
	float textHeight= screenSize.getHeight() * 0.025;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::AlertWindow::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(textHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Text */
	if (!this->scannedFlag) {
		auto pageRect = this->getLocalBounds().withTrimmedLeft(listWidth);
		g.setColour(textColor);
		g.setFont(textFont);
		g.drawFittedText(this->scanningMes, pageRect,
			juce::Justification::centred, 1, 1.f);
	}
}

void ConfigComponent::setPage(int index) {
	this->list->selectRow(index);
	this->showPage(index);
}

int ConfigComponent::getPage() const {
	return this->list->getLastRowSelected();
}

void ConfigComponent::scan() {
	/** Var */
	auto transDir = utils::getTransDir();
	auto fontDir = utils::getFontDir();
	auto themeDir = utils::getThemeDir();
	auto layoutDir = utils::getLayoutDir();

	auto comp = ConfigComponent::SafePointer{ this };

	/** Scan */
	auto scanJob = [transDir, fontDir, themeDir, layoutDir, comp] {
		/** Results */
		juce::StringArray langList, fontList, themeList, layoutList;

		/** Language */
		{
			auto list = transDir.findChildFiles(
				juce::File::findDirectories, true, "*",
				juce::File::FollowSymlinks::noCycles);
			for (auto& i : list) {
				langList.add(i.getFileName());
			}
		}
		/** Font */
		{
			auto list = fontDir.findChildFiles(
				juce::File::findFiles, true, "*.ttf",
				juce::File::FollowSymlinks::noCycles);
			for (auto& i : list) {
				fontList.add(i.getFileNameWithoutExtension());
			}
		}
		/** Theme */
		{
			auto list = themeDir.findChildFiles(
				juce::File::findDirectories, true, "*",
				juce::File::FollowSymlinks::noCycles);
			for (auto& i : list) {
				themeList.add(i.getFileName());
			}
		}
		/** Layout */
		{
			auto list = layoutDir.findChildFiles(
				juce::File::findFiles, true, "*.json",
				juce::File::FollowSymlinks::noCycles);
			for (auto& i : list) {
				layoutList.add(i.getFileNameWithoutExtension());
			}
		}

		juce::MessageManager::callAsync(
			[langList, fontList, themeList, layoutList, comp] {
				if (comp) {
					/** Create Pages */
					comp->createStartupPage({
						themeList, langList, layoutList, fontList });
					comp->createFunctionPage();
					comp->createAudioPage();
					comp->createOutputPage();
					comp->createPluginPage();
					comp->createKeyMappingPage();
					comp->resized();
					comp->repaint();

					/** Set Flag */
					comp->scannedFlag = true;

					/** Show Page */
					comp->showPage(comp->getPage());
				}
			});
		};
	MainThreadPool::getInstance()->runJob(scanJob);
}

void ConfigComponent::showPage(int index) {
	for (int i = 0; i < this->pageList.size(); i++) {
		this->pageList.getUnchecked(i)->setVisible(
			this->scannedFlag && (i == index));
	}
}

void ConfigComponent::createStartupPage(const StartupOptions& option) {
	auto panel = std::make_unique<juce::PropertyPanel>(TRANS("Startup Config"));
	this->addAndMakeVisible(panel.get());

	juce::Array<juce::PropertyComponent*> props;
	props.add(new ConfigLabelProp{ "The following settings will take effect after restarting this application." });
	props.add(new ConfigChoiceProp{ "startup", "language", option.language });
	props.add(new ConfigChoiceProp{ "startup", "theme", option.theme });
	props.add(new ConfigChoiceProp{ "startup", "font", option.font });
	props.add(new ConfigChoiceProp{ "startup", "layout", option.layout });
	props.add(new ConfigWhiteSpaceProp{});
	panel->addProperties(props);

	this->pageList.add(std::move(panel));
}

void ConfigComponent::createFunctionPage() {
	auto panel = std::make_unique<juce::PropertyPanel>(TRANS("Function Config"));
	this->addAndMakeVisible(panel.get());

	auto returnToStartUpdateCallback = [](const juce::var& data) {
		quickAPI::setReturnToStartOnStop(data);
		return true;
		};
	auto returnToStartValueCallback = []()->const juce::var {
		return quickAPI::getReturnToStartOnStop();
		};
	auto anonymousUpdateCallback = [](const juce::var& data) {
		quickAPI::setAnonymousMode(data);
		return true;
		};
	auto anonymousValueCallback = []()->const juce::var {
		return quickAPI::getAnonymousMode();
		};

	juce::Array<juce::PropertyComponent*> audioProps;
	audioProps.add(new ConfigBooleanProp{ "function", "return-on-stop",
		"Disabled", "Enabled", returnToStartUpdateCallback , returnToStartValueCallback});
	audioProps.add(new ConfigBooleanProp{ "function", "anonymous-mode",
		"Disabled", "Enabled", anonymousUpdateCallback , anonymousValueCallback });
	audioProps.add(new ConfigWhiteSpaceProp{});
	panel->addSection(TRANS("Audio Core"), audioProps);

	auto cpuPaintingUpdateCallback = [](const juce::var& data) {
		flowUI::FlowWindowHub::setOpenGL(!((bool)data));
		return true;
		};

	juce::Array<juce::PropertyComponent*> performProps;
	performProps.add(new ConfigLabelProp{ "The effect of some settings will be delayed." });
	performProps.add(new ConfigBooleanProp{ "function", "cpu-painting",
		"Disabled", "Enabled", cpuPaintingUpdateCallback , ConfigPropHelper::GetValueCallback{} });
	performProps.add(new ConfigWhiteSpaceProp{});
	panel->addSection(TRANS("Performance"), performProps);

	auto projRegFunc = [] {
		CommandManager::getInstance()->invokeDirectly(
			(juce::CommandID)GUICommandType::RegProj, true);
		};
	auto projUnregFunc = [] {
		CommandManager::getInstance()->invokeDirectly(
			(juce::CommandID)GUICommandType::UnregProj, true);
		};

	juce::Array<juce::PropertyComponent*> systemProps;
	systemProps.add(new ConfigLabelProp{ "Some settings are only available on certain operating systems." });
	systemProps.add(new ConfigButtonProp{ "proj-reg", "Register", projRegFunc });
	systemProps.add(new ConfigButtonProp{ "proj-unreg", "Unregister", projUnregFunc });
	systemProps.add(new ConfigWhiteSpaceProp{});
	panel->addSection(TRANS("System"), systemProps);

	this->pageList.add(std::move(panel));
}

void ConfigComponent::createAudioPage() {
	/** Selector */
	auto selector = quickAPI::createAudioDeviceSelector();

	/** Viewport */
	auto viewport = std::make_unique<juce::Viewport>(TRANS("Audio Config"));
	viewport->setViewedComponent(selector.release(), true);
	viewport->setScrollBarsShown(true, false);
	viewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addAndMakeVisible(viewport.get());

	/** Add To Page */
	this->pageList.add(std::move(viewport));
}

void ConfigComponent::createOutputPage() {
	/** Editor */
	auto editor = std::make_unique<OutputConfigComponent>();

	/** Viewport */
	auto viewport = std::make_unique<juce::Viewport>(TRANS("Output Config"));
	viewport->setViewedComponent(editor.release(), true);
	viewport->setScrollBarsShown(true, false);
	viewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addAndMakeVisible(viewport.get());

	/** Add To Page */
	this->pageList.add(std::move(viewport));
}

void ConfigComponent::createPluginPage() {
	/** Editor */
	auto editor = std::make_unique<PluginConfigComponent>();

	/** Viewport */
	auto viewport = std::make_unique<juce::Viewport>(TRANS("Plugin Config"));
	viewport->setViewedComponent(editor.release(), true);
	viewport->setScrollBarsShown(true, false);
	viewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->addAndMakeVisible(viewport.get());

	/** Add To Page */
	this->pageList.add(std::move(viewport));
}

void ConfigComponent::createKeyMappingPage() {

}
