#pragma once

#include <JuceHeader.h>
#include "../../audioCore/AC_API.h"

class CoreCallbackWrapperBase {
public:
	virtual ~CoreCallbackWrapperBase() = default;

protected:
	CoreCallbackWrapperBase(const std::type_info& type)
		: type(type) {
	};

	bool isType(const std::type_info& type) const {
		return this->type == type;
	};

private:
	const std::type_info& type;

	JUCE_DECLARE_NON_COPYABLE(CoreCallbackWrapperBase)
	JUCE_LEAK_DETECTOR(CoreCallbackWrapperBase)
};

template<typename... T>
class CoreCallbackWrapper final : public CoreCallbackWrapperBase {
public:
	using Func = std::function<void(T...)>;

	CoreCallbackWrapper(const Func& func)
		: CoreCallbackWrapperBase(typeid(Func)), func(func) {
	};

	void invoke(T... args) const {
		/** Type Mismatch */
		if (!this->isType(typeid(Func))) {
			jassertfalse;
			return;
		}

		/** Invoke Function */
		this->func(args...);
	}

private:
	const Func func;

	JUCE_DECLARE_NON_COPYABLE(CoreCallbackWrapper)
	JUCE_LEAK_DETECTOR(CoreCallbackWrapper<T...>)
};

template<>
class CoreCallbackWrapper<void> final : public CoreCallbackWrapperBase {
public:
	using Func = std::function<void(void)>;

	CoreCallbackWrapper(const Func& func)
		: CoreCallbackWrapperBase(typeid(Func)), func(func) {
	};

	void invoke() const {
		/** Type Mismatch */
		if (!this->isType(typeid(Func))) {
			jassertfalse;
			return;
		}

		/** Invoke Function */
		this->func();
	}

private:
	const Func func;

	JUCE_DECLARE_NON_COPYABLE(CoreCallbackWrapper)
	JUCE_LEAK_DETECTOR(CoreCallbackWrapper<void>)
};

class CoreCallbacks final : private juce::DeletedAtShutdown {
public:
	CoreCallbacks();

	using CallbackType = UICallbackType;

	void addCallback(CallbackType type,
		std::unique_ptr<CoreCallbackWrapperBase> wrapper);
	const juce::OwnedArray<CoreCallbackWrapperBase>& getCallbackList(CallbackType type) const;

private:
	std::array<juce::OwnedArray<CoreCallbackWrapperBase>, (int)CallbackType::TypeMaxNum> list;

public:
	static CoreCallbacks* getInstance();
	static void releaseInstance();

private:
	static CoreCallbacks* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreCallbacks)
};

template<typename... T>
class CoreCallbackAPI final {
	CoreCallbackAPI() = delete;

public:
	static void invoke(CoreCallbacks::CallbackType type, T... args) {
		auto& list = CoreCallbacks::getInstance()->getCallbackList(type);
		for (auto i : list) {
			if (auto wrapper = dynamic_cast<CoreCallbackWrapper<T...>*>(i)) {
				wrapper->invoke(args...);
			}
		}
	};

	static void add(CoreCallbacks::CallbackType type, const std::function<void(T...)>& func) {
		auto wrapper = std::make_unique<CoreCallbackWrapper<T...>>(func);
		CoreCallbacks::getInstance()->addCallback(
			type, std::unique_ptr<CoreCallbackWrapperBase>(wrapper.release()));
	};
};

template<>
class CoreCallbackAPI<void> final {
	CoreCallbackAPI() = delete;

public:
	static void invoke(UICallbackType type) {
		auto& list = CoreCallbacks::getInstance()->getCallbackList(type);
		for (auto i : list) {
			if (auto wrapper = dynamic_cast<CoreCallbackWrapper<void>*>(i)) {
				wrapper->invoke();
			}
		}
	};

	static void add(CoreCallbacks::CallbackType type, const std::function<void(void)>& func) {
		auto wrapper = std::make_unique<CoreCallbackWrapper<void>>(func);
		CoreCallbacks::getInstance()->addCallback(
			type, std::unique_ptr<CoreCallbackWrapperBase>(wrapper.release()));
	};
};
