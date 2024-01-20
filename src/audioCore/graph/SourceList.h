#pragma once

#include <JuceHeader.h>
#include "../source/CloneableSource.h"
#include "../project/Serializable.h"

class SourceList final : public Serializable {
public:
	SourceList() = default;

	/**
	 * StartTime, EndTime, Offset, Pointer.
	 */
	using SeqBlock = std::tuple<
		double, double, double,
		CloneableSource::SafePointer<>>;

	/**
	 * @attention Call this only on audio thread. Get the lock before use this method.
	 */
	std::tuple<int, int> match(double startTime, double endTime) const;
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
	int size() const;
	int add(const SeqBlock& block);
	void remove(int index);

	void clearGraph();

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	juce::Array<SeqBlock, juce::CriticalSection> list;
	mutable int lastIndex = -1;

	int binarySearchInsert(int low, int high, double t) const;
	int binarySearchStart(int low, int high, double t) const;
	int seqSearchEnd(int low, int high, double t) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceList)
};
