#include "MackieControlHub.h"
#include "../misc/AudioLock.h"

MackieControlHub::~MackieControlHub() {
	juce::ScopedWriteLock locker(audioLock::getMackieLock());

	for (auto& [device, index] : this->inputDevices) {
		device->stop();
	}
	for (auto& [device, index] : this->outputDevices) {
		device->stopBackgroundThread();
	}

	this->inputDevices.clear();
	this->outputDevices.clear();
}

juce::MidiInput* MackieControlHub::openInputDevice(
	const juce::String& deviceIdentifier, int index) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getMackieLock());

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

juce::MidiOutput* MackieControlHub::openOutputDevice(
	const juce::String& deviceIdentifier, int index) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getMackieLock());

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
	juce::ScopedWriteLock locker(audioLock::getMackieLock());

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
	juce::ScopedWriteLock locker(audioLock::getMackieLock());

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
	for (int i = 0; i < this->inputDevices.size(); i++) {
		auto& [device, index] = this->inputDevices.getReference(i);
		if (device->getIdentifier() == deviceIdentifier) {
			return i;
		}
	}
	return -1;
}

int MackieControlHub::findOutputDevice(const juce::String& deviceIdentifier) const {
	for (int i = 0; i < this->outputDevices.size(); i++) {
		auto& [device, index] = this->outputDevices.getReference(i);
		if (device->getIdentifier() == deviceIdentifier) {
			return i;
		}
	}
	return -1;
}

int MackieControlHub::findInputDevice(juce::MidiInput* ptr) const {
	for (int i = 0; i < this->inputDevices.size(); i++) {
		auto& [device, index] = this->inputDevices.getReference(i);
		if (device.get() == ptr) {
			return i;
		}
	}
	return -1;
}
int MackieControlHub::findOutputDevice(juce::MidiOutput* ptr) const {
	for (int i = 0; i < this->outputDevices.size(); i++) {
		auto& [device, index] = this->outputDevices.getReference(i);
		if (device.get() == ptr) {
			return i;
		}
	}
	return -1;
}

juce::MidiInput* MackieControlHub::getInputDevice(int index) const {
	if (index < 0 || index >= this->inputDevices.size()) { return nullptr; }

	return std::get<0>(this->inputDevices.getReference(index)).get();
}

juce::MidiOutput* MackieControlHub::getOutputDevice(int index) const {
	if (index < 0 || index >= this->outputDevices.size()) { return nullptr; }

	return std::get<0>(this->outputDevices.getReference(index)).get();
}

int MackieControlHub::getInputDeviceIndex(int index) const {
	if (index < 0 || index >= this->inputDevices.size()) { return 0; }

	return std::get<1>(this->inputDevices.getReference(index));
}

int MackieControlHub::getOutputDeviceIndex(int index) const {
	if (index < 0 || index >= this->outputDevices.size()) { return 0; }

	return std::get<1>(this->outputDevices.getReference(index));
}

bool MackieControlHub::setInputDeviceIndex(int index, int deviceIndex) {
	juce::ScopedWriteLock locker(audioLock::getMackieLock());
	if (index < 0 || index >= this->inputDevices.size()) { return false; }

	auto& [device, devIdx] = this->inputDevices.getReference(index);
	devIdx = deviceIndex;
	return true;
}

bool MackieControlHub::setOutputDeviceIndex(int index, int deviceIndex) {
	juce::ScopedWriteLock locker(audioLock::getMackieLock());
	if (index < 0 || index >= this->outputDevices.size()) { return false; }

	auto& [device, devIdx] = this->outputDevices.getReference(index);
	devIdx = deviceIndex;
	return true;
}

void MackieControlHub::removeUnavailableDevices(
	const juce::Array<juce::MidiDeviceInfo>& inputDevices,
	const juce::Array<juce::MidiDeviceInfo>& outputDeivces) {
	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getMackieLock());

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

void MackieControlHub::removeUnavailableDevices() {
	auto input = juce::MidiInput::getAvailableDevices();
	auto output = juce::MidiOutput::getAvailableDevices();
	this->removeUnavailableDevices(input, output);
}

