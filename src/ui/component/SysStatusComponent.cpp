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
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);

	/** Translate */
	this->nameTrans.insert(std::make_pair("cpu", TRANS("cpu")));
	this->nameTrans.insert(std::make_pair("audio", TRANS("audio")));
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
		juce::Label::ColourIds::outlineWhenEditingColourId);
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

void SysStatusComponent::mouseUp(const juce::MouseEvent& event) {
	/** Size */
	int watchWidth = this->getHeight() * 1.25;
	int watchHeight = this->getHeight() / 3;

	/** Right Button */
	if (event.mods.isRightButtonDown()) {
		if (event.position.getX() < (this->getWidth() - watchWidth)) {
			this->showCurveMenu();
		}
		else {
			this->showWatchMenu(event.position.getY() / watchHeight);
		}
		return;
	}
}

void SysStatusComponent::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	int watchWidth = this->getHeight() * 1.25;
	int watchHeight = this->getHeight() / 3;

	/** Curve */
	if (event.position.getX() < (this->getWidth() - watchWidth)) {
		juce::String name = this->curveName;
		this->setTooltip(this->nameTrans[name] + ": " +
			this->getValueText(name, this->getCurve(this->curveData.size() - 1)));
	}
	/** Watch */
	else {
		int index = event.position.getY() / watchHeight;
		if (index < 0 || index >= this->watchName.size()
			|| index >= this->watchData.size()) {
			return;
		}

		juce::String name = this->watchName[index];
		this->setTooltip(this->nameTrans[name] + ": " +
			this->getValueText(name, this->watchData[index]));
	}
}

void SysStatusComponent::mouseExit(const juce::MouseEvent& /*event*/) {
	this->setTooltip("");
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
	else if (name == "audio") { return quickAPI::getCPUUsage(); }
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
		if (value < (uint64_t)1024) {
			return juce::String{ value, 2, false } + "B";
		}
		if (value < ((uint64_t)1024 * 1024)) {
			return juce::String{ value / (uint64_t)1024, 2, false } + "KB";
		}
		if (value < ((uint64_t)1024 * 1024 * 1024)) {
			return juce::String{ value / ((uint64_t)1024 * 1024), 2, false } + "MB";
		}
		if (value < ((uint64_t)1024 * 1024 * 1024 * 1024)) {
			return juce::String{ value / ((uint64_t)1024 * 1024 * 1024), 2, false } + "GB";
		}
		return juce::String{ value / ((uint64_t)1024 * 1024 * 1024 * 1024), 2, false } + "TB";
	}

	return juce::String{ value * 100, 2, false } + "%";
}

void SysStatusComponent::showCurveMenu() {
	/** Show Menu */
	auto menu = this->createMenu(this->curveName, this->curveData.size(), true);
	int result = menu.show();

	/** Result */
	auto conf = ConfigManager::getInstance()->get("sysstat").getDynamicObject();
	if (!conf) { return; }
	switch (result) {
	case 1:
		conf->setProperty("curve", "cpu");
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;
	case 2:
		conf->setProperty("curve", "audio");
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;
	case 3:
		conf->setProperty("curve", "mem");
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;
	case 4:
		conf->setProperty("curve", "mem-process");
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;

	case 101:
		conf->setProperty("points", 10);
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;
	case 102:
		conf->setProperty("points", 20);
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;
	case 103:
		conf->setProperty("points", 50);
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;
	}
}

void SysStatusComponent::showWatchMenu(int index) {
	if (index < 0 || index >= this->watchName.size()
		|| index >= this->watchData.size()) { return; }

	/** Show Menu */
	auto menu = this->createMenu(this->watchName[index], 0, false);
	int result = menu.show();

	/** Set Func */
	auto setFunc = [index, size = this->watchName.size()](const juce::String& name)->bool {
		auto conf = ConfigManager::getInstance()->get("sysstat").getDynamicObject();
		if (!conf) { return false; }

		auto& array = conf->getProperty("watch");
		auto ptrArray = array.getArray();
		if (!ptrArray) { return false; }
		if (ptrArray->size() < size) {
			ptrArray->resize(size);
		}

		ptrArray->set(index, name);
		conf->setProperty("watch", juce::var{ *ptrArray });
		ConfigManager::getInstance()->saveConfig("sysstat");
		return true;
	};

	/** Result */
	switch (result) {
	case 1:
		if (!setFunc("cpu")) { return; }
		break;
	case 2:
		if (!setFunc("audio")) { return; }
		break;
	case 3:
		if (!setFunc("mem")) { return; }
		break;
	case 4:
		if (!setFunc("mem-process")) { return; }
		break;
	}
}

juce::PopupMenu SysStatusComponent::createMenu(const juce::String& currentName,
	int currentPoints, bool isCurve) {
	juce::PopupMenu menu;

	menu.addItem(1, this->nameTrans["cpu"], true, currentName == "cpu", nullptr);
	menu.addItem(2, this->nameTrans["audio"], true, currentName == "audio", nullptr);
	menu.addItem(3, this->nameTrans["mem"], true, currentName == "mem", nullptr);
	menu.addItem(4, this->nameTrans["mem-process"], true, currentName == "mem-process", nullptr);

	menu.addSeparator();

	menu.addItem(101, "10", isCurve, currentPoints == 10, nullptr);
	menu.addItem(102, "20", isCurve, currentPoints == 20, nullptr);
	menu.addItem(103, "50", isCurve, currentPoints == 50, nullptr);

	return menu;
}

