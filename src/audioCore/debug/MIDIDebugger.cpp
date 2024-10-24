#include "MIDIDebugger.h"

MIDIDebugger::MIDIDebugger()
	: AnimatedAppComponent() {
	this->setFramesPerSecond(10);
}

void MIDIDebugger::update() {
	/** Nothing To Do */
}

void MIDIDebugger::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour textColor = laf.findColour(
		juce::TextEditor::ColourIds::textColourId);
	juce::Colour backgroundColor = laf.findColour(
		juce::TextEditor::ColourIds::backgroundColourId);

	/** Size */
	int paddingWidth = 10, paddingHeight = 10;
	int lineHeight = 20;
	float fontHeight = 15.0f;

	/** Font */
	juce::Font textFont(juce::FontOptions{ fontHeight });

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Text */
	g.setColour(textColor);
	g.setFont(textFont);

	int top = paddingHeight;
	for (auto& i : this->mesList) {
		juce::Rectangle<int> textRect(
			paddingWidth, top,
			this->getWidth() - paddingWidth * 2, lineHeight);
		g.drawFittedText(i, textRect,
			juce::Justification::centredLeft, 1, 1.f);
		top += lineHeight;
	}
}

void MIDIDebugger::addMessage(const juce::MidiMessage& message, bool isInput) {
	juce::Time current = juce::Time::getCurrentTime();

	juce::String text;
	text += current.formatted("[%H:%M:%S] ");
	text += (isInput ? "[I] " : "[O] ");
	text += message.getDescription();
	text += " | ";
	text += juce::String::toHexString(
		message.getRawData(), message.getRawDataSize(), 1);
	this->mesList.insert(this->mesList.begin(), text);

	if (this->mesList.size() > this->maxNum) {
		this->mesList.resize(this->maxNum);
	}
}

void MIDIDebugger::setMaxNum(int num) {
	this->maxNum = std::max(num, 0);
}

int MIDIDebugger::getMaxNum() const {
	return this->maxNum;
}
