#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "Scroller.h"
#include "SeqTimeRuler.h"

class SeqView final
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	SeqView();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index);
	void updateBlock(int track, int index);
	void updateTempo();

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	class TrackList : public juce::Component {
	public:
		TrackList() = default;

		int size() const;
		void remove(int index);
		void add(std::unique_ptr<juce::Component> newComp);

		void update(int index);
		void updateBlock(int track, int index);

		void updateHPos(double pos, double itemSize);
		void updateVPos(double pos, double itemSize);

	private:
		juce::OwnedArray<juce::Component> list;

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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqView)
};
