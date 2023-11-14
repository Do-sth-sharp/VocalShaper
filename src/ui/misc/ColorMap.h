#pragma once

#include <JuceHeader.h>

class ColorMap final : private juce::DeletedAtShutdown {
public:
	ColorMap() = default;

	void set(const juce::String& key, const juce::Colour& value);
	const juce::Colour get(const juce::String& key) const;

public:
	static const juce::Colour fromString(const juce::String& str);

private:
	std::map<juce::String, juce::Colour> map;

public:
	static ColorMap* getInstance();
	static void releaseInstance();

private:
	static ColorMap* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorMap)
};
