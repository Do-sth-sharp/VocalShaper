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
		" v" + utils::getAudioPlatformVersionString() +
		" " + utils::getReleaseName() + "\n" +
		TRANS("Open Source Digital Audio Workstation") + "\n" +
		TRANS("Branch:") + " " + utils::getReleaseBranch() + "\n" +
		TRANS("Complie Time:") + " " + utils::getAudioPlatformComplieTime();
	this->copyMes = TRANS("This product available under the MPL 2.0 license.") + "\n" +
		TRANS("VocalSharp Org.") + " © 2023-2024 " + TRANS("All Rights Reserved.");

	/** Staff Text */
	this->staffTitle = TRANS("Staffs");
	this->staffList =
		TRANS("WuChang") + TRANS("(Architecture, Audio Core, Editor)") + "\n" +
		TRANS("Jingang") + TRANS("(SVS Engine)") + "\n" +
		TRANS("Serge S") + TRANS("(UI Design)") + "\n" +
		TRANS("BakaDoge") + TRANS("(Cooperation)");

	/** Illustraion Text */
	this->illustTitle = TRANS("Illustraion");

	/** Thanks Text */
	this->thanksTitle = TRANS("Special Thanks");
	this->thanksList =
		TRANS("GeraintDou") + "\n" +
		TRANS("xemisyah") + "\n" +
		TRANS("Lin Yuansu P") + "\n" +
		TRANS("Severle") + "\n" +
		TRANS("Warsic Music Club") + "\n" +
		TRANS("Software Engineering and Visualization Laboratory of HFUT(Xuancheng)");

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
	int textHeight = screenSize.getHeight() * 0.022;
	int productNameHeight = screenSize.getHeight() * 0.045;

	int paddingWidth = screenSize.getWidth() * 0.025;
	int paddingHeight = screenSize.getHeight() * 0.035;
	int picWidth = screenSize.getWidth() * 0.22;
	int picHeight = screenSize.getHeight() * 0.43;
	int splitWidth = screenSize.getWidth() * 0.0075;
	int splitHeight = screenSize.getHeight() * 0.015;

	float picRoundCorner = screenSize.getHeight() * 0.01;

	/** Font */
	juce::Font nameFont(this->nameType);
	nameFont.setHeight(productNameHeight);
	nameFont.setBold(true);
	juce::Font titleFont(titleHeight);
	juce::Font textFont(textHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** LOGO */
	juce::Rectangle<int> logoRect(
		paddingWidth, paddingHeight,
		productNameHeight, productNameHeight);
	g.drawImageWithin(this->logo,
		logoRect.getX(), logoRect.getY(),
		logoRect.getWidth(), logoRect.getHeight(),
		juce::RectanglePlacement::onlyReduceInSize);

	/** Product Name */
	juce::Rectangle<int> nameRect(
		logoRect.getRight() + splitWidth, paddingHeight,
		nameFont.getStringWidth(this->productName), productNameHeight);
	g.setFont(nameFont);
	g.setColour(productNameColor);
	g.drawFittedText(this->productName, nameRect,
		juce::Justification::centred, 1, 1.f);

	/** Version */
	juce::Rectangle<int> verRect(
		paddingWidth, logoRect.getBottom() + splitHeight,
		this->getWidth() - picWidth - paddingWidth * 3,
		paddingHeight + picHeight - (logoRect.getBottom() + splitHeight));
	g.setFont(textFont);
	g.setColour(textColor);
	g.drawFittedText(this->verMes, verRect,
		juce::Justification::topLeft, 4, 1.f);
	g.drawFittedText(this->copyMes, verRect,
		juce::Justification::bottomLeft, 2, 1.f);

	/** Picture */
	juce::Rectangle<int> picRect(
		this->getWidth() - paddingWidth - picWidth, paddingHeight,
		picWidth, picHeight);
	if (this->relImg) {
		/** Background */
		g.setColour(this->picBackGroundColor);
		g.fillRoundedRectangle(picRect.toFloat(), picRoundCorner);

		/** Pic Rect */
		double scale = (picRect.getHeight() * this->imgScale) / this->relImg->getHeight();
		juce::Point<int> picCentrePoint = picRect.getCentre();

		juce::Rectangle<int> imgRect(
			picCentrePoint.getX() - (this->relImg->getWidth() * scale / 2),
			picCentrePoint.getY() - (this->relImg->getHeight() * scale / 2),
			this->relImg->getWidth() * scale, this->relImg->getHeight() * scale);
		g.drawImageWithin(*(this->relImg.get()),
			imgRect.getX(), imgRect.getY(),
			imgRect.getWidth(), imgRect.getHeight(),
			juce::RectanglePlacement::onlyReduceInSize);
	}

	/** Staff Title */
	juce::Rectangle<int> staffTitleRect(
		paddingWidth, picRect.getBottom() + paddingHeight * 2,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->staffTitle, staffTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Staff Text */
	int staffLines = juce::StringArray::fromLines(this->staffList).size();
	juce::Rectangle<int> staffRect(
		paddingWidth, staffTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * staffLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->staffList, staffRect,
		juce::Justification::centredTop, staffLines, 1.f);

	/** Illustraion Title */
	juce::Rectangle<int> illustTitleRect(
		paddingWidth, staffRect.getBottom() + paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->illustTitle, illustTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Illustraion Text */
	juce::Rectangle<int> illustRect(
		paddingWidth, illustTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * this->splashList.size());
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->illustList, illustRect,
		juce::Justification::centredTop, this->splashList.size(), 1.f);

	/** Thanks Title */
	juce::Rectangle<int> thanksTitleRect(
		paddingWidth, illustRect.getBottom() + paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->thanksTitle, thanksTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Thanks Text */
	int thanksLines = juce::StringArray::fromLines(this->thanksList).size();
	juce::Rectangle<int> thanksRect(
		paddingWidth, thanksTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * thanksLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->thanksList, thanksRect,
		juce::Justification::centredTop, thanksLines, 1.f);
}

int AboutComponent::getHeightPrefered() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 1.7;
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

	/** Get Current Splash */
	for (auto& [name, var] : list) {
		if (name == utils::getReleaseName() && var.isObject()) {
			this->picBackGroundColor = ColorMap::fromString(var["background"].toString());

			auto relImgTemp = std::make_unique<juce::Image>(
				RCManager::getInstance()->loadImage(
					utils::getAppRootDir().getChildFile(var["picture"].toString())));

			juce::Rectangle<int> cutArea(0, 0, relImgTemp->getWidth(), relImgTemp->getHeight());
			auto cutArray = var["cut"].getArray();
			if (cutArray->size() >= 4) {
				cutArea = { (int)((double)(cutArray->getUnchecked(0)) * relImgTemp->getWidth()),
				(int)((double)(cutArray->getUnchecked(1)) * relImgTemp->getHeight()),
				(int)((double)(cutArray->getUnchecked(2)) * relImgTemp->getWidth()),
				(int)((double)(cutArray->getUnchecked(3)) * relImgTemp->getHeight()) };
			}
			if (var["scaleAbout"].isDouble()) {
				this->imgScale = (double)(var["scaleAbout"]);
			}

			this->relImg = std::make_unique<juce::Image>(
				relImgTemp->getClippedImage(cutArea));
		}
	}

	/** Get Illust List */
	this->illustList.clear();
	for (auto& [name, var] : list) {
		if (var.isObject()) {
			this->illustList +=
				TRANS(var["illustrator"].toString()) + " - " + TRANS(name) + "\n";
		}
	}

	/** Repaint */
	this->repaint();
}
