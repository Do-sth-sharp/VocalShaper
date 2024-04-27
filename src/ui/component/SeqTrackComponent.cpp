#include "SeqTrackComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SeqTrackComponent::SeqTrackComponent() {
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

		this->muteButton->update(index);

		this->repaint();
	}
}

void SeqTrackComponent::updateBlock(int blockIndex) {
	/** TODO */
}

void SeqTrackComponent::updateMute() {
	this->muteButton->update(this->index);
}

void SeqTrackComponent::updateHPos(double pos, double itemSize) {
	/** TODO */
}

void SeqTrackComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int compressModeHeight = screenSize.getHeight() * 0.055;

	int headWidth = screenSize.getWidth() * 0.1;

	int paddingHeight = screenSize.getHeight() * 0.015;
	int paddingWidth = screenSize.getWidth() * 0.01;
	float lineThickness = screenSize.getHeight() * 0.0025;

	int nameWidth = screenSize.getWidth() * 0.065;
	int nameHeight = screenSize.getHeight() * 0.025;

	int muteButtonHeight = screenSize.getHeight() * 0.0225;
	int buttonSplitWidth = screenSize.getWidth() * 0.0025;

	bool isCompressMode = this->getHeight() <= compressModeHeight;

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
