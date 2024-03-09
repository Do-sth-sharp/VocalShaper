#include "SeqView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"

SeqView::SeqView()
	: FlowComponent(TRANS("Track")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSeq());

	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return this->getViewWidth(); },
		[this] { return this->getTimeLength(); },
		[this] { return this->getTimeWidthLimit(); },
		[this](int pos, int itemSize) { this->updateHPos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		Scroller::PaintItemPreviewFunc{});
	this->addAndMakeVisible(this->hScroller.get());

	this->vScroller = std::make_unique<Scroller>(true,
		[this] { return this->getViewHeight(); },
		[this] { return (double)this->getTrackNum(); },
		[this] { return this->getTrackHeightLimit(); },
		[this](int pos, int itemSize) { this->updateVPos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		Scroller::PaintItemPreviewFunc{});
	this->addAndMakeVisible(this->vScroller.get());
}

void SeqView::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.025;
	int headWidth = screenSize.getWidth() * 0.1;

	/** Scroller */
	juce::Rectangle<int> hScrollerRect(
		headWidth, this->getHeight() - scrollerHeight,
		this->getWidth() - headWidth - scrollerWidth, scrollerHeight);
	this->hScroller->setBounds(hScrollerRect);
	juce::Rectangle<int> vScrollerRect(
		this->getWidth() - scrollerWidth, rulerHeight,
		scrollerWidth, this->getHeight() - rulerHeight - scrollerHeight);
	this->vScroller->setBounds(vScrollerRect);

	/** Update View Pos */
	this->hScroller->update();
	this->vScroller->update();
}

int SeqView::getViewWidth() const {
	return this->hScroller->getWidth();
}

double SeqView::getTimeLength() const {
	/** TODO */
	return 0;
}

std::tuple<int, int> SeqView::getTimeWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { (int)(screenSize.getWidth() * 0.02), (int)(screenSize.getWidth() * 0.2) };
}

void SeqView::updateHPos(int pos, int itemSize) {
	/** TODO */
}

int SeqView::getViewHeight() const {
	return this->vScroller->getHeight();
}

int SeqView::getTrackNum() const {
	/** TODO */
	return 0;
}

std::tuple<int, int> SeqView::getTrackHeightLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { (int)(screenSize.getHeight() * 0.025), (int)(screenSize.getWidth() * 0.4) };
}

void SeqView::updateVPos(int pos, int itemSize) {
	/** TODO */
}
