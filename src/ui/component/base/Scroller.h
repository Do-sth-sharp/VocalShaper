#pragma once

#include <JuceHeader.h>
#include "ScrollerBase.h"

class Scroller : public ScrollerBase {
public:
	using ViewSizeFunc = std::function<double(void)>;
	using ItemNumFunc = std::function<double(void)>;
	using ItemSizeLimitFunc = std::function<std::tuple<double, double>(void)>;
	using UpdatePosFunc = std::function<void(double, double)>;
	using PaintPreviewFunc = std::function<void(juce::Graphics&, int, int, bool)>;
	using PaintItemPreviewFunc = std::function<void(juce::Graphics&, int, int, int, bool)>;
	using PlayPosFunc = std::function<double(void)>;

public:
	Scroller() = delete;
	Scroller(bool vertical,
		const ViewSizeFunc& viewSizeCallback,
		const ItemNumFunc& itemNumCallback,
		const ItemSizeLimitFunc& itemSizeLimitCallback,
		const UpdatePosFunc& updatePosCallback,
		const PaintPreviewFunc& paintPreviewCallback = PaintPreviewFunc{},
		const PaintItemPreviewFunc& paintItemPreviewCallback = PaintItemPreviewFunc{},
		const PlayPosFunc& playPosCallback = PlayPosFunc{});

protected:
	double createViewSize() override;
	double createItemNum() override;
	std::tuple<double, double> createItemSizeLimit() override;

	void updatePos(double pos, double itemSize) override;

	void paintPreview(juce::Graphics& g, int width, int height, bool vertical) override;
	void paintItemPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical) override;

	double getPlayPos() override;

private:
	const ViewSizeFunc viewSizeCallback;
	const ItemNumFunc itemNumCallback;
	const ItemSizeLimitFunc itemSizeLimitCallback;
	const UpdatePosFunc updatePosCallback;
	const PaintPreviewFunc paintPreviewCallback;
	const PaintItemPreviewFunc paintItemPreviewCallback;
	const PlayPosFunc playPosCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scroller)
};
