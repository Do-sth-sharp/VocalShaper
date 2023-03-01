#include "MIDIDebugger.h"

MIDIDebugger::MIDIDebugger()
	: Component("MIDIDebugger") {
	this->messageOutput = std::make_unique<juce::TextEditor>();
	this->messageOutput->setMultiLine(true);
	this->messageOutput->setReadOnly(true);
	this->messageOutput->setTabKeyUsedAsCharacter(true);
	this->messageOutput->setScrollToShowCursor(true);
	this->addAndMakeVisible(this->messageOutput.get());
}

void MIDIDebugger::resized() {
	this->messageOutput->setBounds(this->getLocalBounds());
}

void MIDIDebugger::addMessage(const juce::MidiMessage& message) {
	//TODO
}
