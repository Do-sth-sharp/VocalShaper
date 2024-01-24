#include "PluginEditorHub.h"
#include "CoreCallbacks.h"
#include "../../audioCore/AC_API.h"

PluginEditorHub::PluginEditorHub() {
	/** Instr Update Callback */
	CoreCallbacks::getInstance()->addInstrChanged(
		[](int) {
			PluginEditorHub::getInstance()->updateInstr();
		}
	);
}

void PluginEditorHub::openInstr(int index) {
	if (this->checkInstr(index)) { return; }

	if (auto plugin = quickAPI::getInstrPointer(index)) {
		if (auto editor = quickAPI::getInstrEditor(plugin)) {
			/** Create */
			auto container = std::make_unique<PluginEditor>(
				quickAPI::getInstrName(plugin),
				PluginType::Instr, plugin, editor);

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
		if (auto editor = quickAPI::getInstrEditor(plugin)) {
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
	CoreCallbacks::getInstance()->invokeInstrChanged(-1);
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
