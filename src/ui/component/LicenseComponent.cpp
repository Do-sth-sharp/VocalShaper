#include "LicenseComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"

LicenseComponent::LicenseComponent() {
	/** TODO License List Model */

	/** License List */
	this->licenseList = std::make_unique<juce::ListBox>(
		TRANS("License List"), nullptr);
	this->licenseList->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forListBox());
	this->addAndMakeVisible(this->licenseList.get());

	/** License Content */
	this->licenseContent = std::make_unique<juce::TextEditor>(
		TRANS("License Content"));
	this->licenseContent->setMultiLine(true);
	this->licenseContent->setReadOnly(true);
	auto defaultTextColor = this->getLookAndFeel().findColour(
		juce::TextEditor::ColourIds::textColourId);
	this->licenseContent->setTextToShowWhenEmpty(
		TRANS("Select a license in the list to show content."), defaultTextColor);
	this->addAndMakeVisible(this->licenseContent.get());

	/** TODO Scan For Licenses */
}

void LicenseComponent::resized() {
	/** TODO Resize Components */
}
