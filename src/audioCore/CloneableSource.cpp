#include "CloneableSource.h"

std::atomic_int CloneableSource::globalCounter = 0;

CloneableSource::CloneableSource(const juce::String& name)
	: id(CloneableSource::globalCounter++), name(name) {
}

CloneableSource::CloneableSource(const CloneableSource* src, const juce::String& name)
	: CloneableSource(name) {
	this->cloneFrom(src);
}

bool CloneableSource::cloneFrom(const CloneableSource* src) {
	if (this->clone(src)) {
		this->name = src->name;
		this->isSaved = (bool)src->isSaved;
		return true;
	}
	return false;
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

int CloneableSource::getSourceLength() const {
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
