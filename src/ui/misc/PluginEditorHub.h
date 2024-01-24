#pragma once

#include <JuceHeader.h>
#include "../component/PluginEditor.h"

class PluginEditorHub final : private juce::DeletedAtShutdown {
public:
	PluginEditorHub();

	void openInstr(int index);
	void closeInstr(int index);
	bool checkInstr(int index) const;

private:
	juce::OwnedArray<PluginEditor> instrEditors;
	juce::OwnedArray<PluginEditor> effectEditors;

	friend class PluginEditorContent;
	void deleteInstrEditor(PluginEditor* ptr);
	void deleteEffectEditor(PluginEditor* ptr);
	void closeEditor(PluginEditor* ptr);
	void openEditor(PluginEditor* ptr);

	void updateInstr();

public:
	static PluginEditorHub* getInstance();
	static void releaseInstance();

private:
	static PluginEditorHub* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorHub)
};
