#include "UICallback.h"

void UICallback::setCallback(UICallbackType type,
	std::unique_ptr<UICallbackWrapperBase> wrapper) {
	this->list[(int)type] = std::move(wrapper);
}

UICallbackWrapperBase* UICallback::getCallback(UICallbackType type) const {
	return this->list[(int)type].get();
}

UICallback* UICallback::getInstance() {
	return UICallback::instance
		? UICallback::instance
		: (UICallback::instance = new UICallback{});
}

void UICallback::releaseInstance() {
	if (UICallback::instance) {
		delete UICallback::instance;
		UICallback::instance = nullptr;
	}
}

UICallback* UICallback::instance = nullptr;
