#include "PluginPropComponent.h"
#include "../misc/CoreActions.h"
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
	this->midiChannel->onChange = [this] { this->changeMIDIChannel(); };
	this->addAndMakeVisible(this->midiChannel.get());

	/** MIDI CC Intercept */
	this->midiCCIntercept = std::make_unique<juce::ToggleButton>(TRANS("CC Intercept"));
	this->midiCCIntercept->setWantsKeyboardFocus(false);
	this->midiCCIntercept->onClick = [this] { this->changeMIDICCIntercept(); };
	this->addAndMakeVisible(this->midiCCIntercept.get());

	/** MIDI Output */
	this->midiOutput = std::make_unique<juce::ToggleButton>(TRANS("MIDI Output"));
	this->midiOutput->setWantsKeyboardFocus(false);
	this->midiOutput->onClick = [this] { this->changeMIDIOutput(); };
	this->addAndMakeVisible(this->midiOutput.get());

	/** Automation List */
	this->automaticModel = std::make_unique<PluginAutomationModel>(plugin, type);
	this->automaticList = std::make_unique<juce::TableListBox>(
		TRANS("Automation"), this->automaticModel.get());
	this->automaticList->getHeader().addColumn(
		TRANS("Parameter"), 1, 30, 30, -1,
		juce::TableHeaderComponent::ColumnPropertyFlags::visible |
		juce::TableHeaderComponent::ColumnPropertyFlags::resizable |
		juce::TableHeaderComponent::ColumnPropertyFlags::sortable);
	this->automaticList->getHeader().addColumn(
		TRANS("CC Controller"), 2, 30, 30, -1,
		juce::TableHeaderComponent::ColumnPropertyFlags::visible |
		juce::TableHeaderComponent::ColumnPropertyFlags::resizable |
		juce::TableHeaderComponent::ColumnPropertyFlags::sortable);
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
	int tableColumnWidth = screenSize.getWidth() * 0.15;

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
		columnWidth, lineHeight);
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
	this->automaticList->setHeaderHeight(lineHeight);
	this->automaticList->getHeader().setColumnWidth(1, tableColumnWidth);
	this->automaticList->getHeader().setColumnWidth(2, tableColumnWidth);
	this->automaticList->setRowHeight(lineHeight);
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
	if (this->plugin) {
		switch (this->type) {
		case PluginType::Instr: {
			this->midiChannel->setSelectedItemIndex(
				quickAPI::getInstrMIDIChannel(this->plugin),
				juce::NotificationType::dontSendNotification);
			this->midiCCIntercept->setToggleState(
				quickAPI::getInstrMIDICCIntercept(this->plugin),
				juce::NotificationType::dontSendNotification);
			this->midiOutput->setToggleState(
				quickAPI::getInstrMIDIOutput(this->plugin),
				juce::NotificationType::dontSendNotification);
			break;
		}
		case PluginType::Effect: {
			this->midiChannel->setSelectedItemIndex(
				quickAPI::getEffectMIDIChannel(this->plugin),
				juce::NotificationType::dontSendNotification);
			this->midiCCIntercept->setToggleState(
				quickAPI::getEffectMIDICCIntercept(this->plugin),
				juce::NotificationType::dontSendNotification);
			this->midiOutput->setToggleState(
				quickAPI::getEffectMIDIOutput(this->plugin),
				juce::NotificationType::dontSendNotification);
			break;
		}
		}

		this->automaticModel->update();
		this->automaticList->updateContent();
		this->automaticList->repaint();
	}
}

void PluginPropComponent::changeMIDIChannel() {
	int channel = this->midiChannel->getSelectedItemIndex();
	if (this->plugin) {
		switch (this->type) {
		case PluginType::Instr: {
			CoreActions::setInstrMIDIChannel(this->plugin, channel);
			break;
		}
		case PluginType::Effect: {
			CoreActions::setEffectMIDIChannel(this->plugin, channel);
			break;
		}
		}
	}
}

void PluginPropComponent::changeMIDICCIntercept() {
	bool intercept = this->midiCCIntercept->getToggleState();
	if (this->plugin) {
		switch (this->type) {
		case PluginType::Instr: {
			CoreActions::setInstrMIDICCIntercept(this->plugin, intercept);
			break;
		}
		case PluginType::Effect: {
			CoreActions::setEffectMIDICCIntercept(this->plugin, intercept);
			break;
		}
		}
	}
}

void PluginPropComponent::changeMIDIOutput() {
	bool output = this->midiOutput->getToggleState();
	if (this->plugin) {
		switch (this->type) {
		case PluginType::Instr: {
			CoreActions::setInstrMIDIOutput(this->plugin, output);
			break;
		}
		case PluginType::Effect: {
			CoreActions::setEffectMIDIOutput(this->plugin, output);
			break;
		}
		}
	}
}
