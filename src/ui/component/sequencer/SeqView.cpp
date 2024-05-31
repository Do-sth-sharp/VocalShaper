#include "SeqView.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../misc/CoreCallbacks.h"
#include "../../misc/Tools.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

#define SEQ_TAIL_SEC 10;

int SeqView::TrackList::size() const {
	return this->list.size();
}

void SeqView::TrackList::remove(int index) {
	this->list.remove(index, true);
}

void SeqView::TrackList::add(
	std::unique_ptr<SeqTrackComponent> newComp) {
	this->addAndMakeVisible(newComp.get());
	this->list.add(std::move(newComp));
}

void SeqView::TrackList::update(int index) {
	this->list[index]->update(index);
}

void SeqView::TrackList::updateBlock(int track, int index) {
	if (track >= 0 && track < this->list.size()) {
		this->list[track]->updateBlock(index);
	}
}

void SeqView::TrackList::updateMute(int index) {
	if (index >= 0 && index < this->list.size()) {
		this->list[index]->updateMute();
	}
}

void SeqView::TrackList::updateRec(int index) {
	if (index >= 0 && index < this->list.size()) {
		this->list[index]->updateRec();
	}
}

void SeqView::TrackList::updateInstr(int index) {
	if (index >= 0 && index < this->list.size()) {
		this->list[index]->updateInstr();
	}
	else {
		for (auto i : this->list) {
			i->updateInstr();
		}
	}
}

void SeqView::TrackList::updateMixerTrack() {
	for (auto i : this->list) {
		i->updateMixerTrack();
	}
}

void SeqView::TrackList::updateDataRef(int index) {
	if (index >= 0 && index < this->list.size()) {
		this->list[index]->updateDataRef();
	}
}

void SeqView::TrackList::updateData(int index) {
	if (index >= 0 && index < this->list.size()) {
		this->list[index]->updateData();
	}
	else {
		for (auto i : this->list) {
			i->updateData();
		}
	}
}

void SeqView::TrackList::updateHPos(double pos, double itemSize) {
	for (auto i : this->list) {
		i->updateHPos(pos, itemSize);
	}
}

void SeqView::TrackList::updateVPos(double pos, double itemSize) {
	for (int i = 0; i < this->list.size(); i++) {
		juce::Rectangle<int> trackRect(
			0, i * itemSize - pos,
			this->getWidth(), itemSize);
		this->list[i]->setBounds(trackRect);
	}
}

void SeqView::TrackList::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->add();
	}
}

