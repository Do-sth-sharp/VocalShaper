#include "MIDISourceEditor.h"

MIDISourceEditor::MIDISourceEditor() {
	/** TODO */
}

void MIDISourceEditor::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.fillAll(backgroundColor);
}

void MIDISourceEditor::update(uint64_t ref) {
	this->ref = ref;
	/** TODO */
}
