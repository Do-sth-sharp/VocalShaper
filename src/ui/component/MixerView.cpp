#include "MixerView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"

MixerView::MixerView()
	: FlowComponent(TRANS("Mixer")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMixer());

	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return this->getViewWidth(); },
		[this] { return this->getTrackNum(); },
		[this] { return this->getTrackWidthLimit(); },
		[this](int pos, int itemSize) { this->updatePos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		[this](juce::Graphics& g, int itemIndex,
			int width, int height, bool vertical) {
				this->paintTrackPreview(g, itemIndex, width, height, vertical); });
	this->addAndMakeVisible(this->hScroller.get());
}

void MixerView::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.025;

	/** Scroller */
	juce::Rectangle<int> scrollerRect(
		0, this->getHeight() - scrollerHeight,
		this->getWidth(), scrollerHeight);
	this->hScroller->setBounds(scrollerRect);

	/** Update View Pos */
	this->hScroller->update();
}

void MixerView::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

int MixerView::getViewWidth() const {
	return this->getWidth();
}

int MixerView::getTrackNum() const {
	/** TODO */
	return 20;
}

std::tuple<int, int> MixerView::getTrackWidthLimit() const {
	/** TODO */
	return { this->getWidth() / 8, this->getWidth() / 4 };
}

void MixerView::updatePos(int pos, int itemSize) {
	/** TODO */
}

void MixerView::paintTrackPreview(juce::Graphics& g, int itemIndex,
	int width, int height, bool vertical) {
	/** TODO */
}
