#include "SeqTrackComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../misc/PluginEditorHub.h"
#include "../../misc/DragSourceType.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

SeqTrackComponent::SeqTrackComponent(
	const DragStartFunc& dragStartFunc,
	const DragProcessFunc& dragProcessFunc,
	const DragEndFunc& dragEndFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSeqTrack());

	/** Track Name Buton */
	this->trackName = std::make_unique<juce::TextButton>("0 - " + TRANS("Untitled"));
	this->trackName->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSeqTrackName());
	this->trackName->setWantsKeyboardFocus(false);
	this->trackName->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->trackName->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->trackName->onClick = [this] {
		this->editTrackName();
		};
	this->addAndMakeVisible(this->trackName.get());

	/** Mute Button */
	this->muteButton = std::make_unique<SeqTrackMuteComponent>();
	this->addAndMakeVisible(this->muteButton.get());

	/** Record Button */
	this->recButton = std::make_unique<SeqTrackRecComponent>();
	this->addAndMakeVisible(this->recButton.get());

	/** Instr Button */
	this->instrButton = std::make_unique<RightClickableTextButton>("-",
		[this] { this->instrEditorShow(); },
		[this] { this->instrMenuShow(); });
	this->instrButton->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forInstrName());
	this->instrButton->setWantsKeyboardFocus(false);
	this->instrButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->instrButton->setConnectedEdges(juce::Button::ConnectedOnRight);
	this->addChildComponent(this->instrButton.get());

	/** Instr Bypass Icon */
	this->instrBypassIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Device", "shut-down-line").getFullPathName());
	this->instrBypassIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->instrBypassIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Device", "shut-down-line").getFullPathName());
	this->instrBypassIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Instr Offline Icon */
	this->instrOfflineIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Editor", "link-unlink").getFullPathName());
	this->instrOfflineIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	this->instrOfflineIconOn = flowUI::IconManager::getSVG(
		utils::getIconFile("Editor", "link-unlink").getFullPathName());
	this->instrOfflineIconOn->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));

	/** Instr Bypass Button */
	this->instrBypassButton = std::make_unique<juce::DrawableButton>(
		TRANS("Bypass Instrument"), juce::DrawableButton::ImageOnButtonBackground);
	this->instrBypassButton->setImages(
		this->instrBypassIcon.get(), nullptr, nullptr, nullptr,
		this->instrBypassIconOn.get(), nullptr, nullptr, nullptr);
	this->instrBypassButton->setWantsKeyboardFocus(false);
	this->instrBypassButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->instrBypassButton->setConnectedEdges(juce::Button::ConnectedOnLeft);
	this->instrBypassButton->onClick = [this] { this->instrBypass(); };
	this->addChildComponent(this->instrBypassButton.get());

	/** Instr Offline Button */
	this->instrOfflineButton = std::make_unique<juce::DrawableButton>(
		TRANS("Instrument Offline"), juce::DrawableButton::ImageOnButtonBackground);
	this->instrOfflineButton->setImages(
		this->instrOfflineIcon.get(), nullptr, nullptr, nullptr,
		this->instrOfflineIconOn.get(), nullptr, nullptr, nullptr);
	this->instrOfflineButton->setWantsKeyboardFocus(false);
	this->instrOfflineButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->instrOfflineButton->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->instrOfflineButton->onClick = [this] { this->instrOffline(); };
	this->addChildComponent(this->instrOfflineButton.get());

	/** IO */
	this->midiOutput = std::make_unique<SeqTrackIOComponent>(true);
	this->addChildComponent(this->midiOutput.get());

	this->audioOutput = std::make_unique<SeqTrackIOComponent>(false);
	this->addChildComponent(this->audioOutput.get());

	/** Level Meter */
	this->levelMeter = std::make_unique<SeqTrackLevelMeter>();
	this->addChildComponent(this->levelMeter.get());

	/** Content */
	this->content = std::make_unique<SeqTrackContentViewer>(
		dragStartFunc, dragProcessFunc, dragEndFunc);
	this->addAndMakeVisible(this->content.get());
}

void SeqTrackComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->trackColor = quickAPI::getSeqTrackColor(index);

		auto& laf = this->getLookAndFeel();
		if (utils::isLightColor(this->trackColor)) {
			this->idColor = laf.findColour(
				juce::Label::ColourIds::textWhenEditingColourId);
		}
		else {
			this->idColor = laf.findColour(
				juce::Label::ColourIds::textColourId);
		}

		auto name = quickAPI::getSeqTrackName(index);
		if (name.isEmpty()) {
			name = TRANS("Untitled");
		}
		this->trackName->setButtonText(juce::String{ index } + " - " + name);

		this->updateMute();
		this->updateRec();

		this->updateInstr();

		this->midiOutput->update(index);
		this->audioOutput->update(index);

		this->levelMeter->update(index);

		this->content->update(index);

		this->setTooltip(this->createToolTipString());

		this->repaint();
	}
}

