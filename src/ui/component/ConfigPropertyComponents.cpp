#include "ConfigPropertyComponents.h"
#include "../misc/ConfigManager.h"

ConfigPropHelper::ConfigPropHelper(
	const juce::String& className,
	const juce::String& propName)
	: className(className), propName(propName) {
	/** Nothing To Do */
}

const juce::var& ConfigPropHelper::get() const {
	return ConfigManager::getInstance()
		->get(this->className)[juce::Identifier{ this->propName }];
}

bool ConfigPropHelper::update(const juce::var& data) {
	auto& classVar = ConfigManager::getInstance()->get(this->className);
	if (!classVar.isObject()) { return false; }
	auto classObj = classVar.getDynamicObject();
	if (!classObj) { return false; }

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
	const juce::String& buttonOnName)
	: BooleanPropertyComponent(
		TRANS(propName), buttonOnName, buttonOffName),
	ConfigPropHelper(className, propName) {
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

ConfigButtonProp::ConfigButtonProp(
	const juce::String& propName,
	const juce::String& buttonText,
	const ActiveFunc& activeFunc)
	: ButtonPropertyComponent(TRANS(propName), false),
	buttonText(TRANS(buttonText)), activeFunc(activeFunc) {
	/** Nothing To Do */
}

void ConfigButtonProp::buttonClicked() {
	this->activeFunc();
}

juce::String ConfigButtonProp::getButtonText() const {
	return this->buttonText;
}

ConfigChoiceProp::ConfigChoiceProp(const juce::String& className,
	const juce::String& propName,
	const juce::StringArray& choices,
	ValueType valueType)
	: ChoicePropertyComponent(TRANS(propName)),
	ConfigPropHelper(className, propName),
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

const juce::StringArray ConfigChoiceProp::transChoices(const juce::StringArray& choices) {
	juce::StringArray result;
	for (auto& s : choices) {
		result.add(TRANS(s));
	}
	return result;
}
