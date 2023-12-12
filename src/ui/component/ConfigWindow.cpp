#include "ConfigWindow.h"
#include "ConfigComponent.h"
#include "../misc/RCManager.h"
#include "../Utils.h"

ConfigWindow::ConfigWindow()
	: DocumentWindow(
		TRANS("Config"),
		juce::LookAndFeel::getDefaultLookAndFeel().findColour(
			juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton) {
	this->setUsingNativeTitleBar(true);
	this->setContentOwned(new ConfigComponent, false);
	this->setResizable(true, false);
	this->centreWithSize(this->getWidth(), this->getHeight());

	/** OpenGL */
	this->renderer.attachTo(*this);

	/** Icon */
	juce::File iconFile = utils::getResourceFile("logo.png");
	this->iconTemp = RCManager::getInstance()->loadImage(iconFile);
	this->setIcon(this->iconTemp);
	this->getPeer()->setIcon(this->iconTemp);

	/** ToolTip */
	this->toolTip = std::make_unique<juce::TooltipWindow>(this);
}

ConfigWindow::~ConfigWindow() {
	this->renderer.detach();
}

void ConfigWindow::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Limit Size */
	this->setResizeLimits(
		screenSize.getWidth() * 0.5, screenSize.getHeight() * 0.5,
		INT_MAX, INT_MAX);

	this->ResizableWindow::resized();
}

void ConfigWindow::closeButtonPressed() {
	this->exitModalState();
}

void ConfigWindow::setPage(int index) {
	if (auto content = dynamic_cast<ConfigComponent*>(
		this->getContentComponent())) {
		content->setPage(index);
	}
}
