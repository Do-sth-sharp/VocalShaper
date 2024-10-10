#include "ARAArchive.h"

#define MAGIC_NUMBER 0xbabedead
#define WASTED_SIZE (sizeof(int) + sizeof(int64_t))

ARAArchiveReader::ARAArchiveReader(
	const juce::MemoryBlock& block)
	: block(block) {
	/** Get Archive ID */
	juce::MemoryInputStream stream(block, false);
	if (stream.readInt() != MAGIC_NUMBER) {
		return;
	}

	size_t idLength = stream.readInt64();

	if (idLength > 0) {
		this->valid = (stream.readIntoMemoryBlock(this->archiveID, idLength) == idLength);
	}
};

bool ARAArchiveReader::isValid() const {
	return this->valid;
}

size_t ARAArchiveReader::getSize() const {
	if (!this->isValid()) { return 0; }

	return this->block.getSize() - WASTED_SIZE - this->archiveID.getSize();
}

const char* ARAArchiveReader::getID() const {
	if (!this->isValid()) { return nullptr; }

	return static_cast<const char*>(this->archiveID.getData());
}

bool ARAArchiveReader::read(size_t position, size_t length,
	uint8_t buffer[]) {
	if (!this->isValid()) { return false; }

	if ((position + length) <= this->getSize()) {
		std::memcpy(buffer, juce::addBytesToPointer(
			this->block.getData(),
			position + WASTED_SIZE + this->archiveID.getSize()), length);
		return true;
	}

	return false;
}

ARAArchiveWriter::ARAArchiveWriter(
	juce::MemoryOutputStream& stream, const char* archiveID)
	: stream(stream) {
	stream.writeInt(MAGIC_NUMBER);

	this->idLength = std::strlen(archiveID) + 1;
	stream.writeInt64(this->idLength);

	stream.write(archiveID, this->idLength);
}

bool ARAArchiveWriter::write(size_t position, size_t length,
	const uint8_t buffer[]) {
	return (this->stream.setPosition(WASTED_SIZE + this->idLength + position) && this->stream.write(buffer, length));
}
