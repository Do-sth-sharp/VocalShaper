#pragma once

#include <JuceHeader.h>

class ConfigPropHelper {
public:
	using UpdateCallback = std::function<bool(const juce::var&)>;
	using GetValueCallback = std::function<const juce::var(void)>;

public:
	ConfigPropHelper(const juce::String& className,
		const juce::String& propName,
		const UpdateCallback& updateCallback = [](const juce::var&) { return true; },
		const GetValueCallback& getValueCallback = GetValueCallback{});
	virtual ~ConfigPropHelper() = default;

	const juce::var get() const;
	bool update(const juce::var& data);
	bool save();
	bool updateThenSave(const juce::var& data);

private:
	const juce::String className, propName;
	const UpdateCallback updateCallback;
	const GetValueCallback getValueCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigPropHelper)
};

class ConfigBooleanProp final
	:  public juce::BooleanPropertyComponent,
	public ConfigPropHelper {
public:
	ConfigBooleanProp(const juce::String& className,
		const juce::String& propName,
		const juce::String& buttonOffName = {},
		const juce::String& buttonOnName = {},
		const UpdateCallback& updateCallback = [](const juce::var&) { return true; },
		const GetValueCallback& getValueCallback = GetValueCallback{});

	void setState(bool newState) override;
	bool getState() const override;

	void resized() override;

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

	void resized() override;

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
		ValueType valueType = ValueType::NameVal,
		const UpdateCallback& updateCallback = [](const juce::var&) { return true; },
		const GetValueCallback& getValueCallback = GetValueCallback{});

	void setIndex(int newIndex) override;
	int getIndex() const override;

	void resized() override;

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
		double skewFactor = 1.0, bool symmetricSkew = false,
		const UpdateCallback& updateCallback = [](const juce::var&) { return true; },
		const GetValueCallback& getValueCallback = GetValueCallback{});

	void setValue(double newValue) override;
	double getValue() const override;

	void resized() override;

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
		ValueType valueType = ValueType::TextVal,
		const UpdateCallback& updateCallback = [](const juce::var&) { return true; },
		const GetValueCallback& getValueCallback = GetValueCallback{});

	void setText(const juce::String& newText) override;
	juce::String getText() const override;

	void resized() override;

private:
	const ValueType valueType;
	const bool isMultiLine;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigTextProp)
};

class ConfigLabelProp final
	: public juce::PropertyComponent {
public:
	ConfigLabelProp(const juce::String& text,
		bool isMultiLine = false);

	void resized() override;
	void refresh() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<juce::Label> label = nullptr;
	const bool isMultiLine;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigLabelProp)
};

class ConfigWhiteSpaceProp final
	: public juce::PropertyComponent {
public:
	ConfigWhiteSpaceProp();

	void resized() override;
	void refresh() override;
	void paint(juce::Graphics& g) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigWhiteSpaceProp)
};
