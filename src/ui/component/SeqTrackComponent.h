#pragma once

#include <JuceHeader.h>
#include "SeqTrackMuteComponent.h"
#include "SeqTrackRecComponent.h"

class SeqTrackComponent final : public juce::Component {
public:
	SeqTrackComponent();

	void update(int index);
	void updateBlock(int blockIndex);
	void updateMute();
	void updateRec();
	void updateHPos(double pos, double itemSize);

	void resized() override;
	void paint(juce::Graphics& g) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	int index = -1;
	juce::Colour trackColor, idColor;

	std::unique_ptr<juce::TextButton> trackName = nullptr;
	std::unique_ptr<SeqTrackMuteComponent> muteButton = nullptr;
	std::unique_ptr<SeqTrackRecComponent> recButton = nullptr;

	std::unique_ptr<juce::TextButton> instrButton = nullptr;
	std::unique_ptr<juce::DrawableButton> instrBypassButton = nullptr;
	std::unique_ptr<juce::DrawableButton> instrOfflineButton = nullptr;
	std::unique_ptr<juce::Drawable> instrBypassIcon = nullptr, instrBypassIconOn = nullptr;
	std::unique_ptr<juce::Drawable> instrOfflineIcon = nullptr, instrOfflineIconOn = nullptr;

	void editTrackName();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackComponent)
};
