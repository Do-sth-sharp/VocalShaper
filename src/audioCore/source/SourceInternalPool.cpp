#include "SourceInternalPool.h"

std::shared_ptr<SourceInternalContainer>
SourceInternalPool::add(const juce::String& name,
	SourceInternalContainer::SourceType type) {
	auto it = this->list.find(name);
	if (it == this->list.end()) {
		auto item = std::make_shared<SourceInternalContainer>(type, name);
		this->list.insert({ name, item });
		return item;
	}

	if (it->second->getType() == type) {
		return it->second;
	}
	
	return nullptr;
}

std::shared_ptr<SourceInternalContainer>
SourceInternalPool::find(const juce::String& name) const {
	auto it = this->list.find(name);
	if (it != this->list.end()) {
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<SourceInternalContainer>
SourceInternalPool::fork(const juce::String& name) {
	auto it = this->list.find(name);
	if (it != this->list.end()) {
		auto item = std::make_shared<SourceInternalContainer>(*(it->second));
		this->list.insert({ item->getName(), item });
		return item;
	}
	return nullptr;
}

void SourceInternalPool::checkSourceReleased(const juce::String& name) {
	auto it = this->list.find(name);
	if (it != this->list.end()) {
		if (it->second.use_count() == 1) {
			this->list.erase(it);
		}
	}
}

SourceInternalPool* SourceInternalPool::getInstance() {
	return SourceInternalPool::instance ? SourceInternalPool::instance
		: (SourceInternalPool::instance = new SourceInternalPool{});
}

void SourceInternalPool::releaseInstance() {
	if (SourceInternalPool::instance) {
		delete SourceInternalPool::instance;
		SourceInternalPool::instance = nullptr;
	}
}

SourceInternalPool* SourceInternalPool::instance = nullptr;
