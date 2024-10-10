#pragma once

#include <JuceHeader.h>

class ARAArchiveReader final {
public:
	ARAArchiveReader() = delete;
	ARAArchiveReader(
		const juce::MemoryBlock& block);

	bool isValid() const;

	size_t getSize() const;
	const char* getID() const;

	bool read(size_t position, size_t length,
		uint8_t buffer[]);

private:
	const juce::MemoryBlock& block;
	juce::MemoryBlock archiveID;
	bool valid = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAArchiveReader)
};

class ARAArchiveWriter final {
public:
	ARAArchiveWriter() = delete;
	ARAArchiveWriter(
		juce::MemoryOutputStream& stream, const char* archiveID);

	bool write(size_t position, size_t length,
		const uint8_t buffer[]);

private:
	juce::MemoryOutputStream& stream;
	size_t idLength;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ARAArchiveWriter)
};
