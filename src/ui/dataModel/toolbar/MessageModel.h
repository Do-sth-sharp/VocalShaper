#pragma once

#include <JuceHeader.h>

class MessageModel final
	: public juce::ChangeBroadcaster,
	private juce::DeletedAtShutdown {
public:
	MessageModel() = default;

	using Callback = std::function<void(void)>;
	using Message = std::tuple<juce::Time, juce::String, Callback>;

	void addMessage(const Message& mes);
	void addNow(const juce::String& mes, const Callback& callback);
	void clear();

	const juce::Array<Message>& getList() const;
	int getNum() const;
	bool isEmpty() const;

private:
	juce::Array<Message> list;

public:
	static MessageModel* getInstance();
	static void releaseInstance();

private:
	static MessageModel* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageModel)
};
