#include "InitTaskList.h"

void InitTaskList::add(const InitTask& task) {
	this->tasks.add(task);
}

void InitTaskList::runNow() {
	std::function<void(void)> taskWrappedHead = [] {};
	for (int i = this->tasks.size() - 1; i >= 0; i--) {
		auto wrappedFunc =
			[func = this->tasks.getUnchecked(i),
			next = taskWrappedHead] {
				func();
				juce::MessageManager::callAsync(next);
			};
		taskWrappedHead = wrappedFunc;
	}

	juce::MessageManager::callAsync(taskWrappedHead);
}

InitTaskList* InitTaskList::getInstance() {
	return InitTaskList::instance ? InitTaskList::instance 
		: (InitTaskList::instance = new InitTaskList{});
}

void InitTaskList::releaseInstance() {
	if (InitTaskList::instance) {
		delete InitTaskList::instance;
		InitTaskList::instance = nullptr;
	}
}

InitTaskList* InitTaskList::instance = nullptr;
