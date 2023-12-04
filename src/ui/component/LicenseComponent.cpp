#include "LicenseComponent.h"
#include "../Utils.h"

LicenseComponent::LicenseComponent() {
	/** License List Model */
	this->licenseListModel = std::make_unique<LicenseListModel>();

	/** License List */
	this->licenseList = std::make_unique<juce::ListBox>(
		TRANS("License List"), this->licenseListModel.get());
	this->addAndMakeVisible(this->licenseList.get());

	/** License Content */
	this->licenseContent = std::make_unique<juce::TextEditor>(
		TRANS("License Content"));
	this->licenseContent->setMultiLine(true);
	this->licenseContent->setReadOnly(true);
	this->licenseContent->setCaretVisible(false);
	this->licenseContent->setWantsKeyboardFocus(false);
	auto emptyTextColor = this->getLookAndFeel().findColour(
		juce::TextEditor::ColourIds::shadowColourId + 1);
	this->licenseContent->setTextToShowWhenEmpty(
		TRANS("Select a license in the list to show content."), emptyTextColor);
	this->addAndMakeVisible(this->licenseContent.get());

	/** TODO Scan For Licenses */
}

void LicenseComponent::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Size */
	int listWidth = screenSize.getWidth() * 0.1;

	/** List */
	this->licenseList->setBounds(
		this->getLocalBounds().withWidth(listWidth));
	
	/** Content */
	this->licenseContent->setBounds(
		this->getLocalBounds().withTrimmedLeft(listWidth));
}
