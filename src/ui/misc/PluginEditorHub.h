#pragma once

#include <JuceHeader.h>
#include "../component/PluginEditor.h"

class PluginEditorHub final : private juce::DeletedAtShutdown {
public:
	PluginEditorHub();

	void openInstr(int index);
	void closeInstr(int index);
	bool checkInstr(int index) const;

	void openEffect(int track, int index);
	void closeEffect(int track, int index);
	bool checkEffect(int track, int index) const;

	void setOpenGL(bool opneGLOn);
	void setIcon(const juce::String& path);

private:
	juce::OwnedArray<PluginEditor> instrEditors;
	juce::OwnedArray<PluginEditor> effectEditors;
	bool openGLOn = true;
	juce::Image iconTemp;

	friend class PluginEditorContent;
	void deleteInstrEditor(PluginEditor* ptr);
	void deleteEffectEditor(PluginEditor* ptr);
	void closeEditor(PluginEditor* ptr);
	void openEditor(PluginEditor* ptr);

	void updateInstr();
	void updateEffect();

public:
	static PluginEditorHub* getInstance();
	static void releaseInstance();

private:
	static PluginEditorHub* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorHub)
};
