#include "SourceEditor.h"
#include "../../Utils.h"

SourceEditor::SourceEditor()
	: FlowComponent(TRANS("Resource Editor")) {
	/** Switch Bar */
	auto switchCallback = [this](SourceSwitchBar::SwitchState state) {
		this->switchEditor(state);
		};
	this->switchBar = std::make_unique<SourceSwitchBar>(switchCallback);
	this->addAndMakeVisible(this->switchBar.get());
}

void SourceEditor::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int switchBarHeight = screenSize.getHeight() * 0.03;

	/** Switch Bar */
	juce::Rectangle<int> switchBarRect(
		0, 0, this->getWidth(), switchBarHeight);
	this->switchBar->setBounds(switchBarRect);
}

void SourceEditor::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.fillAll(backgroundColor);
}

void SourceEditor::switchEditor(SourceSwitchBar::SwitchState state) {
	/** TODO */
}
