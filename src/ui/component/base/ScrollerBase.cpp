#include "ScrollerBase.h"
#include "../../Utils.h"

#define DELTA 0.000001

ScrollerBase::ScrollerBase(bool vertical)
	: vertical(vertical) {
	/** Init Temp */
	this->backTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
	this->frontTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);

	/** Update Later */
	juce::MessageManager::callAsync(
		[comp = juce::Component::SafePointer(this)] {
			if (comp) {
				comp->update();
			}
		}
	);
};

void ScrollerBase::paint(juce::Graphics& g) {
	/** Back Temp */
	if (this->backTemp) {
		g.drawImageAt(*(this->backTemp.get()), 0, 0);
	}

	/** Play Pos */
	if (this->showPos) {
		/** Size */
		auto screenSize = utils::getScreenSize(this);
		int paddingWidth = screenSize.getWidth() * 0.0015;
		int paddingHeight = screenSize.getHeight() * 0.0025;
		float posWidth = screenSize.getWidth() * 0.001;
		float posHeight = posWidth;

		/** Colour */
		auto& laf = this->getLookAndFeel();
		juce::Colour posColor = laf.findColour(
			juce::Label::ColourIds::outlineColourId);

		/** Pos */
		double pos = this->getPlayPos();
		float posPer = pos / this->itemNum;
		double trackLength = this->vertical ? this->getHeight() : this->getWidth();

		juce::Rectangle<float> posLineRect(
			this->vertical ? paddingWidth : posPer * trackLength - posWidth / 2,
			this->vertical ? posPer * trackLength - posHeight / 2 : paddingHeight,
			this->vertical ? this->getWidth() - paddingWidth * 2 : posWidth,
			this->vertical ? posHeight : this->getHeight() - paddingHeight * 2);
		g.setColour(posColor);
		g.fillRect(posLineRect);
	}

	/** Front Temp */
	if (this->frontTemp) {
		g.drawImageAt(*(this->frontTemp.get()), 0, 0);
	}
}

void ScrollerBase::resized() {
	/** Update Image Temp */
	int width = this->getWidth(), height = this->getHeight();
	width = std::max(width, 1);
	height = std::max(height, 1);
	this->backTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->frontTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	if (this->getWidth() > 0 && this->getHeight() > 0) {
		this->updateImageTemp();
	}
}

void ScrollerBase::update() {
	/** Get Size */
	this->viewSize = this->createViewSize();
	std::tie(this->itemMinSize, this->itemMaxSize) = this->createItemSizeLimit();
	this->itemNum = this->limitItemNum(this->createItemNum());
	this->itemSize = this->limitItemSize(this->itemSize);

	/** Update Pos */
	this->setPos(this->viewPos);
}

void ScrollerBase::setPos(double pos) {
	/** Limit Pos */
	pos = this->limitPos(pos);

	/** Set Pos */
	this->viewPos = pos;

	/** Repaint */
	this->updateImageTemp();

	/** Update Content */
	this->updatePos(pos, this->itemSize);
}

void ScrollerBase::setItemSize(double size) {
	/** Limit Size */
	size = this->limitItemSize(size);

	/** Set Size */
	this->itemSize = size;

	/** Repaint */
	this->updateImageTemp();

	/** Update Content */
	this->updatePos(this->viewPos, size);
}

void ScrollerBase::setShouldShowPlayPos(bool showPos) {
	this->showPos = showPos;
	this->repaint();
}

void ScrollerBase::updateLevelMeter() {
	/** Repaint */
	if (this->showPos) {
		this->repaint();
	}
}

double ScrollerBase::getViewPos() const {
	return this->viewPos;
}

double ScrollerBase::getViewSize() const {
	return this->viewSize;
}

double ScrollerBase::getItemSize() const {
	return this->itemSize;
}

double ScrollerBase::getItemNum() const {
	return this->itemNum;
}

double ScrollerBase::getItemMinSize() const {
	return this->itemMinSize;
}

double ScrollerBase::getItemMaxSize() const {
	return this->itemMaxSize;
}

double ScrollerBase::getActualTotalSize() const {
	return this->itemSize * this->itemNum;
}

void ScrollerBase::mouseMove(const juce::MouseEvent& event) {
	this->updateState(event.getPosition(), false);
}

void ScrollerBase::scroll(double delta) {
	/** Get Total Size */
	double totalSize = this->getActualTotalSize();

	/** Get Start Pos */
	auto [startPos, endPos] = this->getThumb();
	double startPer = startPos / (double)this->getTrackLength();
	double endPer = endPos / (double)this->getTrackLength();

	/** Set Pos */
	double trueStartPos = startPer * totalSize;
	this->setPos(trueStartPos + delta);
}

