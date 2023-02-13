#include "AudioCommand.h"

AudioCommand::AudioCommand(AudioCore* parent) 
	: parent(parent) {
	jassert(parent);
}

const AudioCommand::CommandResult AudioCommand::processCommand(const juce::String& command) {
	AudioCommand::CommandResult result{ false, "Empty Command" };

	return result;
}

void AudioCommand::processCommandAsync(const juce::String& command, AudioCommand::CommandCallback callback) {

}
