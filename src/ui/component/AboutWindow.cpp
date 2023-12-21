#include "AboutWindow.h"
#include "../misc/RCManager.h"
#include "../misc/ConfigManager.h"
#include "../Utils.h"

AboutWindow::AboutWindow()
	: DocumentWindow(
		TRANS("About"),
		juce::LookAndFeel::getDefaultLookAndFeel().findColour(
			juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton) {
	/** Content */
	this->content = std::make_unique<AboutComponent>();
	this->viewport = std::make_unique<juce::Viewport>(TRANS("About"));
	this->viewport->setViewedComponent(this->content.get(), false);
	this->viewport->setScrollBarsShown(true, false);
	this->viewport->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->setContentNonOwned(this->viewport.get(), false);

	/** Config */
	this->setUsingNativeTitleBar(true);
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

AboutWindow::~AboutWindow() {
	if (this->renderer) {
		this->renderer->detach();
	}
}

void AboutWindow::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Limit Size */
	this->setResizeLimits(
		screenSize.getWidth() * 0.5, screenSize.getHeight() * 0.5,
		INT_MAX, INT_MAX);

	/** Viewport Pos */
	auto viewPos = this->viewport->getViewPosition();

	/** Resize Viewport */
	this->ResizableWindow::resized();

	/** Content Size */
	int contentWidth = this->viewport->getMaximumVisibleWidth();
	int contentHeight = this->content->getHeightPrefered();
	this->content->setBounds(0, 0,
		contentWidth, std::max(contentHeight, this->getHeight()));

	/** Set Viewport Pos */
	this->viewport->setViewPosition(viewPos);
}

void AboutWindow::closeButtonPressed() {
	this->exitModalState();
}
