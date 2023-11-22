#pragma once

#include <JuceHeader.h>

namespace quickAPI {
	bool checkRendering();
	bool checkSourceIORunning();
	bool checkPluginLoading();
	bool checkPluginSearching();

	bool checkProjectSaved();
	bool checkSourcesSaved();

	bool checkForAudioSource(int index);
	bool checkForMidiSource(int index);
	bool checkForSynthSource(int index);
}
