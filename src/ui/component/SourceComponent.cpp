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

	/** Synthesizer Button */
	this->synthButtonHeader = TRANS("Synthesizer:");
	this->synthButtonEmptyStr = TRANS("Empty");

	this->synthesizerButton = std::make_unique<juce::TextButton>(TRANS("Synthesizer"));
	this->synthesizerButton->setWantsKeyboardFocus(false);
	this->synthesizerButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->synthesizerButton->onClick = [this] { this->showSynthesizerMenu(); };
	this->addAndMakeVisible(this->synthesizerButton.get());

	/** Dst */
	this->dst = std::make_unique<SourceDstComponent>();
	this->addAndMakeVisible(this->dst.get());
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

	int synthLineHeight = screenSize.getHeight() * 0.02;

	/** Name Line */
	juce::Rectangle<int> nameLineArea(
		selectBarWidth + paddingWidth, paddingHeight,
		this->getWidth() - paddingWidth * 2 - selectBarWidth, nameLineHeight);

	/** Name */
	auto nameRect = nameLineArea.withTrimmedLeft(idAreaWidth + splitWidth);
	this->nameEditor->setBounds(nameRect);

	/** Dst */
	juce::Rectangle<int> dstRect(
		selectBarWidth + paddingWidth, nameLineArea.getBottom() + splitHeight,
		synthLineHeight, synthLineHeight);
	this->dst->setBounds(dstRect);

	/** Synthesizer */
	juce::Rectangle<int> synthRect(
		selectBarWidth + paddingWidth + synthLineHeight + splitWidth, nameLineArea.getBottom() + splitHeight,
		nameLineArea.getWidth() - synthLineHeight - splitWidth, synthLineHeight);
	this->synthesizerButton->setBounds(synthRect);
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

	int synthLineHeight = screenSize.getHeight() * 0.02;

	int infoLineHeight = screenSize.getHeight() * 0.02;
	float infoFontHeight = screenSize.getHeight() * 0.015;

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
	juce::Colour infoTextColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font idFont(idFontHeight);
	juce::Font infoFont(infoFontHeight);

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

	/** Synthesizer */
	juce::Rectangle<int> synthRect(
		selectBarWidth + paddingWidth, nameLineArea.getBottom() + splitHeight,
		nameLineArea.getWidth(), synthLineHeight);

	/** Info */
	juce::Rectangle<int> infoLineArea(
		selectBarWidth + paddingWidth, synthRect.getBottom() + splitHeight,
		nameLineArea.getWidth(), infoLineHeight);
	g.setColour(infoTextColor);
	g.setFont(infoFont);
	g.drawFittedText(this->infoStr, infoLineArea,
		juce::Justification::centredLeft, 1, 1.f);
}

void SourceComponent::update(int index, bool selected) {
	this->selected = selected;
	if (index >= 0 && index < quickAPI::getSourceNum()) {
		this->index = index;
		this->id = quickAPI::getSourceId(index);
		this->dstIndex = quickAPI::getSourceSynthDstIndex(index);
		this->dstID = quickAPI::getSourceId(this->dstIndex);
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
		this->isRecording = quickAPI::checkSourceRecordingNow(index);

		this->synthesizerButton->setButtonText(
			this->synthButtonHeader + " " +
			(this->synthesizer.isNotEmpty() ? this->synthesizer : this->synthButtonEmptyStr));

		this->dst->update(this->index, this->dstIndex);

		this->infoStr = TRANS(this->typeName) + ", ";
		this->infoStr += (utils::createTimeString(utils::splitTime(this->length)) + ", ");
		if (this->isRecording) {
			this->infoStr += TRANS("Recording");
		}
		else if (this->isIOTask) {
			this->infoStr += TRANS("IO Running");
		}
		else if (this->isSynthing) {
			this->infoStr += TRANS("Synthing");
		}
		else {
			this->infoStr += TRANS("Ready");
		}

		this->setTooltip(this->createTooltip());
	}

	this->repaint();
}

