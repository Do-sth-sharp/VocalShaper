#include "QuickGet.h"
#include "../AudioCore.h"
#include "../source/CloneableSourceManager.h"

namespace quickAPI {
	juce::Component* getAudioDebugger() {
		return AudioCore::getInstance()->getAudioDebugger();
	}

	juce::Component* getMidiDebugger() {
		return AudioCore::getInstance()->getMIDIDebugger();
	}

	const juce::File getProjectDir() {
		return utils::getProjectDir();
	}

	const juce::StringArray getSourceNames() {
		juce::StringArray result;

		int size = CloneableSourceManager::getInstance()->getSourceNum();
		for (int i = 0; i < size; i++) {
			auto ptr = CloneableSourceManager::getInstance()->getSource(i);
			if (!ptr) { result.add("-"); continue; }

			juce::String name = ptr->getName();
			if (name.isEmpty()) {
				name = utils::getProjectDir().getChildFile(ptr->getPath()).getFileName();
			}
			result.add(name);
		}

		return result;
	}
}