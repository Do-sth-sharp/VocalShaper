#include "AboutComponent.h"
#include "../../misc/ColorMap.h"
#include "../../misc/RCManager.h"
#include "../../misc/MainThreadPool.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

AboutComponent::AboutComponent() {
	this->setWantsKeyboardFocus(true);

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
		TRANS("Compile Time:") + " " + utils::getAudioPlatformCompileTime() + "\n" +
		TRANS("Compiler:") + " " + utils::getCompilerVersion() + "\n" +
		TRANS("JUCE Version:") + " " + utils::getJUCEVersion() + "\n" +
#if JUCE_PLUGINHOST_VST3
		TRANS("VST3 SDK Version:") + " " + utils::getVST3SDKVersion() + "\n" +
#endif //JUCE_PLUGINHOST_VST3
#if JUCE_PLUGINHOST_VST
		TRANS("VST2 SDK Version:") + " " + utils::getVST2SDKVersion() + "\n" +
#endif //JUCE_PLUGINHOST_VST
#if JUCE_PLUGINHOST_ARA
		TRANS("ARA SDK Version:") + " " + utils::getARASDKVersion() + "\n" +
#endif //JUCE_PLUGINHOST_ARA
#if JUCE_PLUGINHOST_LV2
		TRANS("LV2 SDK Version:") + " " + utils::getLV2Version();
#endif //JUCE_PLUGINHOST_LV2
	this->copyMes = TRANS("This product available under the GPLv3 license.") + "\n" +
		TRANS("VocalSharp Org.") + " © 2023-2024 " + TRANS("All Rights Reserved.");

	/** Staff Text */
	this->staffTitle = TRANS("Staffs");
	this->staffList =
		TRANS("WuChang") + " - " + TRANS("Architecture, Audio Core, Editor") + "\n" +
		TRANS("Jingang") + " - " + TRANS("SVS Engine") + "\n" +
		TRANS("Serge S") + " - " + TRANS("UI Design") + "\n" +
		TRANS("BakaDoge") + " - " + TRANS("Cooperation");
	this->staffLines = juce::StringArray::fromLines(this->staffList).size();

	/** Illustraion Text */
	this->illustTitle = TRANS("Illustraion");

	/** Translation Text */
	this->transTitle = TRANS("Translation");

	/** Theme Text */
	this->themeTitle = TRANS("Theme Design");

	/** Thanks Text */
	this->thanksTitle = TRANS("Special Thanks");
	this->thanksList =
		TRANS("GeraintDou") + "\n" +
		TRANS("SineStriker") + "\n" +
		TRANS("xemisyah") + "\n" +
		TRANS("Yukitoha") + "\n" +
		TRANS("Lin Yuansu P") + "\n" +
		TRANS("Severle") + "\n" +
		TRANS("Warsic Music Club") + "\n" +
		TRANS("Software Engineering and Visualization Laboratory of HFUT(Xuancheng)");
	this->thanksLines = juce::StringArray::fromLines(this->thanksList).size();

	/** Tester Text */
	this->testerTitle = TRANS("Early Testers");
	this->testerList = "-";
	this->testerLines = juce::StringArray::fromLines(this->testerList).size();

	/** Colors */
	this->colorA[0] = ColorMap::getInstance()->get("ThemeColorA1");
	this->colorA[1] = ColorMap::getInstance()->get("ThemeColorA2");

	this->colorB[0] = ColorMap::getInstance()->get("ThemeColorB0");
	this->colorB[1] = ColorMap::getInstance()->get("ThemeColorB1");
	this->colorB[2] = ColorMap::getInstance()->get("ThemeColorB2");
	this->colorB[3] = ColorMap::getInstance()->get("ThemeColorB3");
	this->colorB[4] = ColorMap::getInstance()->get("ThemeColorB4");
	this->colorB[5] = ColorMap::getInstance()->get("ThemeColorB5");
	this->colorB[6] = ColorMap::getInstance()->get("ThemeColorB6");
	this->colorB[7] = ColorMap::getInstance()->get("ThemeColorB7");
	this->colorB[8] = ColorMap::getInstance()->get("ThemeColorB8");
	this->colorB[9] = ColorMap::getInstance()->get("ThemeColorB9");
	this->colorB[10] = ColorMap::getInstance()->get("ThemeColorB10");

	this->colorC[0] = ColorMap::getInstance()->get("ThemeColorC1");
	this->colorC[1] = ColorMap::getInstance()->get("ThemeColorC2");
	this->colorC[2] = ColorMap::getInstance()->get("ThemeColorC3");
	this->colorC[3] = ColorMap::getInstance()->get("ThemeColorC4");
	this->colorC[4] = ColorMap::getInstance()->get("ThemeColorC5");
	this->colorC[5] = ColorMap::getInstance()->get("ThemeColorC6");

	/** Scan For Authors */
	this->scanForSplash();
	this->scanForTrans();
	this->scanForTheme();
}

