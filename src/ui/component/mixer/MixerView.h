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

	void update(int index);
	void updateGain(int index);
	void updatePan(int index);
	void updateFader(int index);
	void updateMute(int index);
	void updateEffect(int track, int index);
	void updateSeqTrack(int index);

	void mouseUp(const juce::MouseEvent& event) override;

private:
	std::unique_ptr<Scroller> hScroller = nullptr;
	juce::OwnedArray<MixerTrackComponent> trackList;
	juce::Array<juce::Colour> colorTemp;

	void add();

	int getViewWidth() const;
	int getTrackNum() const;
	std::tuple<double, double> getTrackWidthLimit() const;

	void updatePos(double pos, double itemSize);

	void paintTrackPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerView)
};
