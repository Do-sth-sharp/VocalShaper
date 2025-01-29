#include "SeqTrackComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../misc/PluginEditorHub.h"
#include "../../misc/DragSourceType.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

SeqTrackComponent::SeqTrackComponent(
	const ScrollFunc& scrollFunc,
	const WheelFunc& wheelHFunc,
	const WheelAltFunc& wheelAltHFunc,
	const WheelFunc& wheelVFunc,
	const WheelAltFunc& wheelAltVFunc,
	const DragStartFunc& dragStartFunc,
	const DragProcessFunc& dragProcessFunc,
	const DragEndFunc& dragEndFunc,
	const SeqTrackSelectFunc& trackSelectFunc)
	: wheelVFunc(wheelVFunc), wheelAltVFunc(wheelAltVFunc),
	trackSelectFunc(trackSelectFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::SeqTrack));

	/** Track Name Buton */
	this->trackName = std::make_unique<juce::TextButton>("0 - " + TRANS("Untitled"));
	this->trackName->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::SeqTrackName));
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
	this->addChildComponent(this->muteButton.get());

	/** Solo Button */
	this->soloButton = std::make_unique<SeqTrackSoloComponent>();
	this->addChildComponent(this->soloButton.get());

	/** Input Monitoring Button */
	this->inputMonitoringButton = std::make_unique<SeqTrackInputMonitoringComponent>();
	this->addChildComponent(this->inputMonitoringButton.get());

	/** Record Button */
	this->recButton = std::make_unique<SeqTrackRecComponent>();
	this->addChildComponent(this->recButton.get());

	/** Instr Button */
	this->instrButton = std::make_unique<RightClickableTextButton>("-",
		[this] { this->instrEditorShow(); },
		[this] { this->instrMenuShow(); });
	this->instrButton->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::InstrName));
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

	/** Level Meter */
	this->levelMeter = std::make_unique<SeqTrackLevelMeter>();
	this->addAndMakeVisible(this->levelMeter.get());

	/** Content */
	this->content = std::make_unique<SeqTrackContentViewer>(
		scrollFunc, wheelHFunc, wheelAltHFunc,
		dragStartFunc, dragProcessFunc, dragEndFunc, trackSelectFunc);
	this->addAndMakeVisible(this->content.get());

	/** Focus */
	this->setWantsKeyboardFocus(true);
	this->setMouseClickGrabsKeyboardFocus(true);
}

void SeqTrackComponent::updateIndex(int index) {
	this->index = index;
	if (index > -1) {
		this->muteButton->updateIndex(index);
		this->soloButton->updateIndex(index);
		this->inputMonitoringButton->updateIndex(index);
		this->recButton->updateIndex(index);

		this->levelMeter->updateIndex(index);

		this->content->updateIndex(index);
	}
}

void SeqTrackComponent::updateInfo() {
	this->trackColor = quickAPI::getTrackColor(
		{ quickAPI::TrackType::Track, this->index });

	auto& laf = this->getLookAndFeel();
	auto textColorLight = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	auto textColorDark = laf.findColour(
		juce::Label::ColourIds::textColourId);
	this->idColor = utils::chooseTextColor(this->trackColor, textColorLight, textColorDark);

	auto name = quickAPI::getTrackName(
		{ quickAPI::TrackType::Track, this->index });
	if (name.isEmpty()) {
		name = TRANS("Untitled");
	}
	this->trackName->setButtonText(juce::String{ index } + " - " + name);

	this->repaint();
}

void SeqTrackComponent::updateBlock(int blockIndex) {
	this->content->updateBlock(blockIndex);
}

void SeqTrackComponent::updateMuteSolo() {
	this->muteButton->update();
	this->soloButton->update();
}

void SeqTrackComponent::updateInputMonitoring() {
	this->inputMonitoringButton->update();
}

