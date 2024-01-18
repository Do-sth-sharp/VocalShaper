#include "LimitedCall.h"

LimitedCall::LimitedCall() {
	this->timer = std::make_unique<CallbackTimer>(this);
}

LimitedCall::~LimitedCall() {
	this->timer->stopTimer();
}

void LimitedCall::call(const Func& func,
	int countLimit, int timeout) {
	this->timer->stopTimer();

	{
		std::lock_guard locker(this->lock);
		this->func = func;
		if (this->count >= countLimit) {
			LimitedCall::invoke(func);
			this->count = 0;
		}
		else {
			this->count++;
			this->timer->startTimer(timeout);
		}
	}
}

void LimitedCall::invoke(const Func& func) {
	juce::MessageManager::callAsync(func);
}
