#pragma once

#include <JuceHeader.h>

class SeqTrackComponent final : public juce::Component {
public:
	SeqTrackComponent();

	void update(int index);
	void updateBlock(int blockIndex);
	void updateHPos(double pos, double itemSize);

	void resized() override;
	void paint(juce::Graphics& g) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	int index = -1;
	juce::Colour trackColor, idColor;

	std::unique_ptr<juce::TextButton> trackName = nullptr;

	void editTrackName();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackComponent)
};
