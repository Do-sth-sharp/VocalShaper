#include "SourceView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"

SourceView::SourceView()
	: FlowComponent(TRANS("Source")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSourceView());

	/** Source List */
	this->sources = std::make_unique<SourceListModel>(this);
	this->list = std::make_unique<juce::ListBox>(
		TRANS("Source List"), this->sources.get());
	this->addAndMakeVisible(this->list.get());

	/** Update Callback */
	CoreCallbacks::getInstance()->addSourceChanged(
		[comp = SourceView::SafePointer(this)](int) {
			if (comp) {
				comp->update();
			}
		}
	);
}

SourceView::~SourceView() {
	this->list->setModel(nullptr);
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

void SourceView::update() {
	this->list->updateContent();
}
