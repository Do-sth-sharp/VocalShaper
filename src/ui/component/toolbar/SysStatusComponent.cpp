#include "SysStatusComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/ConfigManager.h"
#include "../../misc/SysStatus.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SysStatusComponent::SysStatusComponent()
	: AnimatedAppComponent() {
	/** Animate */
	this->setFramesPerSecond(2);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::SysStatus));
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);

	/** Name */
	this->watchName[WatchType::CPU] = TRANS("CPU");
	this->watchName[WatchType::Audio] = TRANS("Audio");
	this->watchName[WatchType::Memory] = TRANS("RAM");
	this->watchName[WatchType::ProcessMem] = TRANS("Process RAM");

	/** Refresh */
	this->refresh();
}

void SysStatusComponent::refresh() {
	/** Get Config */
	juce::var& conf = ConfigManager::getInstance()->get("sysstat");

	/** Check Curve Type And Size */
	int curveType{ (int)conf["curve"] };
	if (curveType < 0 || curveType >= WatchType::TotalNum) {
		curveType = 0;
	}

	int curveSize = conf["points"];
	if (curveType != this->currentCurve || curveSize != this->curveData.size()) {
		this->currentCurve = (WatchType)curveType;
		this->clearCurve(curveSize);
	}

	/** Check Watch List */
	auto watchArray = conf["watch"].getArray();
	for (int i = 0; i < watchArray->size(); i++) {
		if (i < this->currentWatchList.size()) {
			int watchType{ (int)watchArray->getUnchecked(i) };
			if (watchType < 0 || watchType >= WatchType::TotalNum) {
				watchType = 0;
			}

			this->currentWatchList[i] = (WatchType)watchType;
		}
	}
}

