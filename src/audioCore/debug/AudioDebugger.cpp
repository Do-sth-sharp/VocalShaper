﻿#include "AudioDebugger.h"
#include "../AudioCore.h"
#include "../misc/Device.h"
#include "../command/AudioCommand.h"

AudioDebugger::AudioDebugger() {
	this->setOpaque(true);

	/** Device Selector */
	this->deviceSelector = Device::createDeviceSelector();
	this->addAndMakeVisible(this->deviceSelector.get());

	/** Command */
	this->commandOutput = std::make_unique<CommandOutputComponent>();
	this->addAndMakeVisible(this->commandOutput.get());

	this->commandInput = std::make_unique<juce::TextEditor>();
	this->commandInput->setMultiLine(false);
	this->commandInput->setTabKeyUsedAsCharacter(true);
	this->commandInput->setScrollToShowCursor(true);
	this->commandInput->setPopupMenuEnabled(false);
	this->commandInput->onReturnKey =
		[input = this->commandInput.get(),
		debugger = juce::Component::SafePointer(this)] {
		auto str = input->getText();
		input->clear();
		if (debugger) {
			debugger->output(">>>" + str + "\n");
		}

		AudioCommand::getInstance()->processCommandAsync(str, [debugger](const AudioCommand::CommandResult& result) {
			if (debugger) {
				if (std::get<0>(result)) {
					debugger->output(std::get<2>(result) + "\n");
				}
				else {
					debugger->output("<<<FAILED!!!>>>\n");
					debugger->output(std::get<2>(result) + "\n");
				}
				debugger->output("\n");
			}
		});
	};
	this->addAndMakeVisible(this->commandInput.get());
}

void AudioDebugger::resized() {
	this->deviceSelector->setBounds(
		this->getLocalBounds().withTrimmedRight(this->getWidth() / 2));
	this->commandInput->setBounds(
		this->getLocalBounds().withTrimmedLeft(this->getWidth() / 2).withHeight(this->getHeight() * 0.05));
	this->commandOutput->setBounds(
		this->getLocalBounds().withTrimmedLeft(this->getWidth() / 2).withTrimmedTop(this->getHeight() * 0.05));
}

void AudioDebugger::paint(juce::Graphics& g) {
	g.fillAll(
		juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
			juce::ResizableWindow::backgroundColourId));
}

void AudioDebugger::userTriedToCloseWindow() {
	this->setVisible(false);
}

void AudioDebugger::output(const juce::String& mes) {
	this->commandOutput->add(mes);
}
