#include "ActionSet.h"

#include "../AudioCore.h"
#include "../misc/Device.h"

ActionSetDeviceAudioType::ActionSetDeviceAudioType(const juce::String& type)
	: type(type) {}

bool ActionSetDeviceAudioType::doAction() {
	Device::getInstance()->setCurrentAudioDeviceType(this->type);

	juce::String result;
	result += "Current Audio Device Type: " + Device::getInstance()->getCurrentAudioDeviceType() + "\n";
	result += "Current Audio Input Device: " + Device::getInstance()->getAudioInputDeviceName() + "\n";
	result += "Current Audio Output Device: " + Device::getInstance()->getAudioOutputDeviceName() + "\n";
	this->output(result);
	return true;
}
