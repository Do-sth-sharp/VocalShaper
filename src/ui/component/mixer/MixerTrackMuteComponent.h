#pragma once

#include <JuceHeader.h>

class MixerTrackMuteComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	using RightButtonCallback = std::function<void(void)>;
	MixerTrackMuteComponent(const RightButtonCallback& rightCallback);

	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void update(int index);

private:
	const RightButtonCallback rightCallback;

	int index = -1;
	bool mute = false;

	void changeMute();
	void showMenu();

	juce::PopupMenu createMenu() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTrackMuteComponent)
};