void ScrollerBase::scale(double centerPer, double thumbPer, double delta) {
	/** Get Total Size */
	double totalSize = this->getActualTotalSize();
	
	/** Get Start Pos */
	auto [startPos, endPos] = this->getThumb();
	double startPer = startPos / (double)this->getTrackLength();
	double endPer = endPos / (double)this->getTrackLength();
	
	/** Get New Per */
	double deltaPer = delta / totalSize;
	double deltaL = deltaPer * thumbPer;
	double deltaR = deltaPer - deltaL;
	double newStartPer = startPer + deltaL;
	double newEndPer = endPer - deltaR;
	newEndPer = std::max(newStartPer, newEndPer);

	/** Get Item Size */
	double itemSize = this->viewSize / ((newEndPer - newStartPer) * this->itemNum);
	itemSize = this->limitItemSize(itemSize);

	/** Get Start Pos */
	double newPer = (this->viewSize / itemSize) / this->itemNum;
	newStartPer = centerPer - newPer * thumbPer;

	/** Set Pos */
	this->setItemSize(itemSize);
	this->setPos(newStartPer * this->getActualTotalSize());
}

void ScrollerBase::mouseWheelOutside(float deltaY, bool reversed) {
	/** Get Wheel Delta */
	double delta = (1.0 + ((this->itemSize - this->itemMinSize) / (this->itemMaxSize - this->itemMinSize)))
		* deltaY * (reversed ? 1 : -1) * 100.0;
	
	/** Set Pos */
	this->scroll(delta);
}

void ScrollerBase::mouseWheelOutsideWithAlt(
	double centerNum, double thumbPer, float deltaY, bool reversed) {
	/** Get Wheel Delta */
	double delta = (1.0 + ((this->itemSize - this->itemMinSize) / (this->itemMaxSize - this->itemMinSize)))
		* deltaY * (reversed ? -1 : 1) * 100.0;
	
	/** Get Scale Center */
	double centerPer = centerNum / this->itemNum;

	/** Scale */
	this->scale(centerPer, thumbPer, delta);
}

void ScrollerBase::mouseDrag(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		auto pos = event.getPosition();
		auto caredPos = this->getCaredPos(pos);

		/** Check State */
		switch (this->state) {
		case State::PressedStart:
			this->setStart(caredPos);
			break;
		case State::PressedEnd:
			this->setEnd(caredPos);
			break;
		case State::PressedThumb: {
			auto [startPos, endPos] = this->getThumb();
			double halfThumbSize = (endPos - startPos) * this->thumbPressedPer;
			double newThumbStartPos = caredPos - halfThumbSize;
			double newPos = (newThumbStartPos / (double)this->getTrackLength()) * this->getActualTotalSize();
			this->setPos(newPos);
			break;
		}
		default:
			break;
		}
	}
}

void ScrollerBase::mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		auto pos = event.getPosition();
		auto caredPos = this->getCaredPos(pos);

		/** Change State */
		this->updateState(pos, true);

		/** Check State */
		switch (this->state) {
		case State::PressedStart:
			this->setStart(caredPos);
			break;
		case State::PressedEnd:
			this->setEnd(caredPos);
			break;
		case State::PressedThumb: {
			auto [startPos, endPos] = this->getThumb();
			if (caredPos >= startPos && caredPos < endPos) {
				/** Press On Thumb */
				this->recordThumbPress(caredPos);
			}
			else {
				/** Press On Track */
				double halfThumbSize = (endPos - startPos) / 2;
				double newThumbStartPos = caredPos - halfThumbSize;
				double newPos = (newThumbStartPos / (double)this->getTrackLength()) * this->getActualTotalSize();
				this->setPos(newPos);
				this->recordThumbPress(caredPos);
			}
			break;
		}
		default:
			break;
		}
	}
}

void ScrollerBase::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		auto pos = event.getPosition();
		auto caredPos = this->getCaredPos(pos);

		/** Check State */
		switch (this->state) {
		case State::PressedStart:
			this->setStart(caredPos);
			break;
		case State::PressedEnd:
			this->setEnd(caredPos);
			break;
		case State::PressedThumb: {
			auto [startPos, endPos] = this->getThumb();
			double halfThumbSize = (endPos - startPos) * this->thumbPressedPer;
			double newThumbStartPos = caredPos - halfThumbSize;
			double newPos = (newThumbStartPos / (double)this->getTrackLength()) * this->getActualTotalSize();
			this->setPos(newPos);
			break;
		}
		default:
			break;
		}

		/** Update State */
		this->updateState(pos, false);
	}
}

