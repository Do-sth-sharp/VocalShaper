#include "CompManager.h"

CompManager::CompManager() {
	for (int i = 0; i < static_cast<int>(CompType::CompMaxSize); i++) {
		this->compList.add(nullptr);
	}
}

void CompManager::set(CompType type, std::unique_ptr<flowUI::FlowComponent> comp) {
	this->compList.set(static_cast<int>(type), comp.release(), true);
}

flowUI::FlowComponent* CompManager::get(CompType type) const {
	return this->compList.getUnchecked(static_cast<int>(type));
}

void CompManager::autoLayout(const juce::String& path) const {
	flowUI::FlowWindowHub::autoLayout(path, this->getLayoutList());
}

void CompManager::saveLayout(const juce::String& path) const {
	flowUI::FlowWindowHub::saveLayout(path, this->getLayoutList());
}

void CompManager::maxMainWindow() const {
	if (flowUI::FlowWindowHub::getSize() > 0) {
		if (auto window = flowUI::FlowWindowHub::getWindow(0)) {
			window->setFullScreen(true);
		}
	}
}

bool CompManager::isOpened(CompType type) const {
	auto comp = this->get(type);
	if (!comp) { return false; }
	return comp->isOpened();
}

void CompManager::open(CompType type, int windowIdx) const {
	this->close(type);
	
	if (auto comp = this->get(type)) {
		if (auto window = flowUI::FlowWindowHub::getWindow(windowIdx)) {
			window->openComponent(comp);
		}
	}
}

void CompManager::close(CompType type) const {
	auto comp = this->get(type);
	if (!comp) { return; }

	int windSize = flowUI::FlowWindowHub::getSize();
	for (int i = 0; i < windSize; i++) {
		if (auto window = flowUI::FlowWindowHub::getWindow(i)) {
			window->closeComponent(comp);
		}
	}
}

const juce::Array<flowUI::FlowComponent*> CompManager::getLayoutList() const {
	return {
		this->get(CompType::ToolBar),
		this->get(CompType::PluginView),
		this->get(CompType::SourceView),
		this->get(CompType::TrackView),
		this->get(CompType::MixerView),
		this->get(CompType::SourceEditView),
		this->get(CompType::SourceRecordView),
		this->get(CompType::AudioDebugger),
		this->get(CompType::MidiDebugger)
	};
}

CompManager* CompManager::getInstance() {
	return CompManager::instance ? CompManager::instance
		: (CompManager::instance = new CompManager{});
}

void CompManager::releaseInstance() {
	if (CompManager::instance) {
		delete CompManager::instance;
		CompManager::instance = nullptr;
	}
}

CompManager* CompManager::instance = nullptr;
