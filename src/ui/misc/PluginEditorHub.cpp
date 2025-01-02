#include "PluginEditorHub.h"
#include "CoreCallbacks.h"
#include "RCManager.h"
#include "../../audioCore/AC_API.h"

PluginEditorHub::PluginEditorHub() {
	/** Instr Update Callback */
	CoreCallbackAPI<int>::add(CoreCallbacks::CallbackType::TrackInstrChanged,
		[](int index) {
			PluginEditorHub::getInstance()->updateInstr(index);
		}
	);

	/** Effect Update Callback */
	CoreCallbackAPI<int, int, int>::add(CoreCallbacks::CallbackType::TrackEffectChanged,
		[](int type, int track, int index) {
			PluginEditorHub::getInstance()->updateEffect(type, track, index);
		}
	);
}

PluginEditorHub::~PluginEditorHub() {
	this->closeAll();
}

void PluginEditorHub::openInstr(int index) {
	if (this->checkInstr(index)) { return; }

	if (auto plugin = quickAPI::getInstrPointer(index)) {
		if (auto editor = quickAPI::getInstrEditor(plugin)) {
			/** Create */
			auto container = std::make_shared<PluginEditor>(
				quickAPI::getInstrName(plugin),
				PluginType::Instr, plugin, editor);
			container->setOpenGL(this->openGLOn);
			container->setWindowIcon(this->iconTemp);

			container->update(index);

			/** Show */
			this->openEditor(container.get());

			/** Add To List */
			this->instrEditors.insert({ this->getInstrRef(index), container });

			/** Callback */
			CoreCallbackAPI<int>::invoke(CoreCallbacks::CallbackType::TrackInstrChanged, index);
		}
	}
}

void PluginEditorHub::closeInstr(int index) {
	if (auto plugin = quickAPI::getInstrPointer(index)) {
		if (auto editor = quickAPI::getInstrEditor(plugin)) {
			auto it = this->instrEditors.find(this->getInstrRef(index));
			if (it != this->instrEditors.end()) {
				auto container = it->second;
				if (container->getEditor() == editor) {
					/** Close */
					this->closeEditor(container.get());

					/** Remove From List */
					this->instrEditors.erase(it);

					/** Callback */
					CoreCallbackAPI<int>::invoke(CoreCallbacks::CallbackType::TrackInstrChanged, index);
				}
			}
		}
	}
}

bool PluginEditorHub::checkInstr(int index) const {
	if (auto plugin = quickAPI::getInstrPointer(index)) {
		if (auto editor = quickAPI::getInstrEditorExists(plugin)) {
			auto it = this->instrEditors.find(this->getInstrRef(index));
			if (it != this->instrEditors.end()) {
				auto container = it->second;
				if (container->getEditor() == editor) {
					return true;
				}
			}
		}
	}
	return false;
}

void PluginEditorHub::openEffect(int type, int track, int index) {
	if (this->checkEffect(type, track, index)) { return; }

	if (auto plugin = quickAPI::getEffectPointer({ (quickAPI::TrackType)type, track }, index)) {
		if (auto editor = quickAPI::getEffectEditor(plugin)) {
			/** Create */
			auto container = std::make_shared<PluginEditor>(
				quickAPI::getEffectName(plugin),
				PluginType::Effect, plugin, editor);
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
}

void PluginEditorHub::closeEffect(int type, int track, int index) {
	if (auto plugin = quickAPI::getEffectPointer({ (quickAPI::TrackType)type, track }, index)) {
		if (auto editor = quickAPI::getEffectEditor(plugin)) {
			auto it = this->effectEditors.find(this->getEffectRef(type, track, index));
			if (it != this->effectEditors.end()) {
				auto container = it->second;
				if (container->getEditor() == editor) {
					/** Close */
					this->closeEditor(container.get());

					/** Remove From List */
					this->effectEditors.erase(it);

					/** Callback */
					CoreCallbackAPI<int, int, int>::invoke(
						CoreCallbacks::CallbackType::TrackEffectChanged, type, track, index);
				}
			}
		}
	}
}

bool PluginEditorHub::checkEffect(int type, int track, int index) const {
	if (auto plugin = quickAPI::getEffectPointer({ (quickAPI::TrackType)type, track }, index)) {
		if (auto editor = quickAPI::getEffectEditorExists(plugin)) {
			auto it = this->effectEditors.find(this->getEffectRef(type, track, index));
			if (it != this->effectEditors.end()) {
				auto container = it->second;
				if (container->getEditor() == editor) {
					return true;
				}
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

void PluginEditorHub::updateInstr(int index) {
	if (auto ref = this->getInstrRef(index)) {
		auto it = this->instrEditors.find(ref);
		if (it != this->instrEditors.end()) {
			it->second->update(index);
		}
	}
}

void PluginEditorHub::updateEffect(int type, int track, int index) {
	if (auto ref = this->getEffectRef(type, track, index)) {
		auto it = this->effectEditors.find(ref);
		if (it != this->effectEditors.end()) {
			it->second->update(type, track, index);
		}
	}
}

PluginEditorHub::RefType PluginEditorHub::getInstrRef(int index) const {
	auto ptr = quickAPI::getInstrPointer(index);
	return reinterpret_cast<RefType>(ptr.getPlugin());
}

PluginEditorHub::RefType PluginEditorHub::getEffectRef(int type, int track, int index) const {
	auto ptr = quickAPI::getEffectPointer({ (quickAPI::TrackType)type, track }, index);
	return reinterpret_cast<RefType>(ptr.getPlugin());
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