void AboutComponent::paint(juce::Graphics& g) {
	/** Screen */
	auto screenSize = utils::getScreenSize(this);

	/** Color */
	juce::Colour backgroundColor = this->colorB[2];
	juce::Colour titleColor = this->colorA[1];
	juce::Colour textColor = this->colorB[9];
	juce::Colour productNameColor = this->colorB[10];

	/** Size */
	int titleHeight = screenSize.getHeight() * 0.04;
	int textHeight = screenSize.getHeight() * 0.022;
	int productNameHeight = screenSize.getHeight() * 0.045;
	int colorHeight = titleHeight;

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
	juce::Rectangle<int> staffRect(
		paddingWidth, staffTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * this->staffLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->staffList, staffRect,
		juce::Justification::centredTop, this->staffLines, 1.f);

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
		this->getWidth() - paddingWidth * 2, textHeight * this->illustLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->illustList, illustRect,
		juce::Justification::centredTop, this->illustLines, 1.f);

	/** Translation Title */
	juce::Rectangle<int> transTitleRect(
		paddingWidth, illustRect.getBottom() + paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->transTitle, transTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Translation Text */
	juce::Rectangle<int> transRect(
		paddingWidth, transTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * this->transLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->transList, transRect,
		juce::Justification::centredTop, this->transLines, 1.f);

	/** Theme Title */
	juce::Rectangle<int> themeTitleRect(
		paddingWidth, transRect.getBottom() + paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->themeTitle, themeTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Theme Text */
	juce::Rectangle<int> themeRect(
		paddingWidth, themeTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * this->themeLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->themeList, themeRect,
		juce::Justification::centredTop, this->themeLines, 1.f);

	/** Thanks Title */
	juce::Rectangle<int> thanksTitleRect(
		paddingWidth, themeRect.getBottom() + paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->thanksTitle, thanksTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Thanks Text */
	juce::Rectangle<int> thanksRect(
		paddingWidth, thanksTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * this->thanksLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->thanksList, thanksRect,
		juce::Justification::centredTop, this->thanksLines, 1.f);

	/** Tester Title */
	juce::Rectangle<int> testerTitleRect(
		paddingWidth, thanksRect.getBottom() + paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->testerTitle, testerTitleRect,
		juce::Justification::centred, 1, 1.f);

	/** Tester Text */
	juce::Rectangle<int> testerRect(
		paddingWidth, testerTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, textHeight * this->testerLines);
	g.setColour(textColor);
	g.setFont(textFont);
	g.drawFittedText(this->testerList, testerRect,
		juce::Justification::centredTop, this->testerLines, 1.f);

	/** Color Standard */
	for (int i = 0; i < this->colorC.size(); i++) {
		g.setColour(this->colorC[i]);
		g.fillRect(this->getWidth() / 2 - colorHeight * (int)(this->colorC.size()) / 2 + colorHeight * i,
			this->getHeight() - paddingHeight - colorHeight, colorHeight, colorHeight);
	}
	for (int i = 0; i < this->colorB.size(); i++) {
		g.setColour(this->colorB[i]);
		g.fillRect(this->getWidth() / 2 - colorHeight * (int)(this->colorB.size()) / 2 + colorHeight * i,
			this->getHeight() - paddingHeight - colorHeight * 2 - splitHeight, colorHeight, colorHeight);
	}
	for (int i = 0; i < this->colorA.size(); i++) {
		g.setColour(this->colorA[i]);
		g.fillRect(this->getWidth() / 2 - colorHeight * (int)(this->colorA.size()) / 2 + colorHeight * i,
			this->getHeight() - paddingHeight - colorHeight * 3 - splitHeight * 2, colorHeight, colorHeight);
	}
}

int AboutComponent::getHeightPrefered() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 1.7;
}

void AboutComponent::scanForSplash() {
	/** Clear List */
	this->relImg = nullptr;
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
	this->illustLines = 0;
	for (auto& [name, var] : list) {
		if (var.isObject()) {
			this->illustLines++;
			this->illustList +=
				TRANS(var["illustrator"].toString()) + " - " + TRANS(name) + "\n";
		}
	}

	/** Repaint */
	this->repaint();
}

void AboutComponent::scanForTrans() {
	/** Clear */
	this->transLines = 0;
	this->repaint();

	/** Var */
	auto transDir = utils::getTransDir();
	auto comp = AboutComponent::SafePointer{ this };

	/** Scan */
	auto scanJob = [transDir, comp] {
		auto list = transDir.findChildFiles(
			juce::File::findDirectories, true, "*",
			juce::File::FollowSymlinks::noCycles);

		TransConfigs result;
		for (auto& i : list) {
			juce::String name = i.getFileName();
			juce::File config = utils::getTransConfigFile(name);

			juce::var data = juce::JSON::parse(config);
			result.add({ name, data });
		}

		juce::MessageManager::callAsync(
			[result, comp] {
				if (comp) {
					comp->updateTransList(result);
				}
			});
		};
	MainThreadPool::getInstance()->runJob(scanJob);
}

void AboutComponent::updateTransList(const TransConfigs& list) {
	/** Get Trans List */
	this->transList.clear();
	this->transLines = 0;
	for (auto& [name, var] : list) {
		if (var.isObject()) {
			if (auto authors = var["authors"].getArray()) {
				for (int i = 0; i < authors->size(); i++) {
					this->transList +=
						TRANS(authors->getReference(i).toString());
					if (i < authors->size() - 1) {
						this->transList += ", ";
					}
				}

				this->transList += " - " + TRANS(name) + "\n";
				this->transLines++;
			}
		}
	}

	/** Repaint */
	this->repaint();
}

void AboutComponent::scanForTheme() {
	/** Clear */
	this->themeLines = 0;
	this->repaint();

	/** Var */
	auto themeDir = utils::getThemeDir();
	auto comp = AboutComponent::SafePointer{ this };

	/** Scan */
	auto scanJob = [themeDir, comp] {
		auto list = themeDir.findChildFiles(
			juce::File::findDirectories, true, "*",
			juce::File::FollowSymlinks::noCycles);

		ThemeConfigs result;
		for (auto& i : list) {
			juce::String name = i.getFileName();
			juce::File config = utils::getThemeConfigFile(name);

			juce::var data = juce::JSON::parse(config);
			result.add({ name, data });
		}

		juce::MessageManager::callAsync(
			[result, comp] {
				if (comp) {
					comp->updateThemeList(result);
				}
			});
		};
	MainThreadPool::getInstance()->runJob(scanJob);
}

void AboutComponent::updateThemeList(const ThemeConfigs& list) {
	/** Get Theme List */
	this->themeList.clear();
	this->themeLines = 0;
	for (auto& [name, var] : list) {
		if (var.isObject()) {
			if (auto designers = var["designers"].getArray()) {
				for (int i = 0; i < designers->size(); i++) {
					this->themeList +=
						TRANS(designers->getReference(i).toString());
					if (i < designers->size() - 1) {
						this->themeList += ", ";
					}
				}

				this->themeList += " - " + TRANS(name) + "\n";
				this->themeLines++;
			}
		}
	}

	/** Repaint */
	this->repaint();
}
