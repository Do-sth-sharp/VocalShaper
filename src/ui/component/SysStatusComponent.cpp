#include "SysStatusComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/ConfigManager.h"
#include "../misc/SysStatus.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SysStatusComponent::SysStatusComponent()
	: AnimatedAppComponent() {
	/** Animate */
	this->setFramesPerSecond(2);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSysStatus());

	/** Translate */
	this->nameTrans.insert(std::make_pair("cpu", TRANS("cpu")));
	this->nameTrans.insert(std::make_pair("cpu-audio", TRANS("cpu-audio")));
	this->nameTrans.insert(std::make_pair("mem", TRANS("mem")));
	this->nameTrans.insert(std::make_pair("mem-process", TRANS("mem-process")));
}

void SysStatusComponent::update() {
	/** Get Config */
	juce::var& conf = ConfigManager::getInstance()->get("sysstat");

	/** Check Curve Name And Size */
	juce::String curveName = conf["curve"].toString();
	int curveSize = conf["points"];
	if (curveName != this->curveName || curveSize != this->curveData.size()) {
		this->curveName = curveName;
		this->clearCurve(curveSize);
	}

	/** Get Curve Data */
	this->addCurve(this->getData(curveName));

	/** Get Watch Name */
	auto watchArray = conf["watch"].getArray();
	for (int i = 0; i < watchArray->size(); i++) {
		if (i < this->watchName.size()) {
			this->watchName[i] = watchArray->getUnchecked(i).toString();
		}
	}

	/** Get Watch Data */
	for (int i = 0; i < this->watchName.size(); i++) {
		if (i < this->watchData.size()) {
			this->watchData[i] = this->getData(this->watchName[i]);
		}
	}
}

void SysStatusComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();
	auto screenSize = utils::getScreenSize(this);

	/** Size */
	int watchLabelHeight = this->getHeight() / 6;
	int watchTextHeight = this->getHeight() / 6;
	int watchWidth = this->getHeight() * 1.25;
	int watchPaddingWidth = this->getHeight() * 0.1;
	int curvePaddingWidth = this->getHeight() * 0.05;
	int curveLabelWidth = this->getHeight() * 1.0;
	int labelHeight = watchLabelHeight;
	int textHeight = watchTextHeight;
	float curveThickness = this->getHeight() * 0.01;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour labelColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour curveColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour alertColor = juce::Colours::red;

	/** Font */
	juce::Font labelFont(labelHeight);
	juce::Font textFont(textHeight);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Watch */
	for (int i = 0; i < this->watchData.size() && i < this->watchName.size(); i++) {
		juce::Rectangle<int> labelRect(
			this->getWidth() - watchWidth + watchPaddingWidth, i * (watchLabelHeight + watchTextHeight),
			watchWidth - watchPaddingWidth * 2, watchLabelHeight);
		juce::Rectangle<int> textRect(
			this->getWidth() - watchWidth + watchPaddingWidth, i * (watchLabelHeight + watchTextHeight) + watchLabelHeight,
			watchWidth - watchPaddingWidth * 2, watchTextHeight);

		juce::String name = this->watchName[i];
		double value = this->watchData[i];

		g.setColour(labelColor);
		g.setFont(labelFont);
		g.drawFittedText(this->nameTrans[name], labelRect,
			juce::Justification::centredLeft, 1, 1.f);
		g.setColour(this->getAlert(name, value) ? alertColor : textColor);
		g.setFont(textFont);
		g.drawFittedText(this->getValueText(name, value), textRect,
			juce::Justification::centredRight, 1, 1.f);
	}

	/** Curve Label */
	auto [vMin, vMax] = this->getRange();
	int curveWidth = this->getWidth() - watchWidth;

	juce::Rectangle<int> maxLabelRect(
		curveWidth - curvePaddingWidth - curveLabelWidth, 0,
		curveLabelWidth, watchLabelHeight);
	juce::Rectangle<int> minLabelRect(
		curveWidth - curvePaddingWidth - curveLabelWidth, this->getHeight() - watchLabelHeight,
		curveLabelWidth, watchLabelHeight);

	g.setColour(labelColor);
	g.setFont(labelFont);
	g.drawFittedText(
		this->getValueText(this->curveName, vMax), maxLabelRect,
		juce::Justification::centredRight, 1, 1.f);
	g.drawFittedText(
		this->getValueText(this->curveName, vMin), minLabelRect,
		juce::Justification::centredRight, 1, 1.f);

	/** Curve */
	double vTemp = 0;
	if (this->curveData.size() > 0) {
		juce::Path curvePath;
		curvePath.startNewSubPath(
			0, this->getHeight() - ((vTemp = this->getCurve(0)) - vMin) / (vMax - vMin) * this->getHeight());
		for (int i = 1; i < this->curveData.size(); i++) {
			curvePath.lineTo(
				((double)i / (this->curveData.size() - 1)) * curveWidth,
				this->getHeight() - ((vTemp = this->getCurve(i)) - vMin) / (vMax - vMin) * this->getHeight());
		}

		g.setColour(curveColor);
		g.strokePath(curvePath, juce::PathStrokeType{ curveThickness });
	}

	/** Curve Value */
	juce::Rectangle<int> curveLabelRect(
		curvePaddingWidth, 0,
		curveLabelWidth, watchLabelHeight);
	juce::Rectangle<int> curveTextRect(
		curvePaddingWidth, watchLabelHeight,
		curveLabelWidth, watchTextHeight);
	g.setColour(labelColor);
	g.setFont(labelFont);
	g.drawFittedText(
		this->nameTrans[this->curveName], curveLabelRect,
		juce::Justification::centredLeft, 1, 1.f);
	g.setColour(this->getAlert(this->curveName, vTemp) ? alertColor : textColor);
	g.setFont(textFont);
	g.drawFittedText(
		this->getValueText(this->curveName, vTemp), curveTextRect,
		juce::Justification::centredLeft, 1, 1.f);
}

