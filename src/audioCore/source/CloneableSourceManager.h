#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"
#include "../project/Serializable.h"

class CloneableSourceManager final 
	: private juce::DeletedAtShutdown, public Serializable {
public:
	CloneableSourceManager() = default;
	~CloneableSourceManager() override = default;

	/**
	* @attention	You must call this from message thread.
	*/
	bool addSource(std::unique_ptr<CloneableSource> src);
	/**
	* @attention	You must call this from message thread.
	*/
	bool removeSource(CloneableSource* src);
	/**
	* @attention	You must call this from message thread.
	*/
	bool removeSource(int index);
	CloneableSource::SafePointer<> getSource(int index) const;
	int getSourceNum() const;
	juce::ReadWriteLock& getLock() const;

	/**
	* @attention	You must call this from message thread.
	*/
	bool setSourceSynthesizer(int index, const juce::String& identifier);
	/**
	* @attention	You must call this from message thread.
	*/
	bool synthSource(int index);

	/**
	* @attention	You must call this from message thread.
	*/
	bool cloneSource(int index);
	/**
	* @attention	You must call this from message thread.
	*/
	bool cloneSourceAsync(int index);
	/**
	* @attention	You must call this from message thread.
	*/
	template<IsCloneable T>
	bool createNewSource(
		double sampleRate = 48000, int channelNum = 2, double length = 0);
	/**
	* @attention	You must call this from message thread.
	*/
	template<IsCloneable T>
	bool createNewSourceThenLoad(const juce::String& path, bool copy);
	/**
	* @attention	You must call this from message thread.
	*/
	template<IsCloneable T>
	bool createNewSourceThenLoadAsync(const juce::String& path, bool copy);
	/**
	* @attention	You must call this from message thread.
	*/
	bool saveSource(int index, const juce::String& path) const;
	/**
	* @attention	You must call this from message thread.
	*/
	bool saveSourceAsync(int index, const juce::String& path) const;
	/**
	* @attention	You must call this from message thread.
	*/
	bool exportSource(int index, const juce::String& path) const;
	/**
	* @attention	You must call this from message thread.
	*/
	bool exportSourceAsync(int index, const juce::String& path) const;
	/**
	* @attention	You must call this from message thread.
	*/
	bool reloadSourceAsync(int index, const juce::String& path, bool copy);

	void prepareToPlay(double sampleRate, int bufferSize);

	void clearGraph();

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	juce::OwnedArray<CloneableSource, juce::CriticalSection> sourceList;
	mutable juce::ReadWriteLock lock;
	double sampleRate = 0;
	int bufferSize = 0;

public:
	static CloneableSourceManager* getInstance();
	static void releaseInstance();

private:
	static CloneableSourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloneableSourceManager)
};
