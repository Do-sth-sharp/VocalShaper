#include "KnobBase.h"
#include "../../Utils.h"

#define PI 3.14159265//3589793238462643383279502884197169399375105

KnobBase::KnobBase(const juce::String& name, double defaultValue,
	double minValue, double maxValue, int numberOfDecimalPlaces)
	: name(name), defaultValue(defaultValue),
	minValue(minValue), maxValue(maxValue),
	numberOfDecimalPlaces(numberOfDecimalPlaces) {
	this->setValue(defaultValue);
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void KnobBase::setValue(double value, bool sendChange) {
	this->value = this->limitValue(value);

	this->valueStr = juce::String{ this->value, this->numberOfDecimalPlaces };

	if (sendChange && this->onChange) {
		this->onChange(this->value);
	}

	this->repaint();

	this->setTooltip(this->name + " " + this->valueStr);
}

double KnobBase::getValue() const {
	return this->value;
}

void KnobBase::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0025;
	int paddingHeight = screenSize.getHeight() * 0.005;

	float lineThickness = screenSize.getHeight() * 0.0015;

	int nameHeight = screenSize.getHeight() * 0.015;
	float nameFontHeight = screenSize.getHeight() * 0.0125;
	int valueHeight = screenSize.getHeight() * 0.02;
	float valueFontHeight = screenSize.getHeight() * 0.015;

	float r = std::min(this->getWidth() - paddingWidth * 2,
		this->getHeight() - paddingHeight * 2 - valueHeight - nameHeight / 2) * 0.5f;
	
	constexpr float invalidRad = PI / 2;
	float nameXOffset = r * std::sin(invalidRad / 2);
	float nameYOffset = r * std::cos(invalidRad / 2);

	juce::Point<float> rCenter(
		this->getWidth() * 0.5f,
		this->getHeight() * 0.5f - (r + nameYOffset + valueHeight + nameHeight * 0.5f) * 0.5f + r);

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour lineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);
	juce::Colour nameColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour valueColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font nameFont(nameFontHeight);
	juce::Font valueFont(valueFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Circle */
	juce::Rectangle<float> circleRect(
		rCenter.getX() - r, rCenter.getY() - r, r * 2, r * 2);
	g.setColour(lineColor);
	g.drawEllipse(circleRect, lineThickness);

	/** Name */
	juce::Rectangle<float> nameRect(
		rCenter.getX() - nameXOffset,
		rCenter.getY() + nameYOffset - nameHeight / 2,
		nameXOffset * 2, nameHeight);
	g.setColour(backgroundColor);
	g.fillRect(nameRect);

	g.setColour(nameColor);
	g.setFont(nameFont);
	g.drawFittedText(this->name, nameRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);

	/** Cursor */
	float cursorRad = invalidRad / 2 + ((this->value - this->minValue) / (this->maxValue - this->minValue)) * (PI * 2 - invalidRad);
	juce::Line<float> cursorLine(
		rCenter.getX(), rCenter.getY(),
		rCenter.getX() - r * std::sin(cursorRad),
		rCenter.getY() + r * std::cos(cursorRad));
	g.setColour(lineColor);
	g.drawLine(cursorLine, lineThickness);

	/** Value */
	juce::Rectangle<float> valueRect(
		paddingWidth, rCenter.getY() + nameYOffset + nameHeight / 2,
		this->getWidth() - paddingWidth * 2, valueHeight);
	g.setColour(valueColor);
	g.setFont(valueFont);
	g.drawFittedText(this->valueStr, valueRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void KnobBase::mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->pressedValue = this->value;
	}
	else if (event.mods.isRightButtonDown()) {
		this->setValue(this->defaultValue, true);
	}
}

void KnobBase::mouseDrag(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()
		&& event.mouseWasDraggedSinceMouseDown()) {
		int dy = event.getDistanceFromDragStartY();

		double dValue = -dy * (this->maxValue - this->minValue) / 200;

		this->setValue(this->pressedValue + dValue, true);
	}
}

double KnobBase::limitValue(double value) const {
	value = std::min(value, this->maxValue);
	value = std::max(value, this->minValue);
	return value;
}
