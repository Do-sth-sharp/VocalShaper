#pragma once

#include <JuceHeader.h>

class LimitedCall final {
public:
	LimitedCall();
	~LimitedCall();

	using Func = std::function<void(void)>;
	void call(const Func& func, int countLimit = 1, int timeout = 100);

private:
	std::mutex lock;
	int count = 0;
	Func func;

	static void invoke(const Func& func);

	class CallbackTimer final : public juce::Timer {
	public:
		explicit CallbackTimer(LimitedCall* parent) : parent(parent) {};

		void timerCallback() override {
			std::lock_guard locker(this->parent->lock);
			this->parent->count = 0;
			if (this->parent->func) {
				LimitedCall::invoke(this->parent->func);
			}
		}

	private:
		LimitedCall* const parent;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CallbackTimer)
	};
	std::unique_ptr<CallbackTimer> timer = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LimitedCall)
};
