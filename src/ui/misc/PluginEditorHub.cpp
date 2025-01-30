#include "PluginEditorHub.h"
#include "CoreCallbacks.h"
#include "RCManager.h"
#include "../../audioCore/AC_API.h"

PluginEditorHub::PluginEditorHub() {
	/** Update Callback */
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackAdded,
		[](int type, int index) {
			PluginEditorHub::getInstance()->trackAdded(type, index);
		}
	);
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackRemoved,
		[](int type, int index) {
			PluginEditorHub::getInstance()->trackRemoved(type, index);
		}
	);
	CoreCallbackAPI<int, int, int, int>::add(CoreCallbacks::CallbackType::TrackEffectIndexChanged,
		[](int type, int track, int oldIndex, int newIndex) {
			PluginEditorHub::getInstance()->effectIndexChanged(type, track, oldIndex, newIndex);
		}
	);
}

PluginEditorHub::~PluginEditorHub() {
	this->closeAll();
}

void PluginEditorHub::openInstr(int index) {
	if (this->checkInstr(index)) { return; }

	if (auto editor = quickAPI::getInstrEditor(index)) {
		/** Create */
		auto container = std::make_shared<PluginEditor>(
			quickAPI::getInstrName(index),
			PluginType::Instr, editor);
		container->setOpenGL(this->openGLOn);
		container->setWindowIcon(this->iconTemp);

		container->update(0, index, 0);

		/** Show */
		this->openEditor(container.get());

		/** Add To List */
		this->instrEditors.insert({ this->getInstrRef(index), container });

		/** Callback */
		CoreCallbackAPI<int>::invoke(CoreCallbacks::CallbackType::TrackInstrChanged, index);
	}
}

void PluginEditorHub::closeInstr(int index) {
	auto it = this->instrEditors.find(this->getInstrRef(index));
	if (it != this->instrEditors.end()) {
		/** Close */
		this->closeEditor(it->second.get());

		/** Remove From List */
		this->instrEditors.erase(it);

		/** Callback */
		CoreCallbackAPI<int>::invoke(CoreCallbacks::CallbackType::TrackInstrChanged, index);
	}
}

bool PluginEditorHub::checkInstr(int index) const {
	if (auto editor = quickAPI::getInstrEditorExists(index)) {
		auto it = this->instrEditors.find(this->getInstrRef(index));
		if (it != this->instrEditors.end()) {
			auto container = it->second;
			if (container->getEditor() == editor) {
				return true;
			}
		}
	}
	return false;
}

void PluginEditorHub::openEffect(int type, int track, int index) {
	if (this->checkEffect(type, track, index)) { return; }

	if (auto editor = quickAPI::getEffectEditor({ (quickAPI::TrackType)type, track }, index)) {
		/** Create */
		auto container = std::make_shared<PluginEditor>(
			quickAPI::getEffectName({ (quickAPI::TrackType)type, track }, index),
			PluginType::Effect, editor);
		container->setOpenGL(this->openGLOn);
		container->setWindowIcon(this->iconTemp);

		container->update(type, track, index);

		/** Show */
		this->openEditor(container.get());

		/** Add To List */
		this->effectEditors.insert({ this->getEffectRef(type, track, index), container });

		/** Callback */
		CoreCallbackAPI<int, int, int>::invoke(
			CoreCallbacks::CallbackType::TrackEffectChanged, type, track, index);
	}
}

void PluginEditorHub::closeEffect(int type, int track, int index) {
	auto it = this->effectEditors.find(this->getEffectRef(type, track, index));
	if (it != this->effectEditors.end()) {
		/** Close */
		this->closeEditor(it->second.get());

		/** Remove From List */
		this->effectEditors.erase(it);

		/** Callback */
		CoreCallbackAPI<int, int, int>::invoke(
			CoreCallbacks::CallbackType::TrackEffectChanged, type, track, index);
	}
}

bool PluginEditorHub::checkEffect(int type, int track, int index) const {
	if (auto editor = quickAPI::getEffectEditorExists({ (quickAPI::TrackType)type, track }, index)) {
		auto it = this->effectEditors.find(this->getEffectRef(type, track, index));
		if (it != this->effectEditors.end()) {
			auto container = it->second;
			if (container->getEditor() == editor) {
				return true;
			}
		}
	}
	return false;
}

void PluginEditorHub::setOpenGL(bool openGLOn) {
	this->openGLOn = openGLOn;
	for (auto& i : this->instrEditors) {
		i.second->setOpenGL(openGLOn);
	}
	for (auto i : this->effectEditors) {
		i.second->setOpenGL(openGLOn);
	}
}

