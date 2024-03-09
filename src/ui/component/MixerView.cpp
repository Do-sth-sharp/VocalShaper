#include "MixerView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreCallbacks.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

MixerView::MixerView()
	: FlowComponent(TRANS("Mixer")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMixer());

	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return (double)(this->getViewWidth()); },
		[this] { return (double)(this->getTrackNum()); },
		[this] { return this->getTrackWidthLimit(); },
		[this](double pos, double itemSize) { this->updatePos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		[this](juce::Graphics& g, int itemIndex,
			int width, int height, bool vertical) {
				this->paintTrackPreview(g, itemIndex, width, height, vertical); });
	this->addAndMakeVisible(this->hScroller.get());

	/** Update Callback */
	CoreCallbacks::getInstance()->addTrackChanged(
		[comp = MixerView::SafePointer(this)](int index) {
			if (comp) {
				comp->update(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addTrackGainChanged(
		[comp = MixerView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateGain(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addTrackPanChanged(
		[comp = MixerView::SafePointer(this)](int index) {
			if (comp) {
				comp->updatePan(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addTrackFaderChanged(
		[comp = MixerView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateFader(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addTrackMuteChanged(
		[comp = MixerView::SafePointer(this)](int index) {
			if (comp) {
				comp->updateMute(index);
			}
		}
	);
	CoreCallbacks::getInstance()->addEffectChanged(
		[comp = MixerView::SafePointer(this)](int track, int index) {
			if (comp) {
				comp->updateEffect(track, index);
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

void MixerView::update(int index) {
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
	if (index >= 0 && index < this->trackList.size()) {
		this->trackList[index]->update(index);
	}
	else {
		for (int i = 0; i < this->trackList.size(); i++) {
			this->trackList[i]->update(i);
		}
	}

	/** Update Color Temp */
	if (index >= 0 && index < this->colorTemp.size()) {
		if (index < newSize) {
			this->colorTemp.getReference(index) = quickAPI::getMixerTrackColor(index);
		}
		if (this->colorTemp.size() > newSize) {
			this->colorTemp.resize(newSize);
		}
	}
	else {
		this->colorTemp.clear();
		for (int i = 0; i < this->trackList.size(); i++) {
			this->colorTemp.add(quickAPI::getMixerTrackColor(i));
		}
	}

	/** Update View Pos */
	this->hScroller->update();
}

void MixerView::updateGain(int index) {
	if (index >= 0 && index < this->trackList.size()) {
		this->trackList[index]->updateGain();
	}
}

void MixerView::updatePan(int index) {
	if (index >= 0 && index < this->trackList.size()) {
		this->trackList[index]->updatePan();
	}
}

void MixerView::updateFader(int index) {
	if (index >= 0 && index < this->trackList.size()) {
		this->trackList[index]->updateFader();
	}
}

void MixerView::updateMute(int index) {
	if (index >= 0 && index < this->trackList.size()) {
		this->trackList[index]->updateMute();
	}
}

void MixerView::updateEffect(int track, int index) {
	if (track >= 0 && track < this->trackList.size()) {
		this->trackList[track]->updateEffect(index);
	}
	else {
		for (auto i : this->trackList) {
			i->updateEffect(index);
		}
	}
}

void MixerView::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->add();
	}
}

void MixerView::add() {
	CoreActions::insertTrackGUI();
}

int MixerView::getViewWidth() const {
	return this->getWidth();
}

int MixerView::getTrackNum() const {
	return this->trackList.size();
}

std::tuple<double, double> MixerView::getTrackWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getWidth() * 0.04,
		screenSize.getWidth() * 0.05 };
}

void MixerView::updatePos(double pos, double itemSize) {
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
