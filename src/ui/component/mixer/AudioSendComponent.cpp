#include "AudioSendComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../misc/DragSourceType.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

AudioSendComponent::AudioSendComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::Send));

	/** Channel Icon */
	this->channelIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Editor", "node-tree").getFullPathName());
	this->channelIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

	/** Channel Button */
	this->channelButton = std::make_unique<juce::DrawableButton>(
		TRANS("Channel Send"), juce::DrawableButton::ImageOnButtonBackground);
	this->channelButton->setImages(this->channelIcon.get());
	this->channelButton->setWantsKeyboardFocus(false);
	this->channelButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->channelButton->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->channelButton->onClick = [this] { this->editSend(); };
	this->addAndMakeVisible(this->channelButton.get());

	/** Mouse Cursor */
	this->setMouseCursor(juce::MouseCursor::CopyingCursor);
}

void AudioSendComponent::resized() {
	/** Size */
	int buttonHeight = this->getHeight();
	int buttonWidth = buttonHeight;

	int right = this->getWidth();

	/** Channel Button */
	juce::Rectangle<int> channelRect(
		right - buttonWidth, 0,
		buttonWidth, buttonHeight);
	this->channelButton->setBounds(channelRect);
	right -= buttonWidth;
}

void AudioSendComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int textPaddingWidth = screenSize.getWidth() * 0.0025;
	float textHeight = this->getHeight() * 0.8;

	int buttonHeight = this->getHeight();
	int buttonWidth = buttonHeight;

	int right = this->getWidth();
	right -= buttonWidth;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(juce::FontOptions{ textHeight });

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Text */
	if (this->valid) {
		juce::Rectangle<int> textRect(
			textPaddingWidth, 0,
			right - textPaddingWidth * 2, this->getHeight());
		g.setColour(textColor);
		g.setFont(textFont);
		g.drawFittedText(this->name, textRect,
			juce::Justification::centredLeft, 1, 0.75f);
	}
}

void AudioSendComponent::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float splitLineThickness = screenSize.getHeight() * 0.0015;
	float splitLineLength = this->getWidth() * 0.8;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour splitLineColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);

	/** Split */
	juce::Rectangle<float> topSplit(
		this->getWidth() / 2.0 - splitLineLength / 2,
		-splitLineThickness / 2, splitLineLength, splitLineThickness);
	juce::Rectangle<float> bottomSplit(
		this->getWidth() / 2.0 - splitLineLength / 2,
		this->getHeight() - splitLineThickness / 2, splitLineLength, splitLineThickness);
	g.setColour(splitLineColor);
	g.fillRect(topSplit);
	g.fillRect(bottomSplit);
}

void AudioSendComponent::update(int type, int track, int slot) {
	this->type = type;
	this->track = track;
	this->slot = slot;

	this->valid = quickAPI::getTrackAudioSendDst(
		{ (quickAPI::TrackType)type, track }, slot).second >= 0;

	this->channelButton->setEnabled(this->valid);

	this->name = this->getSendName();

	this->setMouseCursor(this->valid
		? juce::MouseCursor::PointingHandCursor
		: juce::MouseCursor::CopyingCursor);

	this->repaint();

	this->setTooltip(this->createToolTip());
}

void AudioSendComponent::mouseUp(const juce::MouseEvent& event) {
	if (this->valid) {
		if (event.mods.isLeftButtonDown()) {
			if (!event.mouseWasDraggedSinceMouseDown()) {
				this->editSend();
			}
		}
		else if (event.mods.isRightButtonDown()) {
			this->showMenu();
		}
	}
	else {
		if (!event.mouseWasDraggedSinceMouseDown()) {
			this->showAddMenu();
		}
	}
}

void AudioSendComponent::mouseDrag(const juce::MouseEvent& event) {
	/** Start Drag */
	if (event.mods.isLeftButtonDown()) {
		this->startDrag();
	}
}

const juce::String AudioSendComponent::getSendName() const {
	return quickAPI::getTrackAudioSendDstName(
		{ (quickAPI::TrackType)this->type, this->track }, this->slot);
}

void AudioSendComponent::showAddMenu() {
	/** Callback */
	auto addCallback = [comp = juce::Component::SafePointer{ this }](int type, int index) {
		if (comp) {
			comp->addSend(type, index);
		}
		};

	/** Create Menu */
	auto menu = this->createAddMenu(addCallback);
	[[maybe_unused]] int result = menu.show();
}

