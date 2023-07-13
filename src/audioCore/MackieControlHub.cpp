#include "MackieControlHub.h"

MackieControlHub::~MackieControlHub() {
	juce::ScopedWriteLock locker(this->deviceListLock);

	for (auto& [device, index] : this->inputDevices) {
		device->stop();
	}
	for (auto& [device, index] : this->outputDevices) {
		device->stopBackgroundThread();
	}

	this->inputDevices.clear();
	this->outputDevices.clear();
}

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

		ptr->start();
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

		ptr->startBackgroundThread();
		return ptr;
	}

	return nullptr;
}

bool MackieControlHub::closeInputDevice(const juce::String& deviceIdentifier) {
	/** Lock */
	juce::ScopedWriteLock locker(this->deviceListLock);

	/** Find Device */
	auto deviceIndex = this->findInputDevice(deviceIdentifier);

	/** Remove Device */
	if (deviceIndex > -1) {
		auto& [device, index] = this->inputDevices.getReference(deviceIndex);
		device->stop();
		this->inputDevices.remove(deviceIndex);
		return true;
	}

	return false;
}

bool MackieControlHub::closeOutputDevice(const juce::String& deviceIdentifier) {
	/** Lock */
	juce::ScopedWriteLock locker(this->deviceListLock);

	/** Find Device */
	auto deviceIndex = this->findOutputDevice(deviceIdentifier);

	/** Remove Device */
	if (deviceIndex > -1) {
		auto& [device, index] = this->outputDevices.getReference(deviceIndex);
		device->stopBackgroundThread();
		this->outputDevices.remove(deviceIndex);
		return true;
	}

	return false;
}

int MackieControlHub::findInputDevice(const juce::String& deviceIdentifier) const {
	juce::ScopedReadLock locker(this->deviceListLock);
	for (int i = 0; i < this->inputDevices.size(); i++) {
		auto& [device, index] = this->inputDevices.getReference(i);
		if (device->getIdentifier() == deviceIdentifier) {
			return i;
		}
	}
	return -1;
}

int MackieControlHub::findOutputDevice(const juce::String& deviceIdentifier) const {
	juce::ScopedReadLock locker(this->deviceListLock);
	for (int i = 0; i < this->outputDevices.size(); i++) {
		auto& [device, index] = this->outputDevices.getReference(i);
		if (device->getIdentifier() == deviceIdentifier) {
			return i;
		}
	}
	return -1;
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
