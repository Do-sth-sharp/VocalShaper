#pragma once

#include <JuceHeader.h>

class CoreCommandTarget final
	: public juce::ApplicationCommandTarget,
	private juce::DeletedAtShutdown {
public:
	CoreCommandTarget();
	~CoreCommandTarget();

	juce::ApplicationCommandTarget* getNextCommandTarget() override;
	void getAllCommands(juce::Array<juce::CommandID>& commands) override;
	void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
	bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

	void systemRequestOpen(const juce::String& path);

private:
	void newProject() const;
	void openProject() const;
	void saveProject() const;
	void loadSource() const;
	void loadSynthSource() const;
	void saveSource() const;
	void exportSource() const;
	void render() const;

	void undo() const;
	void redo() const;

	void play() const;
	void stop() const;
	void record() const;
	void rewind() const;

private:
	bool checkForSave() const;
	void selectForSource(const std::function<void(int)>& callback) const;
	void selectForMixerTracks(
		const std::function<void(const juce::Array<int>&)>& callback) const;

	bool checkForUndo() const;
	bool checkForRedo() const;
	const juce::String getUndoName() const;
	const juce::String getRedoName() const;

	bool checkForPlaying() const;
	bool checkForRecording() const;

	std::unique_ptr<juce::ListBox> trackListBox = nullptr;
	std::unique_ptr<juce::ListBoxModel> trackListBoxModel = nullptr;

public:
	static CoreCommandTarget* getInstance();
	static void releaseInstance();

private:
	static CoreCommandTarget* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreCommandTarget)
};
