#include "RCManager.h"

RCManager::~RCManager() {
	this->clear();
}

void RCManager::clear() {
	juce::ImageCache::releaseUnusedImages();
	this->types.clear();
}

juce::Image RCManager::loadImage(const juce::File& file) {
	return juce::ImageCache::getFromFile(file);
}

juce::Typeface::Ptr RCManager::loadType(const juce::File& file) {
	/** Find In Temp */
	auto it = this->types.find(file.getFullPathName());
	if (it != this->types.end()) {
		return it->second;
	}

	/** Load */
	auto fontSize = file.getSize();
	auto ptrFontData = std::unique_ptr<char[]>(new char[fontSize]);

	auto fontStream = file.createInputStream();
	fontStream->read(ptrFontData.get(), fontSize);

	auto ptr = juce::Typeface::createSystemTypefaceFor(ptrFontData.get(), fontSize);

	/** Set Temp */
	this->types.insert(std::make_pair(file.getFullPathName(), ptr));

	/** Return */
	return ptr;
}

RCManager* RCManager::getInstance() {
	return RCManager::instance ? RCManager::instance
		: (RCManager::instance = new RCManager{});
}

void RCManager::releaseInstance() {
	if (RCManager::instance) {
		delete RCManager::instance;
		RCManager::instance = nullptr;
	}
}

RCManager* RCManager::instance = nullptr;
