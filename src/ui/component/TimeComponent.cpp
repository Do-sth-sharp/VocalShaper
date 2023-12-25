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
	int timeAreaHeight = this->getHeight() * 0.8;
	int timePaddingWidth = this->getHeight() * 0.1;
	int timePaddingHeight = this->getHeight() * 0.1;
	int timeNumWidth = this->getHeight() * 0.5;
	float timeLineSize = this->getHeight() * 0.075;
	float timeSplitSize = this->getHeight() * 0.05;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour timeColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();

	/** TODO Time */
	g.setColour(timeColor);
	juce::Rectangle<int> testNumRect(
		timePaddingWidth, timePaddingHeight,
		timeNumWidth, timeAreaHeight - timePaddingHeight * 2);
	TimeComponent::paintNum(g, testNumRect, timeLineSize, timeSplitSize, 8);
}

uint8_t TimeComponent::convertBits(uint8_t num) {
	constexpr std::array<uint8_t, 16> code{
		0x3F,/**< 0011 1111 */
		0x06,/**< 0000 0110 */
		0x5B,/**< 0101 1011 */
		0x4F,/**< 0100 1111 */
		0x66,/**< 0110 0110 */
		0x6D,/**< 0110 1101 */
		0x7D,/**< 0111 1101 */
		0x07,/**< 0000 0111 */
		0x7F,/**< 0111 1111 */
		0x6F,/**< 0110 1111 */
		0x77,/**< 0111 0111 */
		0x78,/**< 0111 1000 */
		0x69,/**< 0011 1001 */
		0x5E,/**< 0101 1110 */
		0x79,/**< 0111 1001 */
		0x71/**< 0111 0001 */
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
			area.getWidth() - lineHeadSize * 2.f, lineThickness);
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

	/** TODO */
}
