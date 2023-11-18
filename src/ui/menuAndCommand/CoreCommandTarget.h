#pragma once

#include <JuceHeader.h>

class CoreCommandTarget final
	: public juce::ApplicationCommandTarget,
	private juce::DeletedAtShutdown {
public:
	CoreCommandTarget() = default;

	juce::ApplicationCommandTarget* getNextCommandTarget() override;
	void getAllCommands(juce::Array<juce::CommandID>& commands) override;
	void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
	bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

public:
	static CoreCommandTarget* getInstance();
	static void releaseInstance();

private:
	static CoreCommandTarget* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreCommandTarget)
};
