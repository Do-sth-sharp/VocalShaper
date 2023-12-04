#include "RCManager.h"

RCManager::~RCManager() {
	this->clear();
}

void RCManager::clear() {
	juce::ImageCache::releaseUnusedImages();
}

juce::Image RCManager::loadImage(const juce::File& file) {
	return juce::ImageCache::getFromFile(file);
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
