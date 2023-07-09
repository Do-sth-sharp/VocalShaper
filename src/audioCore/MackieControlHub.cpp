#include "MackieControlHub.h"

juce::MidiInput* MackieControlHub::openInputDevice(const juce::String& deviceIdentifier, int index) {
	/** Lock */
	juce::ScopedWriteLock locker(this->deviceListLock);

	/** Device Has Opened */
	for (auto& [device, ind] : this->inputDevices) {
		if (device->getIdentifier() == deviceIdentifier) {
			ind = index;
			return device.get();
		}
	}

	/** Open Device */
	auto ptrDevice = juce::MidiInput::openDevice(deviceIdentifier, this);
	if (ptrDevice) {
		auto ptr = ptrDevice.get();
		this->inputDevices.add(std::make_tuple(std::move(ptrDevice), index));
		return ptr;
	}

	return nullptr;
}

juce::MidiOutput* MackieControlHub::openOutputDevice(const juce::String& deviceIdentifier, int index) {
	/** Lock */
	juce::ScopedWriteLock locker(this->deviceListLock);

	/** Device Has Opened */
	for (auto& [device, ind] : this->outputDevices) {
		if (device->getIdentifier() == deviceIdentifier) {
			ind = index;
			return device.get();
		}
	}

	/** Open Device */
	auto ptrDevice = juce::MidiOutput::openDevice(deviceIdentifier);
	if (ptrDevice) {
		auto ptr = ptrDevice.get();
		this->outputDevices.add(std::make_tuple(std::move(ptrDevice), index));
		return ptr;
	}

	return nullptr;
}

const juce::Array<juce::MidiDeviceInfo> MackieControlHub::getAvailableInputDevices() {
	return juce::MidiInput::getAvailableDevices();
}

const juce::Array<juce::MidiDeviceInfo> MackieControlHub::getAvailableOutputDevices() {
	return juce::MidiOutput::getAvailableDevices();
}

void MackieControlHub::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
	juce::MessageManager::callAsync(
		[parent = juce::WeakReference(this), source, message] {
			if (parent) { parent->processInputMessage(source, message); }});
}

void MackieControlHub::processInputMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
	/** TODO Process Mackie Control Message */
}
