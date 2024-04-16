#include "TempoEditor.h"
#include "../misc/RCManager.h"
#include "../Utils.h"

TempoEditorContent::TempoEditorContent(bool defaultIsTempo,
	double defaultTempo, int defaultNumerator, int defaultDenominator,
	bool switchable) {
	/** Toggle */
	this->tempoToggle = std::make_unique<juce::ToggleButton>(TRANS("Tempo"));
	this->tempoToggle->setWantsKeyboardFocus(false);
	this->tempoToggle->onClick = [this] { this->setIsTempo(true); };
	this->tempoToggle->setToggleState(defaultIsTempo,
		juce::NotificationType::dontSendNotification);
	this->tempoToggle->setEnabled(switchable);
	this->addAndMakeVisible(this->tempoToggle.get());

	this->beatToggle = std::make_unique<juce::ToggleButton>(TRANS("Beat"));
	this->beatToggle->setWantsKeyboardFocus(false);
	this->beatToggle->onClick = [this] { this->setIsTempo(false); };
	this->beatToggle->setToggleState(!defaultIsTempo,
		juce::NotificationType::dontSendNotification);
	this->beatToggle->setEnabled(switchable);
	this->addAndMakeVisible(this->beatToggle.get());

	/** Tempo Editor */
	this->tempoEditor = std::make_unique<juce::TextEditor>(TRANS("Tempo"));
	this->tempoEditor->setHasFocusOutline(false);
	this->tempoEditor->setMultiLine(false);
	this->tempoEditor->setJustification(juce::Justification::centredLeft);
	this->tempoEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->tempoEditor->setPopupMenuEnabled(false);
	this->tempoEditor->setInputFilter(
		new utils::TextDoubleFilter{ 0, 300, 2 }, true);
	this->tempoEditor->setText(juce::String{ defaultTempo, 2 });
	this->tempoEditor->setEnabled(defaultIsTempo);
	this->addAndMakeVisible(this->tempoEditor.get());

	/** Beat Editor */
	this->numeratorEditor = std::make_unique<juce::TextEditor>(TRANS("Beat"));
	this->numeratorEditor->setHasFocusOutline(false);
	this->numeratorEditor->setMultiLine(false);
	this->numeratorEditor->setJustification(juce::Justification::centredLeft);
	this->numeratorEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->numeratorEditor->setPopupMenuEnabled(false);
	this->numeratorEditor->setInputFilter(
		new utils::TextIntegerFilter{ 0, 30 }, true);
	this->numeratorEditor->setText(juce::String{ defaultNumerator });
	this->numeratorEditor->setEnabled(!defaultIsTempo);
	this->addAndMakeVisible(this->numeratorEditor.get());

	const juce::StringArray denominatorList{ "2", "4", "8", "16", "32" };
	int defalutIndex = denominatorList.indexOf(juce::String{ defaultDenominator });
	if (defalutIndex < 0) {
		defalutIndex = denominatorList.indexOf("4");
	}
	this->denominatorEditor = std::make_unique<juce::ComboBox>(TRANS("Beat"));
	this->denominatorEditor->addItemList(denominatorList, 1);
	this->denominatorEditor->setWantsKeyboardFocus(false);
	this->denominatorEditor->setSelectedItemIndex(defalutIndex,
		juce::NotificationType::dontSendNotification);
	this->denominatorEditor->setEnabled(!defaultIsTempo);
	this->addAndMakeVisible(this->denominatorEditor.get());

	this->beatSplitLabel = std::make_unique<juce::Label>(TRANS("Beat"), "/");
	this->beatSplitLabel->setJustificationType(juce::Justification::centred);
	this->addAndMakeVisible(this->beatSplitLabel.get());
}

