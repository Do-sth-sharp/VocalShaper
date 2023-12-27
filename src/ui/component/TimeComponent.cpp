#include "TimeComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/ConfigManager.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

TimeComponent::TimeComponent() {
	/** Animate */
	this->setFramesPerSecond(25);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSysStatus());
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);

	/** Config */
	auto& conf = ConfigManager::getInstance()->get("time");
	this->showSec = conf["show-sec"];
}

void TimeComponent::update() {
	/** Get Values From Audio Core */
	std::tie(this->timeInMeasure, this->timeInBeat) = quickAPI::getTimeInBeat();
	this->timeInSec = quickAPI::getTimeInSecond();
	auto levels = quickAPI::getAudioOutputLevel();
	this->level = {
		(levels.size() > 0) ? levels[0] : 0.f,
		(levels.size() > 1) ? levels[1] : 0.f };
	this->isPlaying = quickAPI::isPlaying();
	this->isRecording = quickAPI::isRecording();
}

void TimeComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Size */
	int timeAreaHeight = this->getHeight() * 0.65;
	int timePaddingWidth = this->getHeight() * 0.1;
	int timePaddingHeight = this->getHeight() * 0.1;

	int timeNumBiggerWidth = this->getHeight() * 0.25;
	int timeColonBiggerWidth = this->getHeight() * 0.1;
	int timeDotBiggerWidth = timeColonBiggerWidth;
	float timeLineBiggerSize = this->getHeight() * 0.04;
	float timeSplitBiggerSize = this->getHeight() * 0.02;
	int timeNumSplitBiggerWidth = this->getHeight() * 0.075;

	double timeSmallerScale = 0.6;
	int timeNumSmallerWidth = timeNumBiggerWidth * timeSmallerScale;
	int timeColonSmallerWidth = timeColonBiggerWidth * timeSmallerScale;
	int timeDotSmallerWidth = timeDotBiggerWidth * timeSmallerScale;
	float timeLineSmallerSize = timeLineBiggerSize * timeSmallerScale;
	float timeSplitSmallerSize = timeSplitBiggerSize * timeSmallerScale;
	int timeNumSplitSmallerWidth = timeNumSplitBiggerWidth * timeSmallerScale;

	int levelAreaWidth = this->getWidth() * 0.7;
	int levelPaddingWidth = this->getHeight() * 0.05;
	int levelPaddingHeight = this->getHeight() * 0.05;
	float levelSplitSize = this->getHeight() * 0.04;

	int statusPaddingWidth = levelPaddingWidth;
	int statusPaddingHeight = levelPaddingHeight;
	int statusItemWidth = this->getHeight() * 0.2;
	int statusSplitWidth = this->getHeight() * 0.04;
	float statusLineSize = this->getHeight() * 0.125;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour timeColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour recordColor = juce::Colours::red;
	juce::Colour playColor = juce::Colours::green;

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Time */
	g.setColour(timeColor);
	juce::Rectangle<int> numAreaRect(
		timePaddingWidth, timePaddingHeight,
		this->getWidth() - timePaddingWidth * 2, timeAreaHeight - timePaddingHeight * 2);

	if (this->showSec) {
		/** Get Num */
		std::array<uint8_t, 8> num = { 0 };
		auto [hour, minute, sec, msec] = TimeComponent::parseTimeSec(this->timeInSec);

		num[0] = (hour / 1) % 10;
		num[1] = (minute / 10) % 10;
		num[2] = (minute / 1) % 10;
		num[3] = (sec / 10) % 10;
		num[4] = (sec / 1) % 10;
		num[5] = (msec / 100) % 10;
		num[6] = (msec / 10) % 10;
		num[7] = (msec / 1) % 10;

		/** Paint Hour */
		for (int i = 0; i <= 0; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumBiggerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Paint Colon */
		{
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeColonBiggerWidth);
			TimeComponent::paintColon(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Paint Minute */
		for (int i = 1; i <= 2; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumBiggerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Paint Colon */
		{
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeColonBiggerWidth);
			TimeComponent::paintColon(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Paint Sec */
		for (int i = 3; i <= 4; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumBiggerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Scale Area */
		numAreaRect.removeFromTop(numAreaRect.getHeight() * (1 - timeSmallerScale));

		/** Paint Dot */
		{
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeDotSmallerWidth);
			TimeComponent::paintDot(g, numRect,
				timeLineSmallerSize, timeSplitSmallerSize);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitSmallerWidth);
		}

		/** Paint MSec */
		for (int i = 5; i <= 7; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumSmallerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineSmallerSize, timeSplitSmallerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitSmallerWidth);
		}
	}
	else {
		/** Get Num */
		std::array<uint8_t, 8> num = { 0 };
		auto [measure, beat, mbeat] = TimeComponent::parseTimeBeat(this->timeInMeasure, this->timeInBeat);

		num[0] = (measure / 1000) % 10;
		num[1] = (measure / 100) % 10;
		num[2] = (measure / 10) % 10;
		num[3] = (measure / 1) % 10;
		num[4] = (beat / 10) % 10;
		num[5] = (beat / 1) % 10;
		num[6] = (mbeat / 10) % 10;
		num[7] = (mbeat / 1) % 10;

		/** Paint Measure */
		for (int i = 0; i <= 3; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumBiggerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Paint Colon */
		{
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeColonBiggerWidth);
			TimeComponent::paintColon(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Paint Beat */
		for (int i = 4; i <= 5; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumBiggerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineBiggerSize, timeSplitBiggerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitBiggerWidth);
		}

		/** Scale Area */
		numAreaRect.removeFromTop(numAreaRect.getHeight()* (1 - timeSmallerScale));

		/** Paint Dot */
		{
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeDotSmallerWidth);
			TimeComponent::paintDot(g, numRect,
				timeLineSmallerSize, timeSplitSmallerSize);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitSmallerWidth);
		}

		/** Paint MBeat */
		for (int i = 6; i <= 7; i++) {
			juce::Rectangle<int> numRect = numAreaRect.withWidth(timeNumSmallerWidth);
			TimeComponent::paintNum(g, numRect,
				timeLineSmallerSize, timeSplitSmallerSize, num[i]);

			numAreaRect.removeFromLeft(numRect.getWidth());
			numAreaRect.removeFromLeft(timeNumSplitSmallerWidth);
		}
	}

	/** Level Meter */
	juce::Rectangle<int> levelRect(
		levelPaddingWidth, timeAreaHeight + levelPaddingHeight,
		levelAreaWidth - levelPaddingWidth * 2, this->getHeight() - timeAreaHeight - levelPaddingHeight * 2);
	TimeComponent::paintLevelMeter(
		g, levelRect, this->level, levelSplitSize, true);

	/** Status */
	juce::Rectangle<int> statusRect(
		levelAreaWidth + statusPaddingWidth, timeAreaHeight + statusPaddingHeight,
		this->getWidth() - levelAreaWidth - statusPaddingWidth * 2, this->getHeight() - timeAreaHeight - statusPaddingHeight * 2);
	
	/** Playing Status */
	{
		juce::Rectangle<int> statusArea
			= statusRect.withTrimmedLeft(statusRect.getWidth() - statusItemWidth);
		g.setColour(playColor);
		TimeComponent::paintPlayStatus(
			g, statusArea, statusLineSize, this->isPlaying);

		statusRect.removeFromRight(statusArea.getWidth());
		statusRect.removeFromRight(statusSplitWidth);
	}

	/** Recording Status */
	{
		juce::Rectangle<int> statusArea
			= statusRect.withTrimmedLeft(statusRect.getWidth() - statusItemWidth);
		g.setColour(recordColor);
		TimeComponent::paintRecordStatus(
			g, statusArea, statusLineSize, this->isRecording);

		statusRect.removeFromRight(statusArea.getWidth());
		statusRect.removeFromRight(statusSplitWidth);
	}
}

