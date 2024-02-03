#include "MixerTrackComponent.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

MixerTrackComponent::MixerTrackComponent() {
	/** Side Chain */
	this->sideChain = std::make_unique<SideChainComponent>();
	this->addAndMakeVisible(this->sideChain.get());

	/** IO */
	this->midiInput = std::make_unique<MixerTrackIOComponent>(true, true);
	this->addAndMakeVisible(this->midiInput.get());

	this->audioInput = std::make_unique<MixerTrackIOComponent>(true, false);
	this->addAndMakeVisible(this->audioInput.get());

	this->midiOutput = std::make_unique<MixerTrackIOComponent>(false, true);
	this->addAndMakeVisible(this->midiOutput.get());

	this->audioOutput = std::make_unique<MixerTrackIOComponent>(false, false);
	this->addAndMakeVisible(this->audioOutput.get());
}

void MixerTrackComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int colorHeight = screenSize.getHeight() * 0.015;

	int sideChainHeight = screenSize.getHeight() * 0.02;
	int sideChainHideHeight = screenSize.getHeight() * 0.3;
	bool sideChainShown = this->getHeight() >= sideChainHideHeight;

	int ioHeight = screenSize.getHeight() * 0.02;
	int ioWidth = ioHeight;
	int ioHideHeight = screenSize.getHeight() * 0.2;
	bool ioShown = this->getHeight() >= ioHideHeight;

	int top = 0, bottom = this->getHeight();
	top += colorHeight;

	/** Side Chain */
	if (sideChainShown) {
		juce::Rectangle<int> sideChainRect(
			0, top, this->getWidth(), sideChainHeight);
		this->sideChain->setBounds(sideChainRect);

		top += sideChainRect.getHeight();
	}
	this->sideChain->setVisible(sideChainShown);

	/** Input */
	if (ioShown) {
		juce::Rectangle<int> midiRect(
			this->getWidth() * 3 / 10 - ioWidth / 2,
			top, ioWidth, ioHeight);
		this->midiInput->setBounds(midiRect);

		juce::Rectangle<int> audioRect(
			this->getWidth() * 7 / 10 - ioWidth / 2,
			top, ioWidth, ioHeight);
		this->audioInput->setBounds(audioRect);

		top += ioHeight;
	}
	this->midiInput->setVisible(ioShown);
	this->audioInput->setVisible(ioShown);

	/** Output */
	if (ioShown) {
		juce::Rectangle<int> midiRect(
			this->getWidth() * 3 / 10 - ioWidth / 2,
			bottom - ioHeight, ioWidth, ioHeight);
		this->midiOutput->setBounds(midiRect);

		juce::Rectangle<int> audioRect(
			this->getWidth() * 7 / 10 - ioWidth / 2,
			bottom - ioHeight, ioWidth, ioHeight);
		this->audioOutput->setBounds(audioRect);

		bottom -= ioHeight;
	}
	this->midiOutput->setVisible(ioShown);
	this->audioOutput->setVisible(ioShown);
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
	juce::Font titleFont(titleFontHeight);

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
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Outline */
	auto totalRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(totalRect, outlineThickness);
}

void MixerTrackComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->name = quickAPI::getMixerTrackName(index);
		this->trackColor = quickAPI::getMixerTrackColor(index);

		auto& laf = this->getLookAndFeel();
		if (utils::isLightColor(this->trackColor)) {
			this->nameColor = laf.findColour(
				juce::Label::ColourIds::textWhenEditingColourId);
		}
		else {
			this->nameColor = laf.findColour(
				juce::Label::ColourIds::textColourId);
		}

		this->sideChain->update(index);

		this->repaint();
	}
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
			CoreActions::setTrackColorGUI(this->index);
		}
	}
	else if (event.mods.isLeftButtonDown()) {
		if (y >= 0 && y < colorHeight) {
			CoreActions::setTrackNameGUI(this->index);
		}
	}
}
