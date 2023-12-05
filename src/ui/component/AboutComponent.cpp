#include "AboutComponent.h"
#include "../misc/ColorMap.h"
#include "../misc/RCManager.h"
#include "../misc/MainThreadPool.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

AboutComponent::AboutComponent() {
	/** LOGO */
	this->logo = RCManager::getInstance()->loadImage(
		utils::getResourceFile("logo.png"));

	/** Typeface */
	juce::File nameFontFile = utils::getFontFile("OpenSans-Bold");
	this->nameType = RCManager::getInstance()->loadType(nameFontFile);

	/** About Text */
	this->productName = utils::getAudioPlatformName();
	this->verMes = utils::getAudioPlatformName() +
		" " + utils::getAudioPlatformVersionString() +
		" " + utils::getReleaseName() + "\n" +
		TRANS("Open Source Digital Audio Workstation") + "\n" +
		TRANS("Branch:") + " " + utils::getReleaseBranch() + "\n" +
		TRANS("Complie Time:") + " " + utils::getAudioPlatformComplieTime() + "\n" +
		TRANS("VocalSharp Org.") + " © 2023-2024 " + TRANS("All Rights Reserved.");

	/** Scan For Splash */
	this->scanForSplash();
}

void AboutComponent::paint(juce::Graphics& g) {
	/** Screen */
	auto screenSize = utils::getScreenSize(this);

	/** Color */
	juce::Colour backgroundColor =
		ColorMap::getInstance()->get("ThemeColorB2");
	juce::Colour titleColor =
		ColorMap::getInstance()->get("ThemeColorA2");
	juce::Colour textColor =
		ColorMap::getInstance()->get("ThemeColorB9");
	juce::Colour productNameColor =
		ColorMap::getInstance()->get("ThemeColorB10");

	/** Size */
	int titleHeight = screenSize.getHeight() * 0.04;
	int textHeight = screenSize.getHeight() * 0.02;
	int paddingWidth = screenSize.getWidth() * 0.025;
	int paddingHeight = screenSize.getHeight() * 0.035;

	/** Font */
	juce::Font nameFont(this->nameType);
	nameFont.setHeight(titleHeight);
	nameFont.setBold(true);
	juce::Font titleFont(titleHeight);
	juce::Font textFont(textHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** LOGO */
	juce::Rectangle<int> logoRect(
		paddingWidth, paddingHeight,
		titleHeight, titleHeight);
	g.drawImageWithin(this->logo,
		logoRect.getX(), logoRect.getY(),
		logoRect.getWidth(), logoRect.getHeight(),
		juce::RectanglePlacement::onlyReduceInSize);

	/** Production Name */
	juce::Rectangle<int> nameRect(
		logoRect.getRight() + screenSize.getWidth() * 0.0075, paddingHeight,
		titleFont.getStringWidth(this->productName), titleHeight);
	g.setFont(nameFont);
	g.setColour(productNameColor);
	g.drawFittedText(this->productName, nameRect,
		juce::Justification::centred, 1, 1.f);

	/** Version */
	juce::Rectangle<int> verRect(
		paddingWidth, logoRect.getBottom() + screenSize.getHeight() * 0.015,
		this->getWidth() - paddingWidth * 2, textHeight * 6);
	g.setFont(textFont);
	g.setColour(textColor);
	g.drawFittedText(this->verMes, verRect,
		juce::Justification::topLeft, 5, 1.f);
}

int AboutComponent::getHeightPrefered() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.7;
}

void AboutComponent::scanForSplash() {
	/** Clear List */
	this->splashList.clear();
	this->repaint();

	/** Var */
	auto splashDir = utils::getSplashConfigDir();
	auto comp = AboutComponent::SafePointer{ this };

	/** Scan */
	auto scanJob = [splashDir, comp] {
		auto list = splashDir.findChildFiles(
			juce::File::findFiles, true, "*.json",
			juce::File::FollowSymlinks::noCycles);

		SplashConfigs result;
		for (auto& i : list) {
			juce::String name = i.getFileNameWithoutExtension();
			juce::var data = juce::JSON::parse(i);
			result.add({ name, data });
		}

		juce::MessageManager::callAsync(
			[result, comp] {
				if (comp) {
					comp->updateSplashList(result);
				}
			});
	};
	MainThreadPool::getInstance()->runJob(scanJob);
}

void AboutComponent::updateSplashList(const SplashConfigs& list) {
	/** Set List */
	this->splashList = list;

	/** TODO Get Current Splash */

	/** Repaint */
	this->repaint();
}