void ScrollerBase::mouseExit(const juce::MouseEvent& /*event*/) {
	this->resetState();
}

void ScrollerBase::mouseWheelMove(const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel) {
	/** Check State */
	if (this->state == State::PressedThumb
		|| this->state == State::PressedStart
		|| this->state == State::PressedEnd) {
		return;
	}

	if (event.mods == juce::ModifierKeys::altModifier) {
		/** Get Thumb Per */
		auto [startPos, endPos] = this->getThumb();
		double startPer = startPos / (double)this->getTrackLength();
		double endPer = endPos / (double)this->getTrackLength();

		/** Get Scale Center */
		auto pos = event.getPosition();
		auto caredPos = this->getCaredPos(pos);
		double thumbPer = (caredPos - startPos) / (double)(endPos - startPos);
		double centerPer = caredPos / (double)this->getTrackLength();
		if (thumbPer < 0) {
			thumbPer = 0;
			centerPer = startPer;
		}
		if (thumbPer > 1) {
			thumbPer = 1;
			centerPer = endPer;
		}

		/** Scale */
		this->mouseWheelOutsideWithAlt(
			centerPer * this->itemNum, thumbPer, wheel.deltaY, wheel.isReversed);
	}
	else {
		this->mouseWheelOutside(wheel.deltaY, wheel.isReversed);
	}
}

void ScrollerBase::recordThumbPress(double pos) {
	auto [startPos, endPos] = this->getThumb();
	this->thumbPressedPer = (pos - startPos) / (double)(endPos - startPos);
}

void ScrollerBase::setStart(double start) {
	/** Get Size */
	double startPer = start / (double)this->getTrackLength();
	double endPer = (this->viewPos + this->viewSize) / (double)this->getActualTotalSize();
	if (startPer < 0.0) { startPer = 0.0; }
	if (startPer > (endPer - DELTA)) { startPer = endPer - DELTA; }
	double viewNum = (endPer - startPer) * this->itemNum;
	double itemSize = this->limitItemSize(this->viewSize / viewNum);
	
	/** Set Size */
	this->itemSize = itemSize;

	/** Get Pos */
	double endPos = endPer * this->getActualTotalSize();
	double pos = endPos - this->viewSize;
	if (pos < 0) { pos = 0; }

	/** Set Pos */
	this->viewPos = pos;

	/** Repaint */
	this->updateImageTemp();

	/** Update Content */
	this->updatePos(pos, itemSize);
}

void ScrollerBase::setEnd(double end) {
	/** Get Size */
	double startPer = this->viewPos / (double)this->getActualTotalSize();
	double endPer = end / (double)this->getTrackLength();
	if (endPer > 1.0) { endPer = 1.0; }
	if (endPer < (startPer + DELTA)) { endPer = startPer + DELTA; }
	double viewNum = (endPer - startPer) * this->itemNum;
	double itemSize = this->limitItemSize(this->viewSize / viewNum);

	/** Set Size */
	this->itemSize = itemSize;

	/** Get Pos */
	double pos = startPer * this->getActualTotalSize();
	if (pos < 0) { pos = 0; }

	/** Set Pos */
	this->viewPos = pos;

	/** Repaint */
	this->updateImageTemp();

	/** Update Content */
	this->updatePos(pos, itemSize);
}

double ScrollerBase::limitPos(double pos) const {
	pos = std::min(pos, this->getActualTotalSize() - this->viewSize);
	pos = std::max(pos, 0.0);
	return pos;
}

double ScrollerBase::limitItemSize(double size) const {
	size = std::min(size, this->itemMaxSize);
	size = std::max(size, this->itemMinSize);
	return size;
}

double ScrollerBase::limitItemNum(double num) const {
	return std::max(num, this->viewSize / (double)this->itemMinSize);
}

int ScrollerBase::getJudgeSize() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.0075;
}

int ScrollerBase::getTrackLength() const {
	return this->vertical ? this->getHeight() : this->getWidth();
}

int ScrollerBase::getCaredPos(const juce::Point<int>& pos) const {
	return this->vertical ? pos.getY() : pos.getX();
}

std::tuple<double, double> ScrollerBase::getThumb() const {
	double actualTotalSize = this->getActualTotalSize();
	double trackLength = this->vertical ? this->getHeight() : this->getWidth();
	double startPos = (this->viewPos / (double)actualTotalSize) * trackLength;
	double endPos = ((this->viewPos + this->viewSize) / (double)actualTotalSize) * trackLength;
	return { startPos, endPos };
}

void ScrollerBase::resetState() {
	this->state = State::Normal;
	this->updateCursor();
	this->updateImageTemp();
}

