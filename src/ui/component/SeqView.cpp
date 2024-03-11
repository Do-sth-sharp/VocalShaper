#include "SeqView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqView::SeqView()
	: FlowComponent(TRANS("Track")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSeq());

	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return (double)(this->getViewWidth()); },
		[this] { return this->getTimeLength(); },
		[this] { return this->getTimeWidthLimit(); },
		[this](double pos, double itemSize) { this->updateHPos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		Scroller::PaintItemPreviewFunc{});
	this->addAndMakeVisible(this->hScroller.get());

	this->vScroller = std::make_unique<Scroller>(true,
		[this] { return (double)(this->getViewHeight()); },
		[this] { return (double)(this->getTrackNum()); },
		[this] { return this->getTrackHeightLimit(); },
		[this](double pos, double itemSize) { this->updateVPos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		[this](juce::Graphics& g, int itemIndex,
			int width, int height, bool vertical) {
				this->paintTrackPreview(g, itemIndex, width, height, vertical); });
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

void SeqView::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.025;
	int headWidth = screenSize.getWidth() * 0.1;

	float lineThickness = screenSize.getHeight() * 0.0025;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour headBackgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Head Bar */
	juce::Rectangle<int> headRect(
		0, 0, headWidth, this->getHeight());
	g.setColour(headBackgroundColor);
	g.fillRect(headRect);

	/** Top Head Split */
	juce::Rectangle<float> headTopLineRect(
		0, rulerHeight - lineThickness / 2, headWidth, lineThickness);
	g.setColour(outlineColor);
	g.fillRect(headTopLineRect);

	/** Bottom Head Split */
	juce::Rectangle<float> bottomTopLineRect(
		0, this->getHeight() - scrollerHeight - lineThickness / 2, headWidth, lineThickness);
	g.setColour(outlineColor);
	g.fillRect(bottomTopLineRect);
}

void SeqView::update(int index) {
	/** Create Or Remove Track */
	int currentSize = this->trackList.size();
	int newSize = quickAPI::getSeqTrackNum();
	if (currentSize > newSize) {
		for (int i = currentSize - 1; i >= newSize; i--) {
			this->trackList.remove(i, true);
		}
	}
	else {
		for (int i = currentSize; i < newSize; i++) {
			auto track = std::make_unique<juce::Component>();
			this->addAndMakeVisible(track.get());
			this->trackList.add(std::move(track));
		}
	}

	/** Update Tracks */
	if (index >= 0 && index < this->trackList.size()) {
		//this->trackList[index]->update(index);
	}
	else {
		for (int i = 0; i < this->trackList.size(); i++) {
			//this->trackList[i]->update(i);
		}
	}

	/** Update Color Temp */
	if (index >= 0 && index < this->colorTemp.size()) {
		if (index < newSize) {
			this->colorTemp.getReference(index) = quickAPI::getSeqTrackColor(index);
		}
		if (this->colorTemp.size() > newSize) {
			this->colorTemp.resize(newSize);
		}
	}
	else {
		this->colorTemp.clear();
		for (int i = 0; i < this->trackList.size(); i++) {
			this->colorTemp.add(quickAPI::getSeqTrackColor(i));
		}
	}

	/** Update View Pos */
	this->vScroller->update();
	this->hScroller->update();
}

int SeqView::getViewWidth() const {
	return this->hScroller->getWidth();
}

double SeqView::getTimeLength() const {
	/** TODO */
	return 0;
}

std::tuple<double, double> SeqView::getTimeWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getWidth() * 0.02, screenSize.getWidth() * 0.2 };
}

void SeqView::updateHPos(double pos, double itemSize) {
	/** TODO */
}

int SeqView::getViewHeight() const {
	return this->vScroller->getHeight();
}

int SeqView::getTrackNum() const {
	/** TODO */
	return 0;
}

std::tuple<double, double> SeqView::getTrackHeightLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getHeight() * 0.025, screenSize.getWidth() * 0.4 };
}

void SeqView::updateVPos(double pos, double itemSize) {
	/** TODO */
}

void SeqView::paintTrackPreview(juce::Graphics& g, int itemIndex,
	int width, int height, bool vertical) {
	/** TODO */
}
