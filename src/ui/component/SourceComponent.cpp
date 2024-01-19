#include "SourceComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

SourceComponent::SourceComponent(
	const std::function<void(int)>& selectCallback)
	: selectCallback(selectCallback) {
	this->setWantsKeyboardFocus(true);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSourceView());

	/** Name Editor */
	this->nameEditor = std::make_unique<juce::TextEditor>(TRANS("Source Name"));
	this->nameEditor->setHasFocusOutline(false);
	this->nameEditor->setMultiLine(false);
	this->nameEditor->setJustification(juce::Justification::centredLeft);
	this->nameEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->nameEditor->setPopupMenuEnabled(false);
	this->nameEditor->setTextToShowWhenEmpty(
		TRANS("Untitled"),
		this->getLookAndFeel().findColour(
			juce::TextEditor::ColourIds::shadowColourId + 1));
	auto updateNameFunc = [this] {
		auto name = this->nameEditor->getText();
		this->updateName(name);
		};
	this->nameEditor->onReturnKey = updateNameFunc;
	this->nameEditor->onFocusLost = updateNameFunc;
	this->addAndMakeVisible(this->nameEditor.get());
}

void SourceComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int paddingHeight = screenSize.getHeight() * 0.01;
	int paddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.005;
	int splitWidth = screenSize.getWidth() * 0.005;

	int selectBarWidth = screenSize.getWidth() * 0.005;

	int nameLineHeight = screenSize.getHeight() * 0.03;
	int idAreaWidth = screenSize.getWidth() * 0.015;

	/** Name Line */
	juce::Rectangle<int> nameLineArea(
		selectBarWidth + paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2 - selectBarWidth, nameLineHeight);

	/** Name */
	auto nameRect = nameLineArea.withTrimmedLeft(idAreaWidth + splitWidth);
	this->nameEditor->setBounds(nameRect);
}

void SourceComponent::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int paddingHeight = screenSize.getHeight() * 0.01;
	int paddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.005;
	int splitWidth = screenSize.getWidth() * 0.005;

	int outLineThickness = screenSize.getWidth() * 0.0015;
	int selectBarWidth = screenSize.getWidth() * 0.005;

	int nameLineHeight = screenSize.getHeight() * 0.03;
	float idFontHeight = screenSize.getHeight() * 0.02;
	int idAreaWidth = screenSize.getWidth() * 0.015;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour idTextColor = laf.findColour(
		juce::TextEditor::ColourIds::shadowColourId + 1);
	juce::Colour outLineColor = laf.findColour(this->itemDragAccepted
		? (juce::ListBox::ColourIds::outlineColourId + 1)
		: juce::ListBox::ColourIds::outlineColourId);
	juce::Colour selectBarColor = laf.findColour(this->selected
		? (juce::Label::ColourIds::outlineWhenEditingColourId)
		: juce::Label::ColourIds::outlineColourId);

	/** Font */
	juce::Font idFont(idFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Select Bar */
	juce::Rectangle<float> selectRect(
		0, 0, selectBarWidth, this->getHeight());
	g.setColour(selectBarColor);
	g.fillRect(selectRect);

	/** Border */
	auto outLineRect = this->getLocalBounds();
	g.setColour(outLineColor);
	g.drawRect(outLineRect, outLineThickness);

	/** Name Line */
	juce::Rectangle<int> nameLineArea(
		selectBarWidth + paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2 - selectBarWidth, nameLineHeight);

	/** ID */
	auto idRect = nameLineArea.withWidth(idAreaWidth);
	g.setColour(idTextColor);
	g.setFont(idFont);
	g.drawFittedText("#" + juce::String{ this->id },
		idRect, juce::Justification::centredLeft, 1);
}

void SourceComponent::update(int index, bool selected) {
	this->selected = selected;
	if (index >= 0 && index < quickAPI::getSourceNum()) {
		this->index = index;
		this->id = quickAPI::getSourceId(index);
		this->type = (int)(quickAPI::getSourceType(index));
		this->nameEditor ->setText(quickAPI::getSourceName(index));
		this->typeName = quickAPI::getSourceTypeName(index);
		this->length = quickAPI::getSourceLength(index);
		this->channels = quickAPI::getSourceChannelNum(index);
		this->tracks = quickAPI::getSourceTrackNum(index);
		this->events = quickAPI::getSourceEventNum(index);
		this->synthesizer = quickAPI::getSourceSynthesizerName(index);
		this->sampleRate = quickAPI::getSourceSampleRate(index);
		this->isIOTask = quickAPI::checkSourceIOTask(index);
		this->isSynthing = quickAPI::checkSourceSynthing(index);
	}

	this->repaint();
}

bool SourceComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	if (this->type != quickAPI::SourceType::SynthSource) { return false; }

	if ((int)(des["type"]) != (int)(DragSourceType::Plugin)) { return false; }
	if (!des["instrument"]) { return false; }

	return true;
}

void SourceComponent::itemDragEnter(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->preDrop();
}

void SourceComponent::itemDragExit(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();
}

