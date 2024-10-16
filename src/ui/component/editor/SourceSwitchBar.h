#pragma once

#include <JuceHeader.h>

class SourceSwitchBar final : public juce::Component {
public:
	enum class SwitchState {
		Off = 1, Audio, MIDI
	};
	using StateChangedCallback = std::function<void(SwitchState)>;
	SourceSwitchBar() = delete;
	SourceSwitchBar(const StateChangedCallback& stateCallback);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index, uint64_t audioRef, uint64_t midiRef);
	void switchTo(SwitchState state);

private:
	const StateChangedCallback stateCallback;
	std::unique_ptr<juce::Drawable> switchIcon = nullptr;
	std::unique_ptr<juce::DrawableButton> switchButton = nullptr;
	std::unique_ptr<juce::TextButton> nameButton = nullptr;

	SwitchState current = SwitchState::Off;
	int index = -1;
	juce::String trackName, trackEmptyName;
	uint64_t audioRef = 0, midiRef = 0;
	juce::String audioName, midiName;
	juce::String audioCreateStr, midiCreateStr;

	void showSwitchMenu();
	void switchInternal(SwitchState state);
	void syncButtonName();
	juce::PopupMenu createSwitchMenu() const;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceSwitchBar)
};
