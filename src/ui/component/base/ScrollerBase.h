#pragma once

#include <JuceHeader.h>
#include "../../misc/LevelMeterHub.h"

class ScrollerBase : public juce::Component,
	public LevelMeterHub::Target {
public:
	ScrollerBase() = delete;
	ScrollerBase(bool vertical);
	virtual ~ScrollerBase() = default;

	void paint(juce::Graphics& g) override;
	void resized() override;

	void update();
	void setPos(double pos);
	void setItemSize(double size);
	void setShouldShowPlayPos(bool showPos);

	void updateLevelMeter() override;

	double getViewPos() const;
	double getViewSize() const;
	double getItemSize() const;
	double getItemNum() const;
	double getItemMinSize() const;
	double getItemMaxSize() const;

	double getActualTotalSize() const;

	void scroll(double delta);
	void scale(double centerPer, double thumbPer, double delta);
	void mouseWheelOutside(float deltaY, bool reversed);
	void mouseWheelOutsideWithAlt(double centerNum, double thumbPer, float deltaY, bool reversed);

	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel) override;

protected:
	virtual double createViewSize() = 0;
	virtual double createItemNum() = 0;
	virtual std::tuple<double, double> createItemSizeLimit() = 0;

	virtual void updatePos(double pos, double itemSize) = 0;

	virtual void paintPreview(juce::Graphics& g,
		int width, int height, bool vertical) {};
	virtual void paintItemPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical) {};

	virtual double getPlayPos() { return -1; };

private:
	const bool vertical;

	double viewPos = 0, viewSize = 0;
	double itemSize = 0, itemNum = 0;
	double itemMinSize = 0, itemMaxSize = 0;

	bool showPos = false;

	std::unique_ptr<juce::Image> backTemp = nullptr;
	std::unique_ptr<juce::Image> frontTemp = nullptr;

	enum class State {
		Normal, HoverThumb, HoverStart, HoverEnd,
		PressedThumb, PressedStart, PressedEnd
	};
	State state = State::Normal;

	double thumbPressedPer = -1;
	void recordThumbPress(double pos);

	void setStart(double start);
	void setEnd(double end);

	double limitPos(double pos) const;
	double limitItemSize(double size) const;
	double limitItemNum(double num) const;

	int getJudgeSize() const;
	int getTrackLength() const;
	int getCaredPos(const juce::Point<int>& pos) const;
	std::tuple<double, double> getThumb() const;

	void resetState();
	void updateState(const juce::Point<int>& pos, bool pressed);
	void updateCursor();

	void updateImageTemp();
	void updateBackTemp();
	void updateFrontTemp();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollerBase)
};
