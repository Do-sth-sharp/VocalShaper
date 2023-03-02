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
	juce::String text;
	text += message.getDescription();
	text += " | ";
	text += juce::String::toHexString(message.getRawData(), message.getRawDataSize(), 1);
	text += "\n";
	juce::MessageManager::callAsync(
		[output = juce::Component::SafePointer(this->messageOutput.get()), text] {
			if (output) {
				output->moveCaretToEnd();
				output->insertTextAtCaret(text);
			}
		});
}
