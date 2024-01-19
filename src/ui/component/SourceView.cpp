#include "SourceView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../Utils.h"

SourceView::SourceView()
	: FlowComponent(TRANS("Source")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSourceView());

	/** Source List */
	this->sources = std::make_unique<SourceListModel>(
		[this](int index) { this->select(index); }
	);
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
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int itemHeight = screenSize.getHeight() * 0.1;

	/** Source List */
	this->list->setBounds(this->getLocalBounds());

	/** Source Item Height */
	this->list->setRowHeight(itemHeight);
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

void SourceView::select(int index) {
	this->list->selectRow(index);
}
