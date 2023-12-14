#include "ConfigComponent.h"
#include "ConfigPropertyComponents.h"
#include "../misc/MainThreadPool.h"
#include "../Utils.h"

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

	/** List */
	this->list->setBounds(
		this->getLocalBounds().withWidth(listWidth));

	/** List Height */
	this->list->setRowHeight(screenSize.getHeight() * 0.05);

	/** Page */
	auto pageRect = this->getLocalBounds().withTrimmedLeft(listWidth);
	for (auto i : this->pageList) {
		i->setBounds(pageRect);
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
	panel->addProperties(props);

	this->pageList.add(std::move(panel));
}

void ConfigComponent::createFunctionPage() {

}

void ConfigComponent::createAudioPage() {

}

void ConfigComponent::createOutputPage() {

}

void ConfigComponent::createPluginPage() {

}

void ConfigComponent::createKeyMappingPage() {

}
