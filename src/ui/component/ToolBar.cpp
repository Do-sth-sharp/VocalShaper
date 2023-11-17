#include "ToolBar.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include <IconManager.h>

ToolBar::ToolBar()
	: FlowComponent(TRANS("ToolBar")) {
	/** ToolBar LookAndFeel */
	this->setLookAndFeel(LookAndFeelFactory::getInstance()->forToolBar());

	/** Main Button */
	auto mainButtonLAF = LookAndFeelFactory::getInstance()->forMainButton();

	this->mainButton = std::make_unique<juce::DrawableButton>(
		TRANS("Main Button"), juce::DrawableButton::ImageOnButtonBackground);
	this->mainButton->setLookAndFeel(mainButtonLAF);
	this->mainButton->setWantsKeyboardFocus(false);
	this->mainButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->mainButton->setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->mainButton->onClick = [] {
		/** TODO Show Main Menu */
	};
	this->addAndMakeVisible(this->mainButton.get());

	if (auto mainButtonIcon = flowUI::IconManager::getSVG("./RemixIcon/System/menu-line.svg")) {
		mainButtonIcon->replaceColour(juce::Colours::black,
			mainButtonLAF->findColour(juce::DrawableButton::ColourIds::textColourId));
		this->mainButton->setImages(mainButtonIcon.get());
	}
}

void ToolBar::resized() {
	/** Screen Size */
	auto window = this->getWindow();
	if (!window) { return; }
	auto screenSize = window->getScreenSize();

	/** Main Button */
	int mainButtonHeight = this->getHeight() * 0.5;
	juce::Rectangle<int> mainButtonRect(
		0, 0, mainButtonHeight, mainButtonHeight);
	this->mainButton->setBounds(mainButtonRect);
}

void ToolBar::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}
