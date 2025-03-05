#pragma once

#include <JuceHeader.h>
#include "../component/plugin/PluginEditor.h"

class PluginEditorHub final : private juce::DeletedAtShutdown {
public:
	PluginEditorHub();
	~PluginEditorHub();

	void openInstr(int index);
	void closeInstr(int index);
	bool checkInstr(int index) const;

	void openEffect(int type, int track, int index);
	void closeEffect(int type, int track, int index);
	bool checkEffect(int type, int track, int index) const;

	void setOpenGL(bool openGLOn);
	void setBufferedPainting(bool bufferedPainting);
	void setIcon(const juce::String& path);

	void closeAll();

private:
	using RefType = uint64_t;
	std::unordered_map<RefType, std::shared_ptr<PluginEditor>> instrEditors;
	std::unordered_map<RefType, std::shared_ptr<PluginEditor>> effectEditors;
	bool openGLOn = true;
	bool bufferedPainting = true;
	juce::Image iconTemp;

	void closeEditor(PluginEditor* ptr);
	void openEditor(PluginEditor* ptr);

	void trackAdded(int type, int track);
	void trackRemoved(int type, int track);
	void effectIndexChanged(int type, int track, int oldIndex, int newIndex);
	void instrChanged(int track);
	void effectChanged(int type, int track, int index);

	RefType getInstrRef(int index) const;
	RefType getEffectRef(int type, int track, int index) const;

private:
	friend class PluginEditorContent;
	void closeInstr(PluginEditor* ptr);
	void closeEffect(PluginEditor* ptr);

public:
	static PluginEditorHub* getInstance();
	static void releaseInstance();

private:
	static PluginEditorHub* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorHub)
};
