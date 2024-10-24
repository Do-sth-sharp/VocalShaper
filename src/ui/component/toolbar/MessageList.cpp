#include "MessageList.h"
#include "../../Utils.h"

MessageList::MessageList() {}

void MessageList::paint(juce::Graphics& g) {
	auto& laf = this->getLookAndFeel();

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int paddingWidth = screenSize.getWidth() * 0.01;
	int paddingHeight = screenSize.getHeight() * 0.015;
	float splitLineHeight = screenSize.getHeight() * 0.001;

	/** Color */
	juce::Colour backgroundColor = laf.findColour(
		juce::TextEditor::ColourIds::backgroundColourId);
	juce::Colour splitColor = laf.findColour(
		juce::TextEditor::ColourIds::outlineColourId);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Items */
	int topPos = 0;
	for (auto& [mes, layout, height] : this->list) {
		/** Draw Text */
		juce::Rectangle<float> textRect(
			paddingWidth, topPos + paddingHeight,
			this->getWidth() - paddingWidth * 2,
			height - paddingHeight * 2);
		layout.draw(g, textRect);

		/** Draw Split Line */
		int bottomPos = topPos + height;
		juce::Rectangle<float> splitRect(
			paddingWidth, bottomPos - splitLineHeight / 2,
			this->getWidth() - paddingWidth * 2, splitLineHeight);
		g.setColour(splitColor);
		g.fillRect(splitRect);

		/** Next Pos */
		topPos = bottomPos;
	}
}

void MessageList::update(const juce::Array<MessageModel::Message>& list) {
	auto& laf = this->getLookAndFeel();

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	int paddingWidth = screenSize.getWidth() * 0.01;
	int paddingHeight = screenSize.getHeight() * 0.015;
	float textHeight = screenSize.getHeight() * 0.02;

	/** Color */
	juce::Colour timeColor = laf.findColour(
		juce::TextEditor::ColourIds::shadowColourId + 1);
	juce::Colour textColor = laf.findColour(
		juce::TextEditor::ColourIds::textColourId);

	/** Font */
	juce::Font textFont(juce::FontOptions{ textHeight });

	/** Messages */
	this->list.clear();
	for (auto& i : list) {
		/** Get Data */
		auto& [time, mes, callback] = i;

		/** Text Attributed */
		juce::AttributedString atStr;
		atStr.setFont(textFont);
		atStr.setJustification(juce::Justification::topLeft);
		atStr.setWordWrap(juce::AttributedString::WordWrap::byWord);

		/** Time */
		atStr.append(time.formatted("%H:%M:%S "), textFont, timeColor);

		/** Message */
		atStr.append(mes, textFont, textColor);

		/** Layout */
		juce::TextLayout layout;
		layout.createLayoutWithBalancedLineLengths(
			atStr, this->getWidth() - paddingWidth * 2);

		/** Add To Temp */
		this->list.insert(0, { i, layout, (int)(layout.getHeight()) + paddingHeight * 2 });
	}

	/** Repaint */
	this->repaint();
}

void MessageList::refresh() {
	/** Get Messages */
	juce::Array<MessageModel::Message> listTemp;
	for (auto& [mes, layout, height] : this->list) {
		listTemp.add(mes);
	}

	/** Update */
	this->update(listTemp);
}

int MessageList::getHeightPrefered() const {
	int total = 0;
	for (auto& [mes, layout, height] : this->list) {
		total += height;
	}
	return total;
}

void MessageList::mouseMove(const juce::MouseEvent& event) {
	auto [found, callback] = this->findByYPos(event.position.getY());
	if (found) {
		this->setMouseCursor(callback
			? juce::MouseCursor::PointingHandCursor
			: juce::MouseCursor::NormalCursor);
		return;
	}

	this->setMouseCursor(juce::MouseCursor::NormalCursor);
}

void MessageList::mouseUp(const juce::MouseEvent& event) {
	auto [found, callback] = this->findByYPos(event.position.getY());
	if (found && callback) {
		callback();
	}
}

std::tuple<bool, MessageModel::Callback> MessageList::findByYPos(float y) const {
	int total = 0;
	for (auto& [mes, layout, height] : this->list) {
		if (y >= total && y < (total + height)) {
			auto& [time, str, callback] = mes;
			return { true, callback };
		}

		total += height;
	}

	return { false, MessageModel::Callback{} };
}
