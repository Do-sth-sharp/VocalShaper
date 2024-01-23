#include "InstrView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../misc/CoreActions.h"
#include "../misc/DragSourceType.h"
#include "../Utils.h"

InstrView::InstrView()
	: FlowComponent(TRANS("Instrument")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forInstrView());

	/** Instrument List */
	this->instrListModel = std::make_unique<InstrListModel>();
	this->instrList = std::make_unique<juce::ListBox>(
		TRANS("Instrument List"), this->instrListModel.get());
	this->addAndMakeVisible(this->instrList.get());

	/** Update Callback */
	CoreCallbacks::getInstance()->addInstrChanged(
		[comp = InstrView::SafePointer(this)](int) {
			if (comp) {
				comp->update();
			}
		}
	);
}

void InstrView::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int listItemHeight = screenSize.getHeight() * 0.025;

	/** List Box */
	this->instrList->setBounds(this->getLocalBounds());
	this->instrList->setRowHeight(listItemHeight);
}

void InstrView::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}

void InstrView::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int dropLinePaddingWidth = screenSize.getWidth() * 0.005;
	float dropLineThickness = screenSize.getHeight() * 0.0025;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour dropLineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);

	/** Drop Line */
	auto& [dropIndex, dropYPos] = this->dropItemState;
	if (dropIndex > -1) {
		int dropYPosLocal = dropYPos + this->instrList->getY();
		juce::Rectangle<float> lineRect(
			this->instrList->getX() + dropLinePaddingWidth,
			dropYPosLocal - dropLineThickness / 2,
			this->instrList->getWidth() - dropLinePaddingWidth * 2,
			dropLineThickness);
		g.setColour(dropLineColor);
		g.fillRect(lineRect);
	}
}

void InstrView::update() {
	this->instrList->updateContent();
}

bool InstrView::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		if (!des["instrument"]) { return false; }
		return true;
	}

	return false;
}

void InstrView::itemDragEnter(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->preDrop(dragSourceDetails.localPosition);
}

void InstrView::itemDragMove(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->preDrop(dragSourceDetails.localPosition);
}

void InstrView::itemDragExit(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();
}

void InstrView::itemDropped(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();
	
	auto& des = dragSourceDetails.description;

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		juce::String pid = des["id"].toString();

		int index = this->getInsertIndex(dragSourceDetails.localPosition);
		if (index > -1) {
			CoreActions::insertInstrGUI(index, pid);
		}
		
		return;
	}
}

void InstrView::preDrop(const juce::Point<int>& pos) {
	int index = this->getInsertIndex(pos);
	if (index > -1) {
		int yPos = 0;
		int rowNum = this->instrListModel->getNumRows();
		if (index < rowNum) {
			auto rowPos = this->instrList->getRowPosition(index, true);
			yPos = rowPos.getY();
		}
		else {
			auto rowPos = this->instrList->getRowPosition(
				rowNum - 1, true);
			yPos = rowPos.getBottom();
		}
		
		this->dropItemState = { index, yPos };
		this->repaint();
	}
}

void InstrView::endDrop() {
	this->dropItemState = { -1, 0 };
	this->repaint();
}

int InstrView::getInsertIndex(const juce::Point<int>& pos) {
	return this->instrList->
		getInsertionIndexForPosition(pos.getX(), pos.getY());
}
