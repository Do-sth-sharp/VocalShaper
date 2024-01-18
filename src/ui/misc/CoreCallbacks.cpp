#include "CoreCallbacks.h"
#include "../../audioCore/AC_API.h"

CoreCallbacks::CoreCallbacks() {
	UICallbackAPI<const juce::String&, const juce::String&>::set(UICallbackType::ErrorAlert,
		[](const juce::String& title, const juce::String& mes) {
			CoreCallbacks::getInstance()->invokeError(title, mes);
		});
	UICallbackAPI<bool>::set(UICallbackType::PlayStateChanged,
		[](bool status) {
			CoreCallbacks::getInstance()->invokePlayingStatus(status);
		});
	UICallbackAPI<bool>::set(UICallbackType::RecordStateChanged,
		[](bool status) {
			CoreCallbacks::getInstance()->invokeRecordingStatus(status);
		});
	UICallbackAPI<const juce::String&>::set(UICallbackType::ErrorMessage,
		[](const juce::String& mes) {
			CoreCallbacks::getInstance()->invokeErrorMes(mes);
		});
	UICallbackAPI<bool>::set(UICallbackType::PluginSearchStateChanged,
		[](bool state) {
			CoreCallbacks::getInstance()->invokeSearchPlugin(state);
		});
	UICallbackAPI<int>::set(UICallbackType::SourceChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSourceChanged(index);
		});
}

void CoreCallbacks::addError(const ErrorCallback& callback) {
	this->error.add(callback);
}

void CoreCallbacks::addPlayingStatus(const PlayingStatusCallback& callback) {
	this->playingStatus.add(callback);
}

void CoreCallbacks::addRecordingStatus(const RecordingStatusCallback& callback) {
	this->recordingingStatus.add(callback);
}

void CoreCallbacks::addErrorMes(const ErrorMesCallback& callback) {
	this->errorMes.add(callback);
}

void CoreCallbacks::addSearchPlugin(const SearchPluginCallback& callback) {
	this->searchPlugin.add(callback);
}

void CoreCallbacks::addSourceChanged(const SourceChangedCallback& callback) {
	this->sourceChanged.add(callback);
}

void CoreCallbacks::invokeError(
	const juce::String& title, const juce::String& mes) const {
	for (auto& i : this->error) {
		i(title, mes);
	}
}

void CoreCallbacks::invokePlayingStatus(bool status) const {
	for (auto& i : this->playingStatus) {
		i(status);
	}
}

void CoreCallbacks::invokeRecordingStatus(bool status) const {
	for (auto& i : this->recordingingStatus) {
		i(status);
	}
}

void CoreCallbacks::invokeErrorMes(const juce::String& mes) const {
	for (auto& i : this->errorMes) {
		i(mes);
	}
}

void CoreCallbacks::invokeSearchPlugin(bool status) const {
	for (auto& i : this->searchPlugin) {
		i(status);
	}
}

void CoreCallbacks::invokeSourceChanged(int index) const {
	for (auto& i : this->sourceChanged) {
		i(index);
	}
}

CoreCallbacks* CoreCallbacks::getInstance() {
	return CoreCallbacks::instance ? CoreCallbacks::instance
		: (CoreCallbacks::instance = new CoreCallbacks{});
}

void CoreCallbacks::releaseInstance() {
	if (CoreCallbacks::instance) {
		delete CoreCallbacks::instance;
		CoreCallbacks::instance = nullptr;
	}
}

CoreCallbacks* CoreCallbacks::instance = nullptr;
