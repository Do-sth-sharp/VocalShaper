#include "AudioDebugger.h"
#include "AudioCore.h"
#include "AudioCommand.h"

AudioDebugger::AudioDebugger(AudioCore* parent)
	: parent(parent) {
	jassert(parent);

	this->setOpaque(true);

	/** Device Selector */
	this->deviceSelector = std::make_unique<juce::AudioDeviceSelectorComponent>(
		*(parent->audioDeviceManager.get()), 0, 1024, 2, 1024,
		true, true, false, false);
	this->addAndMakeVisible(this->deviceSelector.get());

	/** Command */
	this->commandOutput = std::make_unique<juce::TextEditor>();
	this->commandOutput->setMultiLine(true);
	this->commandOutput->setReadOnly(true);
	this->commandOutput->setTabKeyUsedAsCharacter(true);
	this->commandOutput->setScrollToShowCursor(true);
	this->addAndMakeVisible(this->commandOutput.get());

	this->commandInput = std::make_unique<juce::TextEditor>();
	this->commandInput->setMultiLine(false);
	this->commandInput->setTabKeyUsedAsCharacter(true);
	this->commandInput->setScrollToShowCursor(true);
	this->commandInput->setPopupMenuEnabled(false);
	this->commandInput->onReturnKey =
		[input = this->commandInput.get(),
		output = juce::Component::SafePointer(this->commandOutput.get())] {
		auto str = input->getText();
		input->clear();
		AudioCommand::getInstance()->processCommandAsync(str, [output](const AudioCommand::CommandResult& result) {
			if (output) {
				auto currentText = output->getText();
				output->moveCaretToEnd();
				if (currentText.isNotEmpty() && currentText.getLastCharacter() != '\n') {
					output->insertTextAtCaret("\n");
				}

				output->insertTextAtCaret(">>>" + std::get<1>(result) + "\n");
				if (std::get<0>(result)) {
					output->insertTextAtCaret(std::get<2>(result));
				}
				else {
					output->insertTextAtCaret("<<<FAILED!!!>>>\n");
					output->insertTextAtCaret(std::get<2>(result));
				}
				if (!output->isEmpty() && std::get<2>(result).getLastCharacter() != '\n') {
					output->insertTextAtCaret("\n");
				}
				output->insertTextAtCaret("\n");
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