void SeqTrackComponent::updateRec() {
	this->recButton->update();
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
	int paddingWidth = screenSize.getWidth() * 0.005;
	float lineThickness = screenSize.getHeight() * 0.0025;

	int levelMeterWidth = screenSize.getWidth() * 0.0085;

	int nameWidth = screenSize.getWidth() * 0.065;
	int nameHeight = screenSize.getHeight() * 0.025;

	int buttonSplitWidth = screenSize.getWidth() * 0.0035;

	int compressModeHeight = paddingHeight * 2 + nameHeight;
	bool isCompressMode = this->getHeight() <= compressModeHeight;

	int lineSplitHeight = screenSize.getHeight() * 0.01;
	int instrLineHeight = screenSize.getHeight() * 0.0225;

	int contentLineSplitHeight = screenSize.getHeight() * 0.0075;

	int ioLineHeight = screenSize.getHeight() * 0.020;
	int ioLineSplitWidth = screenSize.getWidth() * 0.0025;

	int ioLineShownHeight = compressModeHeight + lineSplitHeight + ioLineHeight;
	bool isIOLineShown = this->getHeight() >= ioLineShownHeight;

	int instrLineShownHeight = ioLineShownHeight + contentLineSplitHeight + instrLineHeight;
	bool isInstrLineShown = this->getHeight() >= instrLineShownHeight;

	/** Level Meter */
	juce::Rectangle<int> levelMeterRect(
		headWidth - levelMeterWidth, 0,
		levelMeterWidth, this->getHeight());
	this->levelMeter->setBounds(levelMeterRect);

	int topY = 0;
	int leftX = trackColorWidth + paddingWidth, rightX = levelMeterRect.getX() - paddingWidth;

	/** Track Name */
	juce::Rectangle<int> nameRect(
		leftX, isCompressMode ? lineThickness : paddingHeight,
		rightX - leftX,
		isCompressMode ? (this->getHeight() - lineThickness * 2) : nameHeight);
	this->trackName->setBounds(nameRect);

	topY = nameRect.getBottom();

	/** Instr Button */
	juce::Rectangle<int> instrLineRect(
		leftX, topY + lineSplitHeight,
		rightX - leftX, instrLineHeight);
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
	/** Input Monitoring Button */
	juce::Rectangle<int> inputMonitoringRect(
		leftX + buttonSplitWidth, topY + contentLineSplitHeight,
		ioLineHeight, ioLineHeight);
	this->inputMonitoringButton->setBounds(inputMonitoringRect);
	this->inputMonitoringButton->setVisible(isIOLineShown);

	/** Record Button */
	juce::Rectangle<int> recRect(
		inputMonitoringRect.getRight() + buttonSplitWidth, inputMonitoringRect.getY(),
		ioLineHeight, ioLineHeight);
	this->recButton->setBounds(recRect);
	this->recButton->setVisible(isIOLineShown);

	/** Mute Button */
	juce::Rectangle<int> muteRect(
		recRect.getRight() + buttonSplitWidth, inputMonitoringRect.getY(),
		ioLineHeight, ioLineHeight);
	this->muteButton->setBounds(muteRect);
	this->muteButton->setVisible(isIOLineShown);

	/** Solo Button */
	juce::Rectangle<int> soloRect(
		muteRect.getRight() + buttonSplitWidth, inputMonitoringRect.getY(),
		ioLineHeight, ioLineHeight);
	this->soloButton->setBounds(soloRect);
	this->soloButton->setVisible(isIOLineShown);

	if (isIOLineShown) {
		topY = soloRect.getBottom();
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
	juce::Colour outlineColor = laf.findColour(
		this->dragHovered || this->hasKeyboardFocus(true)
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
			CoreActions::setTrackColorGUI(
				quickAPI::TrackType::Track, this->index);
		}
	}
	else if (event.mods.isRightButtonDown()) {
		this->menuShow();
	}
}

void SeqTrackComponent::mouseDoubleClick(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		this->trackSelectFunc(this->index);
	}
}

void SeqTrackComponent::mouseWheelMove(const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel) {
	if (event.mods.isAltDown()) {
		if (auto parent = this->getParentComponent()) {
			float posYInList = event.getEventRelativeTo(parent).position.getY();
			int listHeight = parent->getHeight();

			double thumbPer = posYInList / (double)listHeight;
			double centerNum = this->index + (event.position.getY() / (double)this->getHeight());

			this->wheelAltVFunc(centerNum, thumbPer, wheel.deltaY, wheel.isReversed);
		}
	}
	else {
		this->wheelVFunc(wheel.deltaY, wheel.isReversed);
	}
}

bool SeqTrackComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		return des["instrument"] || des["ara"];
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

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		juce::String pid = des["id"].toString();
		bool ara = des["ara"];

		this->setInstr(pid, ara);
		return;
	}
}

