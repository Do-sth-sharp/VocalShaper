#include "LevelMeterHub.h"

LevelMeterHub::LevelMeterHub()
	: Timer() {
	this->startTimerHz(25);
}

void LevelMeterHub::timerCallback() {
	for (auto i : this->list) {
		i->updateLevelMeter();
	}
}

void LevelMeterHub::add(Target* target) {
	this->remove(target);
	this->list.add(target);
}

void LevelMeterHub::remove(Target* target) {
	this->list.removeAllInstancesOf(target);
}

LevelMeterHub* LevelMeterHub::getInstance() {
	return LevelMeterHub::instance ? LevelMeterHub::instance
		: (LevelMeterHub::instance = new LevelMeterHub{});
}

void LevelMeterHub::releaseInstance() {
	if (LevelMeterHub::instance) {
		delete LevelMeterHub::instance;
		LevelMeterHub::instance = nullptr;
	}
}

LevelMeterHub* LevelMeterHub::instance = nullptr;
