#include "QuickCheck.h"
#include "../misc/Renderer.h"
#include "../source/AudioIOList.h"
#include "../source/CloneableSourceManager.h"
#include "../plugin/PluginLoader.h"
#include "../plugin/Plugin.h"
#include "../project/ProjectInfoData.h"

namespace quickAPI {
	bool checkRendering() {
		return Renderer::getInstance()->getRendering();
	}

	bool checkSourceIORunning() {
		return AudioIOList::getInstance()->isThreadRunning();
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
		int size = CloneableSourceManager::getInstance()->getSourceNum();
		for (int i = 0; i < size; i++) {
			if (auto ptr = CloneableSourceManager::getInstance()->getSource(i)) {
				if (!ptr->checkSaved()) {
					return false;
				}
			}
		}
		return true;
	}
}
