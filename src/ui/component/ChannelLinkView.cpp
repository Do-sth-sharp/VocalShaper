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
	int paddingWidth = screenSize.getWidth() * 0.0125;
	int paddingHeight = screenSize.getHeight() * 0.02;
	int titleWidth = screenSize.getWidth() * 0.016;
	int titleHeight = screenSize.getHeight() * 0.03;
	int cellWidth = screenSize.getWidth() * 0.015;
	int cellHeight = cellWidth;

	int width = paddingWidth * 2 + titleWidth + cellWidth * (this->dstChannelNum + 2);
	int height = paddingHeight * 2 + titleHeight + cellHeight * (this->srcChannelNum + 2);

	return { std::max(width, (int)(screenSize.getWidth() * 0.2)),
		std::max(height, (int)(screenSize.getHeight() * 0.3)) };
}

void ChannelLinkViewContent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0125;
	int paddingHeight = screenSize.getHeight() * 0.02;
	int titleWidth = screenSize.getWidth() * 0.016;
	int titleHeight = screenSize.getHeight() * 0.03;
	int cellWidth = screenSize.getWidth() * 0.015;
	int cellHeight = cellWidth;

	float linkPaddingWidth = screenSize.getWidth() * 0.005;
	float linkPaddingHeight = linkPaddingWidth;

	float lineThickness = screenSize.getHeight() * 0.0015;

	float titleFontHeight = screenSize.getHeight() * 0.025;
	float textFontHeight = screenSize.getHeight() * 0.02;

	int tableWidth = this->getWidth() - paddingWidth * 2 - titleWidth;
	int tableHeight = this->getHeight() - paddingHeight * 2 - titleHeight;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour titleColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);
	juce::Colour textColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour lineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);
	juce::Colour hoveredColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);
	juce::Colour linkedColor = laf.findColour(
		juce::Label::ColourIds::outlineWhenEditingColourId);

	/** Fonts */
	juce::Font titleFont(titleFontHeight);
	juce::Font textFont(textFontHeight);

	/** H Title */
	juce::Rectangle<int> hTitleRect(
		paddingWidth + titleWidth, paddingHeight,
		tableWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->dstName, hTitleRect,
		juce::Justification::centred, 1, 0.f);

	/** V Title */
	juce::Rectangle<int> vTitleRect(
		paddingWidth, paddingHeight + titleHeight,
		titleWidth, tableHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.saveState();
	g.addTransform(juce::AffineTransform{
		0, 1, (float)(vTitleRect.getX()),
		-1, 0, (float)(vTitleRect.getBottom())
		});
	g.drawFittedText(this->srcName,
		vTitleRect.withZeroOrigin().transformedBy(
			juce::AffineTransform{
				0, 1, 0,
				1, 0, 0 }),
		juce::Justification::centred, 1, 0.f);
	g.restoreState();

	/** Table Rect */
	juce::Rectangle<float> tableRect(
		paddingWidth + titleWidth, paddingHeight + titleHeight,
		tableWidth, tableHeight);
	g.setColour(lineColor);
	g.drawRect(tableRect, lineThickness);

	/** H Line */
	g.setColour(lineColor);
	int dstChannelPerBus = this->dstChannels.size();
	for (int i = 0; i <= this->dstChannelNum + 2; i++) {
		if (i >= 2) {
			bool longLine = ((i - 2) % dstChannelPerBus) == 0;
			juce::Rectangle<float> lineRect(
				paddingWidth + titleWidth + i * cellWidth - lineThickness / 2,
				paddingHeight + titleHeight + (longLine ? 0 : cellHeight),
				lineThickness,
				(this->srcChannelNum + 2) * cellHeight - (longLine ? 0 : cellHeight));
			g.fillRect(lineRect);
		}
	}

	/** V Line */
	g.setColour(lineColor);
	int srcChannelPerBus = this->srcChannels.size();
	for (int i = 0; i <= this->srcChannelNum + 2; i++) {
		if (i >= 2) {
			bool longLine = ((i - 2) % srcChannelPerBus) == 0;
			juce::Rectangle<float> lineRect(
				paddingWidth + titleWidth + (longLine ? 0 : cellWidth),
				paddingHeight + titleHeight + i * cellHeight - lineThickness / 2,
				(this->dstChannelNum + 2) * cellWidth - (longLine ? 0 : cellWidth),
				lineThickness);
			g.fillRect(lineRect);
		}
	}

	/** Table Head */
	g.setColour(lineColor);
	juce::Line<float> headLine(
		paddingWidth + titleWidth, paddingHeight + titleHeight,
		paddingWidth + titleWidth + 2 * cellWidth,
		paddingHeight + titleHeight + 2 * cellHeight);
	g.drawLine(headLine, lineThickness);

	/** H Bus ID */
	g.setColour(textColor);
	g.setFont(textFont);
	int dstBusNum = this->dstChannelNum / dstChannelPerBus;
	for (int i = 0; i < dstBusNum; i++) {
		juce::Rectangle<int> textRect(
			paddingWidth + titleWidth + cellWidth * 2 + i * cellWidth * dstChannelPerBus,
			paddingHeight + titleHeight, cellWidth * dstChannelPerBus, cellHeight);
		g.drawFittedText(juce::String{ i }, textRect,
			juce::Justification::centred, 1, 0.f);
	}

	/** V Bus ID */
	g.setColour(textColor);
	g.setFont(textFont);
	int srcBusNum = this->srcChannelNum / srcChannelPerBus;
	for (int i = 0; i < srcBusNum; i++) {
		juce::Rectangle<int> textRect(paddingWidth + titleWidth,
			paddingHeight + titleHeight + cellHeight * 2 + i * cellHeight * srcChannelPerBus,
			cellWidth, cellHeight * srcChannelPerBus);
		g.drawFittedText(juce::String{ i }, textRect,
			juce::Justification::centred, 1, 0.f);
	}

	/** H Channel Name */
	g.setColour(textColor);
	g.setFont(textFont);
	for (int i = 0; i < this->dstChannelNum + 2; i++) {
		if (i >= 2) {
			juce::String name =
				juce::AudioChannelSet::getAbbreviatedChannelTypeName(
					this->dstChannels.getTypeOfChannel((i - 2) % dstChannelPerBus));
			juce::Rectangle<int> textRect(
				paddingWidth + titleWidth + cellWidth * i,
				paddingHeight + titleHeight + cellWidth,
				cellWidth, cellHeight);
			g.drawFittedText(name, textRect,
				juce::Justification::centred, 1, 0.f);
		}
	}

	/** V Channel Name */
	g.setColour(textColor);
	g.setFont(textFont);
	for (int i = 0; i < this->srcChannelNum + 2; i++) {
		if (i >= 2) {
			juce::String name =
				juce::AudioChannelSet::getAbbreviatedChannelTypeName(
					this->srcChannels.getTypeOfChannel((i - 2) % srcChannelPerBus));
			juce::Rectangle<int> textRect(
				paddingWidth + titleWidth + cellWidth,
				paddingHeight + titleHeight + cellHeight * i,
				cellWidth, cellHeight);
			g.drawFittedText(name, textRect,
				juce::Justification::centred, 1, 0.f);
		}
	}

	/** Link */
	auto& [hoverSrc, hoverDst] = this->hovered;
	for (int i = 0; i < this->dstChannelNum; i++) {
		for (int j = 0; j < this->srcChannelNum; j++) {
			bool linked = this->checkLink(j, i);
			bool hovered = (hoverSrc == j) && (hoverDst == i);

			if (hovered) {
				juce::Rectangle<float> backRect(
					paddingWidth + titleWidth + cellWidth * 2 + cellWidth * i + lineThickness / 2,
					paddingHeight + titleHeight + cellHeight * 2 + cellHeight * j + lineThickness / 2,
					cellWidth - lineThickness, cellHeight - lineThickness);
				g.setColour(hoveredColor);
				g.fillRect(backRect);
			}
			if (linked) {
				juce::Rectangle<float> frontRect(
					paddingWidth + titleWidth + cellWidth * 2 + cellWidth * i + linkPaddingWidth,
					paddingHeight + titleHeight + cellHeight * 2 + cellHeight * j + linkPaddingHeight,
					cellWidth - linkPaddingWidth * 2, cellHeight - linkPaddingHeight * 2);
				g.setColour(linkedColor);
				g.fillEllipse(frontRect);
			}
		}
	}
}