void PluginEditorHub::setIcon(const juce::String& path) {
	/** Load Icon */
	juce::File iconFile = juce::File::getSpecialLocation(
		juce::File::SpecialLocationType::hostApplicationPath).getParentDirectory().getChildFile(path);
	this->iconTemp = RCManager::getInstance()->loadImage(iconFile);

	/** Set All Windows */
	for (auto& i : this->instrEditors) {
		i.second->setWindowIcon(this->iconTemp);
	}
	for (auto& i : this->effectEditors) {
		i.second->setWindowIcon(this->iconTemp);
	}
}

void PluginEditorHub::closeAll() {
	for (auto& i : this->instrEditors) {
		this->closeEditor(i.second.get());
	}
	this->instrEditors.clear();

	for (auto& i : this->effectEditors) {
		this->closeEditor(i.second.get());
	}
	this->effectEditors.clear();

	CoreCallbackAPI<int>::invoke(CoreCallbacks::CallbackType::TrackInstrChanged, -1);
	CoreCallbackAPI<int, int, int>::invoke(
		CoreCallbacks::CallbackType::TrackEffectChanged, -1, -1, -1);
}

void PluginEditorHub::closeEditor(PluginEditor* ptr) {
	ptr->setVisible(false);
}

void PluginEditorHub::openEditor(PluginEditor* ptr) {
	ptr->setVisible(true);
}

void PluginEditorHub::trackAdded(int type, int track) {
	/** Instr */
	for (auto it = this->instrEditors.begin(); it != this->instrEditors.end(); it++) {
		auto [currentType, currentTrack, currentIndex] = it->second->getIndex();
		if (currentType == type && currentTrack >= track) {
			it->second->update(type, currentTrack + 1, currentIndex);
		}
	}

	/** Effect */
	for (auto it = this->effectEditors.begin(); it != this->effectEditors.end(); it++) {
		auto [currentType, currentTrack, currentIndex] = it->second->getIndex();
		if (currentType == type && currentTrack >= track) {
			it->second->update(type, currentTrack + 1, currentIndex);
		}
	}
}

void PluginEditorHub::trackRemoved(int type, int track) {
	/** Instr */
	for (auto it = this->instrEditors.begin(); it != this->instrEditors.end(); it++) {
		auto [currentType, currentTrack, currentIndex] = it->second->getIndex();
		if (currentType == type && currentTrack > track) {
			it->second->update(type, currentTrack - 1, currentIndex);
		}
	}

	/** Effect */
	for (auto it = this->effectEditors.begin(); it != this->effectEditors.end(); it++) {
		auto [currentType, currentTrack, currentIndex] = it->second->getIndex();
		if (currentType == type && currentTrack > track) {
			it->second->update(type, currentTrack - 1, currentIndex);
		}
	}
}

void PluginEditorHub::effectIndexChanged(int type, int track, int oldIndex, int newIndex) {
	for (auto it = this->effectEditors.begin(); it != this->effectEditors.end(); it++) {
		auto [currentType, currentTrack, currentIndex] = it->second->getIndex();
		if (currentType == type && currentTrack == track && currentIndex == oldIndex) {
			it->second->update(type, track, newIndex);
		}
	}
}

PluginEditorHub::RefType PluginEditorHub::getInstrRef(int index) const {
	return static_cast<RefType>(quickAPI::getInstrRef(index));
}

PluginEditorHub::RefType PluginEditorHub::getEffectRef(int type, int track, int index) const {
	return static_cast<RefType>(
		quickAPI::getEffectRef({ (quickAPI::TrackType)type, track }, index));
}

void PluginEditorHub::closeInstr(PluginEditor* ptr) {
	for (auto it = this->instrEditors.begin(); it != this->instrEditors.end(); it++) {
		if (it->second.get() == ptr) {
			auto [type, track, index] = it->second->getIndex();

			/** Close */
			this->closeEditor(it->second.get());

			/** Remove From List */
			this->instrEditors.erase(it);

			/** Callback */
			CoreCallbackAPI<int>::invoke(CoreCallbacks::CallbackType::TrackInstrChanged, index);

			break;
		}
	}
}

void PluginEditorHub::closeEffect(PluginEditor* ptr) {
	for (auto it = this->effectEditors.begin(); it != this->effectEditors.end(); it++) {
		if (it->second.get() == ptr) {
			auto [type, track, index] = it->second->getIndex();

			/** Close */
			this->closeEditor(it->second.get());

			/** Remove From List */
			this->effectEditors.erase(it);

			/** Callback */
			CoreCallbackAPI<int, int, int>::invoke(
				CoreCallbacks::CallbackType::TrackEffectChanged, type, track, index);

			break;
		}
	}
}

PluginEditorHub* PluginEditorHub::getInstance() {
	return PluginEditorHub::instance ? PluginEditorHub::instance
		: (PluginEditorHub::instance = new PluginEditorHub{});
}

void PluginEditorHub::releaseInstance() {
	if (PluginEditorHub::instance) {
		delete PluginEditorHub::instance;
		PluginEditorHub::instance = nullptr;
	}
}

PluginEditorHub* PluginEditorHub::instance = nullptr;