void SysStatusComponent::update() {
	static SysStatus::CPUPercTemp cpuTemp;

	/** Get Data */
	this->watchData[WatchType::CPU] = SysStatus::getInstance()->getCPUUsage(cpuTemp);
	this->watchData[WatchType::Audio] = quickAPI::getCPUUsage();
	this->watchData[WatchType::Memory] = SysStatus::getInstance()->getMemUsage();
	this->watchData[WatchType::ProcessMem] = SysStatus::getInstance()->getProcMemUsage();

	/** Get Curve Data */
	this->addCurve(this->watchData[this->currentCurve]);
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
	juce::Font labelFont(juce::FontOptions{ (float)labelHeight });
	juce::Font textFont(juce::FontOptions{ (float)textHeight });

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Watch */
	for (int i = 0; i < this->currentWatchList.size(); i++) {
		juce::Rectangle<int> labelRect(
			this->getWidth() - watchWidth + watchPaddingWidth, i * (watchLabelHeight + watchTextHeight),
			watchWidth - watchPaddingWidth * 2, watchLabelHeight);
		juce::Rectangle<int> textRect(
			this->getWidth() - watchWidth + watchPaddingWidth, i * (watchLabelHeight + watchTextHeight) + watchLabelHeight,
			watchWidth - watchPaddingWidth * 2, watchTextHeight);

		WatchType type = this->currentWatchList[i];
		juce::String name = this->watchName[type];
		double value = this->watchData[type];

		g.setColour(labelColor);
		g.setFont(labelFont);
		g.drawFittedText(name, labelRect,
			juce::Justification::centredLeft, 1, 1.f);
		g.setColour(this->getAlert(type, value) ? alertColor : textColor);
		g.setFont(textFont);
		g.drawFittedText(this->getValueText(type, value), textRect,
			juce::Justification::centredRight, 1, 1.f);
	}

	/** Curve Label */
	auto [vMin, vMax] = this->getRange(this->currentCurve);
	int curveWidth = this->getWidth() - watchWidth;
	auto& curveName = this->watchName[this->currentCurve];

	juce::Rectangle<int> maxLabelRect(
		curveWidth - curvePaddingWidth - curveLabelWidth, 0,
		curveLabelWidth, watchLabelHeight);
	juce::Rectangle<int> minLabelRect(
		curveWidth - curvePaddingWidth - curveLabelWidth, this->getHeight() - watchLabelHeight,
		curveLabelWidth, watchLabelHeight);

	g.setColour(labelColor);
	g.setFont(labelFont);
	g.drawFittedText(
		this->getValueText(this->currentCurve, vMax), maxLabelRect,
		juce::Justification::centredRight, 1, 1.f);
	g.drawFittedText(
		this->getValueText(this->currentCurve, vMin), minLabelRect,
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
	g.drawFittedText(curveName, curveLabelRect,
		juce::Justification::centredLeft, 1, 1.f);
	g.setColour(this->getAlert(this->currentCurve, vTemp) ? alertColor : textColor);
	g.setFont(textFont);
	g.drawFittedText(
		this->getValueText(this->currentCurve, vTemp), curveTextRect,
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
		auto& curveName = this->watchName[this->currentCurve];
		this->setTooltip(curveName + ": " +
			this->getValueText(this->currentCurve, this->getCurve(this->curveData.size() - 1)));
	}
	/** Watch */
	else {
		int index = event.position.getY() / watchHeight;
		if (index < 0 || index >= this->currentWatchList.size()) {
			return;
		}

		WatchType type = this->currentWatchList[index];
		juce::String name = this->watchName[type];
		this->setTooltip(name + ": " +
			this->getValueText(type, this->watchData[type]));
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

std::tuple<double, double> SysStatusComponent::getRange(WatchType type) const {
	if (type == WatchType::ProcessMem) {
		double maxT = 0;
		for (auto i : this->curveData) {
			maxT = std::max(maxT, i);
		}
		return { 0.0 , maxT * 1.25 };
	}

	return { 0.0 , 1.0 };
}

bool SysStatusComponent::getAlert(
	WatchType type, double value) const {
	if (type == WatchType::ProcessMem) {
		return false;
	}

	return value >= 0.9;
}

juce::String SysStatusComponent::getValueText(
	WatchType type, double value) const {
	if (type == WatchType::ProcessMem) {
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
	auto menu = this->createMenu(this->currentCurve, true);
	int result = menu.show();

	/** Result */
	auto conf = ConfigManager::getInstance()->get("sysstat").getDynamicObject();
	if (!conf) { return; }
	switch (result) {
	case 110:
	case 120:
	case 150:
		conf->setProperty("points", result - 100);
		ConfigManager::getInstance()->saveConfig("sysstat");
		break;

	default:
		if (result >= 1 && result <= WatchType::TotalNum) {
			conf->setProperty("curve", result - 1);
			ConfigManager::getInstance()->saveConfig("sysstat");
		}
		break;
	}

	/** Refresh */
	this->refresh();
}

void SysStatusComponent::showWatchMenu(int index) {
	if (index < 0 || index >= this->currentWatchList.size()) { return; }

	/** Show Menu */
	auto menu = this->createMenu(this->currentWatchList[index], false);
	int result = menu.show();

	/** Set Func */
	auto setFunc = [index, size = this->currentWatchList.size()](WatchType type)->bool {
		auto conf = ConfigManager::getInstance()->get("sysstat").getDynamicObject();
		if (!conf) { return false; }

		auto& array = conf->getProperty("watch");
		auto ptrArray = array.getArray();
		if (!ptrArray) { return false; }
		if (ptrArray->size() < size) {
			ptrArray->resize(size);
		}

		ptrArray->set(index, (int)type);
		conf->setProperty("watch", juce::var{ *ptrArray });
		ConfigManager::getInstance()->saveConfig("sysstat");
		return true;
	};

	/** Result */
	if (result >= 1 && result <= WatchType::TotalNum) {
		setFunc((WatchType)(result - 1));
	}

	/** Refresh */
	this->refresh();
}

juce::PopupMenu SysStatusComponent::createMenu(WatchType type, bool isCurve) {
	juce::PopupMenu menu;

	for (int i = 0; i < WatchType::TotalNum; i++) {
		menu.addItem(i + 1, this->watchName[i], true, type == i);
	}

	menu.addSeparator();

	int currentPoints = this->curveData.size();
	menu.addItem(110, "10", isCurve, currentPoints == 10);
	menu.addItem(120, "20", isCurve, currentPoints == 20);
	menu.addItem(150, "50", isCurve, currentPoints == 50);

	return menu;
}

