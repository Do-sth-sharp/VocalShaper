#include "SeqTimeRuler.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/Tools.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTimeRuler::SeqTimeRuler(
	const ScrollFunc& scrollFunc,
	const ScaleFunc& scaleFunc,
	const WheelFunc& wheelFunc,
	const WheelAltFunc& wheelAltFunc)
	: scrollFunc(scrollFunc), scaleFunc(scaleFunc),
	wheelFunc(wheelFunc), wheelAltFunc(wheelAltFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forTimeRuler());

	/** Init Temp */
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
}

void SeqTimeRuler::updateTempoLabel() {
	/** Update Tempo Temp */
	this->tempoTemp = quickAPI::getTempoDataList();

	/** Update Ruler Temp */
	this->updateRulerTemp();
}

void SeqTimeRuler::updateBlock(int /*track*/, int /*index*/) {
	/** Get Total Length */
	this->totalLengthSec = quickAPI::getTotalLength();
}

void SeqTimeRuler::updateHPos(double pos, double itemSize) {
	/** Set Pos */
	this->pos = pos;
	this->itemSize = itemSize;
	std::tie(this->secStart, this->secEnd) = this->getViewArea(pos, itemSize);

	/** Update Line Temp */
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);
	this->updateRulerTemp();
}

void SeqTimeRuler::updateRulerTemp() {
	/** Temp Size */
	juce::Graphics g(*(this->rulerTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float outlineThickness = screenSize.getHeight() * 0.0015;
	float lineThickness = screenSize.getWidth() * 0.00075;
	float longLineHeight = screenSize.getHeight() * 0.05;
	float shortLineHeight = screenSize.getHeight() * 0.015;

	float shortLineIntervalMin = screenSize.getWidth() * 0.01;

	float numFontHeight = screenSize.getHeight() * 0.015;
	float numAreaWidth = screenSize.getWidth() * 0.05;
	float numPaddingWidth = screenSize.getWidth() * 0.0015;
	float numPaddingHeight = screenSize.getHeight() * 0.001;

	float labelFontHeight = screenSize.getHeight() * 0.015;
	float labelHeight = screenSize.getHeight() * 0.0175;
	float labelWidth = screenSize.getWidth() * 0.02;
	float labelOutlineThickness = screenSize.getHeight() * 0.001;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);
	juce::Colour lineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);
	juce::Colour numColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour labelBackgroundColor = laf.findColour(
		juce::TextEditor::ColourIds::backgroundColourId);
	juce::Colour labelTextColor = laf.findColour(
		juce::TextEditor::ColourIds::textColourId);
	juce::Colour labelOutlineColor = laf.findColour(
		juce::TextEditor::ColourIds::outlineColourId);

	/** Font */
	juce::Font numFont(numFontHeight);
	juce::Font labelFont(labelFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Outline */
	auto outlineRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(outlineRect, outlineThickness);

	/** Lines */
	for (auto& [xPos, isLong, barId] : this->lineTemp) {
		/** Check Interval */
		if (!isLong) {
			if (this->minInterval < shortLineIntervalMin) {
				continue;
			}
		}

		/** Line */
		float lineLength = isLong ? longLineHeight : shortLineHeight;
		juce::Rectangle<float> lineRect(
			xPos - lineThickness / 2, this->getHeight() - lineLength,
			lineThickness, lineLength);

		g.setColour(lineColor);
		g.fillRect(lineRect);

		/** Num */
		if (isLong) {
			juce::Rectangle<float> numRect(
				xPos + numPaddingWidth,
				this->getHeight() - numPaddingHeight - numFontHeight,
				numAreaWidth, numFontHeight);

			g.setColour(numColor);
			g.setFont(numFont);
			g.drawFittedText(juce::String{ barId }, numRect.toNearestInt(),
				juce::Justification::centredLeft, 1, 0.5f);
		}
	}

	/** Labels */
	for (auto& [time, tempo, numerator, denominator, isTempo] : this->tempoTemp) {
		float labelXPos = (time - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
		float labelRPos = labelXPos + labelWidth;

		if (labelXPos < this->getWidth() && labelRPos > 0) {
			/** Label Rect */
			juce::Rectangle<float> labelRect(
				labelXPos, isTempo ? 0.f : labelHeight,
				labelWidth, labelHeight);

			/** Background */
			g.setColour(labelBackgroundColor);
			g.fillRect(labelRect);

			/** Outline */
			g.setColour(labelOutlineColor);
			g.drawRect(labelRect, labelOutlineThickness);

			/** Text */
			juce::String text = isTempo ? juce::String{ tempo, 2 } 
			: (juce::String{ numerator } + "/" + juce::String{ denominator });
			g.setColour(labelTextColor);
			g.setFont(labelFont);
			g.drawFittedText(text, labelRect.toNearestInt(),
				juce::Justification::centred, 1, 0.5f);
		}
	}
}

void SeqTimeRuler::updateLevelMeter() {
	/** Get Play Position */
	this->playPosSec = quickAPI::getTimeInSecond();

	/** Get Loop Time */
	std::tie(this->loopStartSec, this->loopEndSec) = quickAPI::getLoopTimeSec();

	/** Repaint */
	this->repaint();
}

void SeqTimeRuler::resized() {
	/** Update Line Temp */
	std::tie(this->secStart, this->secEnd) = this->getViewArea(this->pos, this->itemSize);
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);

	/** Update Ruler Temp */
	int width = this->getWidth(), height = this->getHeight();
	width = std::max(width, 1);
	height = std::max(height, 1);
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateRulerTemp();
}

void SeqTimeRuler::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float cursorThickness = screenSize.getWidth() * 0.00075;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour cursorColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);

	/** Ruler */
	if (!this->rulerTemp) { return; }
	g.drawImageAt(*(this->rulerTemp.get()), 0, 0);

	/** TODO Moving Tempo Label */

	/** Cursor */
	float cursorPosX = ((this->playPosSec - this->secStart) / (this->secEnd - this->secStart)) * this->getWidth();
	juce::Rectangle<float> cursorRect(
		cursorPosX - cursorThickness / 2, 0,
		cursorThickness, this->getHeight());

	g.setColour(cursorColor);
	g.fillRect(cursorRect);

	/** Time Off */
	if (this->loopEndSec > this->loopStartSec) {
		/** Left */
		if (this->loopStartSec > this->secStart) {
			float xPos = (this->loopStartSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			juce::Rectangle<float> offRect(
				0, 0, xPos, this->getHeight());

			g.setColour(offColor);
			g.fillRect(offRect);
		}

		/** Right */
		if (this->loopEndSec < this->secEnd) {
			float xPos = (this->loopEndSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			juce::Rectangle<float> offRect(
				xPos, 0, this->getWidth() - xPos, this->getHeight());

			g.setColour(offColor);
			g.fillRect(offRect);
		}
	}
}

void SeqTimeRuler::mouseDown(const juce::MouseEvent& event) {
	float xPos = event.position.getX();

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float labelAreaHeight = screenSize.getHeight() * 0.035;

	if (event.position.getY() < labelAreaHeight) {
		/** Get Tool Type */
		auto tool = Tools::getInstance()->getType();

		if (event.mods.isLeftButtonDown()) {
			/** Check Tool Type */
			switch (tool) {
			case Tools::Type::Hand:
				/** TODO Move View Area */
				break;
			case Tools::Type::Pencil:
			case Tools::Type::Magic:
				/** Get Label Index */
				this->dragLabelIndex = this->selectTempoLabel(event.position);
				if (this->dragLabelIndex > -1) {
					/** Move Label */
					double labelTime = std::get<0>(this->tempoTemp.getReference(this->dragLabelIndex));
					float labelPos = (labelTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
					this->labelDragOffset = event.position.getX() - labelPos;
					this->labelDragPos = labelPos;
				}
				else {
					/** Add Label */
					double labelTime = this->secStart + (event.position.getX() / (double)this->getWidth()) * (this->secEnd - this->secStart);
					labelTime = this->limitTimeSec(labelTime);
					this->addTempoLabel(labelTime);
				}
				break;
			case Tools::Type::Eraser:
				/** Remove Label */
				int labelIndex = this->selectTempoLabel(event.position);
				if (labelIndex > -1) {
					this->removeTempoLabel(labelIndex);
				}
				break;
			}
		}
		else if (event.mods.isRightButtonDown()) {
			/** Check Tool Type */
			switch (tool) {
			case Tools::Type::Pencil:
				/** Remove Label */
				int labelIndex = this->selectTempoLabel(event.position);
				if (labelIndex > -1) {
					this->removeTempoLabel(labelIndex);
				}
				break;
			}
		}
	}
	else {
		/** Play Position Changed */
		if (event.mods.isLeftButtonDown()) {
			double per = xPos / (double)this->getWidth();
			double timeSec = this->secStart + (this->secEnd - this->secStart) * per;

			quickAPI::setPlayPosition(this->limitTimeSec(timeSec));
		}

		/** Loop Changed */
		else if (event.mods.isRightButtonDown()) {
			double per = xPos / (double)this->getWidth();
			this->mouseDownSecTemp = this->limitTimeSec(this->secStart + (this->secEnd - this->secStart) * per);
		}
	}
}

void SeqTimeRuler::mouseDrag(const juce::MouseEvent& event) {
	/** Auto Scroll */
	float xPos = event.position.getX();
	double delta = 0;
	if (xPos > this->getWidth()) {
		delta = xPos - this->getWidth();
	}
	else if (xPos < 0) {
		delta = xPos;
	}
	
	if (delta != 0) {
		this->scrollFunc(delta / 4);
	}

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float labelAreaHeight = screenSize.getHeight() * 0.035;

	if (event.mouseDownPosition.getY() < labelAreaHeight) {
		/** Move Label */
		if (event.mods.isLeftButtonDown()) {
			if (this->dragLabelIndex > -1) {
				/** Get Label Time */
				double labelTime = this->secStart + ((event.position.getX() - this->labelDragOffset) / (double)this->getWidth()) * (this->secEnd - this->secStart);
				labelTime = this->limitTimeSec(labelTime);
				this->labelDragPos = (labelTime - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
			}
		}
	}
	else {
		/** Play Position Changed */
		if (event.mods.isLeftButtonDown()) {
			double per = xPos / (double)this->getWidth();
			double timeSec = this->secStart + (this->secEnd - this->secStart) * per;

			quickAPI::setPlayPosition(this->limitTimeSec(timeSec));
		}

		/** Loop Changed */
		else if (event.mods.isRightButtonDown()) {
			double per = xPos / (double)this->getWidth();
			double timeSec = this->limitTimeSec(this->secStart + (this->secEnd - this->secStart) * per);

			double loopStart = std::min(this->mouseDownSecTemp, timeSec);
			double loopEnd = std::max(this->mouseDownSecTemp, timeSec);
			quickAPI::setPlayLoop(loopStart, loopEnd);
		}
	}
}

void SeqTimeRuler::mouseUp(const juce::MouseEvent& event) {
	/** Play Position Changed */
	/*if (event.mods.isLeftButtonDown()) {
		double per = event.position.getX() / (double)this->getWidth();
		double timeSec = this->secStart + (this->secEnd - this->secStart) * per;

		quickAPI::setPlayPosition(timeSec);
	}*/

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float labelAreaHeight = screenSize.getHeight() * 0.035;

	if (event.position.getY() < labelAreaHeight) {
		if (event.mods.isLeftButtonDown()) {
			/** Move Label */
			if (this->dragLabelIndex > -1) {
				if (event.mouseWasDraggedSinceMouseDown()) {
					/** Get Label Time */
					double labelTime = this->secStart + ((event.position.getX() - this->labelDragOffset) / (double)this->getWidth()) * (this->secEnd - this->secStart);
					labelTime = this->limitTimeSec(labelTime);

					/** Move Label */
					this->setTempoLabelTime(this->dragLabelIndex, labelTime);
				}
				
				/** Reset State */
				this->dragLabelIndex = -1;
				this->labelDragOffset = 0;
				this->labelDragPos = 0;
			}
		}
	}
	else {
		/** Loop Changed */
		if (event.mods.isRightButtonDown()) {
			if (!(event.mouseWasDraggedSinceMouseDown())) {
				quickAPI::setPlayLoop(0, 0);
			}
		}
	}
}

void SeqTimeRuler::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float labelAreaHeight = screenSize.getHeight() * 0.035;

	/** Cursor */
	if (event.position.getY() < labelAreaHeight) {
		auto cursor = juce::MouseCursor::NormalCursor;

		/** Labels */
		if (this->selectTempoLabel(event.position) > -1) {
			cursor = juce::MouseCursor::PointingHandCursor;
		}

		this->setMouseCursor(cursor);
	}
	else {
		this->setMouseCursor(juce::MouseCursor::IBeamCursor);
	}
}

void SeqTimeRuler::mouseWheelMove(const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel) {
	if (event.mods.isAltDown()) {
		double thumbPer = event.position.getX() / (double)this->getWidth();
		double centerNum = this->secStart + (this->secEnd - this->secStart) * thumbPer;

		this->wheelAltFunc(centerNum, thumbPer, wheel.deltaY, wheel.isReversed);
	}
	else {
		this->wheelFunc(wheel.deltaY, wheel.isReversed);
	}
}

std::tuple<double, double> SeqTimeRuler::getViewArea(
	double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getWidth() / itemSize;
	return { secStart, secStart + secLength };
}

const std::tuple<juce::Array<SeqTimeRuler::LineItem>, double>
SeqTimeRuler::createRulerLine(double pos, double itemSize) const {
	/** Get View Area */
	auto [secStart, secEnd] = this->getViewArea(pos, itemSize);
	double width = this->getWidth();

	/** Get Each Line */
	juce::Array<LineItem> result;
	double minInterval = DBL_MAX;

	double currentTime = secStart;
	int currentTempIndex = -1;
	
	double tempTimeSec = 0, tempTimeQuarter = 0, tempTimeBar = 0;
	double tempSecPerQuarter = 0.5;
	int tempNumerator = 4, tempDenominator = 4;

	while (true/*currentTime < secEnd*/) {
		/** Get Current Tempo */
		int tempIndex = quickAPI::getTempoTempIndexBySec(currentTime);
		if (tempIndex != currentTempIndex) {
			std::tie(tempTimeSec, tempTimeQuarter, tempTimeBar,
				tempSecPerQuarter, tempNumerator, tempDenominator)
				= quickAPI::getTempoData(tempIndex);
			currentTempIndex = tempIndex;
		}

		/** Get Current Bar */
		double secSinceTemp = currentTime - tempTimeSec;
		double quarterSinceTemp = secSinceTemp / tempSecPerQuarter;
		double quarterPerBar = ((4.0 / tempDenominator) * tempNumerator);
		double barSinceTemp = quarterSinceTemp / quarterPerBar;
		double currentTimeBar = tempTimeBar + barSinceTemp;

		double currentBar = std::floor(currentTimeBar);
		double currentBarTimeQuarterSinceTemp = (currentBar - tempTimeBar) * quarterPerBar;
		double currentBarTimeSec = tempTimeSec + currentBarTimeQuarterSinceTemp * tempSecPerQuarter;
		if (juce::approximatelyEqual(currentBarTimeSec, currentTime)) {
			currentBarTimeSec = currentTime;
		}

		/** Check End */
		if (currentBarTimeSec >= secEnd) { break; }

		/** Lines */
		double secPerLine = tempSecPerQuarter * (4.0 / tempDenominator);
		double lineInterval = secPerLine / (secEnd - secStart) * width;
		minInterval = std::min(minInterval, lineInterval);
		for (int i = 0; i < tempNumerator; i++) {
			double lineTime = currentBarTimeSec + i * secPerLine;
			double pos = (lineTime - secStart) / (secEnd - secStart) * width;
			result.add({ pos, i == 0, (int)currentBar });
		}

		/** Increase Time */
		currentTime += (secPerLine * tempNumerator);
	}

	/** Result */
	return { result, minInterval };
}

double SeqTimeRuler::limitTimeSec(double timeSec) {
	double level = Tools::getInstance()->getAdsorb();
	return quickAPI::limitTimeSec(timeSec, level);
}

int SeqTimeRuler::selectTempoLabel(const juce::Point<float> pos) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float labelHeight = screenSize.getHeight() * 0.0175;
	float labelWidth = screenSize.getWidth() * 0.02;

	for (int i = 0; i < this->tempoTemp.size(); i++) {
		auto& [time, tempo, numerator, denominator, isTempo] = this->tempoTemp.getReference(i);

		float labelXPos = (time - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
		float labelRPos = labelXPos + labelWidth;

		if (labelXPos < this->getWidth() && labelRPos > 0) {
			/** Label Rect */
			juce::Rectangle<float> labelRect(
				labelXPos, isTempo ? 0.f : labelHeight,
				labelWidth, labelHeight);

			/** Cursor In Label */
			if (labelRect.contains(pos)) {
				return i;
			}
		}
	}

	/** No In Label */
	return -1;
}

void SeqTimeRuler::removeTempoLabel(int index) {
	/** TODO */
}

void SeqTimeRuler::addTempoLabel(double timeSec) {
	/** TODO */
}

void SeqTimeRuler::setTempoLabelTime(int index, double timeSec) {
	/** TODO */
}
