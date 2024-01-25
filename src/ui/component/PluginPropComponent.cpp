#include "PluginPropComponent.h"
#include "../Utils.h"

PluginPropComponent::PluginPropComponent(
	PluginType type, quickAPI::PluginHolder plugin)
	: type(type), plugin(plugin) {
	/** Titles */
	this->midiTitle = TRANS("MIDI Properties");
	this->automaticTitle = TRANS("Automation");
	this->channelLabel = TRANS("MIDI Channel");

	/** MIDI Channel */
	juce::StringArray midiChannelList;
	for (int i = 0; i <= 16; i++) {
		if (i == 0) {
			midiChannelList.add("-");
		}
		else {
			midiChannelList.add(juce::String{ i });
		}
	}
	this->midiChannel = std::make_unique<juce::ComboBox>(TRANS("MIDI Channel"));
	this->midiChannel->addItemList(midiChannelList, 1);
	this->midiChannel->setWantsKeyboardFocus(false);
	this->addAndMakeVisible(this->midiChannel.get());

	/** MIDI CC Intercept */
	this->midiCCIntercept = std::make_unique<juce::ToggleButton>(TRANS("CC Intercept"));
	this->midiCCIntercept->setWantsKeyboardFocus(false);
	this->addAndMakeVisible(this->midiCCIntercept.get());

	/** MIDI Output */
	this->midiOutput = std::make_unique<juce::ToggleButton>(TRANS("MIDI Output"));
	this->midiOutput->setWantsKeyboardFocus(false);
	this->addAndMakeVisible(this->midiOutput.get());

	/** Automation List */
	this->automaticList = std::make_unique<juce::TableListBox>(
		TRANS("Automation"), nullptr);
	this->addAndMakeVisible(this->automaticList.get());
}

int PluginPropComponent::getPreferedHeight() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.7;
}

void PluginPropComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.01;
	int paddingHeight = screenSize.getHeight() * 0.02;
	int splitWidth = screenSize.getWidth() * 0.005;
	int splitHeight = screenSize.getHeight() * 0.01;

	int titleHeight = screenSize.getHeight() * 0.04;
	int lineHeight = screenSize.getHeight() * 0.04;
	int columnWidth = screenSize.getWidth() * 0.06;

	/** MIDI Title */
	juce::Rectangle<int> midiTitleRect(
		paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);

	/** Channel Label */
	juce::Rectangle<int> channelLabelRect(
		paddingWidth, midiTitleRect.getBottom() + splitHeight,
		columnWidth, lineHeight);

	/** Channel Combo */
	juce::Rectangle<int> channelComboRect(
		channelLabelRect.getRight() + splitWidth, channelLabelRect.getY(),
		columnWidth, channelLabelRect.getHeight());
	this->midiChannel->setBounds(channelComboRect);

	/** MIDI Toggle */
	juce::Rectangle<int> ccInterceptRect(
		paddingWidth, channelLabelRect.getBottom() + splitHeight,
		(this->getWidth() - paddingWidth * 2 - splitWidth * 3) / 4, lineHeight);
	this->midiCCIntercept->setBounds(ccInterceptRect);

	juce::Rectangle<int> midiOutputRect(
		ccInterceptRect.getRight() + splitWidth, ccInterceptRect.getY(),
		ccInterceptRect.getWidth(), ccInterceptRect.getHeight());
	this->midiOutput->setBounds(midiOutputRect);

	/** Automation Title */
	juce::Rectangle<int> autoTitleRect(
		paddingWidth, ccInterceptRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);

	/** Automation List */
	juce::Rectangle<int> autoListRect(
		paddingWidth, autoTitleRect.getBottom() + splitHeight,
		this->getWidth() - paddingWidth * 2,
		this->getHeight() - paddingHeight - autoTitleRect.getBottom() - splitHeight);
	this->automaticList->setBounds(autoListRect);
}

void PluginPropComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.01;
	int paddingHeight = screenSize.getHeight() * 0.02;
	int splitWidth = screenSize.getWidth() * 0.005;
	int splitHeight = screenSize.getHeight() * 0.01;
	int labelPaddingWidth = screenSize.getWidth() * 0.005;

	int titleHeight = screenSize.getHeight() * 0.04;
	int lineHeight = screenSize.getHeight() * 0.04;
	int columnWidth = screenSize.getWidth() * 0.06;

	float titleFontHeight = screenSize.getHeight() * 0.03;
	float labelFontHeight = screenSize.getHeight() * 0.0175;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour titleColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour labelColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font titleFont(titleFontHeight);
	juce::Font labelFont(labelFontHeight);

	/** MIDI Title */
	juce::Rectangle<int> midiTitleRect(
		paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->midiTitle, midiTitleRect,
		juce::Justification::centredLeft, 1, 0.f);

	/** Channel Label */
	juce::Rectangle<int> channelLabelRect(
		paddingWidth + labelPaddingWidth, midiTitleRect.getBottom() + splitHeight,
		columnWidth - labelPaddingWidth * 2, lineHeight);
	g.setColour(labelColor);
	g.setFont(labelFont);
	g.drawFittedText(this->channelLabel, channelLabelRect,
		juce::Justification::centredLeft, 1, 1.f);

	/** Automation Title */
	juce::Rectangle<int> autoTitleRect(
		paddingWidth, channelLabelRect.getBottom() + splitHeight * 2 + lineHeight,
		this->getWidth() - paddingWidth * 2, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->automaticTitle, autoTitleRect,
		juce::Justification::centredLeft, 1, 0.f);
}

void PluginPropComponent::update() {
	/** TODO */
}
