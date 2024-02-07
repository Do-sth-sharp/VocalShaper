#include "FaderBase.h"
#include "../Utils.h"

FaderBase::FaderBase(double defaultValue, const juce::Array<double>& hotDBValues,
	double minValue, double maxValue, int numberOfDecimalPlaces)
	: defaultValue(defaultValue),
	minValue(minValue), maxValue(maxValue),
	numberOfDecimalPlaces(numberOfDecimalPlaces),
	hotDBValues(hotDBValues),
	hotDBStrs(FaderBase::createDBStr(hotDBValues, numberOfDecimalPlaces)),
	hotLinearValues(FaderBase::createLinearValue(hotDBValues)),
	hotDisplayPercents(FaderBase::createDisplayPercent(hotLinearValues, minValue, maxValue)) {
	this->setValue(defaultValue);
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void FaderBase::setValue(double value, bool sendChange) {
	this->value = this->limitValue(value);
	this->valuePercent = FaderBase::convertLinearToDisplayPercent(
		this->value, this->minValue, this->maxValue);

	this->valueStr = juce::String{
		FaderBase::convertLinearToDB(this->value), this->numberOfDecimalPlaces + 1 };

	if (sendChange && this->onChange) {
		this->onChange(this->value);
	}

	this->repaint();

	this->setTooltip(this->valueStr + "dB");
}

double FaderBase::getValue() const {
	return this->value;
}

void FaderBase::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.00175;
	int paddingHeight = screenSize.getHeight() * 0.002;

	float lineThickness = screenSize.getHeight() * 0.001;
	float blockHeight = screenSize.getHeight() * 0.02;
	float blockWidth = screenSize.getWidth() * 0.008;

	int textShownWidth = screenSize.getWidth() * 0.02;
	bool textShown = this->getWidth() >= textShownWidth;
	int textWidth = screenSize.getWidth() * 0.0085;
	int textHeight = screenSize.getHeight() * 0.0125;
	float textFontHeight = screenSize.getHeight() * 0.01;

	float lineSplitWidth = screenSize.getWidth() * 0.004;

	int valueHeight = screenSize.getHeight() * 0.02;
	float valueFontHeight = screenSize.getHeight() * 0.0175;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour lineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(textFontHeight);
	juce::Font valueFont(valueFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Track */
	juce::Line<float> trackLine(
		paddingWidth + blockWidth / 2, paddingHeight + blockHeight / 2,
		paddingWidth + blockWidth / 2, this->getHeight() - paddingHeight - blockHeight / 2);
	g.setColour(lineColor);
	g.drawLine(trackLine, lineThickness);

	/** Value Line */
	for (int i = 0; i < this->hotDisplayPercents.size(); i++) {
		double percent = this->hotDisplayPercents[i];
		float y = paddingHeight + blockHeight / 2 + (1 - percent) * (this->getHeight() - paddingHeight * 2 - blockHeight);

		juce::Line<float> valueLine(
			paddingWidth + blockWidth / 2 + lineSplitWidth, y,
			this->getWidth() - paddingWidth - (textShown ? textWidth : 0), y);
		g.setColour(textColor);
		g.drawLine(valueLine, lineThickness);

		if (textShown) {
			juce::String valueText = this->hotDBStrs[i];

			juce::Rectangle<float> textRect(
				this->getWidth() - paddingWidth - textWidth, y - textHeight / 2,
				textWidth, textHeight);
			g.setColour(textColor);
			g.setFont(textFont);
			g.drawFittedText(valueText, textRect.toNearestInt(),
				juce::Justification::centred, 1, 0.f);
		}
	}

	/** Block */
	float cursorY = paddingHeight + blockHeight / 2 + (1 - this->valuePercent) * (this->getHeight() - paddingHeight * 2 - blockHeight);
	juce::Rectangle<float> blockRect(
		paddingWidth, cursorY - blockHeight / 2,
		blockWidth, blockHeight);
	g.setColour(backgroundColor);
	g.fillRect(blockRect);

	juce::Line<float> cursorLine(
		paddingWidth, cursorY,
		paddingWidth + blockWidth, cursorY);
	g.setColour(textColor);
	g.drawLine(cursorLine, lineThickness);

	g.setColour(textColor);
	g.drawRect(blockRect, lineThickness);

	/** Value */
	if (this->pressed) {
		juce::Rectangle<float> valueRect(
			paddingWidth,
			(blockRect.getY() - valueHeight >= paddingHeight)
			? blockRect.getY() - valueHeight : blockRect.getBottom(),
			this->getWidth() - paddingWidth * 2, valueHeight);
		
		g.setColour(backgroundColor);
		g.fillRect(valueRect);
		
		g.setColour(textColor);
		g.setFont(valueFont);
		g.drawFittedText(this->valueStr, valueRect.toNearestInt(),
			juce::Justification::centred, 1, 0.f);
	}
}

void FaderBase::mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->pressed = true;
	}
	else if (event.mods.isRightButtonDown()) {
		this->pressed = true;
		this->setValue(this->defaultValue, true);
	}
}

void FaderBase::mouseDrag(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()
		&& event.mouseWasDraggedSinceMouseDown()) {
		/** Size */
		auto screenSize = utils::getScreenSize(this);
		int paddingHeight = screenSize.getHeight() * 0.002;
		float blockHeight = screenSize.getHeight() * 0.02;

		/** Value */
		int y = event.getPosition().getY();
		double percent = 1 - (y - paddingHeight - blockHeight / 2.0) / (this->getHeight() - paddingHeight * 2.0 - blockHeight);
		if (percent > 1) { percent = 1; }
		if (percent < 0) { percent = 0; }
		double value = FaderBase::convertDisplayPercentToLinear(percent, this->minValue, this->maxValue);

		this->setValue(value, true);
	}
}

void FaderBase::mouseUp(const juce::MouseEvent& event) {
	this->pressed = false;
	this->repaint();
}

void FaderBase::mouseExit(const juce::MouseEvent& event) {
	this->pressed = false;
	this->repaint();
}

double FaderBase::limitValue(double value) const {
	value = std::min(value, this->maxValue);
	value = std::max(value, this->minValue);
	return value;
}

const juce::StringArray FaderBase::createDBStr(
	const juce::Array<double>& values, int numberOfDecimalPlaces) {
	juce::StringArray result;
	for (auto i : values) {
		result.add(juce::String{ i, numberOfDecimalPlaces }.trimCharactersAtStart("-"));
	}
	return result;
}

const juce::Array<double> FaderBase::createLinearValue(
	const juce::Array<double>& values) {
	juce::Array<double> result;
	for (auto i : values) {
		result.add(FaderBase::convertDBToLinear(i));
	}
	return result;
}

const juce::Array<double> FaderBase::createDisplayPercent(
	const juce::Array<double>& values, double minValue, double maxValue) {
	juce::Array<double> result;
	for (auto i : values) {
		result.add(FaderBase::convertLinearToDisplayPercent(
			i, minValue, maxValue));
	}
	return result;
}

double FaderBase::convertDBToLinear(double value) {
	return std::pow(10.0, value / 20);
}

double FaderBase::convertLinearToDB(double value) {
	return 20 * std::log10(value);
}

double FaderBase::convertLinearToDisplayPercent(
	double value, double minValue, double maxValue) {
	double per = (value - minValue) / (maxValue - minValue);
	return std::pow(per, 0.25);
}

double FaderBase::convertDisplayPercentToLinear(
	double value, double minValue, double maxValue) {
	double per = std::pow(value, 4.0);
	return minValue + per * (maxValue - minValue);
}
