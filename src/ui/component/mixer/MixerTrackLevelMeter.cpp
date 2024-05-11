#include "MixerTrackLevelMeter.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

MixerTrackLevelMeter::MixerTrackLevelMeter() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forLevelMeter());
	this->setMouseCursor(juce::MouseCursor::NoCursor);
}

void MixerTrackLevelMeter::updateLevelMeter() {
	/** Get Value */
	this->values.clear();
	auto valuesTemp = quickAPI::getMixerTrackOutputLevel(this->index);
	for (auto i : valuesTemp) {
		this->values.add(utils::logRMS(i));
	}

	/** Repaint */
	this->repaint();

	/** Tooltip */
	juce::String tooltipStr;
	for (auto i : this->values) {
		tooltipStr += (juce::String{ i, 2 } + " dB, ");
	}
	this->setTooltip(tooltipStr);
}

void MixerTrackLevelMeter::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0025;
	int paddingHeight = screenSize.getHeight() * 0.005;

	int barNum = this->values.size();
	float splitWidth = screenSize.getWidth() * 0.0025 / barNum;
	float lineThickness = screenSize.getHeight() * 0.001;

	int textWidth = screenSize.getWidth() * 0.01;
	int textHeight = screenSize.getHeight() * 0.015;
	float textFontHeight = screenSize.getHeight() * 0.0125;

	int valueHeight = screenSize.getHeight() * 0.015;
	float valueFontHeight = screenSize.getHeight() * 0.014;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour valueColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Font */
	juce::Font textFont(textFontHeight);
	juce::Font valueFont(valueFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Text */
	constexpr float rmsNum = 60.f;
	constexpr auto rmsLine =
		std::to_array({ 0.f, 10.f, 20.f, 30.f, 40.f, 50.f, 60.f });

	juce::Rectangle<float> rmsArea(
		paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2,
		this->getHeight() - paddingHeight * 2);
	for (auto i : rmsLine) {
		float yPos = rmsArea.getY() + (i / rmsNum) * rmsArea.getHeight();

		juce::Rectangle<float> lLine(
			rmsArea.getX(), yPos - lineThickness / 2,
			rmsArea.getCentreX() - textWidth / 2 - rmsArea.getX(), lineThickness);
		juce::Rectangle<float> rLine(
			rmsArea.getCentreX() + textWidth / 2, yPos - lineThickness / 2,
			rmsArea.getRight() - (rmsArea.getCentreX() + textWidth / 2), lineThickness);
		g.setColour(textColor);
		g.fillRect(lLine);
		g.fillRect(rLine);

		juce::Rectangle<float> textRect(
			rmsArea.getCentreX() - textWidth / 2, yPos - textHeight / 2,
			textWidth, textHeight);
		g.setColour(textColor);
		g.setFont(textFont);
		g.drawFittedText(juce::String{ i, 0 }, textRect.toNearestInt(),
			juce::Justification::centred, 1, 0.f);
	}

	/** Bar */
	std::array<float, 3> levelSegs{ 0.66f, 0.86f, 1.f };
	std::array<juce::Colour, levelSegs.size()> levelColors{
		juce::Colours::green,
		juce::Colours::yellow,
		juce::Colours::red };

	if (barNum > 0) {
		float barWidth = (rmsArea.getWidth() - (barNum - 1) * splitWidth) / barNum;
		for (int i = 0; i < barNum; i++) {
			float value = this->values[i];
			float percent = std::max(utils::getLogLevelPercent(value, rmsNum), 0.f);

			for (int j = levelSegs.size() - 1; j >= 0; j--) {
				float height = std::min(percent, levelSegs[j]) * rmsArea.getHeight();
				juce::Rectangle<float> barRect(
					rmsArea.getX() + (barWidth + splitWidth) * i,
					rmsArea.getBottom() - height,
					barWidth, height);

				g.setColour(levelColors[j]);
				g.fillRect(barRect);
			}
		}
	}

	/** Cursor */
	if (this->mouseHovered) {
		float mouseY = this->mousePos.getY();
		float mouseX = this->mousePos.getX();
		if (mouseY >= rmsArea.getY()
			&& mouseY <= rmsArea.getBottom()) {
			float mouseValue = (mouseY - rmsArea.getY()) / rmsArea.getHeight() * rmsNum;

			juce::Rectangle<float> valueRect(
				rmsArea.getX(),
				(mouseY - valueHeight) >= rmsArea.getY() ? mouseY - valueHeight : mouseY,
				rmsArea.getWidth(), valueHeight);
			g.setColour(backgroundColor);
			g.fillRect(valueRect);

			g.setColour(valueColor);
			g.setFont(valueFont);
			g.drawFittedText(juce::String{ mouseValue, 1 }, valueRect.toNearestInt(),
				juce::Justification::centred, 1, 0.f);

			juce::Rectangle<float> cursorLine(
				rmsArea.getX(), mouseY - lineThickness / 2,
				rmsArea.getWidth(), lineThickness);
			g.setColour(valueColor);
			g.fillRect(cursorLine);

			juce::Rectangle<float> cursorPoint(
				mouseX - splitWidth / 2, mouseY - lineThickness / 2,
				splitWidth, lineThickness);
			g.setColour(backgroundColor);
			g.fillRect(cursorPoint);
		}
	}
}

void MixerTrackLevelMeter::mouseMove(
	const juce::MouseEvent& event) {
	this->mouseHovered = true;
	this->mousePos = event.getPosition();
}

void MixerTrackLevelMeter::mouseDrag(
	const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void MixerTrackLevelMeter::mouseExit(
	const juce::MouseEvent& event) {
	this->mouseHovered = false;
}

void MixerTrackLevelMeter::update(int index) {
	this->index = index;
}