bool SeqTrackComponent::isInterestedInFileDrag(const juce::StringArray& files) {
	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(false);
	juce::StringArray midiFormats = quickAPI::getMidiFormatsSupported(false);

	int audioCount = 0, midiCount = 0;

	for (auto& filePath : files) {
		juce::File file(filePath);
		juce::String fileType = "*" + file.getFileExtension();

		if (audioFormats.contains(fileType)) {
			audioCount++;
		}
		else if (midiFormats.contains(fileType)) {
			midiCount++;
		}

		if (audioCount > 1 || midiCount > 1) { return false; }
	}

	if (audioCount + midiCount <= 0) { return false; }
	return audioCount <= 1 && midiCount <= 1;
}

void SeqTrackComponent::fileDragEnter(const juce::StringArray& files, int /*x*/, int /*y*/) {
	if (!this->isInterestedInFileDrag(files)) { return; }
	this->preDrop();
}

void SeqTrackComponent::fileDragExit(const juce::StringArray& files) {
	if (!this->isInterestedInFileDrag(files)) { return; }
	this->endDrop();
}

void SeqTrackComponent::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/) {
	if (!this->isInterestedInFileDrag(files)) { return; }
	this->endDrop();

	juce::StringArray audioFormats = quickAPI::getAudioFormatsSupported(false);
	juce::StringArray midiFormats = quickAPI::getMidiFormatsSupported(false);

	for (auto& filePath : files) {
		juce::File file(filePath);
		juce::String fileType = "*" + file.getFileExtension();

		if (audioFormats.contains(fileType)) {
			/** Load Audio */
			this->setContentAudioRef(filePath);
			break;
		}
		else if (midiFormats.contains(fileType)) {
			/** Load MIDI */
			this->setContentMIDIRef(filePath);
			break;
		}
	}
}

void SeqTrackComponent::focusGained(FocusChangeType cause) {
	this->repaint();
}

void SeqTrackComponent::focusLost(FocusChangeType cause) {
	this->repaint();
}

void SeqTrackComponent::editTrackName() {
	CoreActions::setTrackNameGUI(
		quickAPI::TrackType::Track, this->index);
}

void SeqTrackComponent::instrEditorShow() {
	bool instrValid = quickAPI::isInstrValid(this->index);
	if (!instrValid) { 
		/** Show Add Instr Menu When Instr Invalid */
		this->instrMenuShow();
		return;
	}
	
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
			comp->setInstr(pluginDes.createIdentifierString(), pluginDes.hasARAExtension);
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
	Add = 1, Remove1, MIDITrack
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
	case SeqMenuActionType::MIDITrack: {
		CoreActions::setTrackMIDITrackGUI(this->index);
		break;
	}
	}
}

void SeqTrackComponent::setInstr(const juce::String& pid, bool addARA) {
	CoreActions::insertInstr(this->index, pid, addARA);
}

void SeqTrackComponent::add() {
	CoreActions::insertTrackGUI(
		quickAPI::TrackType::Track, this->index + 1);
}

void SeqTrackComponent::remove() {
	CoreActions::removeTrackGUI(
		quickAPI::TrackType::Track, this->index);
}

void SeqTrackComponent::setContentAudioRef(const juce::String& path) {
	CoreActions::setTrackAudioRefGUI(this->index, path);
}

void SeqTrackComponent::setContentMIDIRef(const juce::String& path) {
	CoreActions::setTrackMIDIRefGUI(this->index, path);
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
	menu.addItem(SeqMenuActionType::MIDITrack, TRANS("Select MIDI Track"));

	return menu;
}

juce::String SeqTrackComponent::createToolTipString() const {
	juce::String result;

	result += "#" + juce::String{ this->index } + " " + quickAPI::getTrackName({ quickAPI::TrackType::Track, this->index }) + "\n";
	result += TRANS("Bus:") + " " + quickAPI::getTrackBusTypeName({ quickAPI::TrackType::Track, this->index }) + "\n";

	if (quickAPI::isInstrValid(this->index)) {
		result += TRANS("Instrument:") + " " + quickAPI::getInstrName(this->index) + "\n";
		result += TRANS("Instrument Offline:") + " " + juce::String{ quickAPI::getInstrOffline(this->index) ? "ON" : "OFF" } + "\n";
		result += TRANS("Instrument Bypass:") + " " + juce::String{ quickAPI::getInstrBypass(this->index) ? "ON" : "OFF" } + "\n";
	}

	return result;
}
