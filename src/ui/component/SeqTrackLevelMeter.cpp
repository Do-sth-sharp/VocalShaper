#include "SeqTrackLevelMeter.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTrackLevelMeter::SeqTrackLevelMeter() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forLevelMeter());
}

void SeqTrackLevelMeter::updateLevelMeter() {
	/** Get Value */
	this->values.clear();
	auto valuesTemp = quickAPI::getSeqTrackOutputLevel(this->index);
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

void SeqTrackLevelMeter::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0025;
	int paddingHeight = screenSize.getHeight() * 0.0025;

	int barNum = this->values.size();
	float splitHeight = screenSize.getHeight() * 0.0035 / barNum;
	float lineThickness = screenSize.getHeight() * 0.001;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

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
		float xPos = rmsArea.getRight() - (i / rmsNum) * rmsArea.getWidth();

		juce::Rectangle<float> line(
			xPos - lineThickness / 2, rmsArea.getY(),
			lineThickness, rmsArea.getHeight());
		g.setColour(textColor);
		g.fillRect(line);
	}

	/** Bar */
	std::array<float, 3> levelSegs{ 0.66f, 0.86f, 1.f };
	std::array<juce::Colour, levelSegs.size()> levelColors{
		juce::Colours::green,
		juce::Colours::yellow,
		juce::Colours::red };

	if (barNum > 0) {
		float barHeight = (rmsArea.getHeight() - (barNum - 1) * splitHeight) / barNum;
		for (int i = 0; i < barNum; i++) {
			float value = this->values[i];
			float percent = std::max(utils::getLogLevelPercent(value, rmsNum), 0.f);

			for (int j = levelSegs.size() - 1; j >= 0; j--) {
				float width = std::min(percent, levelSegs[j]) * rmsArea.getWidth();
				juce::Rectangle<float> barRect(
					rmsArea.getX(),
					rmsArea.getY() + (barHeight + splitHeight) * i,
					width, barHeight);

				g.setColour(levelColors[j]);
				g.fillRect(barRect);
			}
		}
	}
}

void SeqTrackLevelMeter::update(int index) {
	this->index = index;
}