void ChannelLinkViewContent::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0125;
	int paddingHeight = screenSize.getHeight() * 0.02;
	int titleWidth = screenSize.getWidth() * 0.016;
	int titleHeight = screenSize.getHeight() * 0.03;
	int cellWidth = screenSize.getWidth() * 0.015;
	int cellHeight = cellWidth;

	int left = paddingWidth + titleWidth + cellWidth * 2;
	int right = left + cellWidth * this->dstChannelNum;
	int top = paddingHeight + titleHeight + cellHeight * 2;
	int bottom = top + cellHeight * this->srcChannelNum;

	int x = event.position.getX(), y = event.position.getY();

	if (x >= left && x < right && y >= top && y < bottom) {
		this->hovered = { (y - top) / cellHeight, (x - left) / cellWidth };
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	}
	else {
		this->hovered = { -1, -1 };
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
	}

	this->repaint();
}

void ChannelLinkViewContent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void ChannelLinkViewContent::mouseExit(const juce::MouseEvent& /*event*/) {
	this->hovered = { -1, -1 };
	this->setMouseCursor(juce::MouseCursor::NormalCursor);
	this->repaint();
}

void ChannelLinkViewContent::mouseUp(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingWidth = screenSize.getWidth() * 0.0125;
	int paddingHeight = screenSize.getHeight() * 0.02;
	int titleWidth = screenSize.getWidth() * 0.016;
	int titleHeight = screenSize.getHeight() * 0.03;
	int cellWidth = screenSize.getWidth() * 0.015;
	int cellHeight = cellWidth;

	int left = paddingWidth + titleWidth + cellWidth * 2;
	int right = left + cellWidth * this->dstChannelNum;
	int top = paddingHeight + titleHeight + cellHeight * 2;
	int bottom = top + cellHeight * this->srcChannelNum;

	int x = event.position.getX(), y = event.position.getY();

	if (x >= left && x < right && y >= top && y < bottom) {
		int src = (y - top) / cellHeight;
		int dst = (x - left) / cellWidth;

		bool linked = this->checkLink(src, dst);
		if (event.mods.isLeftButtonDown() && (!linked)) {
			this->setLink(src, dst, true);
			this->repaint();
		}
		else if (event.mods.isRightButtonDown() && linked) {
			this->setLink(src, dst, false);
			this->repaint();
		}
	}
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
