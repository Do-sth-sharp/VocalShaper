﻿#include "ScrollerBase.h"
#include "../Utils.h"

ScrollerBase::ScrollerBase(bool vertical)
	: vertical(vertical) {
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
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0015;
	int paddingHeight = screenSize.getHeight() * 0.0025;
	float cornerSize = screenSize.getHeight() * 0.005;
	float outlineThickness = screenSize.getHeight() * 0.001;

	/** Colour */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ScrollBar::ColourIds::backgroundColourId);
	juce::Colour thumbColor = laf.findColour(
		juce::ScrollBar::ColourIds::thumbColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::ScrollBar::ColourIds::trackColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Paint Preview */
	this->paintPreview(g, this->vertical);

	/** Paint Item Preview */
	int actualTotalSize = this->getActualTotalSize();
	int trackLength = this->vertical ? this->getHeight() : this->getWidth();
	int itemAreaSize = (this->itemSize / (double)actualTotalSize) * trackLength;
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

	/** Thumb */
	int startPos = (this->viewPos / (double)actualTotalSize) * trackLength;
	int endPos = ((this->viewPos + this->viewSize) / (double)actualTotalSize) * trackLength;
	juce::Rectangle<float> thumbRect(
		this->vertical ? paddingWidth : startPos,
		this->vertical ? startPos : paddingHeight,
		this->vertical ? (this->getWidth() - paddingWidth * 2) : (endPos - startPos),
		this->vertical ? (endPos - startPos) : (this->getHeight() - paddingHeight * 2));
	g.setColour(thumbColor);
	g.fillRoundedRectangle(thumbRect, cornerSize);
}

void ScrollerBase::update() {
	/** Get Size */
	this->viewSize = this->createViewSize();
	std::tie(this->itemMinSize, this->itemMaxSize) = this->createItemSizeLimit();
	this->itemNum = this->createItemNum();
	this->itemSize = this->limitItemSize(this->itemSize);

	/** Update Pos */
	this->setPos(this->viewPos);
}

void ScrollerBase::setPos(int pos) {
	/** Limit Pos */
	pos = this->limitPos(pos);

	/** Set Pos */
	this->viewPos = pos;

	/** Repaint */
	this->repaint();

	/** Update Content */
	this->updatePos(pos, this->itemSize);
}

int ScrollerBase::getViewPos() const {
	return this->viewPos;
}

int ScrollerBase::getViewSize() const {
	return this->viewSize;
}

int ScrollerBase::getItemSize() const {
	return this->itemSize;
}

int ScrollerBase::getItemNum() const {
	return this->itemNum;
}

int ScrollerBase::getItemMinSize() const {
	return this->itemMinSize;
}

int ScrollerBase::getItemMaxSize() const {
	return this->itemMaxSize;
}

int ScrollerBase::getActualTotalSize() const {
	return std::max(this->viewSize, this->itemSize * this->itemNum);
}

void ScrollerBase::mouseMove(const juce::MouseEvent& event) {
	this->updateState(event.getPosition(), false);
}

void ScrollerBase::mouseDrag(const juce::MouseEvent& event) {
	/** TODO */
}

void ScrollerBase::mouseDown(const juce::MouseEvent& event) {
	/** TODO */
}

void ScrollerBase::mouseUp(const juce::MouseEvent& event) {
	/** TODO */
}

void ScrollerBase::mouseExit(const juce::MouseEvent& /*event*/) {
	this->resetState();
}

int ScrollerBase::limitPos(int pos) const {
	pos = std::min(pos, this->getActualTotalSize() - this->viewSize);
	pos = std::max(pos, 0);
	return pos;
}

int ScrollerBase::limitItemSize(int size) const {
	size = std::min(size, this->itemMaxSize);
	size = std::max(size, this->itemMinSize);
	return size;
}

int ScrollerBase::getJudgeSize() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.0075;
}

void ScrollerBase::resetState() {
	this->state = State::Normal;
	this->updateCursor();
	this->repaint();
}

void ScrollerBase::updateState(const juce::Point<int>& pos, bool pressed) {
	/** Get Size */
	int position = this->vertical ? pos.getY() : pos.getX();

	int actualTotalSize = this->getActualTotalSize();
	int trackLength = this->vertical ? this->getHeight() : this->getWidth();
	int startPos = (this->viewPos / (double)actualTotalSize) * trackLength;
	int endPos = ((this->viewPos + this->viewSize) / (double)actualTotalSize) * trackLength;

	int judgeSize = this->getJudgeSize();
	int startJudgeS = startPos - judgeSize / 2;
	int startJudgeE = startPos + std::min(judgeSize / 2, (endPos - startPos) / 2);
	int endJudgeS = endPos - std::min(judgeSize / 2, (endPos - startPos) / 2);
	int endJudgeE = endPos + judgeSize / 2;

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
	this->repaint();
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
