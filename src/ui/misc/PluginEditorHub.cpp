#include "PluginEditorHub.h"
#include "CoreCallbacks.h"
#include "RCManager.h"
#include "../../audioCore/AC_API.h"

PluginEditorHub::PluginEditorHub() {
	/** Instr Update Callback */
	CoreCallbacks::getInstance()->addInstrChanged(
		[](int) {
			PluginEditorHub::getInstance()->updateInstr();
		}
	);

	/** Effect Update Callback */
	CoreCallbacks::getInstance()->addEffectChanged(
		[](int, int) {
			PluginEditorHub::getInstance()->updateEffect();
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
			auto container = std::make_unique<PluginEditor>(
				quickAPI::getInstrName(plugin),
				PluginType::Instr, plugin, editor);
			container->setOpenGL(this->openGLOn);
			container->setWindowIcon(this->iconTemp);

			/** Show */
			this->openEditor(container.get());

			/** Add To List */
			this->instrEditors.add(std::move(container));

			/** Callback */
			CoreCallbacks::getInstance()->invokeInstrChanged(index);
		}
	}
}

void PluginEditorHub::closeInstr(int index) {
	if (auto plugin = quickAPI::getInstrPointer(index)) {
		if (auto editor = quickAPI::getInstrEditor(plugin)) {
			for (int i = 0; i < this->instrEditors.size(); i++) {
				auto container = this->instrEditors.getUnchecked(i);
				if (container->getEditor() == editor) {
					/** Close */
					this->closeEditor(container);

					/** Remove From List */
					this->instrEditors.removeObject(container, true);

					/** Callback */
					CoreCallbacks::getInstance()->invokeInstrChanged(index);

					/** End Loop */
					break;
				}
			}
		}
	}
}

bool PluginEditorHub::checkInstr(int index) const {
	if (auto plugin = quickAPI::getInstrPointer(index)) {
		if (auto editor = quickAPI::getInstrEditorExists(plugin)) {
			for (int i = 0; i < this->instrEditors.size(); i++) {
				auto container = this->instrEditors.getUnchecked(i);
				if (container->getEditor() == editor) {
					return true;
				}
			}
		}
	}
	return false;
}

void PluginEditorHub::openEffect(int track, int index) {
	if (this->checkEffect(track, index)) { return; }

	if (auto plugin = quickAPI::getEffectPointer(track, index)) {
		if (auto editor = quickAPI::getEffectEditor(plugin)) {
			/** Create */
			auto container = std::make_unique<PluginEditor>(
				quickAPI::getEffectName(plugin),
				PluginType::Effect, plugin, editor);
			container->setOpenGL(this->openGLOn);
			container->setWindowIcon(this->iconTemp);

			/** Show */
			this->openEditor(container.get());

			/** Add To List */
			this->effectEditors.add(std::move(container));

			/** Callback */
			CoreCallbacks::getInstance()->invokeEffectChanged(track, index);
		}
	}
}

void PluginEditorHub::closeEffect(int track, int index) {
	if (auto plugin = quickAPI::getEffectPointer(track, index)) {
		if (auto editor = quickAPI::getEffectEditor(plugin)) {
			for (int i = 0; i < this->effectEditors.size(); i++) {
				auto container = this->effectEditors.getUnchecked(i);
				if (container->getEditor() == editor) {
					/** Close */
					this->closeEditor(container);

					/** Remove From List */
					this->effectEditors.removeObject(container, true);

					/** Callback */
					CoreCallbacks::getInstance()->invokeEffectChanged(track, index);

					/** End Loop */
					break;
				}
			}
		}
	}
}

bool PluginEditorHub::checkEffect(int track, int index) const {
	if (auto plugin = quickAPI::getEffectPointer(track, index)) {
		if (auto editor = quickAPI::getEffectEditorExists(plugin)) {
			for (int i = 0; i < this->effectEditors.size(); i++) {
				auto container = this->effectEditors.getUnchecked(i);
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
	for (auto i : this->instrEditors) {
		i->setOpenGL(openGLOn);
	}
	for (auto i : this->effectEditors) {
		i->setOpenGL(openGLOn);
	}
}

void PluginEditorHub::setIcon(const juce::String& path) {
	/** Load Icon */
	juce::File iconFile = juce::File::getSpecialLocation(
		juce::File::SpecialLocationType::hostApplicationPath).getParentDirectory().getChildFile(path);
	this->iconTemp = RCManager::getInstance()->loadImage(iconFile);

	/** Set All Windows */
	for (auto i : this->instrEditors) {
		i->setWindowIcon(this->iconTemp);
	}
	for (auto i : this->effectEditors) {
		i->setWindowIcon(this->iconTemp);
	}
}

void PluginEditorHub::closeAll() {
	while (this->instrEditors.size() > 0) {
		this->deleteInstrEditor(this->instrEditors.getUnchecked(0));
	}
	while (this->effectEditors.size() > 0) {
		this->deleteEffectEditor(this->effectEditors.getUnchecked(0));
	}
}

void PluginEditorHub::deleteInstrEditor(PluginEditor* ptr) {
	/** Close Comp */
	this->closeEditor(ptr);

	/** Delete Editor Item */
	this->instrEditors.removeObject(ptr, true);

	/** Callback */
	CoreCallbacks::getInstance()->invokeInstrChanged(-1);
}

void PluginEditorHub::deleteEffectEditor(PluginEditor* ptr) {
	/** Close Comp */
	this->closeEditor(ptr);

	/** Delete Editor Item */
	this->effectEditors.removeObject(ptr, true);

	/** Callback */
	CoreCallbacks::getInstance()->invokeEffectChanged(-1, -1);
}

void PluginEditorHub::closeEditor(PluginEditor* ptr) {
	ptr->setVisible(false);
}

void PluginEditorHub::openEditor(PluginEditor* ptr) {
	ptr->setVisible(true);
}

void PluginEditorHub::updateInstr() {
	for (auto i : this->instrEditors) {
		i->update();
	}
}

void PluginEditorHub::updateEffect() {
	for (auto i : this->effectEditors) {
		i->update();
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