void SeqTrackComponent::updateBlock(int blockIndex) {
	this->content->updateBlock(blockIndex);
}

void SeqTrackComponent::updateMute() {
	this->muteButton->update(this->index);
}

void SeqTrackComponent::updateRec() {
	this->recButton->update(this->index);
}

void SeqTrackComponent::updateInstr() {
	bool instrValid = quickAPI::isInstrValid(this->index);
	auto instrName = quickAPI::getInstrName(this->index);
	this->instrButton->setButtonText(
		instrValid ? instrName : "-");
	this->instrButton->setTooltip(
		instrValid ? instrName : "");
	this->instrButton->setToggleState(
		PluginEditorHub::getInstance()->checkInstr(this->index),
		juce::NotificationType::dontSendNotification);

	this->instrBypassButton->setToggleState(
		(!quickAPI::getInstrBypass(this->index)) && instrValid,
		juce::NotificationType::dontSendNotification);
	this->instrOfflineButton->setToggleState(
		(!quickAPI::getInstrOffline(this->index)) && instrValid,
		juce::NotificationType::dontSendNotification);
	this->instrBypassButton->setEnabled(instrValid);
	this->instrOfflineButton->setEnabled(instrValid);

	this->setTooltip(this->createToolTipString());
}

void SeqTrackComponent::updateHPos(double pos, double itemSize) {
	this->content->updateHPos(pos, itemSize);
}

void SeqTrackComponent::updateMixerTrack() {
	this->midiOutput->update(this->index);
	this->audioOutput->update(this->index);
}

void SeqTrackComponent::updateDataRef() {
	this->content->updateDataRef();
}

void SeqTrackComponent::updateData() {
	this->content->updateData();
}

void SeqTrackComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int headWidth = screenSize.getWidth() * 0.1;

	float trackColorWidth = screenSize.getWidth() * 0.005;

	int paddingHeight = screenSize.getHeight() * 0.015;
	int paddingWidth = screenSize.getWidth() * 0.0075;
	float lineThickness = screenSize.getHeight() * 0.0025;

	int nameWidth = screenSize.getWidth() * 0.065;
	int nameHeight = screenSize.getHeight() * 0.025;

	int muteButtonHeight = screenSize.getHeight() * 0.02;
	int buttonSplitWidth = screenSize.getWidth() * 0.0035;

	int compressModeHeight = paddingHeight * 2 + nameHeight;
	bool isCompressMode = this->getHeight() <= compressModeHeight;

	int lineSplitHeight = screenSize.getHeight() * 0.01;
	int instrLineHeight = screenSize.getHeight() * 0.0225;

	int contentLineSplitHeight = screenSize.getHeight() * 0.0075;

	int ioLineHeight = screenSize.getHeight() * 0.015;
	int ioLineSplitWidth = screenSize.getWidth() * 0.0025;

	int ioLineShownHeight = compressModeHeight + lineSplitHeight + ioLineHeight;
	bool isIOLineShown = this->getHeight() >= ioLineShownHeight;

	int instrLineShownHeight = ioLineShownHeight + contentLineSplitHeight + instrLineHeight;
	bool isInstrLineShown = this->getHeight() >= instrLineShownHeight;

	int topY = 0;

	/** Track Name */
	juce::Rectangle<int> nameRect(
		paddingWidth + trackColorWidth, isCompressMode ? lineThickness : paddingHeight,
		headWidth - paddingWidth * 2 - trackColorWidth - muteButtonHeight * 2 - buttonSplitWidth * 2,
		isCompressMode ? (this->getHeight() - lineThickness * 2) : nameHeight);
	this->trackName->setBounds(nameRect);

	/** Mute Button */
	juce::Rectangle<int> muteRect(
		nameRect.getRight() + buttonSplitWidth, nameRect.getCentreY() - muteButtonHeight / 2,
		muteButtonHeight, muteButtonHeight);
	this->muteButton->setBounds(muteRect);

	/** Record Button */
	juce::Rectangle<int> recRect(
		muteRect.getRight() + buttonSplitWidth, nameRect.getCentreY() - muteButtonHeight / 2,
		muteButtonHeight, muteButtonHeight);
	this->recButton->setBounds(recRect);

	topY = nameRect.getBottom();

	/** Instr Button */
	juce::Rectangle<int> instrLineRect(
		paddingWidth + trackColorWidth, topY + lineSplitHeight,
		headWidth - paddingWidth * 2 - trackColorWidth, instrLineHeight);
	auto instrRect = instrLineRect.withTrimmedRight(instrLineHeight * 2);
	this->instrButton->setBounds(instrRect);
	this->instrButton->setVisible(isInstrLineShown);

	/** Instr Offline Button */
	juce::Rectangle<int> instrOfflineRect(
		instrRect.getRight(), instrRect.getY(),
		instrLineHeight, instrLineHeight);
	this->instrOfflineButton->setBounds(instrOfflineRect);
	this->instrOfflineButton->setVisible(isInstrLineShown);

	/** Instr Bypass Button */
	juce::Rectangle<int> instrBypassRect(
		instrOfflineRect.getRight(), instrRect.getY(),
		instrLineHeight, instrLineHeight);
	this->instrBypassButton->setBounds(instrBypassRect);
	this->instrBypassButton->setVisible(isInstrLineShown);

	if (isInstrLineShown) {
		topY = instrLineRect.getBottom();
	}

	/** IO */
	juce::Rectangle<int> midiOutputRect(
		paddingWidth + trackColorWidth, topY + contentLineSplitHeight,
		ioLineHeight, ioLineHeight);
	this->midiOutput->setBounds(midiOutputRect);
	this->midiOutput->setVisible(isIOLineShown);

	juce::Rectangle<int> audioOutputRect(
		midiOutputRect.getRight() + ioLineSplitWidth, midiOutputRect.getY(),
		ioLineHeight, ioLineHeight);
	this->audioOutput->setBounds(audioOutputRect);
	this->audioOutput->setVisible(isIOLineShown);

	/** Level Meter */
	juce::Rectangle<int> levelRect(
		audioOutputRect.getRight() + ioLineSplitWidth, midiOutputRect.getY(),
		headWidth - paddingWidth - audioOutputRect.getRight() - ioLineSplitWidth, ioLineHeight);
	this->levelMeter->setBounds(levelRect);
	this->levelMeter->setVisible(isIOLineShown);

	if (isIOLineShown) {
		topY = midiOutputRect.getBottom();
	}

	/** Content */
	juce::Rectangle<int> contentRect(
		headWidth, 0, this->getWidth() - headWidth, this->getHeight());
	this->content->setBounds(contentRect);
	this->content->setCompressed(isCompressMode);
}

void SeqTrackComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float trackColorWidth = screenSize.getWidth() * 0.005;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Track Color */
	juce::Rectangle<float> trackColorRect(
		0, 0, trackColorWidth, this->getHeight());
	g.setColour(this->trackColor);
	g.fillRect(trackColorRect);
}

void SeqTrackComponent::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float outlineThickness = screenSize.getHeight() * 0.00125;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour outlineColor = laf.findColour(this->dragHovered
		? juce::Label::ColourIds::outlineWhenEditingColourId
		: juce::Label::ColourIds::outlineColourId);

	/** Outline */
	juce::Rectangle<float> outlineRect(
		0, 0, this->getWidth(), this->getHeight());
	g.setColour(outlineColor);
	g.drawRect(outlineRect, outlineThickness);
}

void SeqTrackComponent::mouseMove(const juce::MouseEvent& event) {
	auto screenSize = utils::getScreenSize(this);
	float trackColorWidth = screenSize.getWidth() * 0.005;
	float x = event.position.getX();

	if (x >= 0 && x < trackColorWidth) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	}
	else {
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
	}
}

void SeqTrackComponent::mouseUp(const juce::MouseEvent& event) {
	auto screenSize = utils::getScreenSize(this);
	float trackColorWidth = screenSize.getWidth() * 0.005;
	float x = event.position.getX();

	if (event.mods.isLeftButtonDown()) {
		if (x >= 0 && x < trackColorWidth) {
			CoreActions::setSeqColorGUI(this->index);
		}
	}
	else if (event.mods.isRightButtonDown()) {
		this->menuShow();
	}
}

