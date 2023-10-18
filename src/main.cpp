#include <LUCE.h>
#include "audioCore/AC_API.h"

LUCE_APPLICATION("VocalShaper", "0.0.1", false, "main.lua", {
			luce::loadCore(L);
			luce::loadGUI(L);
			luce::loadGraphics(L);
			luce::loadFlowUI(L);
			luce::loadAudioCore(L);
	});