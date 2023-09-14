#include "CloneableSource.h"

std::atomic_int CloneableSource::globalCounter = 0;

CloneableSource::CloneableSource(const juce::String& name)
	: id(CloneableSource::globalCounter++), name(name) {
}

std::unique_ptr<CloneableSource> CloneableSource::cloneThis() const {
	if (auto dst = this->clone()) {
		dst->name = this->name;
		dst->isSaved = (bool)this->isSaved;
		dst->currentSampleRate = (double)this->currentSampleRate;

		return std::move(dst);
	}
	return nullptr;
}

bool CloneableSource::loadFrom(const juce::File& file) {
	if (this->load(file)) {
		this->isSaved = true;
		return true;
	}
	return false;
}

bool CloneableSource::saveAs(const juce::File& file) const {
	if (this->save(file)) {
		this->isSaved = true;
		return true;
	}
	return false;
}

bool CloneableSource::exportAs(const juce::File& file) const {
	return this->exportt(file);
}

double CloneableSource::getSourceLength() const {
	return this->getLength();
}

bool CloneableSource::checkSaved() const {
	return this->isSaved;
}

void CloneableSource::changed() {
	this->isSaved = false;
}

int CloneableSource::getId() const {
	return this->id;
}

void CloneableSource::setName(const juce::String& name) {
	this->name = name;
}

const juce::String CloneableSource::getName() const {
	return this->name;
}

void CloneableSource::prepareToPlay(double sampleRate, int bufferSize) {
	if (this->currentSampleRate == sampleRate && this->currentBufferSize == bufferSize) { return; }

	this->currentSampleRate = sampleRate;
	this->currentBufferSize = bufferSize;
	this->sampleRateChanged();
}

double CloneableSource::getSampleRate() const {
	return this->currentSampleRate;
}

int CloneableSource::getBufferSize() const {
	return this->currentBufferSize;
}

void CloneableSource::prepareToRecordInternal(
	int inputChannels, double sampleRate,
	int blockSize, bool updateOnly) {
	this->isRecording = true;
	this->prepareToRecord(
		inputChannels, sampleRate, blockSize, updateOnly);
}

void CloneableSource::recordingFinishedInternal() {
	this->recordingFinished();
	this->isRecording = false;
}

bool CloneableSource::checkRecording() const {
	return this->isRecording;
}
