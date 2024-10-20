#include "MIDISourceEditor.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

#define MIDI_TAIL_SEC 10;

MIDISourceEditor::MIDISourceEditor() {
	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return (double)(this->getViewWidth()); },
		[this] { return this->getTimeLength(); },
		[this] { return this->getTimeWidthLimit(); },
		[this](double pos, double itemSize) { this->updateHPos(pos, itemSize); },
		[this](juce::Graphics& g, int width, int height, bool vertical) {
			this->paintNotePreview(g, width, height, vertical); },
			Scroller::PaintItemPreviewFunc{},
			[this] { return this->getPlayPos(); });
	this->hScroller->setShouldShowPlayPos(true);
	this->addAndMakeVisible(this->hScroller.get());

	this->vScroller = std::make_unique<Scroller>(true,
		[this] { return (double)(this->getViewHeight()); },
		[this] { return (double)(this->getKeyNum()); },
		[this] { return this->getKeyHeightLimit(); },
		[this](double pos, double itemSize) { this->updateVPos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		Scroller::PaintItemPreviewFunc{});
	this->addAndMakeVisible(this->vScroller.get());
}

void MIDISourceEditor::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.065;
	int pianoWidth = screenSize.getWidth() * 0.065;

	int adsorbButtonHeight = screenSize.getHeight() * 0.025;
	int adsorbButtonPaddingWidth = screenSize.getWidth() * 0.005;
	int adsorbButtonPaddingHeight = screenSize.getHeight() * 0.005;

	/** Scroller */
	juce::Rectangle<int> hScrollerRect(
		pianoWidth, this->getHeight() - scrollerHeight,
		this->getWidth() - pianoWidth - scrollerWidth, scrollerHeight);
	this->hScroller->setBounds(hScrollerRect);
	juce::Rectangle<int> vScrollerRect(
		this->getWidth() - scrollerWidth, rulerHeight,
		scrollerWidth, this->getHeight() - rulerHeight - scrollerHeight);
	this->vScroller->setBounds(vScrollerRect);

	/** Adsorb Button */
	/*juce::Rectangle<int> adsorbRect(
		headWidth - adsorbButtonPaddingWidth - adsorbButtonHeight,
		rulerHeight - adsorbButtonPaddingHeight - adsorbButtonHeight,
		adsorbButtonHeight, adsorbButtonHeight);
	this->adsorbButton->setBounds(adsorbRect);*/

	/** Time Ruler */
	/*juce::Rectangle<int> rulerRect(
		headWidth, 0,
		hScrollerRect.getWidth(), rulerHeight);
	this->ruler->setBounds(rulerRect);*/

	/** Track List */
	/*juce::Rectangle<int> listRect(
		0, vScrollerRect.getY(),
		vScrollerRect.getX(), vScrollerRect.getHeight());
	this->trackList->setBounds(listRect);*/

	/** Update View Pos */
	this->hScroller->update();
	this->vScroller->update();
}

void MIDISourceEditor::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.065;
	int pianoWidth = screenSize.getWidth() * 0.065;

	float lineThickness = screenSize.getHeight() * 0.0025;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour headBackgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Background */
	g.fillAll(backgroundColor);

	/** Piano Bar */
	juce::Rectangle<int> headRect(
		0, 0, pianoWidth, this->getHeight());
	g.setColour(headBackgroundColor);
	g.fillRect(headRect);

	/** Top Head Split */
	juce::Rectangle<float> headTopLineRect(
		0, rulerHeight - lineThickness / 2, pianoWidth, lineThickness);
	g.setColour(outlineColor);
	g.fillRect(headTopLineRect);

	/** Bottom Head Split */
	juce::Rectangle<float> bottomTopLineRect(
		0, this->getHeight() - scrollerHeight - lineThickness / 2, pianoWidth, lineThickness);
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

void MIDISourceEditor::update(uint64_t ref) {
	this->ref = ref;
	
	/** Total Length */
	this->totalLength = quickAPI::getMIDISourceLength(ref) + MIDI_TAIL_SEC;

	/** TODO */
}

int MIDISourceEditor::getViewWidth() const {
	return this->hScroller->getWidth();
}

double MIDISourceEditor::getTimeLength() const {
	return this->totalLength;
}

std::tuple<double, double> MIDISourceEditor::getTimeWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getWidth() * 0.01, screenSize.getWidth() * 0.5 };
}

double MIDISourceEditor::getPlayPos() const {
	return quickAPI::getTimeInSecond();
}

void MIDISourceEditor::updateHPos(double pos, double itemSize) {
	/** TODO */
}

void MIDISourceEditor::paintNotePreview(juce::Graphics& g,
	int width, int height, bool vertical) {
	/** TODO */
}

int MIDISourceEditor::getViewHeight() const {
	return this->vScroller->getHeight();
}

int MIDISourceEditor::getKeyNum() const {
	return 128;
}

std::tuple<double, double> MIDISourceEditor::getKeyHeightLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getHeight() * 0.01, screenSize.getHeight() * 0.01 };
}

void MIDISourceEditor::updateVPos(double pos, double itemSize) {
	/** TODO */
}