void SeqView::TrackList::add() {
	CoreActions::insertSeqGUI();
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

	/** Notice */
	this->emptyNoticeStr = TRANS("Right click on the blank space to create a new track.");

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
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutside(deltaY, reversed);
			}
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double centerNum, double thumbPer, float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutsideWithAlt(centerNum, thumbPer, deltaY, reversed);
			}
		},
		[comp = ScrollerBase::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragStart();
			}
		},
		[comp = ScrollerBase::SafePointer(this)]
		(int distanceX, int distanceY, bool moveX, bool moveY) {
			if (comp) {
				comp->processAreaDragTo(
					distanceX, distanceY, moveX, moveY);
			}
		},
		[comp = ScrollerBase::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragEnd();
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
	CoreCallbacks::getInstance()->addTempoChanged(
		[comp = SeqView::SafePointer(this)] {
			if (comp) {
				comp->updateTempo();
			}
		}
	);
	CoreCallbacks::getInstance()->addSeqMuteChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateMute(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addSeqRecChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateRec(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addInstrChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateInstr(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addTrackChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateMixerTrack(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addSeqDataRefChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateDataRef(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addSourceChanged(
		[comp = SeqView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateData(index);
			}
		}
	);

	/** Init Temp */
	this->gridTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, true);
}

void SeqView::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.05;
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
		vScrollerRect.getX(), vScrollerRect.getHeight());
	this->trackList->setBounds(listRect);

	/** Update View Pos */
	this->hScroller->update();
	this->vScroller->update();

	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->ruler->getLineTemp();

	/** Update Grid Temp */
	{
		int width = hScrollerRect.getWidth(), height = vScrollerRect.getHeight();
		width = std::max(width, 1);
		height = std::max(height, 1);
		this->gridTemp = std::make_unique<juce::Image>(
			juce::Image::ARGB, width, height, true);
		this->updateGridTemp();
	}
}

void SeqView::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.05;
	int headWidth = screenSize.getWidth() * 0.1;

	float lineThickness = screenSize.getHeight() * 0.0025;

	int emptyTextPaddingWidth = screenSize.getWidth() * 0.015;
	int emptyTextPaddingHeight = screenSize.getHeight() * 0.015;
	float emptyTextFontHeight = screenSize.getHeight() * 0.02;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour headBackgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);
	juce::Colour emptyTextColor = laf.findColour(
		juce::TableListBox::ColourIds::textColourId);

	/** Font */
	juce::Font emptyTextFont(emptyTextFontHeight);

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

	/** Grid */
	if (this->gridTemp) {
		g.drawImageAt(*(this->gridTemp.get()), headWidth, rulerHeight);
	}

	/** Empty Text */
	if (this->trackList->size() <= 0) {
		juce::Rectangle<int> emptyTextRect(
			headWidth + emptyTextPaddingWidth,
			rulerHeight + emptyTextPaddingHeight,
			this->getWidth() - headWidth - scrollerWidth - emptyTextPaddingWidth * 2,
			this->getHeight() - rulerHeight - scrollerHeight - emptyTextPaddingHeight * 2);
		
		g.setColour(emptyTextColor);
		g.setFont(emptyTextFont);
		g.drawFittedText(this->emptyNoticeStr, emptyTextRect,
			juce::Justification::centredTop, 1, 1.f);
	}
}

void SeqView::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.05;
	int headWidth = screenSize.getWidth() * 0.1;

	float cursorThickness = screenSize.getWidth() * 0.00075;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour cursorColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);

	/** Cursor */
	int width = this->getWidth() - headWidth - scrollerWidth;
	int height = this->getHeight() - rulerHeight - scrollerHeight;
	float cursorPosX = ((this->playPosSec - this->secStart) / (this->secEnd - this->secStart)) * width;
	juce::Rectangle<float> cursorRect(
		headWidth + cursorPosX - cursorThickness / 2, rulerHeight,
		cursorThickness, height);

	if (cursorPosX >= 0 && cursorPosX <= width) {
		g.setColour(cursorColor);
		g.fillRect(cursorRect);
	}

	/** Time Off */
	if (this->loopEndSec > this->loopStartSec) {
		/** Left */
		if (this->loopStartSec > this->secStart) {
			float xPos = (this->loopStartSec - this->secStart) / (this->secEnd - this->secStart) * width;
			juce::Rectangle<float> offRect(
				headWidth, rulerHeight, xPos, height);

			g.setColour(offColor);
			g.fillRect(offRect);
		}

		/** Right */
		if (this->loopEndSec < this->secEnd) {
			float xPos = (this->loopEndSec - this->secStart) / (this->secEnd - this->secStart) * width;
			juce::Rectangle<float> offRect(
				headWidth + xPos, rulerHeight, width - xPos, height);

			g.setColour(offColor);
			g.fillRect(offRect);
		}
	}
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
			auto track = std::make_unique<SeqTrackComponent>(
				[comp = ScrollerBase::SafePointer(this->hScroller.get())]
				(double delta) {
					if (comp) {
						comp->scroll(delta);
					}
				},
				[comp = ScrollerBase::SafePointer(this)] {
					if (comp) {
						comp->processAreaDragStart();
					}
				},
				[comp = ScrollerBase::SafePointer(this)]
				(int distanceX, int distanceY, bool moveX, bool moveY) {
					if (comp) {
						comp->processAreaDragTo(
							distanceX, distanceY, moveX, moveY);
					}
				},
				[comp = ScrollerBase::SafePointer(this)] {
					if (comp) {
						comp->processAreaDragEnd();
					}
				});
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
		auto list = quickAPI::getBlockList(i);
		for (auto [startTime, endTime, offset] : list) {
			this->blockTemp.add({ i, startTime, endTime });
		}
	}

	/** Update Length */
	this->totalLength = quickAPI::getTotalLength() + SEQ_TAIL_SEC;

	/** Update View Pos */
	this->hScroller->update();
}

