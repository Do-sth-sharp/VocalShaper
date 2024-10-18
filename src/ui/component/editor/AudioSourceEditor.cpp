#include "AudioSourceEditor.h"

AudioSourceEditor::AudioSourceEditor() {
	/** TODO */
}

void AudioSourceEditor::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.fillAll(backgroundColor);
}

void AudioSourceEditor::update(uint64_t ref) {
	this->ref = ref;
	/** TODO */
}
