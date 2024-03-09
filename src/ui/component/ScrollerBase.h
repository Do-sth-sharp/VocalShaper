#pragma once

#include <JuceHeader.h>

class ScrollerBase : public juce::Component {
public:
	ScrollerBase() = delete;
	ScrollerBase(bool vertical);
	virtual ~ScrollerBase() = default;

	void paint(juce::Graphics& g) override;

	void update();
	void setPos(int pos);
	void setItemSize(int size);

	int getViewPos() const;
	int getViewSize() const;
	int getItemSize() const;
	double getItemNum() const;
	int getItemMinSize() const;
	int getItemMaxSize() const;

	int getActualTotalSize() const;

	void mouseMove(const juce::MouseEvent& event);
	void mouseDrag(const juce::MouseEvent& event);
	void mouseDown(const juce::MouseEvent& event);
	void mouseUp(const juce::MouseEvent& event);
	void mouseExit(const juce::MouseEvent& event);
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel);

protected:
	virtual int createViewSize() = 0;
	virtual double createItemNum() = 0;
	virtual std::tuple<int, int> createItemSizeLimit() = 0;

	virtual void updatePos(int pos, int itemSize) = 0;

	virtual void paintPreview(juce::Graphics& g, bool vertical) {};
	virtual void paintItemPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical) {};

private:
	const bool vertical;

	int viewPos = 0, viewSize = 0;
	int itemSize = 0;
	double itemNum = 0;
	int itemMinSize = 0, itemMaxSize = 0;

	enum class State {
		Normal, HoverThumb, HoverStart, HoverEnd,
		PressedThumb, PressedStart, PressedEnd
	};
	State state = State::Normal;

	double thumbPressedPer = -1;
	void recordThumbPress(int pos);

	void setStart(int start);
	void setEnd(int end);

	int limitPos(int pos) const;
	int limitItemSize(int size) const;
	double limitItemNum(double num) const;

	int getJudgeSize() const;
	int getTrackLength() const;
	int getCaredPos(const juce::Point<int>& pos) const;
	std::tuple<int, int> getThumb() const;

	void resetState();
	void updateState(const juce::Point<int>& pos, bool pressed);
	void updateCursor();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollerBase)
};
