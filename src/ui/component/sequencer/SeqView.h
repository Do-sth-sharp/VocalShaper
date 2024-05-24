#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../base/Scroller.h"
#include "SeqTimeRuler.h"
#include "SeqTrackComponent.h"
#include "../../misc/LevelMeterHub.h"

class SeqView final
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer,
	public LevelMeterHub::Target {
public:
	SeqView();

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void update(int index);
	void updateBlock(int track, int index);
	void updateTempo();
	void updateMute(int index);
	void updateRec(int index);
	void updateInstr(int index);
	void updateLevelMeter() override;
	void updateMixerTrack(int index);
	void updateDataRef(int index);
	void updateData(int index);

	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	class TrackList : public juce::Component {
	public:
		TrackList() = default;

		int size() const;
		void remove(int index);
		void add(std::unique_ptr<SeqTrackComponent> newComp);

		void update(int index);
		void updateBlock(int track, int index);
		void updateMute(int index);
		void updateRec(int index);
		void updateInstr(int index);
		void updateMixerTrack();
		void updateDataRef(int index);
		void updateData(int index);

		void updateHPos(double pos, double itemSize);
		void updateVPos(double pos, double itemSize);

		void mouseUp(const juce::MouseEvent& event) override;

	private:
		juce::OwnedArray<SeqTrackComponent> list;

		void add();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
	};

	std::unique_ptr<SeqTimeRuler> ruler = nullptr;
	std::unique_ptr<juce::DrawableButton> adsorbButton = nullptr;
	std::unique_ptr<juce::Drawable> adsorbIcon = nullptr;

	std::unique_ptr<TrackList> trackList = nullptr;
	juce::Array<juce::Colour> colorTemp;

	/** Track Index, Start, End */
	using BlockItem = std::tuple<int, double, double>;
	juce::Array<BlockItem> blockTemp;
	double totalLength = 0;

	double pos = 0, itemSize = 0;
	double secStart = 0, secEnd = 0;
	double playPosSec = 0;
	double loopStartSec = 0, loopEndSec = 0;

	using LineItem = SeqTimeRuler::LineItem;
	juce::Array<LineItem> lineTemp;
	double minInterval = 0;
	std::unique_ptr<juce::Image> gridTemp = nullptr;

	juce::String emptyNoticeStr;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<double, double> getTimeWidthLimit() const;

	void updateHPos(double pos, double itemSize);
	void paintBlockPreview(juce::Graphics& g,
		int width, int height, bool vertical);

	int getViewHeight() const;
	int getTrackNum() const;
	std::tuple<double, double> getTrackHeightLimit() const;

	void updateVPos(double pos, double itemSize);
	void paintTrackPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical);

	void adsorbButtonClicked();
	juce::PopupMenu createAdsorbMenu();

	void updateGridTemp();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqView)
};
