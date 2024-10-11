#pragma once

#include <JuceHeader.h>

class Counter final {
public:
	using Callback = std::function<void(void)>;
	Counter() = delete;
	Counter(int num, const Callback& callback);

	bool increase();
	void reset();

	class SafePointer {
	private:
		juce::WeakReference<Counter> weakRef;

	public:
		SafePointer() = default;
		SafePointer(Counter* source) : weakRef(source) {};
		SafePointer(const SafePointer& other) noexcept : weakRef(other.weakRef) {};

		SafePointer& operator= (const SafePointer& other) { weakRef = other.weakRef; return *this; };
		SafePointer& operator= (Counter* newSource) { weakRef = newSource; return *this; };

		Counter* getCounter() const noexcept { return dynamic_cast<Counter*> (weakRef.get()); };
		operator Counter* () const noexcept { return getCounter(); };
		Counter* operator->() const noexcept { return getCounter(); };
		void deleteAndZero() { delete getCounter(); };

		bool operator== (Counter* component) const noexcept { return weakRef == component; };
		bool operator!= (Counter* component) const noexcept { return weakRef != component; };
	};

private:
	const int num;
	const Callback callback;
	std::atomic_int count = 0;

	JUCE_DECLARE_WEAK_REFERENCEABLE(Counter)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Counter)
};
