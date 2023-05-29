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
	 * @attention Invoke this only on audio thread, get the lock before use this method.
	 */
	int match(double t) const;
	const juce::CriticalSection& getLock() const noexcept;

private:
	juce::Array<SeqBlock, juce::CriticalSection> list;
	mutable int lastIndex = -1;

	int binarySearch(int low, int high, double t) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceList)
};