void TimeComponent::mouseUp(const juce::MouseEvent& event) {
	/** Size */
	int timeAreaHeight = this->getHeight() * 0.65;

	/** Right Button */
	if (event.mods.isRightButtonDown()) {
		if (event.position.getY() < timeAreaHeight) {
			this->switchTime();
		}
		else {
			/** Nothing To Do */
		}
		return;
	}
}

void TimeComponent::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	int timeAreaHeight = this->getHeight() * 0.65;
	int levelAreaWidth = this->getWidth() * 0.7;

	/** Time */
	if (event.position.getY() < timeAreaHeight) {
		if (this->showSec) {
			auto [hour, minute, sec, msec] = TimeComponent::parseTimeSec(this->timeInSec);
			juce::String str =
				juce::String{ hour } + ":" + juce::String{ minute } + ":"
				+ juce::String{ sec } + "." + juce::String{ msec };
			this->setTooltip(str);
		}
		else {
			auto [measure, beat, mbeat] = TimeComponent::parseTimeBeat(this->timeInMeasure, this->timeInBeat);
			juce::String str =
				juce::String{ measure } + ":" + juce::String{ beat } + "." + juce::String{ mbeat };
			this->setTooltip(str);
		}
	}
	/** Level Meter */
	else if (event.position.getX() < levelAreaWidth) {
		auto [left, right] = this->level;
		left = utils::logRMS(left);
		right = utils::logRMS(right);
		juce::String str =
			juce::String{ left, 2 } + " dB, " + juce::String{ right, 2 } + " dB";
		this->setTooltip(str);
	}
	/** Status */
	else {
		juce::String str;
		if (this->isPlaying) {
			str += (TRANS("Playing") + " ");
		}
		if (this->isRecording) {
			str += (TRANS("Recording") + " ");
		}
		if (str.isEmpty()) {
			str = TRANS("Ready");
		}
		this->setTooltip(str);
	}
}

