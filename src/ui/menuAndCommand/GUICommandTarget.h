#pragma once

#include <JuceHeader.h>

class GUICommandTarget final
	: public juce::ApplicationCommandTarget,
	private juce::DeletedAtShutdown {
public:
	GUICommandTarget() = default;

	juce::ApplicationCommandTarget* getNextCommandTarget() override;
	void getAllCommands(juce::Array<juce::CommandID>& commands) override;
	void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
	bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

private:
	void closeEditor() const;

	void loadLayout() const;
	void saveLayout() const;

public:
	static GUICommandTarget* getInstance();
	static void releaseInstance();

private:
	static GUICommandTarget* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUICommandTarget)
};

