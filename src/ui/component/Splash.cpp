#include "Splash.h"
#include "../misc/ColorMap.h"
#include "../../audioCore/AC_API.h"

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
	this->mesStr = "Waiting...";
	this->platStr = utils::getAudioPlatformName();
	this->verStr = "v" + utils::getAudioPlatformVersionString() + " build " + utils::getAudioPlatformComplieTime();
	this->relStr = utils::getReleaseName();
	this->fraStr = "Made with JUCE.";

	/** Images */
	this->logo = std::make_unique<juce::Image>(
		juce::ImageFileFormat::loadFrom(
			juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
			.getParentDirectory().getChildFile("./rc/logo.png")));

	/** Splash Image */
	{
		juce::File splashConfigFile = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
			.getParentDirectory().getChildFile("./splash/" + this->relStr + ".json");
		juce::var splashConfig = juce::JSON::parse(splashConfigFile);

		this->illustStr = "Illustration by " + splashConfig["illustrator"].toString() + ".";
		this->picBackGroundColor = ColorMap::fromString(splashConfig["background"].toString());

		auto centrePointArray = splashConfig["centrePos"].getArray();
		if (centrePointArray->size() < 2) {
			this->imgCentrePoint = { 0.7,0.5 };
		}
		else {
			this->imgCentrePoint = {
			(double)(centrePointArray->getUnchecked(0)),
			(double)(centrePointArray->getUnchecked(1)) };
		}

		auto relImgTemp = std::make_unique<juce::Image>(
			juce::ImageFileFormat::loadFrom(
				juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
				.getParentDirectory().getChildFile(splashConfig["picture"].toString())));

		juce::Rectangle<int> cutArea(0, 0, relImgTemp->getWidth(), relImgTemp->getHeight());
		auto cutArray = splashConfig["cut"].getArray();
		if (cutArray->size() >= 4) {
			cutArea = { (int)((double)(cutArray->getUnchecked(0)) * relImgTemp->getWidth()),
			(int)((double)(cutArray->getUnchecked(1)) * relImgTemp->getHeight()),
			(int)((double)(cutArray->getUnchecked(2)) * relImgTemp->getWidth()),
			(int)((double)(cutArray->getUnchecked(3)) * relImgTemp->getHeight()) };
		}
		double scale = 1.0;
		if (splashConfig["scale"].isDouble()) {
			scale = (double)(splashConfig["scale"]);
		}

		this->relImg = std::make_unique<juce::Image>(
			relImgTemp->getClippedImage(cutArea).rescaled(
				cutArea.getWidth() * scale, cutArea.getHeight() * scale,
				juce::Graphics::highResamplingQuality));
	}

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
		/** Sub Title */
		juce::File titleFontFile = juce::File::getSpecialLocation(
			juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory()
			.getChildFile("./fonts/OpenSans-BoldItalic.ttf");
		auto titleFontSize = titleFontFile.getSize();
		auto ptrTitleFontData = std::unique_ptr<char[]>(new char[titleFontSize]);

		auto titleFontStream = titleFontFile.createInputStream();
		titleFontStream->read(ptrTitleFontData.get(), titleFontSize);

		this->subTitleType = juce::Typeface::createSystemTypefaceFor(ptrTitleFontData.get(), titleFontSize);
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
	juce::Colour titleColor = ColorMap::fromString("#ffffff");
	juce::Colour subTitleColor = ColorMap::fromString("#ffffff");
	juce::Colour textColor = ColorMap::fromString("#a3a3bf");

	/** Sizes */
	juce::Rectangle<int> backGroundRect = this->getLocalBounds();
	float roundSize = backGroundRect.getHeight() * 0.05f;
	float titleFontHeight = backGroundRect.getHeight() * 0.125f;
	float subTitleFontHeight = backGroundRect.getHeight() * 0.075f;
	float textFontHeight = backGroundRect.getHeight() * 0.06f;

	/** Fonts */
	juce::Font titleFont(this->titleType);
	titleFont.setBold(true);
	titleFont.setHeight(titleFontHeight);

	juce::Font subTitleFont(this->subTitleType);
	subTitleFont.setItalic(true);
	subTitleFont.setHeight(subTitleFontHeight);

	juce::Font textFont(this->textType);
	textFont.setHeight(textFontHeight);

	/** BackGround */
	g.setColour(this->picBackGroundColor);
	g.fillRoundedRectangle(backGroundRect.toFloat().withTrimmedLeft(roundSize), roundSize);
	g.setColour(backGroundColor);
	g.fillRoundedRectangle(backGroundRect.toFloat().withWidth(roundSize * 2), roundSize);
	 
	/** Image */
	if (this->relImg) {
		juce::Rectangle<int> imgRect(
			backGroundRect.getWidth() * this->imgCentrePoint.getX() - (this->relImg->getWidth() / 2),
			backGroundRect.getHeight() * this->imgCentrePoint.getY() - (this->relImg->getHeight() / 2),
			this->relImg->getWidth(), this->relImg->getHeight());
		g.drawImageWithin(*(this->relImg.get()),
			imgRect.getX(), imgRect.getY(),
			imgRect.getWidth(), imgRect.getHeight(),
			juce::RectanglePlacement::onlyReduceInSize);
	}

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

	/** LOGO */
	juce::Rectangle<int> logoRect(
		backGroundRect.getWidth() * 0.055, backGroundRect.getHeight() * 0.12,
		titleFontHeight, titleFontHeight);
	g.drawImageWithin(*(this->logo.get()),
		logoRect.getX(), logoRect.getY(),
		logoRect.getWidth(), logoRect.getHeight(),
		juce::RectanglePlacement::onlyReduceInSize);

	/** Title */
	juce::Rectangle<int> titleRect(
		backGroundRect.getWidth() * 0.055, backGroundRect.getHeight() * 0.25,
		backGroundRect.getWidth() * 0.25, titleFontHeight);

	g.setFont(titleFont);
	g.setColour(titleColor);
	g.drawFittedText(this->platStr, titleRect, juce::Justification::centredLeft, 1, 1.f);

	/** Sub Title */
	juce::Rectangle<int> subTitleRect(
		backGroundRect.getWidth() * 0.28, backGroundRect.getHeight() * 0.285,
		backGroundRect.getWidth() * 0.1, subTitleFontHeight);

	g.setFont(subTitleFont);
	g.setColour(subTitleColor);
	g.drawRoundedRectangle(subTitleRect.toFloat(),
		backGroundRect.getHeight() * 0.025, backGroundRect.getHeight() * 0.005);
	g.drawFittedText(this->relStr, subTitleRect, juce::Justification::centred, 1, 1.f);

	/** Version */
	juce::Rectangle<int> versionRect(
		backGroundRect.getWidth() * 0.055, backGroundRect.getHeight() * 0.65,
		backGroundRect.getWidth() * 0.35, textFontHeight);

	g.setFont(textFont);
	g.setColour(textColor);
	//g.drawRect(versionRect);
	g.drawFittedText(this->verStr, versionRect, juce::Justification::centredLeft, 1, 1.f);

	/** Framework Name */
	juce::Rectangle<int> frameworkRect(
		backGroundRect.getWidth() * 0.055, backGroundRect.getHeight() * 0.7,
		backGroundRect.getWidth() * 0.35, textFontHeight);

	g.setFont(textFont);
	g.setColour(textColor);
	//g.drawRect(frameworkRect);
	g.drawFittedText(this->fraStr, frameworkRect, juce::Justification::centredLeft, 1, 1.f);

	/** Illustrator */
	juce::Rectangle<int> illustRect(
		backGroundRect.getWidth() * 0.055, backGroundRect.getHeight() * 0.75,
		backGroundRect.getWidth() * 0.35, textFontHeight);

	g.setFont(textFont);
	g.setColour(textColor);
	//g.drawRect(illustRect);
	g.drawFittedText(this->illustStr, illustRect, juce::Justification::centredLeft, 1, 1.f);

	/** Status */
	juce::Rectangle<int> statusRect(
		backGroundRect.getWidth() * 0.055, backGroundRect.getHeight() * 0.8,
		backGroundRect.getWidth() * 0.35, textFontHeight);

	g.setFont(textFont);
	g.setColour(textColor);
	//g.drawRect(statusRect);
	g.drawFittedText(this->mesStr, statusRect, juce::Justification::centredLeft, 1, 1.f);
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