void TimeComponent::mouseExit(const juce::MouseEvent& event) {
	this->setTooltip("");
}

void TimeComponent::switchTime() {
	/** Show Menu */
	auto menu = this->createTimeMenu();
	int result = menu.show();

	/** Result */
	auto conf = ConfigManager::getInstance()->get("time").getDynamicObject();
	if (!conf) { return; }

	switch (result) {
	case 1:
		this->showSec = true;
		break;
	case 2:
		this->showSec = false;
		break;
	}

	conf->setProperty("show-sec", this->showSec);
	ConfigManager::getInstance()->saveConfig("time");
}

uint8_t TimeComponent::convertBits(uint8_t num) {
	constexpr std::array<uint8_t, 16> code{
		0x3F,/**< 0: 0011 1111 */
		0x06,/**< 1: 0000 0110 */
		0x5B,/**< 2: 0101 1011 */
		0x4F,/**< 3: 0100 1111 */
		0x66,/**< 4: 0110 0110 */
		0x6D,/**< 5: 0110 1101 */
		0x7D,/**< 6: 0111 1101 */
		0x07,/**< 7: 0000 0111 */
		0x7F,/**< 8: 0111 1111 */
		0x6F,/**< 9: 0110 1111 */
		0x77,/**< A: 0111 0111 */
		0x7C,/**< B: 0111 1100 */
		0x39,/**< C: 0011 1001 */
		0x5E,/**< D: 0101 1110 */
		0x79,/**< E: 0111 1001 */
		0x71/**< F: 0111 0001 */
	};
	if (num < code.size()) { return code[num]; }
	return 0x00;/**< 0000 0000 */
}


