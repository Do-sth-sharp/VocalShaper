#include "QuickCheck.h"
#include "../misc/Renderer.h"
#include "../source/AudioIOList.h"
#include "../plugin/PluginLoader.h"
#include "../plugin/Plugin.h"

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
}
