#include "SysStatusComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/ConfigManager.h"
#include "../misc/SysStatus.h"
#include "../../audioCore/AC_API.h"

SysStatusComponent::SysStatusComponent()
	: AnimatedAppComponent() {
	/** Animate */
	this->setFramesPerSecond(2);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSysStatus());
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

	/** BackGround */
	g.setColour(laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId));
	g.fillAll();
}

void SysStatusComponent::resized() {
	/** TODO */
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
	if (this->curveName == "cpu") { return { 0.0 , 1.0 }; }
	else if (this->curveName == "cpu-audio") { return { 0.0 , 1.0 }; }
	else if (this->curveName == "mem") { return { 0.0 , 1.0 }; }
	else if (this->curveName == "mem-process") {
		double maxT = 0;
		for (auto i : this->curveData) {
			maxT = std::max(maxT, i);
		}
		return { 0.0 , maxT * 1.25 };
	}

	return { 0.0 , 1.0 };
}
