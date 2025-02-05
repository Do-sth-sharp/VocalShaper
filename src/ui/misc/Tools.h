#pragma once

#include <JuceHeader.h>

class Tools final : private juce::DeletedAtShutdown {
public:
	Tools();

	enum class Type {
		Arrow, Pencil
	};
	void setType(Type type);
	Type getType() const;

	void setAdsorb(double adsorbLevel);
	double getAdsorb() const;

	void setFollow(bool follow);
	bool getFollow() const;

	void setMIDIChannel(uint8_t channel);
	uint8_t getMIDIChannel() const;
	void addMIDIChannelChangedListener(juce::ChangeListener* listener);

	void setEditingTrack(int type, int index);
	std::pair<int, int> getEditingTrack() const;
	void addEditingTrackChangedListener(juce::ChangeListener* listener);

private:
	Type type = Type::Arrow;
	double adsorbLevel = 1;
	bool editorFollow = false;
	uint8_t midiChannel = 1;
	int editingTrackType = -1, editingTrackIndex = -1;

	juce::ChangeBroadcaster midiChannelBroadcaster;
	juce::ChangeBroadcaster editingTrackBroadcaster;

public:
	static Tools* getInstance();
	static void releaseInstance();

private:
	static Tools* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tools)
};
