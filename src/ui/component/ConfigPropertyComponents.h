#pragma once

#include <JuceHeader.h>

class ConfigPropHelper {
public:
	ConfigPropHelper(const juce::String& className,
		const juce::String& propName);
	virtual ~ConfigPropHelper() = default;

	const juce::var& get() const;
	bool update(const juce::var& data);
	bool save();
	bool updateThenSave(const juce::var& data);

private:
	const juce::String className, propName;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigPropHelper)
};

class ConfigBooleanProp final
	:  public juce::BooleanPropertyComponent,
	public ConfigPropHelper {
public:
	ConfigBooleanProp(const juce::String& className,
		const juce::String& propName,
		const juce::String& buttonOffName = {},
		const juce::String& buttonOnName = {});

	void setState(bool newState) override;
	bool getState() const override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigBooleanProp)
};

class ConfigButtonProp final : public juce::ButtonPropertyComponent {
public:
	using ActiveFunc = std::function<void(void)>;

public:
	ConfigButtonProp(const juce::String& propName,
		const juce::String& buttonText,
		const ActiveFunc& activeFunc);

	void buttonClicked() override;
	juce::String getButtonText() const override;

private:
	const juce::String buttonText;
	const ActiveFunc activeFunc;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigButtonProp)
};

class ConfigChoiceProp final
	: public juce::ChoicePropertyComponent,
	public ConfigPropHelper {
public:
	enum class ValueType {
		IndexVal, NameVal };

public:
	ConfigChoiceProp(const juce::String& className,
		const juce::String& propName,
		const juce::StringArray& choices,
		ValueType valueType = ValueType::NameVal);

	void setIndex(int newIndex) override;
	int getIndex() const override;

private:
	const juce::StringArray choicesValue;
	const ValueType valueType;

	static const juce::StringArray transChoices(const juce::StringArray& choices);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigChoiceProp)
};

class ConfigSliderProp final
	: public juce::SliderPropertyComponent,
	public ConfigPropHelper {
public:
	ConfigSliderProp(const juce::String& className,
		const juce::String& propName,
		double rangeMin, double rangeMax, double interval = 0.0,
		double skewFactor = 1.0, bool symmetricSkew = false);

	void setValue(double newValue) override;
	double getValue() const override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigSliderProp)
};

class ConfigTextProp final
	: public juce::TextPropertyComponent,
	public ConfigPropHelper {
public:
	enum class ValueType {
		TextVal, IntVal, DoubleVal };

public:
	ConfigTextProp(const juce::String& className,
		const juce::String& propName,
		int maxNumChars = INT_MAX, bool isMultiLine = false,
		bool isEditable = true,
		ValueType valueType = ValueType::TextVal);

	void setText(const juce::String& newText) override;
	juce::String getText() const override;

private:
	const ValueType valueType;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigTextProp)
};
