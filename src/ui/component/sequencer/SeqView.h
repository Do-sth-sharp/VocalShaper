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

	void updateAdd(int index);
	void updateRemove(int index);
	void updateInfo(int index);
	void updateBlock(int track, int index);
	void updateTempo();
	void updateMuteSolo(int index);
	void updateInputMonitoring(int index);
	void updateRec(int index);
	void updateInstr(int index);
	void updateLevelMeter() override;
	void updateDataRef(int index);
	void updateData(int index);

	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	class TrackList : public juce::Component {
	public:
		using ScrollFunc = std::function<void(double)>;
		using WheelFunc = std::function<void(float, bool)>;
		using WheelAltFunc = std::function<void(double, double, float, bool)>;
		using DragStartFunc = std::function<void(void)>;
		using DragProcessFunc = std::function<void(int, int, bool, bool)>;
		using DragEndFunc = std::function<void(void)>;
		using EditingFunc = std::function<void(int)>;
		TrackList(const ScrollFunc& scrollFunc,
			const WheelFunc& wheelHFunc,
			const WheelAltFunc& wheelAltHFunc,
			const WheelFunc& wheelVFunc,
			const WheelAltFunc& wheelAltVFunc,
			const DragStartFunc& dragStartFunc,
			const DragProcessFunc& dragProcessFunc,
			const DragEndFunc& dragEndFunc,
			const EditingFunc& editingFunc);

		int size() const;
		void remove(int index);
		void insert(int index, std::unique_ptr<SeqTrackComponent> newComp);

		void updateIndex(int index);
		void updateInfo(int index);
		void updateBlock(int track, int index);
		void updateMuteSolo(int index);
		void updateInputMonitoring(int index);
		void updateRec(int index);
		void updateInstr(int index);
		void updateDataRef(int index);
		void updateData(int index);

		void updateHPos(double pos, double itemSize);
		void updateVPos(double pos, double itemSize);

		void mouseDown(const juce::MouseEvent& event) override;
		void mouseUp(const juce::MouseEvent& event) override;
		void mouseDrag(const juce::MouseEvent& event) override;
		void mouseWheelMove(const juce::MouseEvent& event,
			const juce::MouseWheelDetails& wheel) override;
		void mouseExit(const juce::MouseEvent& event) override;

	private:
		juce::OwnedArray<SeqTrackComponent> list;

		const ScrollFunc scrollFunc;
		const WheelFunc wheelHFunc;
		const WheelAltFunc wheelAltHFunc;
		const WheelFunc wheelVFunc;
		const WheelAltFunc wheelAltVFunc;
		const DragStartFunc dragStartFunc;
		const DragProcessFunc dragProcessFunc;
		const DragEndFunc dragEndFunc;
		const EditingFunc editingFunc;

		double pos = 0, itemSize = 0;
		double secStart = 0, secEnd = 0;
		double indexStart = 0, indexEnd = 0;

		bool viewMoving = false;

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

	using LineItemList = SeqTimeRuler::LineItemList;
	LineItemList lineTemp;
	std::unique_ptr<juce::Image> gridTemp = nullptr;

	juce::String emptyNoticeStr;

	bool viewMoving = false;
	double moveStartPosX = 0, moveStartPosY = 0;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<double, double> getTimeWidthLimit() const;
	double getPlayPos() const;

	void updateHPos(double pos, double itemSize);
	void paintBlockPreview(juce::Graphics& g,
		int width, int height, bool vertical, double totalNum);

	int getViewHeight() const;
	int getTrackNum() const;
	std::tuple<double, double> getTrackHeightLimit() const;

	void updateVPos(double pos, double itemSize);
	void paintTrackPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical);

	void adsorbButtonClicked();
	juce::PopupMenu createAdsorbMenu();

	void updateGridTemp();
	void updateBlockTemp();

	void processAreaDragStart();
	void processAreaDragTo(int distanceX, int distanceY, bool moveX = true, bool moveY = true);
	void processAreaDragEnd();

	void editing(int index);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqView)
};