void MackieControlHub::sendMackieControlMessage(
	const std::tuple<int, mackieControl::Message>& mes) const {
	/** Get Message */
	auto& [channel, message] = mes;
	if (!message.isMackieControl()) { return; }

	/** Send Message */
	for (auto& [dev, idx] : this->outputDevices) {
		/** Rewrite Message Channel */
		if (message.isSysEx()) {
			switch (std::get<0>(message.getSysExData())) {
			case mackieControl::SysExMessage::FaderTouchSensitivity: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				auto value = std::get<1>(message.getFaderTouchSensitivityData());
				
				auto mes =
					mackieControl::Message::createFaderTouchSensitivity(chan, value);
				dev->sendMessageNow(mackieControl::Message::toMidi(mes));
				continue;
			}
			case mackieControl::SysExMessage::ChannelMeterMode: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				auto value = std::get<1>(message.getChannelMeterModeData());

				auto mes =
					mackieControl::Message::createChannelMeterMode(chan, value);
				dev->sendMessageNow(mackieControl::Message::toMidi(mes));
				continue;
			}
			}
		}
		else if (message.isNote()) {
			auto [type, vel] = message.getNoteData();
			auto rewriteFunc = [&dev, message, vel](mackieControl::NoteMessage base, int chan) {
				auto mes =
					mackieControl::Message::createNote(
						static_cast<mackieControl::NoteMessage>(static_cast<int>(base) + (chan - 1)), vel);
				dev->sendMessageNow(mackieControl::Message::toMidi(mes));
				return;
			};

			switch (type) {
			case mackieControl::NoteMessage::RECRDYCh1:
			case mackieControl::NoteMessage::RECRDYCh2:
			case mackieControl::NoteMessage::RECRDYCh3:
			case mackieControl::NoteMessage::RECRDYCh4:
			case mackieControl::NoteMessage::RECRDYCh5:
			case mackieControl::NoteMessage::RECRDYCh6:
			case mackieControl::NoteMessage::RECRDYCh7:
			case mackieControl::NoteMessage::RECRDYCh8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::NoteMessage::RECRDYCh1, chan);
				continue;
			}
			case mackieControl::NoteMessage::SOLOCh1:
			case mackieControl::NoteMessage::SOLOCh2:
			case mackieControl::NoteMessage::SOLOCh3:
			case mackieControl::NoteMessage::SOLOCh4:
			case mackieControl::NoteMessage::SOLOCh5:
			case mackieControl::NoteMessage::SOLOCh6:
			case mackieControl::NoteMessage::SOLOCh7:
			case mackieControl::NoteMessage::SOLOCh8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::NoteMessage::SOLOCh1, chan);
				continue;
			}
			case mackieControl::NoteMessage::MUTECh1:
			case mackieControl::NoteMessage::MUTECh2:
			case mackieControl::NoteMessage::MUTECh3:
			case mackieControl::NoteMessage::MUTECh4:
			case mackieControl::NoteMessage::MUTECh5:
			case mackieControl::NoteMessage::MUTECh6:
			case mackieControl::NoteMessage::MUTECh7:
			case mackieControl::NoteMessage::MUTECh8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::NoteMessage::MUTECh1, chan);
				continue;
			}
			case mackieControl::NoteMessage::SELECTCh1:
			case mackieControl::NoteMessage::SELECTCh2:
			case mackieControl::NoteMessage::SELECTCh3:
			case mackieControl::NoteMessage::SELECTCh4:
			case mackieControl::NoteMessage::SELECTCh5:
			case mackieControl::NoteMessage::SELECTCh6:
			case mackieControl::NoteMessage::SELECTCh7:
			case mackieControl::NoteMessage::SELECTCh8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::NoteMessage::SELECTCh1, chan);
				continue;
			}
			case mackieControl::NoteMessage::VSelectCh1:
			case mackieControl::NoteMessage::VSelectCh2:
			case mackieControl::NoteMessage::VSelectCh3:
			case mackieControl::NoteMessage::VSelectCh4:
			case mackieControl::NoteMessage::VSelectCh5:
			case mackieControl::NoteMessage::VSelectCh6:
			case mackieControl::NoteMessage::VSelectCh7:
			case mackieControl::NoteMessage::VSelectCh8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::NoteMessage::VSelectCh1, chan);
				continue;
			}
			case mackieControl::NoteMessage::FaderTouchCh1:
			case mackieControl::NoteMessage::FaderTouchCh2:
			case mackieControl::NoteMessage::FaderTouchCh3:
			case mackieControl::NoteMessage::FaderTouchCh4:
			case mackieControl::NoteMessage::FaderTouchCh5:
			case mackieControl::NoteMessage::FaderTouchCh6:
			case mackieControl::NoteMessage::FaderTouchCh7:
			case mackieControl::NoteMessage::FaderTouchCh8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::NoteMessage::FaderTouchCh1, chan);
				continue;
			}
			}
		}
		else if (message.isCC()) {
			auto [type, value] = message.getCCData();
			auto rewriteFunc = [&dev, message, value](mackieControl::CCMessage base, int chan) {
				auto mes =
					mackieControl::Message::createCC(
						static_cast<mackieControl::CCMessage>(static_cast<int>(base) + (chan - 1)), value);
				dev->sendMessageNow(mackieControl::Message::toMidi(mes));
				return;
			};

			switch (type) {
			case mackieControl::CCMessage::VPot1:
			case mackieControl::CCMessage::VPot2:
			case mackieControl::CCMessage::VPot3:
			case mackieControl::CCMessage::VPot4:
			case mackieControl::CCMessage::VPot5:
			case mackieControl::CCMessage::VPot6:
			case mackieControl::CCMessage::VPot7:
			case mackieControl::CCMessage::VPot8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::CCMessage::VPot1, chan);
				continue;
			}
			case mackieControl::CCMessage::VPotLEDRing1:
			case mackieControl::CCMessage::VPotLEDRing2:
			case mackieControl::CCMessage::VPotLEDRing3:
			case mackieControl::CCMessage::VPotLEDRing4:
			case mackieControl::CCMessage::VPotLEDRing5:
			case mackieControl::CCMessage::VPotLEDRing6:
			case mackieControl::CCMessage::VPotLEDRing7:
			case mackieControl::CCMessage::VPotLEDRing8: {
				int chan = channel - idx * 8 + 1;
				if (chan < 1 || chan > 8) { continue; }

				rewriteFunc(mackieControl::CCMessage::VPotLEDRing1, chan);
				continue;
			}
			}
		}
		else if (message.isPitchWheel()) {
			int chan = channel - idx * 8 + 1;
			if (chan < 1 || chan > 8) { continue; }

			auto value = std::get<1>(message.getPitchWheelData());

			auto mes = 
				mackieControl::Message::createPitchWheel(chan, value);
			dev->sendMessageNow(mackieControl::Message::toMidi(mes));
			continue;
		}
		else if (message.isChannelPressure()) {
			int chan = channel - idx * 8 + 1;
			if (chan < 1 || chan > 8) { continue; }

			auto value = std::get<1>(message.getChannelPressureData());

			auto mes =
				mackieControl::Message::createChannelPressure(chan, value);
			dev->sendMessageNow(mackieControl::Message::toMidi(mes));
			continue;
		}

		/** Directly Send */
		dev->sendMessageNow(mackieControl::Message::toMidi(message));
	}
}