bool SeqTrackComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Mixer Track Midi Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackMidiInput)) {
		int trackIndex = des["track"];
		return trackIndex >= 0;
	}

	/** From Mixer Track Audio Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioInput)) {
		int trackIndex = des["track"];
		return trackIndex >= 0;
	}

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		return des["instrument"];
	}

	return false;
}

void SeqTrackComponent::itemDragEnter(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }
	this->preDrop();
}

void SeqTrackComponent::itemDragExit(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }
	this->endDrop();
}

void SeqTrackComponent::itemDropped(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	auto& des = dragSourceDetails.description;
	this->endDrop();

	/** From Mixer Track Midi Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackMidiInput)) {
		int trackIndex = des["track"];

		this->midiOutput->setMidiOutputToMixer(trackIndex, true);
		return;
	}

	/** From Mixer Track Audio Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioInput)) {
		int trackIndex = des["track"];

		this->audioOutput->setAudioOutputToMixer(trackIndex, true);
		return;
	}

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		juce::String pid = des["id"].toString();

		this->setInstr(pid);
		return;
	}
}

void SeqTrackComponent::editTrackName() {
	CoreActions::setSeqNameGUI(this->index);
}

void SeqTrackComponent::instrEditorShow() {
	bool instrValid = quickAPI::isInstrValid(this->index);
	if (!instrValid) { return; }
	
	if (this->instrButton->getToggleState()) {
		PluginEditorHub::getInstance()->closeInstr(this->index);
	}
	else {
		PluginEditorHub::getInstance()->openInstr(this->index);
	}
}

void SeqTrackComponent::instrBypass() {
	CoreActions::bypassInstr(this->index,
		this->instrBypassButton->getToggleState());
}

void SeqTrackComponent::instrOffline() {
	CoreActions::offlineInstr(this->index,
		this->instrOfflineButton->getToggleState());
}

enum InstrMenuActionType {
	Bypass = 1, Offline, Remove
};

void SeqTrackComponent::instrMenuShow() {
	/** Callback */
	auto addCallback = [comp = juce::Component::SafePointer{ this }](const juce::PluginDescription& pluginDes) {
		if (comp) {
			comp->setInstr(pluginDes.createIdentifierString());
		}
		};

	/** Create Menu */
	auto menu = this->createInstrMenu(addCallback);
	int result = menu.show();

	switch (result) {
	case InstrMenuActionType::Bypass: {
		this->instrBypass();
		break;
	}
	case InstrMenuActionType::Offline: {
		this->instrOffline();
		break;
	}
	case InstrMenuActionType::Remove: {
		CoreActions::removeInstrGUI(this->index);
		break;
	}
	}
}

enum SeqMenuActionType {
	Add = 1, Remove1
};

void SeqTrackComponent::menuShow() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case SeqMenuActionType::Add: {
		this->add();
		break;
	}
	case SeqMenuActionType::Remove1: {
		this->remove();
		break;
	}
	}
}

void SeqTrackComponent::setInstr(const juce::String& pid) {
	CoreActions::insertInstr(this->index, pid);
}

void SeqTrackComponent::add() {
	CoreActions::insertSeqGUI(this->index + 1);
}

void SeqTrackComponent::remove() {
	CoreActions::removeSeqGUI(this->index);
}

void SeqTrackComponent::preDrop() {
	this->dragHovered = true;
	this->repaint();
}

void SeqTrackComponent::endDrop() {
	this->dragHovered = false;
	this->repaint();
}

juce::PopupMenu SeqTrackComponent::createInstrMenu(
	const std::function<void(const juce::PluginDescription&)>& addCallback) const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("Edit"), this->createInstrAddMenu(addCallback));
	menu.addItem(InstrMenuActionType::Bypass, TRANS("Bypass"), 
		this->instrBypassButton->isEnabled(),
		this->instrBypassButton->isEnabled() &&
		!(this->instrBypassButton->getToggleState()));
	menu.addItem(InstrMenuActionType::Offline, TRANS("Offline"),
		this->instrOfflineButton->isEnabled(),
		this->instrOfflineButton->isEnabled() && 
		!(this->instrOfflineButton->getToggleState()));
	menu.addItem(InstrMenuActionType::Remove, TRANS("Remove"),
		this->instrBypassButton->isEnabled());

	return menu;
}

juce::PopupMenu SeqTrackComponent::createInstrAddMenu(
	const std::function<void(const juce::PluginDescription&)>& callback) const {
	/** Create Menu */
	auto [valid, list] = quickAPI::getPluginList(true, true);
	if (!valid) { return juce::PopupMenu{}; }
	auto groups = utils::groupPlugin(list, utils::PluginGroupType::Category);
	return utils::createPluginMenu(groups, callback);
}

juce::PopupMenu SeqTrackComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(SeqMenuActionType::Add, TRANS("Add"));
	menu.addItem(SeqMenuActionType::Remove1, TRANS("Remove"));

	return menu;
}

juce::String SeqTrackComponent::createToolTipString() const {
	juce::String result;

	result += "#" + juce::String{ this->index } + " " + quickAPI::getSeqTrackName(this->index) + "\n";
	result += TRANS("Type:") + " " + quickAPI::getSeqTrackType(this->index) + "\n";

	if (quickAPI::isInstrValid(this->index)) {
		result += TRANS("Instrument:") + " " + quickAPI::getInstrName(this->index) + "\n";
		result += TRANS("Instrument Offline:") + " " + juce::String{ quickAPI::getInstrOffline(this->index) ? "ON" : "OFF" } + "\n";
		result += TRANS("Instrument Bypass:") + " " + juce::String{ quickAPI::getInstrBypass(this->index) ? "ON" : "OFF" } + "\n";
	}

	return result;
}
