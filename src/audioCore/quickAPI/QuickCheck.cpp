#include "QuickCheck.h"
#include "../misc/Renderer.h"
#include "../misc/PlayPosition.h"
#include "../plugin/PluginLoader.h"
#include "../plugin/Plugin.h"
#include "../project/ProjectInfoData.h"

namespace quickAPI {
	bool checkRendering() {
		return Renderer::getInstance()->getRendering();
	}

	bool checkSourceIORunning() {
		/** TODO */
		return false;
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
		/** TODO */
		return true;
	}

	bool checkSourceIOTask(int index) {
		/** TODO */
		return false;
	}

	bool checkSourceSynthing(int index) {
		/** TODO */
		return false;
	}

	bool checkSourceRecording(int index) {
		/** TODO */
		return false;
	}

	bool checkSourceRecordingNow(int index) {
		/** TODO */
		return false;
	}
}
