#pragma once

#include <Defs.h>

namespace utils {
	std::tuple<int, int> getChannelIndexAndNumOfBus(
		const juce::AudioProcessor* processor, int busIndex, bool isInput);
}
