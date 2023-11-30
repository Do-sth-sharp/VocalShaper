#include "ToolBar.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include <IconManager.h>

ToolBar::ToolBar()
	: FlowComponent(TRANS("Tool Bar")) {
	/** Main Menu Model */
	this->mainMenuModel = std::make_unique<MainMenuModel>();

	/** Main Menu Bar */
	this->mainMenuBar =
		std::make_unique<MenuBarComponent>(this->mainMenuModel.get());
	this->addAndMakeVisible(this->mainMenuBar.get());
}

ToolBar::~ToolBar() {
	this->mainMenuBar->setModel(nullptr);
}

void ToolBar::resized() {
	/** Screen Size */
	auto window = this->getWindow();
	if (!window) { return; }
	auto screenSize = window->getScreenSize();

	/** Main Menu Bar */
	int mainMenuBarHeight = this->getHeight() * 0.35;
	juce::Rectangle<int> mainMenuBarRect(
		0, 0, mainMenuBarHeight * 15, mainMenuBarHeight);
	this->mainMenuBar->setBounds(mainMenuBarRect);
}

void ToolBar::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}
