#include "SourceView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

SourceView::SourceView()
	: FlowComponent(TRANS("Source")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSourceView());

	/** Source List */
	this->list = std::make_unique<juce::ListBox>(
		TRANS("Source List"), nullptr);
	this->addAndMakeVisible(this->list.get());
}

void SourceView::resized() {
	/** Source List */
	this->list->setBounds(this->getLocalBounds());
}

void SourceView::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** BackGround */
	g.setColour(backgroundColor);
	g.fillAll();
}
