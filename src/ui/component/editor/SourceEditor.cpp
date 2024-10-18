#include "SourceEditor.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreCallbacks.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SourceEditor::SourceEditor()
	: FlowComponent(TRANS("Resource Editor")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forEditor());

	/** Switch Bar */
	auto switchCallback = [this](SourceSwitchBar::SwitchState state) {
		this->switchEditor(state);
		};
	this->switchBar = std::make_unique<SourceSwitchBar>(switchCallback);
	this->addAndMakeVisible(this->switchBar.get());

	/** Editors */
	this->midiEditor = std::make_unique<MIDISourceEditor>();
	this->addChildComponent(this->midiEditor.get());
	this->audioEditor = std::make_unique<AudioSourceEditor>();
	this->addChildComponent(this->audioEditor.get());

	/** Empty Str */
	this->emptyStr = TRANS("Please select a data source for editing.");

	/** Callback */
	CoreCallbacks::getInstance()->addSourceChanged(
		[comp = SourceEditor::SafePointer(this)](int trackIndex) {
			if (comp) {
				comp->update(trackIndex);
			}
		}
	);
	CoreCallbacks::getInstance()->addTrackChanged(
		[comp = SourceEditor::SafePointer(this)](int trackIndex) {
			if (comp) {
				comp->update(trackIndex);
			}
		}
	);
	CoreCallbacks::getInstance()->addEditingTrackChanged(
		[comp = SourceEditor::SafePointer(this)](int trackIndex) {
			if (comp) {
				comp->setTrack(trackIndex);
			}
		}
	);
}

void SourceEditor::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int switchBarHeight = screenSize.getHeight() * 0.03;

	/** Switch Bar */
	juce::Rectangle<int> switchBarRect(
		0, 0, this->getWidth(), switchBarHeight);
	this->switchBar->setBounds(switchBarRect);

	/** Editors */
	juce::Rectangle<int> contentRect = this->getLocalBounds().withTrimmedTop(switchBarHeight);
	this->midiEditor->setBounds(contentRect);
	this->audioEditor->setBounds(contentRect);
}

void SourceEditor::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int switchBarHeight = screenSize.getHeight() * 0.03;

	float fontHeight = screenSize.getHeight() * 0.02;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(fontHeight);

	/** Background */
	g.fillAll(backgroundColor);

	/** Empty Text */
	juce::Rectangle<int> contentRect = this->getLocalBounds().withTrimmedTop(switchBarHeight);
	g.setFont(textFont);
	g.setColour(textColor);
	g.drawFittedText(this->emptyStr, contentRect,
		juce::Justification::centred, 1);
}

void SourceEditor::update() {
	this->update(this->audioRef, this->midiRef);
}

void SourceEditor::setTrack(int trackIndex) {
	this->trackIndex = trackIndex;
	auto audioRef = quickAPI::getSeqTrackAudioRef(trackIndex);
	auto midiRef = quickAPI::getSeqTrackMIDIRef(trackIndex);
	this->update(audioRef, midiRef);
}

void SourceEditor::update(int trackIndex) {
	if ((trackIndex == this->trackIndex) || (trackIndex < 0)) {
		this->setTrack(this->trackIndex);
	}
}

void SourceEditor::update(uint64_t audioRef, uint64_t midiRef) {
	/** Check Source Ref Vaild */
	if (!quickAPI::isAudioSourceValid(audioRef)) {
		audioRef = 0;
	}
	if (!quickAPI::isMIDISourceValid(midiRef)) {
		midiRef = 0;
	}

	/** Update Refs */
	this->audioRef = audioRef;
	this->midiRef = midiRef;
	this->switchBar->update(this->trackIndex, audioRef, midiRef);
	this->midiEditor->update(midiRef);
	this->audioEditor->update(audioRef);
}

void SourceEditor::switchEditor(SourceSwitchBar::SwitchState state) {
	/** Update Switch State */
	this->switchState = state;

	/** Create Source */
	if ((this->audioRef == 0) && (state == SourceSwitchBar::SwitchState::Audio)) {
		CoreActions::createSeqAudioSourceGUI(this->trackIndex);
	}
	if ((this->midiRef == 0) && (state == SourceSwitchBar::SwitchState::MIDI)) {
		CoreActions::createSeqMIDISourceGUI(this->trackIndex);
	}

	/** Change Editor Visible */
	this->midiEditor->setVisible(state == SourceSwitchBar::SwitchState::MIDI);
	this->audioEditor->setVisible(state == SourceSwitchBar::SwitchState::Audio);
}
