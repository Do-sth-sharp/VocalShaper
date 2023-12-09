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
	int mainMenuBarHeight = this->getHeight() * 0.4;
	int sysStatusHideWidth = this->getHeight() * 6;
	bool sysStatusShown = this->getWidth() > sysStatusHideWidth;
	int sysStatusWidth = this->getHeight() * 4;

	/** Main Menu Bar */
	juce::Rectangle<int> mainMenuBarRect(
		0, 0, this->getWidth() - (sysStatusShown ? sysStatusWidth : 0), mainMenuBarHeight);
	this->mainMenuBar->setBounds(mainMenuBarRect);

	/** System Status */
	juce::Rectangle<int> sysStatusRect(
		this->getWidth() - sysStatusWidth, 0,
		sysStatusWidth, this->getHeight());
	this->sysStatus->setBounds(sysStatusRect);
	this->sysStatus->setVisible(sysStatusShown);
}

void ToolBar::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}
