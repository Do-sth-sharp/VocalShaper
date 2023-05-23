#pragma once

#include <JuceHeader.h>

class CloneableSource {
public:
	CloneableSource(const juce::String& name = juce::String{});
	CloneableSource(const CloneableSource* src, const juce::String& name = juce::String{});
	virtual ~CloneableSource() = default;

	bool cloneFrom(const CloneableSource* src);
	bool loadFrom(const juce::File& file);
	bool saveAs(const juce::File& file) const;
	int getSourceLength() const;

	bool checkSaved() const;
	void changed();

	int getId() const;
	void setName(const juce::String& name);
	const juce::String getName() const;

protected:
	virtual bool clone(const CloneableSource* src) = 0;
	virtual bool load(const juce::File& file) = 0;
	virtual bool save(const juce::File& file) const = 0;
	virtual int getLength() const = 0;

private:
	static std::atomic_int globalCounter;
	mutable std::atomic_bool isSaved = true;
	const int id = -1;
	juce::String name;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSource);
};
