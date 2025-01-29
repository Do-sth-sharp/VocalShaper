#include "SeqTrackLevelMeter.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SeqTrackLevelMeter::SeqTrackLevelMeter() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::LevelMeter));
}

void SeqTrackLevelMeter::updateLevelMeter() {
	/** Get Value */
	auto valuesTemp = quickAPI::getTrackSeqOutputLevel(
		{ quickAPI::TrackType::Track, this->index });
	if (this->values.size() == valuesTemp.size()) {
		this->values.clearQuick();
	}
	else {
		this->values.clear();
	}
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
	float paddingWidth = screenSize.getWidth() * 0.002;
	float paddingHeight = screenSize.getHeight() * 0.002;

	int barNum = this->values.size();
	float splitWidth = screenSize.getWidth() * 0.0025 / barNum;
	float lineThickness = screenSize.getHeight() * 0.00075;

	int showRMSLineHeight = screenSize.getHeight() * 0.085;
	bool shouldShowRMSLine = (this->getHeight() >= showRMSLineHeight);

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
	if (shouldShowRMSLine) {
		for (auto i : rmsLine) {
			float yPos = rmsArea.getY() + (i / rmsNum) * rmsArea.getHeight();

			juce::Rectangle<float> line(
				rmsArea.getX(), yPos - lineThickness / 2,
				rmsArea.getWidth(), lineThickness);
			g.setColour(textColor);
			g.fillRect(line);
		}
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
}

void SeqTrackLevelMeter::updateIndex(int index) {
	this->index = index;
}

void SeqTrackLevelMeter::update() {
	/** Nothing To Do */
}
