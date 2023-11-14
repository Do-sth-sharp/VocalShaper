#include "ColorMap.h"

void ColorMap::set(
	const juce::String& key, const juce::Colour& value) {
	this->map.insert({ key, value });
}

const juce::Colour ColorMap::get(const juce::String& key) const {
	auto it = this->map.find(key);
	if (it == this->map.end()) { return juce::Colour{}; }

	return it->second;
}

static uint8_t hexToDec(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return 10 + (c - 'a');
	}
	if (c >= 'A' && c <= 'F') {
		return 10 + (c - 'A');
	}
	return 0;
}

const juce::Colour ColorMap::fromString(const juce::String& str) {
	/** #RRGGBB or #RRGGBBAA */
	if (str.startsWithChar('#')) {
		if (str.length() == 7) {
			return juce::Colour::fromRGB(
				hexToDec(str[1]) * 16 + hexToDec(str[2]),
				hexToDec(str[3]) * 16 + hexToDec(str[4]),
				hexToDec(str[5]) * 16 + hexToDec(str[6])
			);
		}
		if (str.length() == 9) {
			return juce::Colour::fromRGBA(
				hexToDec(str[1]) * 16 + hexToDec(str[2]),
				hexToDec(str[3]) * 16 + hexToDec(str[4]),
				hexToDec(str[5]) * 16 + hexToDec(str[6]),
				hexToDec(str[7]) * 16 + hexToDec(str[8])
			);
		}
	}

	/** RRGGBB or RRGGBBAA */
	if (str.length() == 6) {
		return juce::Colour::fromRGB(
			hexToDec(str[0]) * 16 + hexToDec(str[1]),
			hexToDec(str[2]) * 16 + hexToDec(str[3]),
			hexToDec(str[4]) * 16 + hexToDec(str[5])
		);
	}
	if (str.length() == 8) {
		return juce::Colour::fromRGBA(
			hexToDec(str[0]) * 16 + hexToDec(str[1]),
			hexToDec(str[2]) * 16 + hexToDec(str[3]),
			hexToDec(str[4]) * 16 + hexToDec(str[5]),
			hexToDec(str[6]) * 16 + hexToDec(str[7])
			);
	}

	return juce::Colour{};
}

ColorMap* ColorMap::getInstance() {
	return ColorMap::instance ? ColorMap::instance : (ColorMap::instance = new ColorMap{});
}

void ColorMap::releaseInstance() {
	if (ColorMap::instance) {
		delete ColorMap::instance;
		ColorMap::instance = nullptr;
	}
}

ColorMap* ColorMap::instance = nullptr;
