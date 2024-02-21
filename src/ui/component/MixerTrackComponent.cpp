#include "MixerTrackComponent.h"
#include "../misc/CoreActions.h"
#include "../misc/DragSourceType.h"
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

	/** Knob */
	this->gainKnob = std::make_unique<KnobBase>(
		TRANS("Gain"), 0, -10, 10, 1);
	this->gainKnob->onChange = [this](double value) {
		CoreActions::setTrackGain(this->index, (float)value);
		};
	this->addAndMakeVisible(this->gainKnob.get());

	this->panKnob = std::make_unique<KnobBase>(
		TRANS("Pan"), 0, -1, 1, 2);
	this->panKnob->onChange = [this](double value) {
		CoreActions::setTrackPan(this->index, (float)value);
		};
	this->addAndMakeVisible(this->panKnob.get());

	/** Fader */
	this->fader = std::make_unique<FaderBase>(
		1.0, juce::Array<double>{ 6.0, 0.0, -5.0, -10.0, -15.0, -20.0, -30.0, -40.0, -60.0, -100.0 },
		0.0, 2.0, 0);
	this->fader->onChange = [this](double value) {
		CoreActions::setTrackFader(this->index, (float)value);
		};
	this->addAndMakeVisible(this->fader.get());

	/** Level Meter */
	this->levelMeter = std::make_unique<MixerTrackLevelMeter>();
	this->addAndMakeVisible(this->levelMeter.get());

	/** Mute */
	this->muteButton = std::make_unique<MixerTrackMuteComponent>(
		[this] { this->showMenu(); });
	this->addAndMakeVisible(this->muteButton.get());

	/** Effect List */
	this->effectListModel = std::make_unique<EffectListModel>();
	this->effectList = std::make_unique<juce::ListBox>(
		TRANS("Fx List"), this->effectListModel.get());
	this->addAndMakeVisible(this->effectList.get());
}