void ScrollerBase::updateState(const juce::Point<int>& pos, bool pressed) {
	/** Get Size */
	double position = this->getCaredPos(pos);

	auto [startPos, endPos] = this->getThumb();

	double judgeSize = this->getJudgeSize();
	double startJudgeS = startPos - judgeSize / 2;
	double startJudgeE = startPos + std::min(judgeSize / 2, (endPos - startPos) / 2);
	double endJudgeS = endPos - std::min(judgeSize / 2, (endPos - startPos) / 2);
	double endJudgeE = endPos + judgeSize / 2;

	/** Change State */
	if (pressed) {
		if (this->state == State::PressedThumb
			|| this->state == State::PressedStart
			|| this->state == State::PressedEnd) {
			/** Don't Change State If Already Pressed */
		}
		else {
			if (position >= startJudgeS && position < startJudgeE) {
				this->state = State::PressedStart;
			}
			else if (position >= endJudgeS && position < endJudgeE) {
				this->state = State::PressedEnd;
			}
			else {
				this->state = State::PressedThumb;
			}
		}
	}
	else {
		if (position >= startJudgeS && position < startJudgeE) {
			this->state = State::HoverStart;
		}
		else if (position >= endJudgeS && position < endJudgeE) {
			this->state = State::HoverEnd;
		}
		else if (position >= startJudgeE && position < endJudgeS) {
			this->state = State::HoverThumb;
		}
		else {
			this->state = State::Normal;
		}
	}

	/** Set Cursor */
	this->updateCursor();

	/** Repaint */
	this->updateImageTemp();
}

void ScrollerBase::updateCursor() {
	switch (this->state) {
	case State::Normal:
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
		break;
	case State::HoverThumb:
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
		break;
	case State::PressedThumb:
		this->setMouseCursor(juce::MouseCursor::DraggingHandCursor);
		break;
	case State::HoverStart:
	case State::HoverEnd:
	case State::PressedStart:
	case State::PressedEnd:
		this->setMouseCursor(this->vertical
			? juce::MouseCursor::UpDownResizeCursor
			: juce::MouseCursor::LeftRightResizeCursor);
		break;
	}
}

void ScrollerBase::updateImageTemp() {
	this->updateBackTemp();
	this->updateFrontTemp();
	this->repaint();
}

void ScrollerBase::updateBackTemp() {
	/** Temp Graphic */
	juce::Graphics g(*(this->backTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float outlineThickness = screenSize.getHeight() * 0.001;

	/** Colour */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ScrollBar::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::ScrollBar::ColourIds::trackColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Paint Preview */
	this->paintPreview(g,
		this->getWidth(), this->getHeight(), this->vertical);

	/** Paint Item Preview */
	double itemAreaSize = this->getTrackLength() / this->itemNum;
	for (int i = 0; i < this->itemNum; i++) {
		/** Get Paint Area */
		juce::Rectangle<int> itemRect(
			this->vertical ? 0 : (itemAreaSize * i),
			this->vertical ? (itemAreaSize * i) : 0,
			this->vertical ? this->getWidth() : itemAreaSize,
			this->vertical ? itemAreaSize : this->getHeight());

		/** Add Transform */
		g.saveState();
		g.addTransform(juce::AffineTransform{
			1.f, 0.f, (float)(itemRect.getX()),
			0.f, 1.f, (float)(itemRect.getY()) });

		/** Paint Item */
		this->paintItemPreview(g, i,
			itemRect.getWidth(), itemRect.getHeight(), this->vertical);

		/** Remove Transform */
		g.restoreState();
	}

	/** Outline */
	auto totalRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(totalRect.toFloat(), outlineThickness);
}

void ScrollerBase::updateFrontTemp() {
	/** Temp Graphic */
	this->frontTemp->clear(this->frontTemp->getBounds());
	juce::Graphics g(*(this->frontTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0015;
	int paddingHeight = screenSize.getHeight() * 0.0025;
	float cornerSize = screenSize.getHeight() * 0.005;

	/** Colour */
	auto& laf = this->getLookAndFeel();
	juce::Colour thumbColor = laf.findColour(
		juce::ScrollBar::ColourIds::thumbColourId);

	/** Thumb */
	auto [startPos, endPos] = this->getThumb();
	if (endPos > startPos) {
		juce::Rectangle<float> thumbRect(
			this->vertical ? paddingWidth : startPos,
			this->vertical ? startPos : paddingHeight,
			this->vertical ? (this->getWidth() - paddingWidth * 2) : (endPos - startPos),
			this->vertical ? (endPos - startPos) : (this->getHeight() - paddingHeight * 2));
		g.setColour(thumbColor);
		g.fillRoundedRectangle(thumbRect, cornerSize);
	}
}
