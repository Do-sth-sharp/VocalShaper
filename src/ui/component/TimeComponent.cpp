#include "TimeComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

TimeComponent::TimeComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSysStatus());
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
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

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour timeColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);

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
		int msec = (uint64_t)(this->timeInSec * 1000Ui64) % 1000;
		int sec = (uint64_t)std::floor(this->timeInSec) % 60;
		int minute = ((uint64_t)std::floor(this->timeInSec) / 60) % 60;
		int hour = this->timeInSec / 3600;

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
		/** TODO */
	}
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