void MixerTrackComponent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int colorHeight = screenSize.getHeight() * 0.015;

	int sideChainHeight = screenSize.getHeight() * 0.02;
	int sideChainHideHeight = screenSize.getHeight() * 0.4;
	bool sideChainShown = this->getHeight() >= sideChainHideHeight;

	int ioHeight = screenSize.getHeight() * 0.02;
	int ioWidth = ioHeight;
	int ioHideHeight = screenSize.getHeight() * 0.35;
	bool ioShown = this->getHeight() >= ioHideHeight;

	int knobPaddingWidth = screenSize.getWidth() * 0.0025;
	int knobHeight = screenSize.getHeight() * 0.075;
	int knobWidth = (this->getWidth()- knobPaddingWidth * 2) / (this->panValid ? 2 : 1);
	int knobHideHeight = screenSize.getHeight() * 0.3;
	bool knobShown = this->getHeight() >= knobHideHeight;

	int faderPaddingWidth = screenSize.getWidth() * 0.0025;
	int faderHeight = screenSize.getHeight() * 0.15;
	int faderWidth = (this->getWidth() - faderPaddingWidth * 2) / 2;
	int faderHideHeight = screenSize.getHeight() * 0.3;
	bool faderShown = this->getHeight() >= faderHideHeight;

	int muteHeight = screenSize.getHeight() * 0.035;
	int muteHideHeight = screenSize.getHeight() * 0.25;
	bool muteShown = this->getHeight() >= muteHideHeight;

	int listItemHeight = screenSize.getHeight() * 0.02;

	int top = 0, bottom = this->getHeight();
	top += colorHeight;

	/** Side Chain */
	if (sideChainShown) {
		juce::Rectangle<int> sideChainRect(
			0, top, this->getWidth(), sideChainHeight);
		this->sideChain->setBounds(sideChainRect);

		top += sideChainHeight;
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

	/** Knob */
	if (knobShown) {
		juce::Rectangle<int> gainRect(
			knobPaddingWidth, top,
			knobWidth, knobHeight);
		this->gainKnob->setBounds(gainRect);

		if (this->panValid) {
			juce::Rectangle<int> panRect(
				this->getWidth() - knobPaddingWidth - knobWidth, top,
				knobWidth, knobHeight);
			this->panKnob->setBounds(panRect);
		}

		top += knobHeight;
	}
	this->gainKnob->setVisible(knobShown);
	this->panKnob->setVisible(knobShown && this->panValid);

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

	/** Fader And Level Meter */
	if (faderShown) {
		juce::Rectangle<int> faderRect(
			faderPaddingWidth, bottom - faderHeight,
			faderWidth, faderHeight);
		this->fader->setBounds(faderRect);

		juce::Rectangle<int> levelRect(
			this->getWidth() - faderPaddingWidth - faderWidth, bottom - faderHeight,
			faderWidth, faderHeight);
		this->levelMeter->setBounds(levelRect);

		bottom -= faderHeight;
	}
	this->fader->setVisible(faderShown);
	this->levelMeter->setVisible(faderShown);

	/** Mute */
	if (muteShown) {
		juce::Rectangle<int> muteRect(
			0, bottom - muteHeight,
			this->getWidth(), muteHeight);
		this->muteButton->setBounds(muteRect);

		bottom -= muteHeight;
	}
	this->muteButton->setVisible(muteShown);

	/** Effects */
	juce::Rectangle<int> effectRect(
		0, top, this->getWidth(), bottom - top);
	this->effectList->setBounds(effectRect);
	this->effectList->setRowHeight(listItemHeight);
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
	int dropLinePaddingWidth = screenSize.getWidth() * 0.001;
	float dropLineThickness = screenSize.getHeight() * 0.0025;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour outlineColor = laf.findColour(this->dragHovered
		? juce::Label::ColourIds::outlineWhenEditingColourId
		: juce::Label::ColourIds::outlineColourId);
	juce::Colour dropLineColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);

	/** Outline */
	auto totalRect = this->getLocalBounds();
	g.setColour(outlineColor);
	g.drawRect(totalRect, outlineThickness);

	/** Effect Rect */
	auto effectRect = this->effectList->getBounds();
	g.setColour(backgroundColor);
	g.drawRect(effectRect, outlineThickness);

	/** Drop Line */
	auto& [dropIndex, dropYPos] = this->dropItemState;
	if (dropIndex > -1) {
		int dropYPosLocal = dropYPos + this->effectList->getY();
		juce::Rectangle<float> lineRect(
			this->effectList->getX() + dropLinePaddingWidth,
			dropYPosLocal - dropLineThickness / 2,
			this->effectList->getWidth() - dropLinePaddingWidth * 2,
			dropLineThickness);
		g.setColour(dropLineColor);
		g.fillRect(lineRect);
	}
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

		this->midiInput->update(index);
		this->audioInput->update(index);
		this->midiOutput->update(index);
		this->audioOutput->update(index);

		this->gainKnob->setValue(quickAPI::getMixerTrackGain(index));
		this->panKnob->setValue(quickAPI::getMixerTrackPan(index));
		this->panValid = quickAPI::isMixerTrackPanValid(index);

		this->levelMeter->update(index);

		this->effectListModel->update(index);
		this->effectList->updateContent();

		this->resized();
		this->repaint();
	}
}

void MixerTrackComponent::updateGain() {
	this->gainKnob->setValue(quickAPI::getMixerTrackGain(this->index));
}

void MixerTrackComponent::updatePan() {
	this->panKnob->setValue(quickAPI::getMixerTrackPan(this->index));
}

void MixerTrackComponent::updateFader() {
	this->fader->setValue(quickAPI::getMixerTrackFader(this->index));
}

void MixerTrackComponent::updateMute() {
	this->muteButton->update(this->index);
}

void MixerTrackComponent::updateEffect(int /*index*/) {
	this->effectListModel->update(this->index);
	this->effectList->updateContent();
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
		else {
			this->showMenu();
		}
	}
	else if (event.mods.isLeftButtonDown()) {
		if (y >= 0 && y < colorHeight) {
			CoreActions::setTrackNameGUI(this->index);
		}
	}
}

bool MixerTrackComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	/** From Instr Audio Output */
	if ((int)(des["type"]) == (int)(DragSourceType::InstrOutput)) {
		int instrIndex = des["instr"];
		return instrIndex >= 0;
	}

	/** From Track Audio Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioInput)) {
		int trackIndex = des["track"];
		return (trackIndex >= 0) && (trackIndex != this->index);
	}

	/** From Track Audio Output */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioOutput)) {
		int trackIndex = des["track"];
		return (trackIndex >= 0) && (trackIndex != this->index);
	}

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		if (des["instrument"]) { return false; }
		return true;
	}

	return false;
}

