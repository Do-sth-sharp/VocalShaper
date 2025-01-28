#include "MixerTrackComponent.h"
#include "../../misc/CoreActions.h"
#include "../../misc/DragSourceType.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

MixerTrackComponent::MixerTrackComponent() {
	/** Side Chain */
	this->sideChain = std::make_unique<SideChainComponent>();
	this->addAndMakeVisible(this->sideChain.get());
	
	/** Input Icon */
	this->audioInputIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "login-circle-line").getFullPathName());
	this->audioInputIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->audioInputIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "login-circle-line").getFullPathName());
	this->audioInputIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	this->midiInputIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "login-box-line").getFullPathName());
	this->midiInputIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->midiInputIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("System", "login-box-line").getFullPathName());
	this->midiInputIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Input Button */
	this->audioInputButton = std::make_unique<juce::DrawableButton>(
		TRANS("Audio Input"), juce::DrawableButton::ImageOnButtonBackground);
	this->audioInputButton->setImages(
		this->audioInputIcon.get(), nullptr, nullptr, nullptr,
		this->audioInputIconOn.get(), nullptr, nullptr, nullptr);
	this->audioInputButton->setWantsKeyboardFocus(false);
	this->audioInputButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->audioInputButton->onClick = [this] { this->changeMIDIInput(); };
	this->addAndMakeVisible(this->audioInputButton.get());

	this->midiInputButton = std::make_unique<juce::DrawableButton>(
		TRANS("MIDI Input"), juce::DrawableButton::ImageOnButtonBackground);
	this->midiInputButton->setImages(
		this->midiInputIcon.get(), nullptr, nullptr, nullptr,
		this->midiInputIconOn.get(), nullptr, nullptr, nullptr);
	this->midiInputButton->setWantsKeyboardFocus(false);
	this->midiInputButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->midiInputButton->onClick = [this] { this->changeAudioInput(); };
	this->addAndMakeVisible(this->midiInputButton.get());

	/** Send */
	this->midiSendListModel = std::make_unique<MIDISendListModel>();
	this->midiSendList = std::make_unique<juce::ListBox>(
		TRANS("MIDI Send List"), this->midiSendListModel.get());
	this->midiSendList->setWantsKeyboardFocus(false);
	this->addAndMakeVisible(this->midiSendList.get());

	this->audioSendListModel = std::make_unique<AudioSendListModel>();
	this->audioSendList = std::make_unique<juce::ListBox>(
		TRANS("Audio Send List"), this->audioSendListModel.get());
	this->audioSendList->setWantsKeyboardFocus(false);
	this->addAndMakeVisible(this->audioSendList.get());

	/** Knob */
	this->gainKnob = std::make_unique<KnobBase>(
		TRANS("Gain"), 0, -10, 10, 1);
	this->gainKnob->onChange = [this](double value) {
		CoreActions::setTrackGain(
			(quickAPI::TrackType)this->type, this->index, (float)value);
		};
	this->addAndMakeVisible(this->gainKnob.get());

	this->panKnob = std::make_unique<KnobBase>(
		TRANS("Pan"), 0, -1, 1, 2);
	this->panKnob->onChange = [this](double value) {
		CoreActions::setTrackPan(
			(quickAPI::TrackType)this->type, this->index, (float)value);
		};
	this->addAndMakeVisible(this->panKnob.get());

	/** Fader */
	this->fader = std::make_unique<FaderBase>(
		1.0, juce::Array<double>{ 6.0, 0.0, -5.0, -10.0, -15.0, -20.0, -30.0, -40.0, -60.0, -100.0 },
		0.0, 2.0, 0);
	this->fader->onChange = [this](double value) {
		CoreActions::setTrackFader(
			(quickAPI::TrackType)this->type, this->index, (float)value);
		};
	this->addAndMakeVisible(this->fader.get());

	/** Level Meter */
	this->levelMeter = std::make_unique<MixerTrackLevelMeter>();
	this->addAndMakeVisible(this->levelMeter.get());

	/** Mute */
	this->muteButton = std::make_unique<MixerTrackMuteComponent>(
		[this] { this->showMenu(); });
	this->addAndMakeVisible(this->muteButton.get());

	/** Effect List */
	this->effectListModel = std::make_unique<EffectListModel>();
	this->effectList = std::make_unique<juce::ListBox>(
		TRANS("Fx List"), this->effectListModel.get());
	this->effectList->setWantsKeyboardFocus(false);
	this->addAndMakeVisible(this->effectList.get());

	/** Focus */
	this->setWantsKeyboardFocus(true);
	this->setMouseClickGrabsKeyboardFocus(true);
}

void MixerTrackComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int splitHeight = screenSize.getHeight() * 0.01;

	int colorHeight = screenSize.getHeight() * 0.015;
	int sideChainHeight = screenSize.getHeight() * 0.02;
	int inputButtonHeight = screenSize.getHeight() * 0.02;
	int inputButtonWidth = inputButtonHeight;
	int knobPaddingWidth = screenSize.getWidth() * 0.0025;
	int knobHeight = screenSize.getHeight() * 0.075;
	int knobWidth = (this->getWidth() - knobPaddingWidth * 2) / (this->panValid ? 2 : 1);
	int faderPaddingWidth = screenSize.getWidth() * 0.0025;
	int faderMinHeight = screenSize.getHeight() * 0.25;
	int faderWidth = (this->getWidth() - faderPaddingWidth * 2) / 2;
	int muteSoloHeight = screenSize.getHeight() * 0.03;
	int muteSoloWidth = muteSoloHeight;

	int listItemHeight = screenSize.getHeight() * 0.02;
	int effectListMaxHeight = listItemHeight * quickAPI::getEffectSlotNum();
	int effectListMinHeight = listItemHeight * 2;
	int midiSendListMaxHeight = listItemHeight * quickAPI::getTrackMIDISendSlotNum();
	int midiSendListMinHeight = listItemHeight * 1;
	int audioSendListMaxHeight = listItemHeight * quickAPI::getTrackAudioSendSlotNum();
	int audioSendListMinHeight = listItemHeight * 1;

	int ioHideHeight = colorHeight + sideChainHeight + splitHeight
		+ inputButtonHeight + splitHeight + knobHeight + splitHeight
		+ effectListMinHeight + splitHeight + muteSoloHeight + splitHeight
		+ midiSendListMinHeight + splitHeight + audioSendListMinHeight + splitHeight
		+ faderMinHeight;
	int controlHideHeight = colorHeight + splitHeight
		+ knobHeight + splitHeight
		+ effectListMinHeight + splitHeight + muteSoloHeight + splitHeight
		+ faderMinHeight;
	int effectListHideHeight = colorHeight + splitHeight
		+ effectListMinHeight + splitHeight
		+ faderMinHeight;
	int faderHideHeight = colorHeight + splitHeight
		+ faderMinHeight;

	bool ioShown = this->getHeight() >= ioHideHeight;
	bool controlShown = this->getHeight() >= controlHideHeight;
	bool effectShown = this->getHeight() >= effectListHideHeight;
	bool faderShown = this->getHeight() >= faderHideHeight;

	int expandBaseHeight = colorHeight + sideChainHeight + splitHeight
		+ inputButtonHeight + splitHeight + knobHeight + splitHeight
		+ splitHeight + muteSoloHeight + splitHeight
		+ splitHeight + splitHeight
		+ faderMinHeight;

	int expandStartHeight = ioHideHeight;
	int effectExpandMaxHeight = expandBaseHeight + midiSendListMinHeight + audioSendListMinHeight + effectListMaxHeight;
	int midiSendExpandMaxHeight = expandBaseHeight + effectListMaxHeight + midiSendListMaxHeight * 2;
	int audioSendExpandMaxHeight = expandBaseHeight + effectListMaxHeight + midiSendListMaxHeight + audioSendExpandMaxHeight;

	int expandLevel = 0;
	if (this->getHeight() > expandStartHeight) {
		expandLevel++;
	}
	if (this->getHeight() > effectExpandMaxHeight) {
		expandLevel++;
	}
	if (this->getHeight() > midiSendExpandMaxHeight) {
		expandLevel++;
	}
	if (this->getHeight() > audioSendExpandMaxHeight) {
		expandLevel++;
	}

	float outlineThickness = screenSize.getHeight() * 0.00125;

	int top = 0;
	top += colorHeight;

	/** Side Chain */
	if (ioShown) {
		juce::Rectangle<int> sideChainRect(
			0, top, this->getWidth(), sideChainHeight);
		this->sideChain->setBounds(sideChainRect);

		top += sideChainHeight;
	}
	this->sideChain->setVisible(ioShown);

	top += splitHeight;

	/** Input */
	if (ioShown) {
		int splitWidth = (this->getWidth() - inputButtonWidth * 2) / 3;
		juce::Rectangle<int> midiRect(
			splitWidth, top,
			inputButtonWidth, inputButtonHeight);
		this->midiInputButton->setBounds(midiRect);

		juce::Rectangle<int> audioRect(
			midiRect.getRight() + splitWidth, top,
			inputButtonWidth, inputButtonHeight);
		this->audioInputButton->setBounds(audioRect);

		top += inputButtonHeight;
		top += splitHeight;
	}
	this->midiInputButton->setVisible(ioShown);
	this->audioInputButton->setVisible(ioShown);

	/** Knob */
	if (controlShown) {
		juce::Rectangle<int> gainRect(
			knobPaddingWidth, top,
			knobWidth, knobHeight);
		this->gainKnob->setBounds(gainRect);

		if (this->panValid) {
			juce::Rectangle<int> panRect(
				this->getWidth() - knobPaddingWidth - knobWidth, top,
				knobWidth, knobHeight);
			this->panKnob->setBounds(panRect);
		}

		top += knobHeight;
		top += splitHeight;
	}
	this->gainKnob->setVisible(controlShown);
	this->panKnob->setVisible(controlShown && this->panValid);

	/** Effect */
	if (effectShown) {
		int effectHeight = effectListMinHeight;
		if (expandLevel > 1) {
			effectHeight = effectListMaxHeight;
		}
		else if (expandLevel == 1) {
			effectHeight = this->getHeight() - expandBaseHeight
				- midiSendListMinHeight - audioSendListMinHeight;
		}

		juce::Rectangle<int> effectRect(
			outlineThickness, top,
			this->getWidth() - outlineThickness * 2, effectHeight);
		this->effectList->setBounds(effectRect);
		this->effectList->setRowHeight(listItemHeight);

		top += effectHeight;
		top += splitHeight;
	}
	this->effectList->setVisible(effectShown);

	/** Mute Solo */
	bool hasSoloButton = this->type == (int)quickAPI::TrackType::Track;
	if (controlShown) {
		int splitWidth = (this->getWidth() - muteSoloWidth * 2) / 3;
		juce::Rectangle<int> muteRect(
			hasSoloButton ? splitWidth
			: (this->getWidth() - muteSoloWidth) / 2, top,
			muteSoloWidth, muteSoloHeight);
		this->muteButton->setBounds(muteRect);

		juce::Rectangle<int> soloRect(
			muteRect.getRight() + splitWidth, top,
			muteSoloWidth, muteSoloHeight);
		this->soloButton->setBounds(soloRect);

		top += muteSoloHeight;
		top += splitHeight;
	}
	this->muteButton->setVisible(controlShown);
	this->soloButton->setVisible(controlShown && hasSoloButton);

	/** MIDI Send */
	if (ioShown) {
		int midiSendHeight = midiSendListMinHeight;
		if (expandLevel > 2) {
			midiSendHeight = midiSendListMaxHeight;
		}
		else if (expandLevel == 2) {
			midiSendHeight = (this->getHeight() - expandBaseHeight
				- effectListMaxHeight) / 2;
		}

		juce::Rectangle<int> midiSendRect(
			0, top, this->getWidth(), midiSendHeight);
		this->midiSendList->setBounds(midiSendRect);
		this->midiSendList->setRowHeight(listItemHeight);

		top += midiSendHeight;
		top += splitHeight;
	}
	this->midiSendList->setVisible(ioShown);

	/** Audio Send */
	if (ioShown) {
		int audioSendHeight = audioSendListMinHeight;
		if (expandLevel > 3) {
			audioSendHeight = audioSendListMaxHeight;
		}
		else if (expandLevel == 3) {
			audioSendHeight = this->getHeight() - expandBaseHeight
				- effectListMaxHeight - midiSendListMaxHeight;
		}

		juce::Rectangle<int> audioSendRect(
			0, top, this->getWidth(), audioSendHeight);
		this->audioSendList->setBounds(audioSendRect);
		this->audioSendList->setRowHeight(listItemHeight);

		top += audioSendHeight;
		top += splitHeight;
	}
	this->audioSendList->setVisible(ioShown);

	/** Fader And Level Meter */
	if (faderShown) {
		juce::Rectangle<int> faderRect(
			faderPaddingWidth, top,
			faderWidth, this->getHeight() - top);
		this->fader->setBounds(faderRect);

		juce::Rectangle<int> levelRect(
			this->getWidth() - faderPaddingWidth - faderWidth, top,
			faderWidth, this->getHeight() - top);
		this->levelMeter->setBounds(levelRect);
	}
	this->fader->setVisible(faderShown);
	this->levelMeter->setVisible(faderShown);
}

void MixerTrackComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int colorHeight = screenSize.getHeight() * 0.015;
	int titleBorderWidth = screenSize.getWidth() * 0.00125;

	float titleFontHeight = screenSize.getHeight() * 0.0125;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);

	/** Font */
	juce::Font titleFont(juce::FontOptions{ titleFontHeight });

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Track Color */
	auto totalRect = this->getLocalBounds();
	auto colorRect = totalRect.withHeight(colorHeight);
	g.setColour(this->trackColor);
	g.fillRect(colorRect);

	/** Title */
	juce::String title = juce::String{ this->index };
	if (this->name.isNotEmpty()) {
		title += (" - " + this->name);
	}
	g.setColour(this->nameColor);
	g.setFont(titleFont);
	g.drawFittedText(title,
		colorRect.withTrimmedLeft(titleBorderWidth).withTrimmedRight(titleBorderWidth),
		juce::Justification::centred, 1, 1.f);
}

void MixerTrackComponent::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float outlineThickness = screenSize.getHeight() * 0.00125;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(
		this->dragHovered || this->hasKeyboardFocus(true)
		? juce::Label::ColourIds::outlineWhenEditingColourId
		: juce::Label::ColourIds::outlineColourId);

	/** Effect Rect */
	auto effectRect = this->effectList->getBounds();
	g.setColour(backgroundColor);
	g.drawRect(effectRect, outlineThickness);

	/** Outline */
	auto totalRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(totalRect, outlineThickness);
}

