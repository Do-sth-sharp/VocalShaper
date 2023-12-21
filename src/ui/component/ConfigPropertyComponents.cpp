#include "ConfigPropertyComponents.h"
#include "../misc/ConfigManager.h"
#include "../Utils.h"

#define PROP_HEIGHT 0.04

ConfigPropHelper::ConfigPropHelper(
	const juce::String& className,
	const juce::String& propName,
	const UpdateCallback& updateCallback,
	const GetValueCallback& getValueCallback)
	: className(className), propName(propName),
	updateCallback(updateCallback),
	getValueCallback(getValueCallback) {
	/** Nothing To Do */
}

const juce::var ConfigPropHelper::get() const {
	if (this->getValueCallback) { return this->getValueCallback(); }
	return ConfigManager::getInstance()
		->get(this->className)[juce::Identifier{ this->propName }];
}

bool ConfigPropHelper::update(const juce::var& data) {
	auto& classVar = ConfigManager::getInstance()->get(this->className);
	if (!classVar.isObject()) { return false; }
	auto classObj = classVar.getDynamicObject();
	if (!classObj) { return false; }

	if (!this->updateCallback(data)) { return false; }
	classObj->setProperty(this->propName, data);
	return true;
}

bool ConfigPropHelper::save() {
	return ConfigManager::getInstance()->saveConfig(this->className);
}

bool ConfigPropHelper::updateThenSave(const juce::var& data) {
	return this->update(data) && this->save();
}

ConfigBooleanProp::ConfigBooleanProp(
	const juce::String& className,
	const juce::String& propName,
	const juce::String& buttonOffName,
	const juce::String& buttonOnName,
	const UpdateCallback& updateCallback,
	const GetValueCallback& getValueCallback)
	: BooleanPropertyComponent(
		TRANS(propName), TRANS(buttonOnName), TRANS(buttonOffName)),
	ConfigPropHelper(className, propName,
		updateCallback, getValueCallback) {
	/** Init State */
	juce::MessageManager::callAsync(
		[ptr = juce::PropertyComponent::SafePointer(this)] {
			if (ptr) {
				ptr->refresh();
			}
		});
}

void ConfigBooleanProp::setState(bool newState) {
	if (newState == this->getState()) { return; }

	this->updateThenSave(newState);
	this->refresh();
}

bool ConfigBooleanProp::getState() const {
	return this->get();
}

void ConfigBooleanProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(prefHeight);

	this->juce::BooleanPropertyComponent::resized();
}

ConfigButtonProp::ConfigButtonProp(
	const juce::String& propName,
	const juce::String& buttonText,
	const ActiveFunc& activeFunc)
	: PropertyComponent(TRANS(propName)),
	buttonText(TRANS(buttonText)), activeFunc(activeFunc) {
	this->addAndMakeVisible(this->button);
	this->button.setTriggeredOnMouseDown(false);
	this->button.setWantsKeyboardFocus(false);
	this->button.onClick = [this] { this->buttonClicked(); };
}

void ConfigButtonProp::buttonClicked() {
	this->activeFunc();
}

juce::String ConfigButtonProp::getButtonText() const {
	return this->buttonText;
}

void ConfigButtonProp::refresh() {
	this->button.setButtonText(this->getButtonText());
}

void ConfigButtonProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(prefHeight);

	this->juce::PropertyComponent::resized();
}

ConfigChoiceProp::ConfigChoiceProp(
	const juce::String& className,
	const juce::String& propName,
	const juce::StringArray& choices,
	ValueType valueType,
	const UpdateCallback& updateCallback,
	const GetValueCallback& getValueCallback)
	: ChoicePropertyComponent(TRANS(propName)),
	ConfigPropHelper(className, propName,
		updateCallback, getValueCallback),
	choicesValue(choices), valueType(valueType) {
	/** Init Choices */
	this->choices = ConfigChoiceProp::transChoices(choices);

	/** Init State */
	juce::MessageManager::callAsync(
		[ptr = juce::PropertyComponent::SafePointer(this)] {
			if (ptr) {
				ptr->refresh();
			}
		});
}

void ConfigChoiceProp::setIndex(int newIndex) {
	if (newIndex < 0 || newIndex > this->choicesValue.size()) { return; }
	if (newIndex == this->getIndex()) { return; }

	if (this->valueType == ValueType::IndexVal) {
		this->updateThenSave(newIndex);
	}
	else if (this->valueType == ValueType::NameVal) {
		this->updateThenSave(this->choicesValue[newIndex]);
	}

	this->refresh();
}

int ConfigChoiceProp::getIndex() const {
	auto& val = this->get();
	if (this->valueType == ValueType::IndexVal) {
		return val;
	}
	else if (this->valueType == ValueType::NameVal) {
		return this->choicesValue.indexOf(val.toString());
	}
	return -1;
}

void ConfigChoiceProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(prefHeight);

	this->juce::ChoicePropertyComponent::resized();
}

const juce::StringArray ConfigChoiceProp::transChoices(const juce::StringArray& choices) {
	juce::StringArray result;
	for (auto& s : choices) {
		result.add(TRANS(s));
	}
	return result;
}

ConfigSliderProp::ConfigSliderProp(
	const juce::String& className,
	const juce::String& propName,
	double rangeMin, double rangeMax, double interval,
	double skewFactor, bool symmetricSkew,
	const UpdateCallback& updateCallback,
	const GetValueCallback& getValueCallback)
	: SliderPropertyComponent(TRANS(propName),
		rangeMin, rangeMax, interval, skewFactor, symmetricSkew),
	ConfigPropHelper(className, propName,
		updateCallback, getValueCallback) {
	/** Init State */
	juce::MessageManager::callAsync(
		[ptr = juce::PropertyComponent::SafePointer(this)] {
			if (ptr) {
				ptr->refresh();
			}
		});
}

void ConfigSliderProp::setValue(double newValue) {
	if (newValue == this->getValue()) { return; }

	this->updateThenSave(newValue);
	this->refresh();
}

double ConfigSliderProp::getValue() const {
	return this->get();
}

void ConfigSliderProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(prefHeight);

	this->juce::SliderPropertyComponent::resized();
}

ConfigTextProp::ConfigTextProp(
	const juce::String& className,
	const juce::String& propName,
	int maxNumChars, bool isMultiLine,
	bool isEditable, ValueType valueType,
	const UpdateCallback& updateCallback,
	const GetValueCallback& getValueCallback)
	: TextPropertyComponent(TRANS(propName),
		maxNumChars, isMultiLine, isEditable),
	ConfigPropHelper(className, propName,
		updateCallback, getValueCallback),
	valueType(valueType), isMultiLine(isMultiLine) {
	/** Init State */
	juce::MessageManager::callAsync(
		[ptr = juce::PropertyComponent::SafePointer(this)] {
			if (ptr) {
				ptr->refresh();
			}
		});
}

void ConfigTextProp::setText(const juce::String& newText) {
	if (newText == this->getText()) { return; }

	if (this->valueType == ValueType::TextVal) {
		this->updateThenSave(newText);
	}
	else if (this->valueType == ValueType::IntVal) {
		this->updateThenSave(newText.getLargeIntValue());
	}
	else if (this->valueType == ValueType::DoubleVal) {
		this->updateThenSave(newText.getDoubleValue());
	}

	this->refresh();
}

juce::String ConfigTextProp::getText() const {
	auto val = this->get();
	if (this->valueType == ValueType::TextVal) {
		return val.toString();
	}
	else if (this->valueType == ValueType::IntVal) {
		return juce::String{ (int64_t)val };
	}
	else if (this->valueType == ValueType::DoubleVal) {
		return juce::String{ (double)val };
	}
	return juce::String{};
}

void ConfigTextProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(
		prefHeight * (this->isMultiLine ? 4 : 1));

	this->juce::TextPropertyComponent::resized();
}

ConfigLabelProp::ConfigLabelProp(
	const juce::String& text, bool isMultiLine)
	: PropertyComponent(
		TRANS("Tip"), isMultiLine ? 100 : 25),
	isMultiLine(isMultiLine) {
	this->label = std::make_unique<juce::Label>(
		this->getName(), TRANS(text));
	this->label->setJustificationType(
		isMultiLine ? juce::Justification::topLeft : juce::Justification::centredLeft);
	this->addAndMakeVisible(this->label.get());
}

void ConfigLabelProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(
		prefHeight * (this->isMultiLine ? 4 : 1));

	this->label->setBounds(this->getLocalBounds());
}

void ConfigLabelProp::refresh() {
	/** Nothing To Do */
}

void ConfigLabelProp::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();
	laf.drawPropertyComponentBackground(g, this->getWidth(), this->getHeight(), *this);
}

ConfigWhiteSpaceProp::ConfigWhiteSpaceProp()
	: PropertyComponent(TRANS("White Space"), 25) {}

void ConfigWhiteSpaceProp::resized() {
	auto screenSize = utils::getScreenSize(this);
	int prefHeight = screenSize.getHeight() * PROP_HEIGHT;
	this->setPreferredHeight(prefHeight);
}

void ConfigWhiteSpaceProp::refresh() {
	/** Nothing To Do */
}

void ConfigWhiteSpaceProp::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();
	laf.drawPropertyComponentBackground(g, this->getWidth(), this->getHeight(), *this);
}
