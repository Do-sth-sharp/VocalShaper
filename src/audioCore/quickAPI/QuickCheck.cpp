#include "QuickCheck.h"
#include "../misc/Renderer.h"
#include "../misc/PlayPosition.h"
#include "../misc/SourceIO.h"
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
					if ((!ptr->isAudioSaved()) || (!ptr->isMIDISaved())) {
						return false;
					}
				}
			}
		}
		return true;
	}
}