void TimeComponent::paintNum(
	juce::Graphics& g, const juce::Rectangle<int>& area,
	float lineThickness, float splitThickness, uint8_t num){
	uint8_t bitMask = TimeComponent::convertBits(num);

	/** Size */
	float splitSize = splitThickness / std::sqrt(2.f);
	float halfLineSize = lineThickness / 2.f;
	float lineHeadSize = halfLineSize + splitSize + halfLineSize;
	float angleHeadSize = halfLineSize + splitSize;

	if (bitMask & (1U << 0)) {
		juce::Rectangle<float> lineRect(
			area.getX() + lineHeadSize, area.getY(),
			area.getWidth() - lineHeadSize * 2, lineThickness);
		g.fillRect(lineRect);

		juce::Path leftAnglePath;
		leftAnglePath.startNewSubPath(lineRect.getTopLeft());
		leftAnglePath.lineTo(area.getX() + angleHeadSize, lineRect.getCentreY());
		leftAnglePath.lineTo(lineRect.getBottomLeft());
		leftAnglePath.closeSubPath();
		g.fillPath(leftAnglePath);

		juce::Path rightAnglePath;
		rightAnglePath.startNewSubPath(lineRect.getTopRight());
		rightAnglePath.lineTo(area.getRight() - angleHeadSize, lineRect.getCentreY());
		rightAnglePath.lineTo(lineRect.getBottomRight());
		rightAnglePath.closeSubPath();
		g.fillPath(rightAnglePath);
	}

	if (bitMask & (1U << 1)) {
		juce::Rectangle<float> lineRect(
			area.getRight() - lineThickness, area.getY() + lineHeadSize,
			lineThickness, area.getHeight() / 2 - lineHeadSize - angleHeadSize);
		g.fillRect(lineRect);

		juce::Path topAnglePath;
		topAnglePath.startNewSubPath(lineRect.getTopLeft());
		topAnglePath.lineTo(lineRect.getCentreX(), area.getY() + angleHeadSize);
		topAnglePath.lineTo(lineRect.getTopRight());
		topAnglePath.closeSubPath();
		g.fillPath(topAnglePath);

		juce::Path bottomAnglePath;
		bottomAnglePath.startNewSubPath(lineRect.getBottomLeft());
		bottomAnglePath.lineTo(lineRect.getCentreX(), area.getCentreY() - splitSize);
		bottomAnglePath.lineTo(lineRect.getBottomRight());
		bottomAnglePath.closeSubPath();
		g.fillPath(bottomAnglePath);
	}

	if (bitMask & (1U << 2)) {
		juce::Rectangle<float> lineRect(
			area.getRight() - lineThickness, area.getCentreY() + angleHeadSize,
			lineThickness, area.getHeight() / 2 - lineHeadSize - angleHeadSize);
		g.fillRect(lineRect);

		juce::Path topAnglePath;
		topAnglePath.startNewSubPath(lineRect.getTopLeft());
		topAnglePath.lineTo(lineRect.getCentreX(), area.getCentreY() + splitSize);
		topAnglePath.lineTo(lineRect.getTopRight());
		topAnglePath.closeSubPath();
		g.fillPath(topAnglePath);

		juce::Path bottomAnglePath;
		bottomAnglePath.startNewSubPath(lineRect.getBottomLeft());
		bottomAnglePath.lineTo(lineRect.getCentreX(), area.getBottom() - angleHeadSize);
		bottomAnglePath.lineTo(lineRect.getBottomRight());
		bottomAnglePath.closeSubPath();
		g.fillPath(bottomAnglePath);
	}

	if (bitMask & (1U << 3)) {
		juce::Rectangle<float> lineRect(
			area.getX() + lineHeadSize, area.getBottom() - lineThickness,
			area.getWidth() - lineHeadSize * 2, lineThickness);
		g.fillRect(lineRect);

		juce::Path leftAnglePath;
		leftAnglePath.startNewSubPath(lineRect.getTopLeft());
		leftAnglePath.lineTo(area.getX() + angleHeadSize, lineRect.getCentreY());
		leftAnglePath.lineTo(lineRect.getBottomLeft());
		leftAnglePath.closeSubPath();
		g.fillPath(leftAnglePath);

		juce::Path rightAnglePath;
		rightAnglePath.startNewSubPath(lineRect.getTopRight());
		rightAnglePath.lineTo(area.getRight() - angleHeadSize, lineRect.getCentreY());
		rightAnglePath.lineTo(lineRect.getBottomRight());
		rightAnglePath.closeSubPath();
		g.fillPath(rightAnglePath);
	}

	if (bitMask & (1U << 4)) {
		juce::Rectangle<float> lineRect(
			area.getX(), area.getCentreY() + angleHeadSize,
			lineThickness, area.getHeight() / 2 - lineHeadSize - angleHeadSize);
		g.fillRect(lineRect);

		juce::Path topAnglePath;
		topAnglePath.startNewSubPath(lineRect.getTopLeft());
		topAnglePath.lineTo(lineRect.getCentreX(), area.getCentreY() + splitSize);
		topAnglePath.lineTo(lineRect.getTopRight());
		topAnglePath.closeSubPath();
		g.fillPath(topAnglePath);

		juce::Path bottomAnglePath;
		bottomAnglePath.startNewSubPath(lineRect.getBottomLeft());
		bottomAnglePath.lineTo(lineRect.getCentreX(), area.getBottom() - angleHeadSize);
		bottomAnglePath.lineTo(lineRect.getBottomRight());
		bottomAnglePath.closeSubPath();
		g.fillPath(bottomAnglePath);
	}

	if (bitMask & (1U << 5)) {
		juce::Rectangle<float> lineRect(
			area.getX(), area.getY() + lineHeadSize,
			lineThickness, area.getHeight() / 2 - lineHeadSize - angleHeadSize);
		g.fillRect(lineRect);

		juce::Path topAnglePath;
		topAnglePath.startNewSubPath(lineRect.getTopLeft());
		topAnglePath.lineTo(lineRect.getCentreX(), area.getY() + angleHeadSize);
		topAnglePath.lineTo(lineRect.getTopRight());
		topAnglePath.closeSubPath();
		g.fillPath(topAnglePath);

		juce::Path bottomAnglePath;
		bottomAnglePath.startNewSubPath(lineRect.getBottomLeft());
		bottomAnglePath.lineTo(lineRect.getCentreX(), area.getCentreY() - splitSize);
		bottomAnglePath.lineTo(lineRect.getBottomRight());
		bottomAnglePath.closeSubPath();
		g.fillPath(bottomAnglePath);
	}

	if (bitMask & (1U << 6)) {
		juce::Rectangle<float> lineRect(
			area.getX() + lineHeadSize, area.getCentreY() - lineThickness / 2,
			area.getWidth() - lineHeadSize * 2, lineThickness);
		g.fillRect(lineRect);

		juce::Path leftAnglePath;
		leftAnglePath.startNewSubPath(lineRect.getTopLeft());
		leftAnglePath.lineTo(area.getX() + angleHeadSize, lineRect.getCentreY());
		leftAnglePath.lineTo(lineRect.getBottomLeft());
		leftAnglePath.closeSubPath();
		g.fillPath(leftAnglePath);

		juce::Path rightAnglePath;
		rightAnglePath.startNewSubPath(lineRect.getTopRight());
		rightAnglePath.lineTo(area.getRight() - angleHeadSize, lineRect.getCentreY());
		rightAnglePath.lineTo(lineRect.getBottomRight());
		rightAnglePath.closeSubPath();
		g.fillPath(rightAnglePath);
	}
}

