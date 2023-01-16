#include "Utils.h"

namespace utils {
	std::tuple<int, int> getChannelIndexAndNumOfBus(
		const juce::AudioProcessor* processor, int busIndex, bool isInput) {
		if (!processor) { return std::tuple<int, int>(); }
		if (busIndex < 0 || busIndex >= processor->getBusCount(isInput)) {
			return std::tuple<int, int>();
		}

		int index = 0, num = 0;
		for (int i = 0; i <= busIndex; i++) {
			num = processor->getChannelCountOfBus(isInput, i);
			if (i != busIndex) {
				index += num;
			}
		}

		return std::make_tuple(index, num);
	}
}
