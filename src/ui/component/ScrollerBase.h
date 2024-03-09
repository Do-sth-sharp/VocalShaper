#pragma once

#include <JuceHeader.h>

class ScrollerBase : public juce::Component {
public:
	ScrollerBase() = delete;
	ScrollerBase(bool vertical);
	virtual ~ScrollerBase() = default;

	void paint(juce::Graphics& g) override;

	void update();
	void setPos(double pos);
	void setItemSize(double size);

	double getViewPos() const;
	double getViewSize() const;
	double getItemSize() const;
	double getItemNum() const;
	double getItemMinSize() const;
	double getItemMaxSize() const;

	double getActualTotalSize() const;

	void mouseMove(const juce::MouseEvent& event);
	void mouseDrag(const juce::MouseEvent& event);
	void mouseDown(const juce::MouseEvent& event);
	void mouseUp(const juce::MouseEvent& event);
	void mouseExit(const juce::MouseEvent& event);
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel);

protected:
	virtual double createViewSize() = 0;
	virtual double createItemNum() = 0;
	virtual std::tuple<double, double> createItemSizeLimit() = 0;

	virtual void updatePos(double pos, double itemSize) = 0;

	virtual void paintPreview(juce::Graphics& g, bool vertical) {};
	virtual void paintItemPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical) {};

private:
	const bool vertical;

	double viewPos = 0, viewSize = 0;
	double itemSize = 0, itemNum = 0;
	double itemMinSize = 0, itemMaxSize = 0;

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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollerBase)
};