void TimeComponent::paintColon(
	juce::Graphics& g, const juce::Rectangle<int>& area,
	float lineThickness, float splitThickness) {
	juce::Rectangle<float> upRect(
		area.getCentreX() - lineThickness / 2, area.getY() + area.getHeight() / 4 - lineThickness / 2,
		lineThickness, lineThickness);
	g.fillRect(upRect);

	juce::Rectangle<float> downRect(
		area.getCentreX() - lineThickness / 2, area.getBottom() - area.getHeight() / 4 - lineThickness / 2,
		lineThickness, lineThickness);
	g.fillRect(downRect);
}

void TimeComponent::paintDot(
	juce::Graphics& g, const juce::Rectangle<int>& area,
	float lineThickness, float splitThickness) {
	juce::Rectangle<float> dotRect(
		area.getCentreX() - lineThickness / 2, area.getBottom() - area.getHeight() / 4 - lineThickness / 2,
		lineThickness, lineThickness);
	g.fillRect(dotRect);
}

void TimeComponent::paintLevelMeter(
	juce::Graphics& g, const juce::Rectangle<int>& area,
	const LevelValue& value, float splitThickness, bool logMeter) {
	/** Level Area */
	float barHeight = area.getHeight() / 2.f - splitThickness / 2;
	juce::Rectangle<float> leftRect(
		area.getX(), area.getY(),
		area.getWidth(), barHeight);
	juce::Rectangle<float> rightRect(
		area.getX(), area.getBottom() - barHeight,
		area.getWidth(), barHeight);

	/** Level Segment */
	std::array<float, 3> levelSegs{
		0.66f, 0.86f, 1.f };

	/** Level Value */
	auto [left, right] = value;

	/** Log Value */
	if (logMeter) {
		constexpr float rmsNum = 70.f;
		left = std::max(utils::getLogLevelPercent(utils::logRMS(left)), 0.f);
		right = std::max(utils::getLogLevelPercent(utils::logRMS(right)), 0.f);
	}

	/** Level Color */
	std::array<juce::Colour, levelSegs.size()> levelColors{
		juce::Colours::green,
		juce::Colours::yellow,
		juce::Colours::red };

	/** Paint Lower Level */
	for (int i = levelSegs.size() - 1; i >= 0; i--) {
		juce::Rectangle<float> leftArea
			= leftRect.withWidth(std::fminf(left, levelSegs[i]) * area.getWidth());
		juce::Rectangle<float> rightArea
			= rightRect.withWidth(std::fminf(right, levelSegs[i]) * area.getWidth());

		g.setColour(levelColors[i]);
		g.fillRect(leftArea);
		g.fillRect(rightArea);
	}
}