void AudioSendComponent::showMenu() {
	/** Callback */
	auto addCallback = [comp = juce::Component::SafePointer{ this }](int type, int index) {
		if (comp) {
			comp->addSend(type, index);
		}
		};
	auto editCallback = [comp = juce::Component::SafePointer{ this }]() {
		if (comp) {
			comp->editSend();
		}
		};
	auto removeCallback = [comp = juce::Component::SafePointer{ this }]() {
		if (comp) {
			comp->removeSend();
		}
		};

	/** Create Menu */
	auto menu = this->createMenu(
		addCallback, editCallback, removeCallback);
	[[maybe_unused]] int result = menu.show();
}

void AudioSendComponent::startDrag() {
	if (auto container = juce::DragAndDropContainer::findParentDragContainerFor(this)) {
		container->startDragging(this->getDragSourceDescription(),
			this, juce::ScaledImage{}, true);
	}
}

void AudioSendComponent::addSend(int type, int index) {
	CoreActions::setTrackAudioSendGUI(
		(quickAPI::TrackType)this->type, this->track, this->slot,
		{ (quickAPI::SendDstType)type, index });
}

void AudioSendComponent::editSend() {
	auto dst = quickAPI::getTrackAudioSendDst(
		{ (quickAPI::TrackType)this->type, this->track }, this->slot);
	CoreActions::setTrackAudioSendGUI(
		(quickAPI::TrackType)this->type, this->track, this->slot, dst);
}

void AudioSendComponent::removeSend() {
	CoreActions::removeTrackAudioSendOnSlot(
		(quickAPI::TrackType)this->type, this->track, this->slot);
}

juce::var AudioSendComponent::getDragSourceDescription() const {
	auto object = std::make_unique<juce::DynamicObject>();

	object->setProperty("type", (int)DragSourceType::TrackAudioSend);
	object->setProperty("trackType", this->type);
	object->setProperty("track", this->track);
	object->setProperty("slot", this->slot);

	return juce::var{ object.release() };
}

juce::String AudioSendComponent::createToolTip() const {
	juce::String result;

	if (this->valid) {
		auto dst = quickAPI::getTrackAudioSendDst(
			{ (quickAPI::TrackType)this->type, this->track }, this->slot);

		result = TRANS(quickAPI::getSendTypeName(dst.first)) + " #" + juce::String{ dst.second } + "\n"
			+ this->name + "\n"
			+ TRANS("Channels:") + "\n";

		auto channels = quickAPI::getTrackAudioSendChannels(
			{ (quickAPI::TrackType)this->type, this->track }, this->slot);
		for (auto& i : channels) {
			result += "  " + juce::String{ i.first } + " - " + juce::String{ i.second } + "\n";
		}
	}

	return result;
}

juce::PopupMenu AudioSendComponent::createMenu(
	const std::function<void(int, int)>& addCallback,
	const std::function<void()>& editCallback,
	const std::function<void()>& removeCallback) const {
	/** Current Dst */
	auto dst = quickAPI::getTrackAudioSendDst(
		{ (quickAPI::TrackType)this->type, this->track }, this->slot);

	/** Menu */
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("Destination"),
		this->createAddMenu(addCallback), true);
	menu.addItem(TRANS("Channel Send"),
		dst.second >= 0, false, editCallback);
	menu.addItem(TRANS("Unlink"),
		dst.second >= 0, false, removeCallback);

	return menu;
}

juce::PopupMenu AudioSendComponent::createAddMenu(
	const std::function<void(int, int)>& addCallback) const {
	/** Current Dst */
	auto dst = quickAPI::getTrackAudioSendDst(
		{ (quickAPI::TrackType)this->type, this->track }, this->slot);

	/** Menu */
	juce::PopupMenu menu;

	/** Device */
	menu.addItem(TRANS("Output"), true,
		dst.second >= 0 && dst.first == quickAPI::SendDstType::ToDevice,
		std::bind(addCallback, (int)quickAPI::SendDstType::ToDevice, 0));

	/** Master */
	menu.addSeparator();
	menu.addItem(TRANS("Master Track"), this->type != (int)quickAPI::TrackType::MasterTrack,
		dst.second >= 0 && dst.first == quickAPI::SendDstType::ToMaster,
		std::bind(addCallback, (int)quickAPI::SendDstType::ToMaster, 0));

	/** AUX */
	menu.addSeparator();
	int auxTrackNum = quickAPI::getTrackNum(quickAPI::TrackType::AuxTrack);
	for (int i = 0; i < auxTrackNum; i++) {
		auto name = quickAPI::getTrackName(
			{ quickAPI::TrackType::AuxTrack, i });
		if (name.isEmpty()) {
			name = TRANS("AUX Track") + " #" + juce::String{ i };
		}
		menu.addItem(name,
			this->type != (int)quickAPI::TrackType::MasterTrack,
			dst.second == i && dst.first == quickAPI::SendDstType::ToAUX,
			std::bind(addCallback, (int)quickAPI::SendDstType::ToAUX, i));
	}

	return menu;
}
