#include "SourceEditor.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreCallbacks.h"
#include "../../misc/CoreActions.h"
#include "../../misc/Tools.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

class EditingTrackListener final : public juce::ChangeListener {
public:
	EditingTrackListener() = delete;
	EditingTrackListener(SourceEditor* parent)
		: parent(parent) {
	};

	void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) override {
		int track = Tools::getInstance()->getEditingTrack();
		this->parent->setTrack(track);
	}

private:
	SourceEditor* const parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditingTrackListener)
};

SourceEditor::SourceEditor()
	: FlowComponent(TRANS("Resource Editor")) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::Editor));

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

	/** Track Listener */
	this->editingTrackListener = std::make_unique<EditingTrackListener>(this);
	Tools::getInstance()->addEditingTrackChangedListener(this->editingTrackListener.get());

	/** Empty Str */
	this->emptyStr = TRANS("Please select a data source for editing.");

	/** Callback */
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackAdded,
		[comp = SourceEditor::SafePointer(this)](int type, int index) {
			if ((type == (int)quickAPI::TrackType::Track) && comp) {
				int currentIndex = comp->getCurrentIndex();
				if (currentIndex >= 0 && index <= currentIndex) {
					comp->setTrack(currentIndex + 1);
				}
			}
		}
	);
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackRemoved,
		[comp = SourceEditor::SafePointer(this)](int type, int index) {
			if ((type == (int)quickAPI::TrackType::Track) && comp) {
				int currentIndex = comp->getCurrentIndex();
				if (currentIndex >= 0) {
					if (index < currentIndex) {
						comp->setTrack(currentIndex - 1);
					}
					else if (index == currentIndex) {
						comp->setTrack(-1);
					}
				}
			}
		}
	);
	CoreCallbackAPI<int>::add(CoreCallbacks::CallbackType::TrackDataRefChanged,
		[comp = SourceEditor::SafePointer(this)](int trackIndex) {
			if (comp) {
				comp->update(trackIndex);
			}
		}
	);
	CoreCallbackAPI<int>::add(CoreCallbacks::CallbackType::TrackSourceChanged,
		[comp = SourceEditor::SafePointer(this)](int trackIndex) {
			if (comp) {
				comp->updateData(trackIndex);
			}
		}
	);
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackBlockChanged,
		[comp = SourceEditor::SafePointer(this)](int track, int /*index*/) {
			if (comp) {
				comp->updateBlocks(track);
			}
		}
	);
	CoreCallbackAPI<int, int>::add(CoreCallbacks::CallbackType::TrackInfoChanged,
		[comp = SourceEditor::SafePointer(this)](int type, int trackIndex) {
			if ((type == (int)quickAPI::TrackType::Track) && comp) {
				comp->update(trackIndex);
			}
		}
	);
	CoreCallbackAPI<void>::add(CoreCallbacks::CallbackType::TempoChanged,
		[comp = SourceEditor::SafePointer(this)] {
			if (comp) {
				comp->updateTempo();
			}
		}
	);
	CoreCallbackAPI<void>::add(CoreCallbacks::CallbackType::GraphUpdated,
		[comp = SourceEditor::SafePointer(this)]() {
			if (comp) {
				comp->setTrack(-1);
			}
		});
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
	juce::Font textFont(juce::FontOptions{ fontHeight });

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
	auto audioRef = quickAPI::getTrackAudioRef(
		{ quickAPI::TrackType::Track, trackIndex });
	auto midiRef = quickAPI::getTrackMIDIRef(
		{ quickAPI::TrackType::Track, trackIndex });
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

	/** Update Blocks */
	this->updateBlocks(this->trackIndex);

	/** Update Refs */
	this->audioRef = audioRef;
	this->midiRef = midiRef;
	this->switchBar->update(this->trackIndex, audioRef, midiRef);
	this->midiEditor->update(this->trackIndex, midiRef);
	this->audioEditor->update(this->trackIndex, audioRef);
}

void SourceEditor::updateTempo() {
	this->midiEditor->updateTempo();
	//this->audioEditor->updateTempo();
}

void SourceEditor::updateBlocks(int /*trackIndex*/) {
	this->midiEditor->updateBlocks();
	//this->audioEditor->updateBlocks();
}

void SourceEditor::updateData(int trackIndex) {
	if (trackIndex == this->trackIndex) {
		this->midiEditor->updateData();
		//this->audioEditor->updateData();
	}
}

int SourceEditor::getCurrentIndex() const {
	return this->trackIndex;
}

void SourceEditor::switchEditor(SourceSwitchBar::SwitchState state) {
	/** Update Switch State */
	this->switchState = state;

	/** Canceled Callback */
	auto cancelCallback = [comp = SourceEditor::SafePointer{ this }](int index) {
		if (comp) {
			comp->update(index);
		}
		};

	/** Create Source */
	if ((this->audioRef == 0) && (state == SourceSwitchBar::SwitchState::Audio)) {
		CoreActions::createTrackAudioSourceGUI(this->trackIndex, cancelCallback);
	}
	if ((this->midiRef == 0) && (state == SourceSwitchBar::SwitchState::MIDI)) {
		CoreActions::createTrackMIDISourceGUI(this->trackIndex, cancelCallback);
	}

	/** Change Editor Visible */
	this->midiEditor->setVisible(state == SourceSwitchBar::SwitchState::MIDI);
	this->audioEditor->setVisible(state == SourceSwitchBar::SwitchState::Audio);
}
