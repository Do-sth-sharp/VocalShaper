#pragma once

#include <JuceHeader.h>

class ChannelLinkViewContent final : public juce::Component {
public:
	ChannelLinkViewContent() = delete;
	ChannelLinkViewContent(const std::function<void(int, int, bool)>& callback,
		const juce::Array<std::tuple<int, int>>& initList,
		const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
		int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
		bool initIfEmpty);

	juce::Point<int> getPreferedSize() const;

	void paint(juce::Graphics& g) override;

private:
	const std::function<void(int, int, bool)> callback;
	const juce::AudioChannelSet srcChannels, dstChannels;
	const int srcChannelNum, dstChannelNum;
	const juce::String srcName, dstName;

	juce::BigInteger temp;

	bool checkLink(int srcc, int dstc);
	void setLink(int srcc, int dstc, bool link);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelLinkViewContent)
};

class ChannelLinkView final : public juce::DocumentWindow {
public:
	ChannelLinkView() = delete;
	ChannelLinkView(const std::function<void(int, int, bool)>& callback,
		const juce::Array<std::tuple<int, int>>& initList,
		const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
		int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
		bool initIfEmpty);

	void paint(juce::Graphics& g) override;

private:
	void closeButtonPressed() override;

private:
	std::unique_ptr<juce::Viewport> content = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelLinkView)
};
