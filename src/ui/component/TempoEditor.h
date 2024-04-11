#pragma once

#include <JuceHeader.h>

class TempoEditor;

class TempoEditorContent final : public juce::Component {
public:
	TempoEditorContent() = delete;
	TempoEditorContent(bool defaultIsTempo = true,
		double defaultTempo = 120.0, int defaultNumerator = 4, int defaultDenominator = 4,
		bool switchable = true);

	void resized() override;

	/** isTempo, tempo, numerator, denominator */
	using TempoResult = std::tuple<bool, double, int, int>;
	const TempoResult getResult() const;

private:
	std::unique_ptr<juce::ToggleButton> tempoToggle = nullptr, beatToggle = nullptr;
	std::unique_ptr<juce::TextEditor> tempoEditor = nullptr;
	std::unique_ptr<juce::TextEditor> numeratorEditor = nullptr;
	std::unique_ptr<juce::ComboBox> denominatorEditor = nullptr;
	std::unique_ptr<juce::Label> beatSplitLabel = nullptr;

	void setIsTempo(bool isTempo);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoEditorContent)
};

class TempoEditor final : public juce::DocumentWindow {
public:
	TempoEditor() = delete;
	TempoEditor(bool defaultIsTempo = true,
		double defaultTempo = 120.0, int defaultNumerator = 4, int defaultDenominator = 4,
		bool switchable = true);

	void closeButtonPressed() override;

	using TempoResult = TempoEditorContent::TempoResult;
	const TempoResult getResult() const;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoEditor)
};