void MixerTrackComponent::updateIndex(int type, int index) {
	this->type = type;
	this->index = index;

	this->sideChain->updateIndex(type, index);

	this->midiSendListModel->updateIndex(type, index);
	this->midiSendList->updateContent();
	this->audioSendListModel->updateIndex(type, index);
	this->audioSendList->updateContent();

	this->muteButton->updateIndex(type, index);
	this->soloButton->updateIndex(type, index);

	this->levelMeter->updateIndex(type, index);

	this->effectListModel->updateIndex(type, index);
	this->effectList->updateContent();
}

void MixerTrackComponent::updateInfo() {
	this->name = quickAPI::getTrackName({ (quickAPI::TrackType)this->type, index });
	this->trackColor = quickAPI::getTrackColor({ (quickAPI::TrackType)this->type, index });

	auto& laf = this->getLookAndFeel();
	auto textColorLight = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	auto textColorDark = laf.findColour(
		juce::Label::ColourIds::textColourId);
	this->nameColor = utils::chooseTextColor(this->trackColor, textColorLight, textColorDark);

	this->repaint();
}

void MixerTrackComponent::updateSideChain() {
	this->sideChain->update();
}

void MixerTrackComponent::updateInput() {
	this->audioInputButton->setToggleState(
		!(quickAPI::getTrackAudioInputChannels(
			{ (quickAPI::TrackType)this->type, this->index }).empty()),
		juce::NotificationType::dontSendNotification);
	this->midiInputButton->setToggleState(
		quickAPI::isTrackMIDIInputConnected(
			{ (quickAPI::TrackType)this->type, this->index }),
		juce::NotificationType::dontSendNotification);
}

void MixerTrackComponent::updateSend() {
	this->midiSendList->updateContent();
	this->audioSendList->updateContent();
}

void MixerTrackComponent::updateGain() {
	this->gainKnob->setValue(
		quickAPI::getTrackGain({ (quickAPI::TrackType)this->type, index }));
}

void MixerTrackComponent::updatePan() {
	this->panKnob->setValue(
		quickAPI::getTrackPan({ (quickAPI::TrackType)this->type, index }));
	this->panValid = quickAPI::isTrackPanValid({ (quickAPI::TrackType)this->type, index });

	/** Update Panner State */
	this->resized();
}

void MixerTrackComponent::updateFader() {
	this->fader->setValue(
		quickAPI::getTrackFader({ (quickAPI::TrackType)this->type, index }));
}

void MixerTrackComponent::updateMute() {
	this->muteButton->update();
	this->soloButton->update();
}

void MixerTrackComponent::updateEffect(int index) {
	this->effectList->updateContent();
}

void MixerTrackComponent::updateEffectIndex(int oldIndex, int newIndex) {
	this->effectList->updateContent();
}

void MixerTrackComponent::mouseMove(const juce::MouseEvent& event) {
	auto screenSize = utils::getScreenSize(this);
	int colorHeight = screenSize.getHeight() * 0.015;
	int y = event.position.getY();

	if (y >= 0 && y < colorHeight) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	}
	else {
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
	}
}

