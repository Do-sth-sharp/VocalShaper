#pragma once

#include <JuceHeader.h>
#include "CloneableSource.h"

class SourceList final {
public:
	SourceList() = default;

	using SeqBlock = std::tuple<
		double, double, double,
		CloneableSource::SafePointer<>>;

	/**
	 * @attention Call this only on audio thread. Get the lock before use this method.
	 */
	int match(double t) const;
	/**
	 * @attention Get the lock before use this method.
	 */
	const SeqBlock get(int index) const;
	/**
	 * @attention Get the lock before use this method.
	 */
	const SeqBlock getUnchecked(int index) const;
	/**
	 * @attention Get the lock before use this method.
	 */
	const SeqBlock& getReference(int index) const;
	bool add(const SeqBlock& block);
	const juce::CriticalSection& getLock() const noexcept;

private:
	juce::Array<SeqBlock, juce::CriticalSection> list;
	mutable int lastIndex = -1;

	int binarySearchFast(int low, int high, double t) const;
	int binarySearchInsert(int low, int high, double t) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceList)
};
