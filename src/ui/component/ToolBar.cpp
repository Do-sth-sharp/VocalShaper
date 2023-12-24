#include "ToolBar.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include <IconManager.h>

ToolBar::ToolBar()
	: FlowComponent(TRANS("Tool Bar")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forToolBar());

	/** Main Menu Model */
	this->mainMenuModel = std::make_unique<MainMenuModel>();

	/** Main Menu Bar */
	this->mainMenuBar =
		std::make_unique<MenuBarComponent>(this->mainMenuModel.get());
	this->mainMenuBar->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMainMenu());
	this->addAndMakeVisible(this->mainMenuBar.get());

	/** System Status */
	this->sysStatus = std::make_unique<SysStatusComponent>();
	this->addAndMakeVisible(this->sysStatus.get());

	/** Time */
	this->time = std::make_unique<TimeComponent>();
	this->addAndMakeVisible(this->time.get());
}

ToolBar::~ToolBar() {
	this->mainMenuBar->setModel(nullptr);
}

void ToolBar::resized() {
	/** Screen Size */
	auto window = this->getWindow();
	if (!window) { return; }
	auto screenSize = window->getScreenSize();

	/** Size */
	int splitWidth = this->getHeight() * 0.075;
	int mainMenuBarHeight = this->getHeight() * 0.4;
	int sysStatusHideWidth = this->getHeight() * 12;
	int sysStatusWidth = this->getHeight() * 4;
	int timeHideWidth = this->getHeight() * 7;
	int timeWidth = this->getHeight() * 3;

	bool sysStatusShown = this->getWidth() > sysStatusHideWidth;
	bool timeShown = this->getWidth() > timeHideWidth;

	/** Total Area */
	auto totalArea = this->getLocalBounds();

	/** System Status */
	if (sysStatusShown) {
		juce::Rectangle<int> sysStatusRect(
			totalArea.getRight() - sysStatusWidth, 0,
			sysStatusWidth, this->getHeight());
		this->sysStatus->setBounds(sysStatusRect);

		totalArea.removeFromRight(sysStatusRect.getWidth() + splitWidth);
	}
	this->sysStatus->setVisible(sysStatusShown);
	
	/** Time */
	if (timeShown) {
		juce::Rectangle<int> timeRect(
			totalArea.getRight() - timeWidth, 0,
			timeWidth, this->getHeight());
		this->time->setBounds(timeRect);

		totalArea.removeFromRight(timeRect.getWidth() + splitWidth);
	}
	this->time->setVisible(timeShown);

	/** Main Menu Bar */
	juce::Rectangle<int> mainMenuBarRect(
		0, 0, totalArea.getRight(), mainMenuBarHeight);
	this->mainMenuBar->setBounds(mainMenuBarRect);
}

void ToolBar::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}
