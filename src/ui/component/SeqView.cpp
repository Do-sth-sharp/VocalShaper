﻿#include "SeqView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../misc/Tools.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"
#include <IconManager.h>

int SeqView::TrackList::size() const {
	return this->list.size();
}

void SeqView::TrackList::remove(int index) {
	this->list.remove(index, true);
}

void SeqView::TrackList::add(
	std::unique_ptr<juce::Component> newComp) {
	this->addAndMakeVisible(newComp.get());
	this->list.add(std::move(newComp));
}

void SeqView::TrackList::update(int index) {
	/** TODO */
	//this->list[index]->update(index);
}

void SeqView::TrackList::updateBlock(int track, int index) {
	/** TODO */
}

void SeqView::TrackList::updateHPos(double pos, double itemSize) {
	/** TODO */
}

void SeqView::TrackList::updateVPos(double pos, double itemSize) {
	for (int i = 0; i < this->list.size(); i++) {
		juce::Rectangle<int> trackRect(
			0, i * itemSize - pos,
			this->getWidth(), itemSize);
		this->list[i]->setBounds(trackRect);
	}
}

SeqView::SeqView()
	: FlowComponent(TRANS("Track")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSeq());

	/** Icons */
	this->adsorbIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Design", "align-item-left-line").getFullPathName());
	this->adsorbIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	/** Track List */
	this->trackList = std::make_unique<TrackList>();
	this->addAndMakeVisible(this->trackList.get());

	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return (double)(this->getViewWidth()); },
		[this] { return this->getTimeLength(); },
		[this] { return this->getTimeWidthLimit(); },
		[this](double pos, double itemSize) { this->updateHPos(pos, itemSize); },
		[this](juce::Graphics& g, int width, int height, bool vertical) {
				this->paintBlockPreview(g, width, height, vertical); },
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

	/** Button */
	this->adsorbButton = std::make_unique<juce::DrawableButton>(
		TRANS("Adsorb"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	this->adsorbButton->setImages(this->adsorbIcon.get());
	this->adsorbButton->setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->adsorbButton->setWantsKeyboardFocus(false);
	this->adsorbButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->adsorbButton->onClick = [this] { this->adsorbButtonClicked(); };
	this->addAndMakeVisible(this->adsorbButton.get());

	/** Time Ruler */
	this->ruler = std::make_unique<SeqTimeRuler>(
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double delta) {
			if (comp) {
				comp->scroll(delta);
			}
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double centerPer, double thumbPer, double delta) {
			if (comp) {
				comp->scale(centerPer, thumbPer, delta);
			}
		});
	this->addAndMakeVisible(this->ruler.get());

	/** Update Callback */
	CoreCallbacks::getInstance()->addSeqChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->update(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addSeqBlockChanged(
		[comp = SeqView::SafePointer(this)](int track, int index) {
			if (comp) {
				comp->updateBlock(track, index);
			}
		}
	);
}

void SeqView::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.035;
	int headWidth = screenSize.getWidth() * 0.1;

	int adsorbButtonHeight = screenSize.getHeight() * 0.025;
	int adsorbButtonPaddingWidth = screenSize.getWidth() * 0.005;
	int adsorbButtonPaddingHeight = screenSize.getHeight() * 0.005;

	/** Scroller */
	juce::Rectangle<int> hScrollerRect(
		headWidth, this->getHeight() - scrollerHeight,
		this->getWidth() - headWidth - scrollerWidth, scrollerHeight);
	this->hScroller->setBounds(hScrollerRect);
	juce::Rectangle<int> vScrollerRect(
		this->getWidth() - scrollerWidth, rulerHeight,
		scrollerWidth, this->getHeight() - rulerHeight - scrollerHeight);
	this->vScroller->setBounds(vScrollerRect);

	/** Adsorb Button */
	juce::Rectangle<int> adsorbRect(
		headWidth - adsorbButtonPaddingWidth - adsorbButtonHeight,
		rulerHeight - adsorbButtonPaddingHeight - adsorbButtonHeight,
		adsorbButtonHeight, adsorbButtonHeight);
	this->adsorbButton->setBounds(adsorbRect);

	/** Time Ruler */
	juce::Rectangle<int> rulerRect(
		headWidth, 0,
		hScrollerRect.getWidth(), rulerHeight);
	this->ruler->setBounds(rulerRect);

	/** Track List */
	juce::Rectangle<int> listRect(
		0, vScrollerRect.getY(),
		hScrollerRect.getWidth(), vScrollerRect.getHeight());
	this->trackList->setBounds(listRect);

	/** Update View Pos */
	this->hScroller->update();
	this->vScroller->update();
}

void SeqView::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.035;
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

	/** Top Right Corner */
	juce::Rectangle<float> topRightRect(
		this->getWidth() - scrollerWidth, 0,
		scrollerWidth, rulerHeight);
	g.setColour(headBackgroundColor);
	g.fillRect(topRightRect);

	/** Bottom Right Corner */
	juce::Rectangle<float> bottomRightRect(
		this->getWidth() - scrollerWidth, this->getHeight() - scrollerHeight,
		scrollerWidth, scrollerHeight);
	g.setColour(headBackgroundColor);
	g.fillRect(bottomRightRect);
}

