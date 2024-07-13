#include "QuickCheck.h"
#include "../misc/Renderer.h"
#include "../misc/PlayPosition.h"
#include "../source/SourceIO.h"
#include "../source/SourceManager.h"
#include "../plugin/PluginLoader.h"
#include "../plugin/Plugin.h"
#include "../project/ProjectInfoData.h"
#include "../AudioCore.h"

namespace quickAPI {
	bool checkRendering() {
		return Renderer::getInstance()->getRendering();
	}

	bool checkSourceIORunning() {
		return SourceIO::getInstance()->isThreadRunning();
	}

	bool checkPluginLoading() {
		return PluginLoader::getInstance()->isRunning();
	}

	bool checkPluginSearching() {
		return Plugin::getInstance()->pluginSearchThreadIsRunning();
	}

	bool checkProjectSaved() {
		return ProjectInfoData::getInstance()->checkSaved();
	}

	bool checkSourcesSaved() {
		if (auto graph = AudioCore::getInstance()->getGraph()) {
			int size = graph->getSourceNum();
			for (int i = 0; i < size; i++) {
				if (auto ptr = graph->getSourceProcessor(i)) {
					auto audioRef = ptr->getAudioRef();
					auto midiRef = ptr->getMIDIRef();
					bool audioSaved = SourceManager::getInstance()->isSaved(
						audioRef, SourceManager::SourceType::Audio);
					bool midiSaved = SourceManager::getInstance()->isSaved(
						midiRef, SourceManager::SourceType::MIDI);

					if ((!audioSaved) || (!midiSaved)) {
						return false;
					}
				}
			}
		}
		return true;
	}
}
