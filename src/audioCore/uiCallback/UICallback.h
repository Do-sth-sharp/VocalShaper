#pragma once

#include <JuceHeader.h>

#include "UICallbackType.h"

class UICallbackWrapperBase {
public:
	virtual ~UICallbackWrapperBase() = default;

protected:
	UICallbackWrapperBase(const std::type_info& type)
		: typeName(type.name()) {};

	bool isType(const std::type_info& type) const {
		return std::strcmp(this->typeName, type.name()) == 0;
	};

private:
	const char* typeName = nullptr;
};

template<typename... T>
class UICallbackWrapper final : public UICallbackWrapperBase {
public:
	using Func = std::function<void(T...)>;

	UICallbackWrapper(const Func& func)
		: UICallbackWrapperBase(typeid(Func)), func(func) {};

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
};

template<>
class UICallbackWrapper<void> final : public UICallbackWrapperBase {
public:
	using Func = std::function<void(void)>;

	UICallbackWrapper(const Func& func)
		: UICallbackWrapperBase(typeid(Func)), func(func) {};

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
};

class UICallback final : private juce::DeletedAtShutdown {
public:
	UICallback() = default;

	void setCallback(UICallbackType type,
		std::unique_ptr<UICallbackWrapperBase> wrapper);
	UICallbackWrapperBase* getCallback(UICallbackType type) const;

private:
	std::array<std::unique_ptr<UICallbackWrapperBase>,
		(int)(UICallbackType::TypeMaxNum)> list;

public:
	static UICallback* getInstance();
	static void releaseInstance();

private:
	static UICallback* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UICallback)
};

template<typename... T>
class UICallbackAPI final {
	UICallbackAPI() = delete;

public:
	static void invoke(UICallbackType type, T... args) {
		if (auto wrapper = dynamic_cast<UICallbackWrapper<T...>*>(
			UICallback::getInstance()->getCallback(type))) {
			wrapper->invoke(args...);
		}
	};

	static void set(UICallbackType type, const std::function<void(T...)>& func) {
		auto wrapper = std::make_unique<UICallbackWrapper<T...>>(func);
		UICallback::getInstance()->setCallback(
			type, std::unique_ptr<UICallbackWrapperBase>(wrapper.release()));
	};
};

template<>
class UICallbackAPI<void> final {
	UICallbackAPI() = delete;

public:
	static void invoke(UICallbackType type) {
		if (auto wrapper = dynamic_cast<UICallbackWrapper<void>*>(
			UICallback::getInstance()->getCallback(type))) {
			wrapper->invoke();
		}
	};

	static void set(UICallbackType type, const std::function<void(void)>& func) {
		auto wrapper = std::make_unique<UICallbackWrapper<void>>(func);
		UICallback::getInstance()->setCallback(
			type, std::unique_ptr<UICallbackWrapperBase>(wrapper.release()));
	};
};
