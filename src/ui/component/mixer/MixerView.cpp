#include "MixerView.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreCallbacks.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

MixerView::MixerView()
	: FlowComponent(TRANS("Mixer")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::Mixer));

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

	/** Notice */
	this->emptyNoticeStr = TRANS("Right click on the blank space to create a new track.");

	/** Update Callback */
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackAdded,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateAdd(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackRemoved,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateRemove(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackInfoChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateInfo(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackSideChainChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateSideChain(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackInputConnectionChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateInput(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackSendConnectionChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateSend(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackGainChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateGain(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackPanChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updatePan(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackFaderChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateFader(type, index);
			}
		});
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackMuteSoloChanged,
		[comp = MixerView::SafePointer(this)](int type, int index) {
			if (comp) {
				comp->updateMute(type, index);
			}
		});
	CoreCallbackAPI<int, int, int>::add(CoreCallbacks::CallbackType::TrackEffectChanged,
		[comp = MixerView::SafePointer(this)](int type, int track, int index) {
			if (comp) {
				comp->updateEffect(type, track, index);
			}
		});
	CoreCallbackAPI<int, int, int, int>::add(CoreCallbacks::CallbackType::TrackEffectIndexChanged,
		[comp = MixerView::SafePointer(this)](int type, int track, int oldIndex, int newIndex) {
			if (comp) {
				comp->updateEffectIndex(type, track, oldIndex, newIndex);
			}
		});
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
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.025;

	int emptyTextPaddingWidth = screenSize.getWidth() * 0.015;
	int emptyTextPaddingHeight = screenSize.getHeight() * 0.015;
	float emptyTextFontHeight = screenSize.getHeight() * 0.02;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour emptyTextColor = laf.findColour(
		juce::TableListBox::ColourIds::textColourId);

	/** Font */
	juce::Font emptyTextFont(juce::FontOptions{ emptyTextFontHeight });

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Empty Text */
	if (this->trackList.size() <= 0) {
		juce::Rectangle<int> emptyTextRect(
			emptyTextPaddingWidth,
			emptyTextPaddingHeight,
			this->getWidth() - emptyTextPaddingWidth * 2,
			this->getHeight() - scrollerHeight - emptyTextPaddingHeight * 2);

		g.setColour(emptyTextColor);
		g.setFont(emptyTextFont);
		g.drawFittedText(this->emptyNoticeStr, emptyTextRect,
			juce::Justification::centredTop, 1, 1.f);
	}
}

void MixerView::updateAdd(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Add Track */
	auto track = std::make_unique<MixerTrackComponent>();
	this->addAndMakeVisible(track.get());
	this->trackList.insert(viewIndex, std::move(track));

	/** Update Tracks */
	for (int i = viewIndex; i < this->trackList.size(); i++) {
		auto trackIndex = this->getTypeByViewIndex(i);
		this->trackList[i]->updateIndex(trackIndex.first, trackIndex.second);
	}

	/** Update Color Temp */
	this->colorTemp.insert(viewIndex, quickAPI::getTrackColor({ (quickAPI::TrackType)type, index }));

	/** Update View Pos */
	this->hScroller->update();

	/** Repaint */
	this->repaint();
}

void MixerView::updateRemove(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Remove Track */
	this->trackList.remove(viewIndex);

	/** Update Tracks */
	for (int i = viewIndex; i < this->trackList.size(); i++) {
		auto trackIndex = this->getTypeByViewIndex(i);
		this->trackList[i]->updateIndex(trackIndex.first, trackIndex.second);
	}

	/** Update Color Temp */
	this->colorTemp.remove(viewIndex);

	/** Update View Pos */
	this->hScroller->update();

	/** Repaint */
	this->repaint();
}

void MixerView::updateInfo(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateInfo();
}

void MixerView::updateSideChain(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateSideChain();
}

void MixerView::updateInput(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateInput();
}

void MixerView::updateSend(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateSend();
}

void MixerView::updateGain(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateGain();
}

void MixerView::updatePan(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updatePan();
}

void MixerView::updateFader(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateFader();
}

void MixerView::updateMute(int type, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, index);

	/** Update Track */
	this->trackList[viewIndex]->updateMute();
}

void MixerView::updateEffect(int type, int track, int index) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, track);

	/** Update Track */
	this->trackList[viewIndex]->updateEffect(index);
}

void MixerView::updateEffectIndex(int type, int track, int oldIndex, int newIndex) {
	/** Get View Index */
	int viewIndex = this->getViewIndexByType(type, track);

	/** Update Track */
	this->trackList[viewIndex]->updateEffectIndex(oldIndex, newIndex);
}

void MixerView::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()) {
		this->add();
	}
}

void MixerView::add() {
	CoreActions::insertTrackGUI();
}

int MixerView::getViewIndexByType(int type, int index) const {
	int masterNum = quickAPI::getTrackNum(quickAPI::TrackType::MasterTrack);
	int auxNum = quickAPI::getTrackNum(quickAPI::TrackType::AuxTrack);

	if (type == (int)quickAPI::TrackType::MasterTrack) {
		return index;
	}
	else if (type == (int)quickAPI::TrackType::AuxTrack) {
		return masterNum + index;
	}

	return masterNum + auxNum + index;
}

std::pair<int, int> MixerView::getTypeByViewIndex(int index) const {
	int masterNum = quickAPI::getTrackNum(quickAPI::TrackType::MasterTrack);
	int auxNum = quickAPI::getTrackNum(quickAPI::TrackType::AuxTrack);

	if (index >= masterNum + auxNum) {
		return { (int)quickAPI::TrackType::Track, index - (masterNum + auxNum) };
	}
	else if (index >= masterNum) {
		return { (int)quickAPI::TrackType::AuxTrack, index - masterNum };
	}

	return { (int)quickAPI::TrackType::MasterTrack, index };
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
