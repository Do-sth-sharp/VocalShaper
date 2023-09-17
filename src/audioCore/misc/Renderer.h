#pragma once

#include <JuceHeader.h>

class Renderer final : private juce::DeletedAtShutdown {
public:
	Renderer() = default;

	void setRendering(bool rendering);
	bool getRendering() const;

	/** TODO Prepare To Render */
	/** TODO Write Audo File */

private:
	friend class Track;
	void writeData(int trackNum, const juce::AudioBuffer<float>& buffer, int offset);

private:
	std::atomic_bool rendering = false;

public:
	static Renderer* getInstance();
	static void releaseInstance();

private:
	static Renderer* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Renderer)
};