void SysStatusComponent::resized() {
	/** Nothing To Do */
}

void SysStatusComponent::clearCurve(int size) {
	this->curveData.resize(size);
	for (int i = 0; i < size; i++) {
		this->curveData[i] = 0;
	}
	this->curveHead = 0;
}

void SysStatusComponent::addCurve(double data) {
	if (this->curveData.size() == 0) { return; }

	this->curveData[this->curveHead] = data;

	this->curveHead++;
	if (this->curveHead >= this->curveData.size()) { this->curveHead = 0; }
}

double SysStatusComponent::getCurve(int index) {
	if (index < 0 || index >= this->curveData.size()) { return 0; }

	int realIndex = this->curveHead + index;
	if (realIndex >= this->curveData.size()) { realIndex -= this->curveData.size(); }

	return this->curveData[realIndex];
}

double SysStatusComponent::getData(const juce::String& name) const {
	static SysStatus::CPUPercTemp cpuTemp;

	if (name == "cpu") { return SysStatus::getInstance()->getCPUUsage(cpuTemp); }
	else if (name == "cpu-audio") { return quickAPI::getCPUUsage(); }
	else if (name == "mem") { return SysStatus::getInstance()->getMemUsage(); }
	else if (name == "mem-process") { return SysStatus::getInstance()->getProcMemUsage(); }

	return 0;
}

std::tuple<double, double> SysStatusComponent::getRange() const {
	if (this->curveName == "mem-process") {
		double maxT = 0;
		for (auto i : this->curveData) {
			maxT = std::max(maxT, i);
		}
		return { 0.0 , maxT * 1.25 };
	}

	return { 0.0 , 1.0 };
}

bool SysStatusComponent::getAlert(
	const juce::String& name, double value) const {
	if (name == "mem-process") {
		return false;
	}

	return value >= 0.9;
}

juce::String SysStatusComponent::getValueText(
	const juce::String& name, double value) const {
	if (name == "mem-process") {
		if (value < 1024) {
			return juce::String{ value, 2, false } + "B";
		}
		if (value < (1024 * 1024)) {
			return juce::String{ value / 1024, 2, false } + "KB";
		}
		if (value < (1024 * 1024 * 1024)) {
			return juce::String{ value / (1024 * 1024), 2, false } + "MB";
		}
		if (value < (1024 * 1024 * 1024 * 1024)) {
			return juce::String{ value / (1024 * 1024 * 1024), 2, false } + "GB";
		}
		return juce::String{ value / (1024 * 1024 * 1024 * 1024), 2, false } + "TB";
	}

	return juce::String{ value * 100, 2, false } + "%";
}