void SeqView::updateTempo() {
	/** Update Time Ruler */
	this->ruler->updateTempoLabel();

	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->ruler->getLineTemp();
	this->updateGridTemp();
}

void SeqView::updateMute(int index) {
	this->trackList->updateMute(index);
}

void SeqView::updateRec(int index) {
	this->trackList->updateRec(index);
}

void SeqView::updateInstr(int index) {
	this->trackList->updateInstr(index);
}

void SeqView::updateLevelMeter() {
	/** Get Play Position */
	this->playPosSec = quickAPI::getTimeInSecond();

	/** Get Loop Time */
	std::tie(this->loopStartSec, this->loopEndSec) = quickAPI::getLoopTimeSec();

	/** Get Play State */
	bool isPlaying = quickAPI::isPlaying();

	/** Follow */
	if (isPlaying && Tools::getInstance()->getFollow()) {
		if ((this->playPosSec < this->secStart) || (this->playPosSec > this->secEnd)) {
			this->hScroller->setPos(this->playPosSec * this->itemSize);
		}
	}

	/** Repaint */
	this->repaint();
}

void SeqView::updateMixerTrack(int /*index*/) {
	this->trackList->updateMixerTrack();
}

void SeqView::updateDataRef(int index) {
	this->trackList->updateDataRef(index);
}

void SeqView::updateData(int index) {
	this->trackList->updateData(index);
}

std::tuple<double, double> SeqView::getViewArea(
	double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getViewWidth() / itemSize;
	return { secStart, secStart + secLength };
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
	/** Set Pos */
	this->pos = pos;
	this->itemSize = itemSize;
	std::tie(this->secStart, this->secEnd) = this->getViewArea(pos, itemSize);

	/** Update Comp */
	this->ruler->updateHPos(pos, itemSize);
	this->trackList->updateHPos(pos, itemSize);

	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->ruler->getLineTemp();
	this->updateGridTemp();
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
	return { screenSize.getHeight() * 0.0275, screenSize.getHeight() * 0.2 };
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

void SeqView::updateGridTemp() {
	/** Temp Size */
	juce::Graphics g(*(this->gridTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float longLineThickness = screenSize.getWidth() * 0.00075;
	float shortLineThickness = screenSize.getWidth() * 0.0005;

	float shortLineIntervalMin = screenSize.getWidth() * 0.01;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour lineColor = laf.findColour(
		juce::TableListBox::ColourIds::outlineColourId);
	juce::Colour backgroundColor = laf.findColour(
		juce::TableListBox::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Lines */
	for (auto& [xPos, isLong, barId] : this->lineTemp) {
		/** Check Interval */
		if (!isLong) {
			if (this->minInterval < shortLineIntervalMin) {
				continue;
			}
		}

		/** Line */
		float lineThickness = isLong ? longLineThickness : shortLineThickness;
		juce::Rectangle<float> lineRect(
			xPos - lineThickness / 2, 0,
			lineThickness, this->gridTemp->getHeight());

		g.setColour(lineColor);
		g.fillRect(lineRect);
	}
}

void SeqView::processAreaDragStart() {
	this->viewMoving = true;
	this->moveStartPosX = this->hScroller->getViewPos();
	this->moveStartPosY = this->vScroller->getViewPos();
}

void SeqView::processAreaDragTo(
	int distanceX, int distanceY, bool moveX, bool moveY) {
	if (this->viewMoving) {
		if (moveX) {
			this->hScroller->setPos(this->moveStartPosX - distanceX);
		}
		if (moveY) {
			this->vScroller->setPos(this->moveStartPosY - distanceY);
		}
	}
}

void SeqView::processAreaDragEnd() {
	this->viewMoving = false;
	this->moveStartPosX = this->moveStartPosY = 0;
}