void MixerTrackComponent::mouseUp(const juce::MouseEvent& event) {
	auto screenSize = utils::getScreenSize(this);
	int colorHeight = screenSize.getHeight() * 0.015;
	int y = event.position.getY();

	if (event.mods.isRightButtonDown()) {
		if (y >= 0 && y < colorHeight) {
			CoreActions::setTrackColorGUI(
				(quickAPI::TrackType)this->type, this->index);
		}
		else {
			this->showMenu();
		}
	}
	else if (event.mods.isLeftButtonDown()) {
		if (y >= 0 && y < colorHeight) {
			CoreActions::setTrackNameGUI(
				(quickAPI::TrackType)this->type, this->index);
		}
	}
}

bool MixerTrackComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Track Audio Send */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioSend)) {
		if (this->type == (int)quickAPI::TrackType::Track) {
			return false;
		}

		int trackType = des["trackType"];
		if (trackType == (int)quickAPI::TrackType::MasterTrack) {
			return false;
		}

		int trackIndex = des["track"];
		return (trackIndex >= 0) &&
			(!(trackIndex == this->index && trackType == this->type));
	}

	/** From Track MIDI Send */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackMidiSend)) {
		if (this->type == (int)quickAPI::TrackType::Track) {
			return false;
		}

		int trackType = des["trackType"];
		if (trackType == (int)quickAPI::TrackType::MasterTrack) {
			return false;
		}

		int trackIndex = des["track"];
		return (trackIndex >= 0) &&
			(!(trackIndex == this->index && trackType == this->type));
	}

	return false;
}

void MixerTrackComponent::itemDragEnter(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->preDrop();
}

void MixerTrackComponent::itemDragExit(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();
}

void MixerTrackComponent::itemDropped(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	auto& des = dragSourceDetails.description;
	this->endDrop();

	/** From Track Audio Send */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioSend)) {
		int trackType = des["trackType"];
		int trackIndex = des["track"];
		int slot = des["slot"];

		CoreActions::setTrackAudioSendGUI(
			(quickAPI::TrackType)trackType, trackIndex, slot,
			{ (this->type == (int)quickAPI::TrackType::MasterTrack)
			? quickAPI::SendDstType::ToMaster : quickAPI::SendDstType::ToAUX, this->index });
		return;
	}

	/** From Track MIDI Send */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackMidiSend)) {
		int trackType = des["trackType"];
		int trackIndex = des["track"];
		int slot = des["slot"];

		CoreActions::setTrackMIDISendGUI(
			(quickAPI::TrackType)trackType, trackIndex, slot,
			{ (this->type == (int)quickAPI::TrackType::MasterTrack)
			? quickAPI::SendDstType::ToMaster : quickAPI::SendDstType::ToAUX, this->index });
		return;
	}
}

void MixerTrackComponent::focusGained(FocusChangeType cause) {
	this->repaint();
}

void MixerTrackComponent::focusLost(FocusChangeType cause) {
	this->repaint();
}

void MixerTrackComponent::changeAudioInput() {
	CoreActions::setTrackAudioInputGUI(
		(quickAPI::TrackType)this->type, this->index);
}

void MixerTrackComponent::changeMIDIInput() {
	if (this->midiInputButton->getToggleState()) {
		CoreActions::removeTrackMIDIInput(
			(quickAPI::TrackType)this->type, this->index);
	}
	else {
		CoreActions::addTrackMIDIInput(
			(quickAPI::TrackType)this->type, this->index);
	}
}

void MixerTrackComponent::preDrop() {
	this->dragHovered = true;
	this->repaint();
}

void MixerTrackComponent::endDrop() {
	this->dragHovered = false;
	this->repaint();
}

enum MixerTrackActionType {
	Add = 1, Remove
};

void MixerTrackComponent::showMenu() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case MixerTrackActionType::Add: {
		this->add();
		break;
	}
	case MixerTrackActionType::Remove: {
		this->remove();
		break;
	}
	}
}

void MixerTrackComponent::add() {
	CoreActions::insertTrackGUI(
		(quickAPI::TrackType)this->type, this->index + 1);
}

void MixerTrackComponent::remove() {
	CoreActions::removeTrackGUI(
		(quickAPI::TrackType)this->type, this->index);
}

int MixerTrackComponent::getInsertIndex(const juce::Point<int>& pos) {
	auto listRect = this->effectList->getBounds();
	if (listRect.contains(pos)) {
		auto posRel = pos - listRect.getTopLeft();
		return this->effectList->
			getInsertionIndexForPosition(posRel.getX(), posRel.getY());
	}
	else {
		return -1;
	}
}

juce::PopupMenu MixerTrackComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(MixerTrackActionType::Add, TRANS("Add"));
	menu.addItem(MixerTrackActionType::Remove, TRANS("Remove"));

	return menu;
}
