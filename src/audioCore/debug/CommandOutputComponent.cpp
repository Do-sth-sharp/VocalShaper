#include "CommandOutputComponent.h"

#define COMMAND_OUTPUT_MAX_NUM 20

void CommandOutputComponent::add(const juce::String& line) {
	this->outList.add(line.endsWithChar('\n') ? line : (line + '\n'));
	if (this->outList.size() > COMMAND_OUTPUT_MAX_NUM) {
		this->outList.removeRange(
			0, this->outList.size() - COMMAND_OUTPUT_MAX_NUM);
	}

	this->strTemp = this->outList.joinIntoString("");

	this->repaint();
}

void CommandOutputComponent::paint(juce::Graphics& g) {
	/** Colors */
	auto& laf = this->getLookAndFeel();
	auto backgroundColor = laf.findColour(
		juce::TextEditor::ColourIds::backgroundColourId);
	auto textColor = laf.findColour(
		juce::TextEditor::ColourIds::textColourId);

	/** Font */
	juce::Font font(juce::FontOptions{ 15.f });

	/** Background */
	g.fillAll(backgroundColor);

	/** Text */
	juce::Rectangle<int> textRect(
		10, 10, this->getWidth() - 20, this->getHeight() - 20);
	g.setFont(font);
	g.setColour(textColor);
	g.drawFittedText(this->strTemp, textRect,
		juce::Justification::bottomLeft, INT_MAX, 1.f);
}