void TempoEditorContent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.025;
	int paddingWidth = screenSize.getWidth() * 0.025;

	int toggleHeight = screenSize.getHeight() * 0.025;
	int toggleWidth = screenSize.getWidth() * 0.05;
	int toggleSplitWidth = screenSize.getWidth() * 0.005;

	int lineSplitHeight = screenSize.getHeight() * 0.025;
	int lineHeight = screenSize.getHeight() * 0.035;
	int lineCompWidth = screenSize.getWidth() * 0.1;
	int beatSplitWidth = screenSize.getWidth() * 0.025;

	/** Tempo Toggle */
	juce::Rectangle<int> tempoToggleRect(
		paddingWidth, paddingHeight + lineHeight / 2 - toggleHeight / 2,
		toggleWidth, toggleHeight);
	this->tempoToggle->setBounds(tempoToggleRect);

	/** Tempo Editor */
	juce::Rectangle<int> tempoEditorRect(
		tempoToggleRect.getRight() + toggleSplitWidth, paddingHeight,
		lineCompWidth, lineHeight);
	this->tempoEditor->setBounds(tempoEditorRect);

	/** Beat Toggle */
	juce::Rectangle<int> beatToggleRect(
		paddingWidth, tempoEditorRect.getBottom() + lineSplitHeight + lineHeight / 2 - toggleHeight / 2,
		toggleWidth, toggleHeight);
	this->beatToggle->setBounds(beatToggleRect);

	/** Numerator Editor */
	juce::Rectangle<int> numeratorEditorRect(
		beatToggleRect.getRight() + toggleSplitWidth, tempoEditorRect.getBottom() + lineSplitHeight,
		(lineCompWidth - beatSplitWidth) / 2, lineHeight);
	this->numeratorEditor->setBounds(numeratorEditorRect);

	/** Beat Split */
	juce::Rectangle<int> beatSplitRect(
		numeratorEditorRect.getRight(), numeratorEditorRect.getY(),
		beatSplitWidth, lineHeight);
	this->beatSplitLabel->setBounds(beatSplitRect);

	/** Denominator Editor */
	juce::Rectangle<int> denominatorEditorRect(
		beatSplitRect.getRight(), numeratorEditorRect.getY(),
		numeratorEditorRect.getWidth(), lineHeight);
	this->denominatorEditor->setBounds(denominatorEditorRect);
}

const TempoEditorContent::TempoResult TempoEditorContent::getResult() const {
	double tempo = this->tempoEditor->getText().getDoubleValue();
	if (tempo < 30 || tempo > 300) { tempo = 120.0; }
	int numerator = this->numeratorEditor->getText().getIntValue();
	if (numerator < 1 || numerator > 30) { numerator = 4; }
	int denominator = this->denominatorEditor->getText().getIntValue();
	const juce::Array<int> denominatorList{ 2, 4, 8, 16, 32 };
	if (denominatorList.indexOf(denominator) < 0) { denominator = 4; }

	return { this->tempoToggle->getToggleState(),
		tempo, numerator, denominator };
}

void TempoEditorContent::setIsTempo(bool isTempo) {
	this->tempoToggle->setToggleState(isTempo, juce::NotificationType::dontSendNotification);
	this->tempoEditor->setEnabled(isTempo);

	this->beatToggle->setToggleState(!isTempo, juce::NotificationType::dontSendNotification);
	this->numeratorEditor->setEnabled(!isTempo);
	this->denominatorEditor->setEnabled(!isTempo);
}

TempoEditor::TempoEditor(bool defaultIsTempo,
	double defaultTempo, int defaultNumerator, int defaultDenominator,
	bool switchable)
	: DocumentWindow(TRANS("Tempo Editor"), juce::LookAndFeel::getDefaultLookAndFeel().findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton, true) {
	this->setUsingNativeTitleBar(true);

	/** Icon */
	auto icon = RCManager::getInstance()->loadImage(
		utils::getResourceFile("logo.png"));
	this->setIcon(icon);
	this->getPeer()->setIcon(icon);

	/** Content */
	this->setContentOwned(
		new TempoEditorContent{ defaultIsTempo, defaultTempo,
			defaultNumerator, defaultDenominator, switchable }, false);

	/** Size */
	juce::MessageManager::callAsync(
		[comp = juce::Component::SafePointer(this)] {
			if (comp) {
				auto screenSize = utils::getScreenSize(comp);
				comp->centreWithSize(screenSize.getWidth() * 0.21,
					screenSize.getHeight() * 0.15);
			}
		}
	);
}

void TempoEditor::closeButtonPressed() {
	this->exitModalState();
}

const TempoEditor::TempoResult TempoEditor::getResult() const {
	if (auto content = dynamic_cast<TempoEditorContent*>(this->getContentComponent())) {
		return content->getResult();
	}
	return TempoResult{};
}
