#include "LicenseComponent.h"
#include "../misc/MainThreadPool.h"
#include "../Utils.h"

LicenseComponent::LicenseComponent() {
	/** License List Model */
	this->licenseListModel = std::make_unique<LicenseListModel>();
	this->licenseListModel->setCallback(
		[this](const juce::File& file) {
			this->licenseContent->setText(" ");
			this->load(file);
		});

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

	/** Scan For Licenses */
	this->scan();
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

void LicenseComponent::scan() {
	auto licenseDir = utils::getLicenseDir();
	auto listBox = juce::ListBox::SafePointer{ this->licenseList.get() };

	auto scanJob = [licenseDir, listBox] {
		auto list = licenseDir.findChildFiles(
			juce::File::findFiles, true, "*.txt;*.md",
			juce::File::FollowSymlinks::noCycles);
		juce::MessageManager::callAsync(
			[list, listBox] {
				if (listBox) {
					if (auto model = dynamic_cast<LicenseListModel*>(listBox->getModel())) {
						model->setList(list);
						listBox->updateContent();
					}
				}
			});
	};
	MainThreadPool::getInstance()->runJob(scanJob);
}

void LicenseComponent::load(const juce::File& file) {
	auto textEditor = juce::TextEditor::SafePointer{ this->licenseContent.get() };

	auto loadJob = [file, textEditor] {
		juce::String data = " ";

		juce::FileInputStream stream(file);
		if (stream.openedOk()) {
			data = stream.readString();
		}

		juce::MessageManager::callAsync(
			[data, textEditor] {
				if (textEditor) {
					textEditor->setReadOnly(false);
					textEditor->setText(data);
					textEditor->setReadOnly(true);
				}
			});
	};
	MainThreadPool::getInstance()->runJob(loadJob);
}