void MackieControlHub::handleIncomingMidiMessage(
	juce::MidiInput* source, const juce::MidiMessage& message) {
	juce::MessageManager::callAsync(
		[parent = juce::WeakReference(this), source, message] {
			if (parent) { parent->processInputMessage(source, message); }});
}

void MackieControlHub::processInputMessage(
	juce::MidiInput* source, const juce::MidiMessage& message) const {
	/** Get deviceIndex */
	int deviceIndex = this->getInputDeviceIndex(this->findInputDevice(source));

	/** Build Mackie Message */
	auto mackie = mackieControl::Message::fromMidi(message);
	if (!mackie.isMackieControl()) { return; }

	/** Rewrite Message Channel */
	if (mackie.isSysEx()) {
		switch (std::get<0>(mackie.getSysExData())) {
		case mackieControl::SysExMessage::FaderTouchSensitivity: {
			int channel = std::get<0>(mackie.getFaderTouchSensitivityData()) - 1;
			channel += (deviceIndex * 8);

			this->invokeInternal(std::make_tuple(channel, mackie));
			return;
		}
		case mackieControl::SysExMessage::ChannelMeterMode: {
			int channel = std::get<0>(mackie.getChannelMeterModeData()) - 1;
			channel += (deviceIndex * 8);

			this->invokeInternal(std::make_tuple(channel, mackie));
			return;
		}
		}
	}
	else if (mackie.isNote()) {
		auto [type, vel] = mackie.getNoteData();
		auto rewriteFunc = [this, type, deviceIndex, mackie](mackieControl::NoteMessage base) {
			int channel =
				static_cast<int>(type) - static_cast<int>(base);
			channel += (deviceIndex * 8);

			this->invokeInternal(std::make_tuple(channel, mackie));
			return;
		};

		switch (type) {
		case mackieControl::NoteMessage::RECRDYCh1:
		case mackieControl::NoteMessage::RECRDYCh2:
		case mackieControl::NoteMessage::RECRDYCh3:
		case mackieControl::NoteMessage::RECRDYCh4:
		case mackieControl::NoteMessage::RECRDYCh5:
		case mackieControl::NoteMessage::RECRDYCh6:
		case mackieControl::NoteMessage::RECRDYCh7:
		case mackieControl::NoteMessage::RECRDYCh8: {
			rewriteFunc(mackieControl::NoteMessage::RECRDYCh1);
			return;
		}
		case mackieControl::NoteMessage::SOLOCh1:
		case mackieControl::NoteMessage::SOLOCh2:
		case mackieControl::NoteMessage::SOLOCh3:
		case mackieControl::NoteMessage::SOLOCh4:
		case mackieControl::NoteMessage::SOLOCh5:
		case mackieControl::NoteMessage::SOLOCh6:
		case mackieControl::NoteMessage::SOLOCh7:
		case mackieControl::NoteMessage::SOLOCh8: {
			rewriteFunc(mackieControl::NoteMessage::SOLOCh1);
			return;
		}
		case mackieControl::NoteMessage::MUTECh1:
		case mackieControl::NoteMessage::MUTECh2:
		case mackieControl::NoteMessage::MUTECh3:
		case mackieControl::NoteMessage::MUTECh4:
		case mackieControl::NoteMessage::MUTECh5:
		case mackieControl::NoteMessage::MUTECh6:
		case mackieControl::NoteMessage::MUTECh7:
		case mackieControl::NoteMessage::MUTECh8: {
			rewriteFunc(mackieControl::NoteMessage::MUTECh1);
			return;
		}
		case mackieControl::NoteMessage::SELECTCh1:
		case mackieControl::NoteMessage::SELECTCh2:
		case mackieControl::NoteMessage::SELECTCh3:
		case mackieControl::NoteMessage::SELECTCh4:
		case mackieControl::NoteMessage::SELECTCh5:
		case mackieControl::NoteMessage::SELECTCh6:
		case mackieControl::NoteMessage::SELECTCh7:
		case mackieControl::NoteMessage::SELECTCh8: {
			rewriteFunc(mackieControl::NoteMessage::SELECTCh1);
			return;
		}
		case mackieControl::NoteMessage::VSelectCh1:
		case mackieControl::NoteMessage::VSelectCh2:
		case mackieControl::NoteMessage::VSelectCh3:
		case mackieControl::NoteMessage::VSelectCh4:
		case mackieControl::NoteMessage::VSelectCh5:
		case mackieControl::NoteMessage::VSelectCh6:
		case mackieControl::NoteMessage::VSelectCh7:
		case mackieControl::NoteMessage::VSelectCh8: {
			rewriteFunc(mackieControl::NoteMessage::VSelectCh1);
			return;
		}
		case mackieControl::NoteMessage::FaderTouchCh1:
		case mackieControl::NoteMessage::FaderTouchCh2:
		case mackieControl::NoteMessage::FaderTouchCh3:
		case mackieControl::NoteMessage::FaderTouchCh4:
		case mackieControl::NoteMessage::FaderTouchCh5:
		case mackieControl::NoteMessage::FaderTouchCh6:
		case mackieControl::NoteMessage::FaderTouchCh7:
		case mackieControl::NoteMessage::FaderTouchCh8: {
			rewriteFunc(mackieControl::NoteMessage::FaderTouchCh1);
			return;
		}
		}
	}
	else if (mackie.isCC()) {
		auto [type, value] = mackie.getCCData();
		auto rewriteFunc = [this, type, deviceIndex, mackie](mackieControl::CCMessage base) {
			int channel =
				static_cast<int>(type) - static_cast<int>(base);
			channel += (deviceIndex * 8);

			this->invokeInternal(std::make_tuple(channel, mackie));
			return;
		};

		switch (type) {
		case mackieControl::CCMessage::VPot1:
		case mackieControl::CCMessage::VPot2:
		case mackieControl::CCMessage::VPot3:
		case mackieControl::CCMessage::VPot4:
		case mackieControl::CCMessage::VPot5:
		case mackieControl::CCMessage::VPot6:
		case mackieControl::CCMessage::VPot7:
		case mackieControl::CCMessage::VPot8: {
			rewriteFunc(mackieControl::CCMessage::VPot1);
			return;
		}
		case mackieControl::CCMessage::VPotLEDRing1:
		case mackieControl::CCMessage::VPotLEDRing2:
		case mackieControl::CCMessage::VPotLEDRing3:
		case mackieControl::CCMessage::VPotLEDRing4:
		case mackieControl::CCMessage::VPotLEDRing5:
		case mackieControl::CCMessage::VPotLEDRing6:
		case mackieControl::CCMessage::VPotLEDRing7:
		case mackieControl::CCMessage::VPotLEDRing8: {
			rewriteFunc(mackieControl::CCMessage::VPotLEDRing1);
			return;
		}
		}
	}
	else if (mackie.isPitchWheel()) {
		int channel = std::get<0>(mackie.getPitchWheelData()) - 1;
		channel += (deviceIndex * 8);

		this->invokeInternal(std::make_tuple(channel, mackie));
		return;
	}
	else if (mackie.isChannelPressure()) {
		int channel = std::get<0>(mackie.getChannelPressureData()) - 1;
		channel += (deviceIndex * 8);

		this->invokeInternal(std::make_tuple(channel, mackie));
		return;
	}

	/** Directly Send */
	this->invokeInternal(std::make_tuple(0, mackie));
}

void MackieControlHub::invokeInternal(
	const std::tuple<int, mackieControl::Message>& mes) const {
	if (this->mackieCallback) {
		this->mackieCallback(mes);
	}
}
