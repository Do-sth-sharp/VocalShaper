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

juce::MidiInput* MackieControlHub::getInputDevice(int index) const {
	juce::ScopedReadLock locker(this->deviceListLock);
	if (index < 0 || index >= this->inputDevices.size()) { return nullptr; }

	return std::get<0>(this->inputDevices.getReference(index)).get();
}

juce::MidiOutput* MackieControlHub::getOutputDevice(int index) const {
	juce::ScopedReadLock locker(this->deviceListLock);
	if (index < 0 || index >= this->outputDevices.size()) { return nullptr; }

	return std::get<0>(this->outputDevices.getReference(index)).get();
}

int MackieControlHub::getInputDeviceIndex(int index) const {
	juce::ScopedReadLock locker(this->deviceListLock);
	if (index < 0 || index >= this->inputDevices.size()) { return 0; }

	return std::get<1>(this->inputDevices.getReference(index));
}

int MackieControlHub::getOutputDeviceIndex(int index) const {
	juce::ScopedReadLock locker(this->deviceListLock);
	if (index < 0 || index >= this->outputDevices.size()) { return 0; }

	return std::get<1>(this->outputDevices.getReference(index));
}

bool MackieControlHub::setInputDeviceIndex(int index, int deviceIndex) {
	juce::ScopedWriteLock locker(this->deviceListLock);
	if (index < 0 || index >= this->inputDevices.size()) { return false; }

	auto& [device, devIdx] = this->inputDevices.getReference(index);
	devIdx = deviceIndex;
	return true;
}

bool MackieControlHub::setOutputDeviceIndex(int index, int deviceIndex) {
	juce::ScopedWriteLock locker(this->deviceListLock);
	if (index < 0 || index >= this->outputDevices.size()) { return false; }

	auto& [device, devIdx] = this->outputDevices.getReference(index);
	devIdx = deviceIndex;
	return true;
}

void MackieControlHub::removeUnavailableDevices(
	const juce::Array<juce::MidiDeviceInfo>& inputDevices,
	const juce::Array<juce::MidiDeviceInfo>& outputDeivces) {
	/** Lock */
	juce::ScopedWriteLock locker(this->deviceListLock);

	/** Input */
	for (int i = this->inputDevices.size() - 1; i >= 0; i--) {
		auto& [device, devIdx] = this->inputDevices.getReference(i);

		if (!inputDevices.contains(
			juce::MidiDeviceInfo{device->getName(), device->getIdentifier()})) {
			device->stop();
			this->inputDevices.remove(i);
		}
	}

	/** Output */
	for (int i = this->outputDevices.size() - 1; i >= 0; i--) {
		auto& [device, devIdx] = this->outputDevices.getReference(i);

		if (!inputDevices.contains(
			juce::MidiDeviceInfo{device->getName(), device->getIdentifier()})) {
			device->stopBackgroundThread();
			this->outputDevices.remove(i);
		}
	}
}

void MackieControlHub::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
	juce::MessageManager::callAsync(
		[parent = juce::WeakReference(this), source, message] {
			if (parent) { parent->processInputMessage(source, message); }});
}

void MackieControlHub::processInputMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
	/** TODO Process Mackie Control Message */
}