void SourceComponent::itemDropped(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	this->endDrop();

	auto& des = dragSourceDetails.description;
	juce::String pid = des["id"].toString();

	CoreActions::setSourceSynthesizer(this->index, pid);
}

void SourceComponent::mouseDown(const juce::MouseEvent& /*event*/) {
	this->selectThis();
}

void SourceComponent::mouseUp(const juce::MouseEvent& event) {
	/** Show Menu */
	if (event.mods.isRightButtonDown()) {
		this->showSourceMenu();
	}
}

void SourceComponent::mouseDrag(const juce::MouseEvent& event) {
	/** Start Drag */
	if (event.mods.isLeftButtonDown()) {
		this->startDrag();
	}
}

void SourceComponent::updateName(const juce::String& name) {
	CoreActions::setSourceName(this->index, name);
}

void SourceComponent::preDrop() {
	this->itemDragAccepted = true;
	this->repaint();
}

void SourceComponent::endDrop() {
	this->itemDragAccepted = false;
	this->repaint();
}

void SourceComponent::selectThis() {
	this->selectCallback(this->index);
}

enum SourceActionType {
	NewAudio = 1,
	NewMIDI,
	NewSynth,
	Clone,
	Load,
	LoadSynth,
	Save,
	Export,
	Remove,
	Replace,
	SetName,
	SetSynthesizer,
	Synth
};

void SourceComponent::showSourceMenu() {
	auto menu = this->createSourceMenu();
	auto result = (SourceActionType)(menu.show());

	switch (result) {
	case SourceActionType::NewAudio:
		CoreActions::newAudioSourceGUI();
		break;
	case SourceActionType::NewMIDI:
		CoreActions::newMIDISourceGUI();
		break;
	case SourceActionType::NewSynth:
		CoreActions::newSynthSourceGUI();
		break;
	case SourceActionType::Clone:
		CoreActions::cloneSource(this->index);
		break;
	case SourceActionType::Load:
		CoreActions::loadSourceGUI();
		break;
	case SourceActionType::LoadSynth:
		CoreActions::loadSynthSourceGUI();
		break;
	case SourceActionType::Save:
		CoreActions::saveSourceGUI(this->index);
		break;
	case SourceActionType::Export:
		CoreActions::exportSourceGUI(this->index);
		break;
	case SourceActionType::Remove:
		CoreActions::removeSourceGUI(this->index);
		break;
	case SourceActionType::Replace:
		CoreActions::reloadSourceGUI(this->index);
		break;
	case SourceActionType::SetName:
		CoreActions::setSourceNameGUI(this->index);
		break;
	case SourceActionType::SetSynthesizer:
		CoreActions::setSourceSynthesizerGUI(this->index);
		break;
	case SourceActionType::Synth:
		CoreActions::synthSourceGUI(this->index);
		break;
	}
}

void SourceComponent::startDrag() {
	if (auto container = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
		container->startDragging(this->getDragSourceDescription(),
			this, juce::ScaledImage{}, true);
	}
}

juce::PopupMenu SourceComponent::createSourceMenu() const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("New"), this->createNewMenu());
	menu.addItem(SourceActionType::Load, TRANS("Load"));
	menu.addItem(SourceActionType::LoadSynth, TRANS("Load(Synth)"));
	menu.addItem(SourceActionType::Save, TRANS("Save"),
		(!this->isSynthing) && (!this->isIOTask));
	menu.addItem(SourceActionType::Export, TRANS("Export"),
		(this->type == (int)(quickAPI::SourceType::SynthSource))
		&& (!this->isSynthing) && (!this->isIOTask));
	menu.addItem(SourceActionType::Clone, TRANS("Clone"),
		(!this->isSynthing) && (!this->isIOTask));
	menu.addItem(SourceActionType::Remove, TRANS("Remove"), 
		(!this->isSynthing) && (!this->isIOTask));
	menu.addItem(SourceActionType::Replace, TRANS("Replace"),
		(!this->isSynthing) && (!this->isIOTask));
	menu.addSeparator();
	menu.addItem(SourceActionType::SetName, TRANS("Set Name"));
	menu.addItem(SourceActionType::SetSynthesizer, TRANS("Set Synthesizer"),
		(this->type == (int)(quickAPI::SourceType::SynthSource))
		&& (!this->isSynthing) && (!this->isIOTask));
	menu.addItem(SourceActionType::Synth, TRANS("Synth"),
		(this->type == (int)(quickAPI::SourceType::SynthSource))
		&& (!this->isSynthing) && (!this->isIOTask));

	return menu;
}

juce::PopupMenu SourceComponent::createNewMenu() const {
	juce::PopupMenu menu;

	menu.addItem(SourceActionType::NewAudio, TRANS("Audio"));
	menu.addItem(SourceActionType::NewMIDI, TRANS("MIDI"));
	menu.addItem(SourceActionType::NewSynth, TRANS("Synth"));

	return menu;
}

juce::var SourceComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	object->setProperty("type", (int)DragSourceType::Source);
	object->setProperty("name", this->nameEditor->getText());
	object->setProperty("srcType", this->type);
	object->setProperty("length", this->length);

	return juce::var{ object.release() };
}