void SeqView::update(int index) {
	/** Create Or Remove Track */
	int currentSize = this->trackList->size();
	int newSize = quickAPI::getSeqTrackNum();
	if (currentSize > newSize) {
		for (int i = currentSize - 1; i >= newSize; i--) {
			this->trackList->remove(i);
		}
	}
	else {
		for (int i = currentSize; i < newSize; i++) {
			auto track = std::make_unique<juce::Component>();
			this->trackList->add(std::move(track));
		}
	}

	/** Update Tracks */
	if (index >= 0 && index < this->trackList->size()) {
		this->trackList->update(index);
	}
	else {
		for (int i = 0; i < this->trackList->size(); i++) {
			this->trackList->update(i);
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
		for (int i = 0; i < this->trackList->size(); i++) {
			this->colorTemp.add(quickAPI::getSeqTrackColor(i));
		}
	}

	/** Update View Pos */
	this->vScroller->update();
	this->hScroller->update();
}

void SeqView::updateBlock(int track, int index) {
	/** Update Time Ruler */
	this->ruler->updateBlock(track, index);

	/** Update Tracks */
	this->trackList->updateBlock(track, index);

	/** Clear Temp */
	this->blockTemp.clear();

	/** Get Blocks */
	int trackNum = quickAPI::getSeqTrackNum();
	for (int i = 0; i < trackNum; i++) {
		auto list = quickAPI::getSeqBlockList(i);
		for (auto [startTime, endTime, offset] : list) {
			this->blockTemp.add({ i, startTime, endTime });
		}
	}

	/** Update Length */
	this->totalLength = quickAPI::getTotalLength();

	/** Update View Pos */
	this->hScroller->update();
}

int SeqView::getViewWidth() const {
	return this->hScroller->getWidth();
}

double SeqView::getTimeLength() const {
	return this->totalLength;
}

std::tuple<double, double> SeqView::getTimeWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getWidth() * 0.01, screenSize.getWidth() * 0.5 };
}

void SeqView::updateHPos(double pos, double itemSize) {
	this->ruler->updateHPos(pos, itemSize);
	this->trackList->updateHPos(pos, itemSize);
}

void SeqView::paintBlockPreview(juce::Graphics& g,
	int width, int height, bool /*vertical*/) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.0035;
	float trackMaxHeight = screenSize.getHeight() * 0.0035;

	int trackNum = this->trackList->size();
	float trackHeight = (height - paddingHeight * 2) / (double)(trackNum);
	trackHeight = std::min(trackHeight, trackMaxHeight);

	for (auto [trackIndex, startPos, endPos] : this->blockTemp) {
		if (trackIndex < trackNum) {
			juce::Rectangle<float> blockRect(
				startPos / this->totalLength * width,
				paddingHeight + trackIndex * trackHeight,
				(endPos - startPos) / this->totalLength * width,
				trackHeight);
			juce::Colour blockColor = this->colorTemp[trackIndex];

			g.setColour(blockColor);
			g.fillRect(blockRect);
		}
	}
}

int SeqView::getViewHeight() const {
	return this->vScroller->getHeight();
}

int SeqView::getTrackNum() const {
	return this->trackList->size();
}

std::tuple<double, double> SeqView::getTrackHeightLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getHeight() * 0.025, screenSize.getWidth() * 0.4 };
}

void SeqView::updateVPos(double pos, double itemSize) {
	this->trackList->updateVPos(pos, itemSize);
}

void SeqView::paintTrackPreview(juce::Graphics& g, int itemIndex,
	int width, int height, bool /*vertical*/) {
	/** Limit Size */
	if (itemIndex < 0 || itemIndex >= this->colorTemp.size()) { return; }

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0035;
	int colorWidth = screenSize.getWidth() * 0.0035;

	/** Color */
	juce::Colour color = this->colorTemp[itemIndex];

	/** Draw Color */
	juce::Rectangle<int> colorRect(
		paddingWidth, 0, colorWidth, height);
	g.setColour(color);
	g.fillRect(colorRect);
}

void SeqView::adsorbButtonClicked() {
	auto menu = this->createAdsorbMenu();
	int result = menu.showAt(this->adsorbButton.get());
	if (result == 0) { return; }

	if (result < 0) { Tools::getInstance()->setAdsorb(0); }
	else { Tools::getInstance()->setAdsorb(1 / (double)result); }
}

juce::PopupMenu SeqView::createAdsorbMenu() {
	double currentAdsorb = Tools::getInstance()->getAdsorb();

	juce::PopupMenu menu;
	menu.addItem(1, "1", true, juce::approximatelyEqual(currentAdsorb, 1.0));
	menu.addItem(2, "1/2", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)2));
	menu.addItem(4, "1/4", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)4));
	menu.addItem(6, "1/6", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)6));
	menu.addItem(8, "1/8", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)8));
	menu.addItem(12, "1/12", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)12));
	menu.addItem(16, "1/16", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)16));
	menu.addItem(24, "1/24", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)24));
	menu.addItem(32, "1/32", true, juce::approximatelyEqual(currentAdsorb, 1 / (double)32));
	menu.addItem(-1, "Off", true, juce::approximatelyEqual(currentAdsorb, 0.0));

	return menu;
}
