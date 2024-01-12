#pragma once

#include <JuceHeader.h>

#include "../dataModel/MessageModel.h"

class MessageList final : public juce::Component {
public:
	MessageList();

	void paint(juce::Graphics& g) override;

	void update(const juce::Array<MessageModel::Message>& list);
	void refresh();

	int getHeightPrefered() const;

private:
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

private:
	using MessageTemp = std::tuple<MessageModel::Message, juce::TextLayout, int>;
	juce::Array<MessageTemp> list;

	std::tuple<bool, MessageModel::Callback> findByYPos(float y) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageList)
};
