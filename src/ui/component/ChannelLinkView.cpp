#include "ChannelLinkView.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/RCManager.h"
#include "../Utils.h"

ChannelLinkViewContent::ChannelLinkViewContent(
	const std::function<void(int, int, bool)>& callback,
	const juce::Array<std::tuple<int, int>>& initList,
	const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
	int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
	bool initIfEmpty) : callback(callback), srcChannels(srcChannels), dstChannels(dstChannels),
	srcChannelNum(srcChannelNum), dstChannelNum(dstChannelNum), 
	srcName(srcName), dstName(dstName) {
	/** Create Temp */
	this->temp.setRange(0, srcChannelNum * dstChannelNum, false);
	for (auto& [srcc, dstc] : initList) {
		this->temp.setBit(srcc * dstChannelNum + dstc);
	}

	/** Init If Empty */
	if (initIfEmpty && initList.isEmpty()) {
		int channelNum = std::min(srcChannelNum, dstChannelNum);
		for (int i = 0; i < channelNum; i++) {
			this->setLink(i, i, true);
		}
	}

	/** Change Size */
	auto size = this->getPreferedSize();
	this->setBounds(0, 0, size.getX(), size.getY());
}

juce::Point<int> ChannelLinkViewContent::getPreferedSize() const {
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.05;
	int paddingHeight = screenSize.getHeight() * 0.05;
	int titleWidth = screenSize.getWidth() * 0.05;
	int titleHeight = screenSize.getHeight() * 0.05;
	int cellWidth = screenSize.getWidth() * 0.025;
	int cellHeight = cellWidth;

	return { paddingWidth * 2 + titleWidth + cellWidth * (this->dstChannelNum + 2),
	paddingHeight * 2 + titleHeight + cellHeight * (this->srcChannelNum + 2) };
}

void ChannelLinkViewContent::paint(juce::Graphics& g) {
	/** TODO */
}

bool ChannelLinkViewContent::checkLink(int srcc, int dstc) {
	return this->temp[srcc * this->dstChannelNum + dstc];
}

void ChannelLinkViewContent::setLink(int srcc, int dstc, bool link) {
	if (link) {
		this->temp.setBit(srcc * this->dstChannelNum + dstc);
	}
	else {
		this->temp.clearBit(srcc * this->dstChannelNum + dstc);
	}
	this->callback(srcc, dstc, link);
}

ChannelLinkView::ChannelLinkView(
	const std::function<void(int, int, bool)>& callback,
	const juce::Array<std::tuple<int, int>>& initList,
	const juce::AudioChannelSet& srcChannels, const juce::AudioChannelSet& dstChannels,
	int srcChannelNum, int dstChannelNum, const juce::String& srcName, const juce::String& dstName,
	bool initIfEmpty) 
	: DocumentWindow(TRANS("Audio Channel Link"), juce::LookAndFeel::getDefaultLookAndFeel().findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton, true) {
	this->setUsingNativeTitleBar(true);
	this->setResizable(true, false);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forChannelLink());

	/** Content */
	this->content = std::make_unique<juce::Viewport>(TRANS("Channel Link"));
	this->content->setViewedComponent(new ChannelLinkViewContent{
		callback, initList, srcChannels, dstChannels,
		srcChannelNum, dstChannelNum, srcName, dstName, initIfEmpty
		});
	this->content->setScrollOnDragMode(
		juce::Viewport::ScrollOnDragMode::nonHover);
	this->setContentNonOwned(this->content.get(), false);

	/** Icon */
	auto icon = RCManager::getInstance()->loadImage(
		utils::getResourceFile("logo.png"));
	this->setIcon(icon);
	this->getPeer()->setIcon(icon);

	/** Resize */
	this->centreWithSize(800, 600);
	juce::MessageManager::callAsync(
		[comp = juce::Component::SafePointer(this)] {
			if (comp) {
				auto screenSize = utils::getScreenSize(comp);
				int width = screenSize.getWidth() / 2;
				int height = screenSize.getHeight() / 2;

				if (auto content = dynamic_cast<ChannelLinkViewContent*>(
					comp->content->getViewedComponent())) {
					auto size = content->getPreferedSize();
					width = std::min(width, size.getX());
					height = std::min(height, size.getY());
				}

				comp->centreWithSize(width, height);
			}
		}
	);
}

void ChannelLinkView::paint(juce::Graphics& g) {
	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();
}


void ChannelLinkView::closeButtonPressed() {
	this->exitModalState();
}
