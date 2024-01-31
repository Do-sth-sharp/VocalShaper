#include "MixerView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

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

	/** Update Callback */
	CoreCallbacks::getInstance()->addTrackChanged(
		[comp = MixerView::SafePointer(this)](int) {
			if (comp) {
				comp->update();
			}
		}
	);
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

void MixerView::update() {
	/** Create Or Remove Track */
	int currentSize = this->trackList.size();
	int newSize = quickAPI::getMixerTrackNum();
	if (currentSize > newSize) {
		for (int i = currentSize - 1; i >= newSize; i--) {
			this->trackList.remove(i, true);
		}
	}
	else {
		for (int i = currentSize; i < newSize; i++) {
			auto track = std::make_unique<MixerTrackComponent>();
			this->addAndMakeVisible(track.get());
			this->trackList.add(std::move(track));
		}
	}

	/** Update Tracks */
	for (int i = 0; i < this->trackList.size(); i++) {
		this->trackList[i]->update(i);
	}

	/** Update Color Temp */
	this->colorTemp.clear();
	for (int i = 0; i < this->trackList.size(); i++) {
		this->colorTemp.add(quickAPI::getMixerTrackColor(i));
	}

	/** Update View Pos */
	this->hScroller->update();
}

int MixerView::getViewWidth() const {
	return this->getWidth();
}

int MixerView::getTrackNum() const {
	return this->trackList.size();
}

std::tuple<int, int> MixerView::getTrackWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { (int)(screenSize.getWidth() * 0.04),
		(int)(screenSize.getWidth() * 0.05) };
}

void MixerView::updatePos(int pos, int itemSize) {
	int height = this->getHeight() - this->hScroller->getHeight();
	for (int i = 0; i < this->trackList.size(); i++) {
		juce::Rectangle<int> trackRect(
			i * itemSize - pos, 0,
			itemSize, height);
		this->trackList[i]->setBounds(trackRect);
	}
}

void MixerView::paintTrackPreview(juce::Graphics& g, int itemIndex,
	int width, int height, bool /*vertical*/) {
	/** Limit Size */
	if (itemIndex < 0 || itemIndex >= this->colorTemp.size()) { return; }

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.005;
	int colorHeight = screenSize.getHeight() * 0.005;

	/** Color */
	juce::Colour color = this->colorTemp[itemIndex];

	/** Draw Color */
	juce::Rectangle<int> colorRect(
		0, paddingHeight, width, colorHeight);
	g.setColour(color);
	g.fillRect(colorRect);
}
