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

	bool checkSourceIOTask(int index);
	bool checkSourceSynthing(int index);
	bool checkSourceRecording(int index);
	bool checkSourceRecordingNow(int index);
}
