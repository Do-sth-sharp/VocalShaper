#pragma once

#include <JuceHeader.h>

class CloneableSource;
template<typename T>
concept IsCloneable = std::derived_from<T, CloneableSource>;

class CloneableSource {
public:
	CloneableSource(const juce::String& name = juce::String{});
	CloneableSource(const CloneableSource* src, const juce::String& name = juce::String{});
	virtual ~CloneableSource() = default;

	bool cloneFrom(const CloneableSource* src);
	bool loadFrom(const juce::File& file);
	bool saveAs(const juce::File& file) const;
	bool exportAs(const juce::File& file) const;
	double getSourceLength() const;

	bool checkSaved() const;
	void changed();

	int getId() const;
	void setName(const juce::String& name);
	const juce::String getName() const;

	void prepareToPlay(double sampleRate, int bufferSize);
	double getSampleRate() const;
	int getBufferSize() const;

	template<IsCloneable SourceType = CloneableSource>
	class SafePointer {
	public:
		SafePointer() = default;
		SafePointer(SourceType* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (SourceType* newSource) { weakRef = newSource; return *this; };

		SourceType* getSource() const noexcept { return dynamic_cast<SourceType*> (weakRef.get()); };
		operator SourceType* () const noexcept { return getSource(); };
		SourceType* operator->() const noexcept { return getSource(); };
		void deleteAndZero() { delete getSource(); };

		bool operator== (SourceType* component) const noexcept { return weakRef == component; };
		bool operator!= (SourceType* component) const noexcept { return weakRef != component; };

	private:
		juce::WeakReference<CloneableSource> weakRef;
	};

protected:
	virtual bool clone(const CloneableSource* src) = 0;
	virtual bool load(const juce::File& file) = 0;
	virtual bool save(const juce::File& file) const = 0;
	virtual bool exportt(const juce::File& file) const { return false; };
	virtual double getLength() const = 0;
	virtual void sampleRateChanged() {};

private:
	static std::atomic_int globalCounter;
	mutable std::atomic_bool isSaved = true;
	const int id = -1;
	juce::String name;
	std::atomic<double> currentSampleRate = 0;
	std::atomic_int currentBufferSize = 0;

	JUCE_DECLARE_WEAK_REFERENCEABLE(CloneableSource)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSource)
};