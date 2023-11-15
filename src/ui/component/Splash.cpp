#include "Splash.h"
#include "../misc/ColorMap.h"
#include <audioCore/AC_API.h>

Splash::Splash() : Component() {
	/** Size */
	juce::Rectangle<int> displayArea(0, 0, 1920, 1080);
	if (auto display =
		juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()) {
		displayArea = display->totalArea;
		
	}
	int sizeBase = (int)(std::min(displayArea.getWidth(), displayArea.getHeight()) * 0.03);
	this->centreWithSize(sizeBase * 25, sizeBase * 10);
	this->addToDesktop(juce::ComponentPeer::windowHasDropShadow);
	this->toFront(true);

	/** On Top */
	this->setAlwaysOnTop(true);
	this->setMouseCursor(juce::MouseCursor::WaitCursor);
	this->setOpaque(false);

	/** Timers */
	this->closeTimer = std::make_unique<CloseTimer>(this);
	this->hideTimer = std::make_unique<HideTimer>(this);

	/** Strings */
	this->platStr = utils::getAudioPlatformName();
	this->verStr = "Ver " + utils::getAudioPlatformVersionString() + " Build " + utils::getAudioPlatformComplieTime();
	this->relStr = utils::getReleaseName();

	/** Images */
	this->logo = std::make_unique<juce::Image>(
		juce::ImageFileFormat::loadFrom(
			juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
			.getParentDirectory().getChildFile("./rc/logo.png")));

	/** Typefaces */
	{
		/** Title */
		juce::File titleFontFile = juce::File::getSpecialLocation(
			juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
			.getChildFile("./fonts/OpenSans-Bold.ttf");
		auto titleFontSize = titleFontFile.getSize();
		auto ptrTitleFontData = std::unique_ptr<char[]>(new char[titleFontSize]);

		auto titleFontStream = titleFontFile.createInputStream();
		titleFontStream->read(ptrTitleFontData.get(), titleFontSize);

		this->titleType = juce::Typeface::createSystemTypefaceFor(ptrTitleFontData.get(), titleFontSize);
	}
	{
		/** Text */
		juce::File textFontFile = juce::File::getSpecialLocation(
			juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
			.getChildFile("./fonts/OpenSans-Regular.ttf");
		auto textFontSize = textFontFile.getSize();
		auto ptrTextFontData = std::unique_ptr<char[]>(new char[textFontSize]);

		auto textFontStream = textFontFile.createInputStream();
		textFontStream->read(ptrTextFontData.get(), textFontSize);

		this->textType = juce::Typeface::createSystemTypefaceFor(ptrTextFontData.get(), textFontSize);
	}
}

void Splash::paint(juce::Graphics& g) {
	/** Colors */
	juce::Colour backGroundColor = ColorMap::fromString("#1b1b24");
	juce::Colour pictureBackGroundColor = ColorMap::fromString("#fdf1da"); // TODO
	juce::Colour titleColor = ColorMap::fromString("#ffffff");
	juce::Colour textColor = ColorMap::fromString("#c5c5db");

	/** Sizes */
	juce::Rectangle<int> backGroundRect = this->getLocalBounds();
	float roundSize = backGroundRect.getHeight() * 0.05f;
	float titleFontHeight = backGroundRect.getHeight() * 0.1f;

	/** BackGround */
	g.setColour(backGroundColor);
	g.fillRoundedRectangle(backGroundRect.toFloat(), roundSize);
	g.setColour(pictureBackGroundColor);
	g.fillRoundedRectangle(backGroundRect.toFloat().withTrimmedLeft(roundSize), roundSize);

	/** Text Area */
	juce::Path textAreaPath;
	textAreaPath.startNewSubPath(roundSize, 0.f);
	textAreaPath.lineTo(roundSize, backGroundRect.getHeight());
	textAreaPath.lineTo(backGroundRect.getWidth() * 0.45f, backGroundRect.getHeight());
	textAreaPath.lineTo(backGroundRect.getWidth() * 0.4f, 0.f);
	textAreaPath.lineTo(roundSize, 0.f);
	textAreaPath.closeSubPath();

	g.setColour(backGroundColor);
	g.fillPath(textAreaPath);

	/** Title */
	juce::Rectangle<int> titleRect(
		backGroundRect.getWidth() * 0.065, backGroundRect.getHeight() * 0.2,
		backGroundRect.getWidth() * 0.25, titleFontHeight);

	juce::Font titleFont(this->titleType);
	titleFont.setBold(true);
	titleFont.setHeight(titleFontHeight);
	g.setFont(titleFont);

	g.setColour(titleColor);
	g.drawFittedText(this->platStr, titleRect, juce::Justification::centredLeft, 1, 1.f);

	/** TODO */
}

void Splash::mouseDown(const juce::MouseEvent& e) {
	juce::Point<int> pos = e.getPosition();
	if (pos.getX() > 0 && pos.getY() > 0 &&
		pos.getX() < this->getWidth() && pos.getY() < this->getHeight()) {
		if (this->isReady) {
			this->closeSplash();
		}
	}
}

void Splash::ready() {
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->isReady = true;
	this->closeTimer->startTimer(3000);
}

void Splash::showMessage(const juce::String& message) {
	this->mesStr = message;
	this->repaint();
}

void Splash::closeSplash() {
	if (this->isReady && this->isVisible() && !this->fader.isAnimating()) {
		constexpr int totalMillSec = 2000;
		this->fader.animateComponent(this, this->getBounds(), 0.0f, totalMillSec, false, 0, 0);
		this->hideTimer->startTimer(totalMillSec);
	}
}