void TimeComponent::paintRecordStatus(
	juce::Graphics& g, const juce::Rectangle<int>& area,
	float lineThickness, bool recording) {
	/** Status */
	if (!recording) { return; }

	/** Circle */
	juce::Rectangle<float> contentArea(
		area.getCentreX() - lineThickness / 2, area.getCentreY() - lineThickness / 2,
		lineThickness, lineThickness);
	g.fillEllipse(contentArea);
}

void TimeComponent::paintPlayStatus(
	juce::Graphics& g, const juce::Rectangle<int>& area,
	float lineThickness, bool playing) {
	/** Status */
	if (!playing) { return; }

	/** Size */
	constexpr float corner = 0.2;

	float verSize = lineThickness;
	float horSize = verSize / 2 * std::sqrtf(3.f);
	float halfVerSize = verSize / 2;
	float halfHorSize = horSize / 2;
	float cornerVerSize = corner * verSize;
	float cornerHorSize = corner * horSize;
	float cornerR = cornerVerSize / sqrtf(3.f);

	/** Key Points */
	juce::Point<float> p00(
		area.getCentreX() - halfHorSize,
		area.getCentreY() - halfVerSize + cornerVerSize);
	juce::Point<float> p01(
		area.getCentreX() - halfHorSize + cornerHorSize,
		area.getCentreY() - halfVerSize + cornerVerSize / 2);
	juce::Point<float> p10(
		area.getCentreX() + halfHorSize - cornerHorSize,
		area.getCentreY() - cornerVerSize / 2);
	juce::Point<float> p11(
		area.getCentreX() + halfHorSize - cornerHorSize,
		area.getCentreY() + cornerVerSize / 2);
	juce::Point<float> p20(
		area.getCentreX() - halfHorSize + cornerHorSize,
		area.getCentreY() + halfVerSize - cornerVerSize / 2);
	juce::Point<float> p21(
		area.getCentreX() - halfHorSize,
		area.getCentreY() + halfVerSize - cornerVerSize);

	juce::Point<float> r0(
		area.getCentreX() - halfHorSize + cornerR,
		area.getCentreY() - halfVerSize + cornerVerSize);
	juce::Point<float> r1(
		area.getCentreX() + halfHorSize - cornerR * 2,
		area.getCentreY());
	juce::Point<float> r2(
		area.getCentreX() - halfHorSize + cornerR,
		area.getCentreY() + halfVerSize - cornerVerSize);

	/** Path */
	juce::Path path;
	path.startNewSubPath(p00);
	path.lineTo(p01);
	path.lineTo(p10);
	path.lineTo(p11);
	path.lineTo(p20);
	path.lineTo(p21);
	path.closeSubPath();
	g.fillPath(path);

	/** Rounds */
	g.fillEllipse(r0.getX() - cornerR, r0.getY() - cornerR,
		cornerR * 2, cornerR * 2);
	g.fillEllipse(r1.getX() - cornerR, r1.getY() - cornerR,
		cornerR * 2, cornerR * 2);
	g.fillEllipse(r2.getX() - cornerR, r2.getY() - cornerR,
		cornerR * 2, cornerR * 2);
}

std::tuple<int, int, int, int> TimeComponent::parseTimeSec(double time) {
	int msec = (uint64_t)(time * 1000Ui64) % 1000;
	int sec = (uint64_t)std::floor(time) % 60;
	int minute = ((uint64_t)std::floor(time) / 60) % 60;
	int hour = time / 3600;

	return { hour, minute, sec, msec };
}

std::tuple<int, int, int> TimeComponent::parseTimeBeat(uint64_t tMeasure, double tBeat) {
	int mbeat = (uint64_t)(tBeat * 100Ui64) % 100;
	int beat = (uint64_t)std::floor(tBeat) % 100;
	int measure = (uint64_t)std::floor(tMeasure) % 10000;

	return { measure, beat, mbeat };
}

juce::PopupMenu TimeComponent::createTimeMenu() const {
	juce::PopupMenu menu;
	menu.addItem(1, TRANS("Time In Seconds"), true, this->showSec);
	menu.addItem(2, TRANS("Time In Beats"), true, !(this->showSec));
	return menu;
}
