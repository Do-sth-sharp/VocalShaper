#pragma once

#include <JuceHeader.h>

class SourceSwitchBar final : public juce::Component {
public:
	enum class SwitchState {
		Off, Audio, MIDI
	};
	using StateChangedCallback = std::function<void(SwitchState)>;
	SourceSwitchBar() = delete;
	SourceSwitchBar(const StateChangedCallback& stateCallback);

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	const StateChangedCallback stateCallback;
	std::unique_ptr<juce::Drawable> switchIcon = nullptr;
	std::unique_ptr<juce::DrawableButton> switchButton = nullptr;
	std::unique_ptr<juce::TextButton> nameButton = nullptr;

	void showSwitchMenu();
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceSwitchBar)
};
