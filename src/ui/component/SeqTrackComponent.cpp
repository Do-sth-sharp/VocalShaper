#include "SeqTrackComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../misc/PluginEditorHub.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"
#include <IconManager.h>

SeqTrackComponent::SeqTrackComponent() {
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
	this->instrButton = std::make_unique<juce::TextButton>("-");
	this->instrButton->setWantsKeyboardFocus(false);
	this->instrButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->instrButton->setConnectedEdges(juce::Button::ConnectedOnRight);
	this->instrButton->onClick = [this] { this->instrEditorShow(); };
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
	this->addAndMakeVisible(this->instrBypassButton.get());

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
	this->addAndMakeVisible(this->instrOfflineButton.get());
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

		this->repaint();
	}
}

void SeqTrackComponent::updateBlock(int blockIndex) {
	/** TODO */
}

void SeqTrackComponent::updateMute() {
	this->muteButton->update(this->index);
}

void SeqTrackComponent::updateRec() {
	this->recButton->update(this->index);
}

void SeqTrackComponent::updateInstr() {
	bool instrValid = quickAPI::isInstrValid(this->index);
	this->instrButton->setButtonText(
		instrValid ? quickAPI::getInstrName(this->index) : "-");
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
}

void SeqTrackComponent::updateHPos(double pos, double itemSize) {
	/** TODO */
}

void SeqTrackComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int headWidth = screenSize.getWidth() * 0.1;

	int paddingHeight = screenSize.getHeight() * 0.015;
	int paddingWidth = screenSize.getWidth() * 0.0075;
	float lineThickness = screenSize.getHeight() * 0.0025;

	int nameWidth = screenSize.getWidth() * 0.065;
	int nameHeight = screenSize.getHeight() * 0.025;

	int muteButtonHeight = screenSize.getHeight() * 0.0225;
	int buttonSplitWidth = screenSize.getWidth() * 0.0035;

	int compressModeHeight = paddingHeight * 2 + nameHeight;
	bool isCompressMode = this->getHeight() <= compressModeHeight;

	int lineSplitHeight = screenSize.getHeight() * 0.01;
	int instrLineHeight = screenSize.getHeight() * 0.0225;

	int instrLineShownHeight = paddingHeight * 2 + nameHeight + lineSplitHeight + instrLineHeight;
	bool isInstrLineShown = this->getHeight() >= instrLineShownHeight;

	/** Track Name */
	juce::Rectangle<int> nameRect(
		paddingWidth, isCompressMode ? lineThickness : paddingHeight,
		headWidth - paddingWidth * 2 - muteButtonHeight * 2 - buttonSplitWidth * 2,
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

	/** Instr Button */
	juce::Rectangle<int> instrLineRect(
		paddingWidth, nameRect.getBottom() + lineSplitHeight,
		headWidth - paddingWidth * 2, instrLineHeight);
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
}

void SeqTrackComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.0025;
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

	/** Outline */
	juce::Rectangle<float> outlineRect(
		0, this->getHeight() - lineThickness,
		this->getWidth(), lineThickness);
	g.setColour(outlineColor);
	g.fillRect(outlineRect);
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

	if (event.mods.isLeftButtonDown() || event.mods.isRightButtonDown()) {
		if (x >= 0 && x < trackColorWidth) {
			CoreActions::setSeqColorGUI(this->index);
		}
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
