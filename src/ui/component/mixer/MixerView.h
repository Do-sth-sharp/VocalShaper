#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../base/Scroller.h"
#include "MixerTrackComponent.h"

class MixerView final
	: public flowUI::FlowComponent,
	public juce::DragAndDropContainer {
public:
	MixerView();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void updateAdd(int type, int index);
	void updateRemove(int type, int index);
	void updateInfo(int type, int index);
	void updateSideChain(int type, int index);
	void updateInput(int type, int index);
	void updateSend(int type, int index);
	void updateGain(int type, int index);
	void updatePan(int type, int index);
	void updateFader(int type, int index);
	void updateMute(int type, int index);
	void updateEffect(int type, int track, int index);
	void updateEffectIndex(int type, int track, int oldIndex, int newIndex);
	void updateAll();

	void mouseUp(const juce::MouseEvent& event) override;

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	juce::OwnedArray<MixerTrackComponent> trackList;
	juce::Array<juce::Colour> colorTemp;

	std::unique_ptr<juce::ChangeListener> currentTrackListener = nullptr;

	juce::String emptyNoticeStr;

	void add();

	int getViewIndexByType(int type, int index) const;
	std::pair<int, int> getTypeByViewIndex(int index) const;

	int getViewWidth() const;
	int getTrackNum() const;
	std::tuple<double, double> getTrackWidthLimit() const;

	void updatePos(double pos, double itemSize);

	void editing(int type, int index);

	friend class MixerCurrentTrackListener;
	void setCurrentTrack(int type, int index);

	void paintTrackPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical);

	std::unique_ptr<MixerTrackComponent> createTrackComp();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerView)
};