bool SourceComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		if (!des["instrument"]) { return false; }
		return true;
	}

	/** From Source Synth */
	if ((int)(des["type"]) == (int)(DragSourceType::SourceSynth)) {
		int srcIndex = des["src"];
		return (srcIndex >= 0) && (srcIndex != this->index);
	}

	return false;
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

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		juce::String pid = des["id"].toString();

		CoreActions::setSourceSynthesizer(this->index, pid);
		return;
	}
	
	/** From Source Synth */
	if ((int)(des["type"]) == (int)(DragSourceType::SourceSynth)) {
		int srcIndex = des["src"];
		
		CoreActions::setSourceSynthDst(srcIndex, this->index);
		return;
	}
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
	Clone,
	Load,
	Save,
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
	case SourceActionType::Clone:
		CoreActions::cloneSource(this->index);
		break;
	case SourceActionType::Load:
		CoreActions::loadSourceGUI();
		break;
	case SourceActionType::Save:
		CoreActions::saveSourceGUI(this->index);
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

void SourceComponent::showSynthesizerMenu() {
	/** Callback */
	auto callback = [index = this->index](const juce::PluginDescription& plugin) {
		CoreActions::setSourceSynthesizer(index, plugin.createIdentifierString());
		};

	/** Create Menu */
	auto [valid, list] = quickAPI::getPluginList(true, true);
	if (!valid) { return; }
	auto groups = utils::groupPlugin(list, utils::PluginGroupType::Category);
	auto menu = utils::createPluginMenu(groups, callback);

	/** Show Menu */
	menu.showAt(this->synthesizerButton.get());
}

juce::PopupMenu SourceComponent::createSourceMenu() const {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("New"), this->createNewMenu());
	menu.addItem(SourceActionType::Load, TRANS("Load"));
	menu.addItem(SourceActionType::Save, TRANS("Save"),
		(!this->isSynthing) && (!this->isIOTask) && (!this->isRecording));
	menu.addItem(SourceActionType::Clone, TRANS("Clone"),
		(!this->isSynthing) && (!this->isIOTask) && (!this->isRecording));
	menu.addItem(SourceActionType::Remove, TRANS("Remove"), 
		(!this->isSynthing) && (!this->isIOTask) && (!this->isRecording));
	menu.addItem(SourceActionType::Replace, TRANS("Replace"),
		(!this->isSynthing) && (!this->isIOTask) && (!this->isRecording));
	menu.addSeparator();
	menu.addItem(SourceActionType::SetName, TRANS("Set Name"));
	menu.addItem(SourceActionType::SetSynthesizer, TRANS("Set Synthesizer"),
		(!this->isSynthing) && (!this->isIOTask));
	menu.addItem(SourceActionType::Synth, TRANS("Synth"),
		(!this->isSynthing) && (!this->isIOTask) && (!this->isRecording)
	&& (!quickAPI::checkSourceIOTask(this->dstIndex))
	&& (!quickAPI::checkSourceSynthing(this->dstIndex)));

	return menu;
}

juce::PopupMenu SourceComponent::createNewMenu() const {
	juce::PopupMenu menu;

	menu.addItem(SourceActionType::NewAudio, TRANS("Audio"));
	menu.addItem(SourceActionType::NewMIDI, TRANS("MIDI"));

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

juce::String SourceComponent::createTooltip() const {
	juce::String result =
		"#" + juce::String{ this->id } + " " + this->nameEditor->getText() + "\n"
		+ TRANS("Type:") + " " + this->typeName + "\n"
		+ TRANS("Length:") + " " + juce::String{ this->length } + "s\n";

	if (this->type == quickAPI::SourceType::AudioSource) {
		result += (TRANS("Channels:") + " " + juce::String{ this->channels } + "\n");
	}
	if (this->type == quickAPI::SourceType::MIDISource) {
		result += (TRANS("Tracks:") + " " + juce::String{ this->tracks } + "\n");
	}
	if (this->type == quickAPI::SourceType::MIDISource) {
		result += (TRANS("Events:") + " " + juce::String{ this->events } + "\n");
	}
	result += (TRANS("Synthesizer:") + " " + this->synthesizer + "\n");
	result += (TRANS("Synth Destination:") + ((this->dstID >= 0) ? (" #" + juce::String{ this->dstID }) : "-") + "\n");
	result += (TRANS("Sample Rate:") + " " + juce::String{ this->sampleRate } + "Hz\n");

	return result;
}
