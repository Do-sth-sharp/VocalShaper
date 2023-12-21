#include "LicenseWindow.h"
#include "LicenseComponent.h"
#include "../misc/RCManager.h"
#include "../misc/ConfigManager.h"
#include "../Utils.h"

LicenseWindow::LicenseWindow()
	: DocumentWindow(
		TRANS("License"),
		juce::LookAndFeel::getDefaultLookAndFeel().findColour(
			juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton) {
	this->setUsingNativeTitleBar(true);
	this->setContentOwned(new LicenseComponent, false);
	this->setResizable(true, false);
	this->centreWithSize(this->getWidth(), this->getHeight());

	/** OpenGL */
	auto& funcVar = ConfigManager::getInstance()->get("function");
	if (!((bool)(funcVar["cpu-painting"]))) {
		this->renderer = std::make_unique<juce::OpenGLContext>();
		this->renderer->attachTo(*this);
	}

	/** Icon */
	juce::File iconFile = utils::getResourceFile("logo.png");
	this->iconTemp = RCManager::getInstance()->loadImage(iconFile);
	this->setIcon(this->iconTemp);
	this->getPeer()->setIcon(this->iconTemp);

	/** ToolTip */
	this->toolTip = std::make_unique<juce::TooltipWindow>(this);
}

LicenseWindow::~LicenseWindow() {
	if (this->renderer) {
		this->renderer->detach();
	}
}

void LicenseWindow::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Limit Size */
	this->setResizeLimits(
		screenSize.getWidth() * 0.5, screenSize.getHeight() * 0.5,
		INT_MAX, INT_MAX);

	this->ResizableWindow::resized();
}

void LicenseWindow::closeButtonPressed() {
	this->exitModalState();
}
