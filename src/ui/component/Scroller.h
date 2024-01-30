#pragma once

#include <JuceHeader.h>
#include "ScrollerBase.h"

class Scroller : public ScrollerBase {
public:
	using ViewSizeFunc = std::function<int(void)>;
	using ItemNumFunc = std::function<int(void)>;
	using ItemSizeLimitFunc = std::function<std::tuple<int, int>(void)>;
	using UpdatePosFunc = std::function<void(int, int)>;
	using PaintPreviewFunc = std::function<void(juce::Graphics&, bool)>;
	using PaintItemPreviewFunc = std::function<void(juce::Graphics&, int, int, int, bool)>;

public:
	Scroller() = delete;
	Scroller(bool vertical,
		const ViewSizeFunc& viewSizeCallback,
		const ItemNumFunc& itemNumCallback,
		const ItemSizeLimitFunc& itemSizeLimitCallback,
		const UpdatePosFunc& updatePosCallback,
		const PaintPreviewFunc& paintPreviewCallback = PaintPreviewFunc{},
		const PaintItemPreviewFunc& paintItemPreviewCallback = PaintItemPreviewFunc{});

protected:
	int createViewSize() override;
	int createItemNum() override;
	std::tuple<int, int> createItemSizeLimit() override;

	void updatePos(int pos, int itemSize) override;

	void paintPreview(juce::Graphics& g, bool vertical) override;
	void paintItemPreview(juce::Graphics& g, int itemIndex,
		int width, int height, bool vertical) override;

private:
	const ViewSizeFunc viewSizeCallback;
	const ItemNumFunc itemNumCallback;
	const ItemSizeLimitFunc itemSizeLimitCallback;
	const UpdatePosFunc updatePosCallback;
	const PaintPreviewFunc paintPreviewCallback;
	const PaintItemPreviewFunc paintItemPreviewCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scroller)
};