void MixerTrackComponent::itemDragEnter(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	auto& des = dragSourceDetails.description;
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		this->preEffectDrop(dragSourceDetails.localPosition);
	}
	else {
		this->preDrop();
	}
}

void MixerTrackComponent::itemDragMove(const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		this->endEffectDrop();
		this->preEffectDrop(dragSourceDetails.localPosition);
	}
}

void MixerTrackComponent::itemDragExit(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	auto& des = dragSourceDetails.description;
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		this->endEffectDrop();
	}
	else {
		this->endDrop();
	}
}

void MixerTrackComponent::itemDropped(
	const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }

	auto& des = dragSourceDetails.description;
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		this->endEffectDrop();
	}
	else {
		this->endDrop();
	}

	/** From Instr Audio Output */
	if ((int)(des["type"]) == (int)(DragSourceType::InstrOutput)) {
		int instrIndex = des["instr"];

		this->audioInput->setAudioInputFromInstr(instrIndex, true);
		return;
	}

	/** From Track Audio Input */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioInput)) {
		int trackIndex = des["track"];

		this->audioOutput->setAudioOutputToSend(trackIndex, true);
		return;
	}

	/** From Track Audio Output */
	if ((int)(des["type"]) == (int)(DragSourceType::TrackAudioOutput)) {
		int trackIndex = des["track"];

		this->audioInput->setAudioInputFromSend(trackIndex, true);
		return;
	}

	/** From Plugins */
	if ((int)(des["type"]) == (int)(DragSourceType::Plugin)) {
		juce::String pid = des["id"].toString();

		int index = this->getInsertIndex(dragSourceDetails.localPosition);
		if (index > -1) {
			CoreActions::insertEffect(this->index, index, pid);
		}

		return;
	}
}

void MixerTrackComponent::preDrop() {
	this->dragHovered = true;
	this->repaint();
}

void MixerTrackComponent::endDrop() {
	this->dragHovered = false;
	this->repaint();
}

void MixerTrackComponent::preEffectDrop(const juce::Point<int>& pos) {
	int index = this->getInsertIndex(pos);
	if (index > -1) {
		int yPos = 0;
		int rowNum = this->effectListModel->getNumRows();
		if (index < rowNum) {
			auto rowPos = this->effectList->getRowPosition(index, true);
			yPos = rowPos.getY();
		}
		else {
			auto rowPos = this->effectList->getRowPosition(
				rowNum - 1, true);
			yPos = rowPos.getBottom();
		}

		this->dropItemState = { index, yPos };
		this->repaint();
	}
}

void MixerTrackComponent::endEffectDrop() {
	this->dropItemState = { -1, 0 };
	this->repaint();
}

enum MixerTrackActionType {
	Add = 1, Remove
};

void MixerTrackComponent::showMenu() {
	auto menu = this->createMenu();
	int result = menu.show();

	switch (result) {
	case MixerTrackActionType::Add: {
		this->add();
		break;
	}
	case MixerTrackActionType::Remove: {
		this->remove();
		break;
	}
	}
}

void MixerTrackComponent::add() {
	CoreActions::insertTrackGUI(this->index + 1);
}

void MixerTrackComponent::remove() {
	CoreActions::removeTrackGUI(this->index);
}

int MixerTrackComponent::getInsertIndex(const juce::Point<int>& pos) {
	auto listRect = this->effectList->getBounds();
	if (listRect.contains(pos)) {
		auto posRel = pos - listRect.getTopLeft();
		return this->effectList->
			getInsertionIndexForPosition(posRel.getX(), posRel.getY());
	}
	else {
		return -1;
	}
}

juce::PopupMenu MixerTrackComponent::createMenu() const {
	juce::PopupMenu menu;

	menu.addItem(MixerTrackActionType::Add, TRANS("Add"));
	menu.addItem(MixerTrackActionType::Remove, TRANS("Remove"));

	return menu;
}
