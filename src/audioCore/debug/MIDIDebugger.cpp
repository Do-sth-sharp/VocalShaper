#include "MIDIDebugger.h"

MIDIDebugger::MIDIDebugger()
	: Component("MIDIDebugger") {
	this->messageOutput = std::make_unique<juce::TextEditor>();
	this->messageOutput->setMultiLine(true);
	this->messageOutput->setReadOnly(true);
	this->messageOutput->setTabKeyUsedAsCharacter(true);
	this->messageOutput->setScrollToShowCursor(true);
	this->messageOutput->setScrollbarsShown(false);
	this->addAndMakeVisible(this->messageOutput.get());
}

void MIDIDebugger::resized() {
	this->messageOutput->setBounds(this->getLocalBounds());
}

void MIDIDebugger::addMessage(const juce::MidiMessage& message, bool input) {
	juce::String text;
	text += message.getDescription();
	text += " | ";
	text += juce::String::toHexString(message.getRawData(), message.getRawDataSize(), 1);
	text += "\n";
	juce::MessageManager::callAsync(
		[output = juce::Component::SafePointer(this->messageOutput.get()),
		text, maxNum = this->maxNum] {
			if (output) {
				juce::String current = output->getText();
				juce::StringArray list = juce::StringArray::fromLines(current);

				list.add(text);
				list.removeEmptyStrings(true);
				if (list.size() > maxNum) {
					list.removeRange(0, list.size() - maxNum);
				}

				output->setText(list.joinIntoString("\n"));
				output->moveCaretToEnd();
			}
		});
}

void MIDIDebugger::setMaxNum(int num) {
	this->maxNum = std::max(num, 0);

	juce::String current = this->messageOutput->getText();
	juce::StringArray list = juce::StringArray::fromLines(current);

	list.removeEmptyStrings(true);
	if (list.size() > maxNum) {
		list.removeRange(0, list.size() - std::max(maxNum, 0));
	}

	this->messageOutput->setText(list.joinIntoString("\n"));
	this->messageOutput->moveCaretToEnd();
}

int MIDIDebugger::getMaxNum() const {
	return this->maxNum;
}
