#include "Counter.h"

Counter::Counter(int num, const Callback& callback)
	: num(num), callback(callback) {}

bool Counter::increase() {
	this->count++;

	if (this->count >= this->num) {
		if (this->callback) {
			juce::MessageManager::callAsync(this->callback);
		}
		return true;
	}

	return false;
}

void Counter::reset() {
	this->count = 0;
}
