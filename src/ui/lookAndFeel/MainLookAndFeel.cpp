#include "MainLookAndFeel.h"
#include "../misc/ColorMap.h"

MainLookAndFeel::MainLookAndFeel()
	: LookAndFeel_V4() {
	/** Set Menu Color */
	this->setColour(juce::PopupMenu::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	this->setColour(juce::PopupMenu::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::PopupMenu::ColourIds::headerTextColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::PopupMenu::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));

	/** Set Alert Color */
	this->setColour(juce::AlertWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	this->setColour(juce::AlertWindow::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::AlertWindow::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	this->setColour(juce::TextButton::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	this->setColour(juce::TextButton::ColourIds::buttonOnColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	this->setColour(juce::TextButton::ColourIds::textColourOffId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextButton::ColourIds::textColourOnId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::outlineColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));

	/** Set Combo Color */
	this->setColour(juce::ComboBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::ComboBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::ComboBox::ColourIds::buttonColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	this->setColour(juce::ComboBox::ColourIds::arrowColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::ComboBox::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));

	/** Set Window Background Color */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Set List Box Color */
	this->setColour(juce::ListBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::ListBox::ColourIds::backgroundColourId + 1,
		ColorMap::getInstance()->get("ThemeColorA1"));/**< Background On */
	this->setColour(juce::ListBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::ListBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::ListBox::ColourIds::textColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Text On */

	/** Set Text Editor Color */
	this->setColour(juce::TextEditor::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TextEditor::ColourIds::highlightColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextEditor::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId,
		juce::Colour::fromRGBA(0, 0, 0, 0));
	this->setColour(juce::TextEditor::ColourIds::shadowColourId + 1,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Empty Text */

	/** Set Scroll Bar Color */
	this->setColour(juce::ScrollBar::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::ScrollBar::ColourIds::thumbColourId,
		ColorMap::getInstance()->get("ThemeColorA1"));
	this->setColour(juce::ScrollBar::ColourIds::trackColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));

	/** Set Slider Color */
	this->setColour(juce::Slider::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::Slider::ColourIds::thumbColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
	this->setColour(juce::Slider::ColourIds::trackColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));
	this->setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));
	this->setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));
	this->setColour(juce::Slider::ColourIds::textBoxTextColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::Slider::ColourIds::textBoxBackgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::Slider::ColourIds::textBoxHighlightColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));
	this->setColour(juce::Slider::ColourIds::textBoxOutlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));

	/** Set Tool Tip Color */
	this->setColour(juce::TooltipWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB5"));
	this->setColour(juce::TooltipWindow::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TooltipWindow::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB5"));

	/** Set Property Comp Color */
	this->setColour(juce::PropertyComponent::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::PropertyComponent::ColourIds::labelTextColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::BooleanPropertyComponent::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::BooleanPropertyComponent::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextPropertyComponent::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextPropertyComponent::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::TextPropertyComponent::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));

	/** Set Table Header Color */
	this->setColour(juce::TableHeaderComponent::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));
	this->setColour(juce::TableHeaderComponent::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::TableHeaderComponent::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TableHeaderComponent::ColourIds::highlightColourId,
		ColorMap::getInstance()->get("ThemeColorB5"));

	/** Set Key Mapping Editor Color */
	this->setColour(juce::KeyMappingEditorComponent::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::KeyMappingEditorComponent::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
}

void MainLookAndFeel::drawTableHeaderColumn(
	juce::Graphics& g, juce::TableHeaderComponent& header,
	const juce::String& columnName, int /*columnId*/,
	int width, int height, bool isMouseOver, bool isMouseDown,
	int columnFlags) {
	auto highlightColour = header.findColour(juce::TableHeaderComponent::highlightColourId);
	auto textColour = header.findColour(juce::TableHeaderComponent::textColourId);

	if (isMouseDown)
		g.fillAll(highlightColour);
	else if (isMouseOver)
		g.fillAll(highlightColour.withMultipliedAlpha(0.625f));

	juce::Rectangle<int> area(width, height);
	area.reduce(4, 0);

	if ((columnFlags & (juce::TableHeaderComponent::sortedForwards | juce::TableHeaderComponent::sortedBackwards)) != 0) {
		juce::Path sortArrow;
		sortArrow.addTriangle(0.0f, 0.0f,
			0.5f, (columnFlags & juce::TableHeaderComponent::sortedForwards) != 0 ? -0.8f : 0.8f,
			1.0f, 0.0f);

		g.setColour(textColour);
		g.fillPath(sortArrow, sortArrow.getTransformToScaleToFit(area.removeFromRight(height / 2).reduced(2).toFloat(), true));
	}

	g.setColour(header.findColour(juce::TableHeaderComponent::textColourId));
	g.setFont(juce::Font((float)height * 0.5f, juce::Font::bold));
	g.drawFittedText(columnName, area, juce::Justification::centredLeft, 1);
}

void MainLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height) {
	g.fillAll(this->findColour(juce::PopupMenu::backgroundColourId));

#if !JUCE_MAC
	g.setColour(this->findColour(juce::ResizableWindow::backgroundColourId));
	g.drawRect(0, 0, width, height);
#endif //!JUCE_MAC
}

void MainLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
	const bool isSeparator, const bool isActive,
	const bool isHighlighted, const bool isTicked,
	const bool hasSubMenu, const juce::String& text,
	const juce::String& shortcutKeyText,
	const juce::Drawable* icon, const juce::Colour* const textColourToUse)
{
	if (isSeparator) {
		auto r = area.reduced(5, 0);
		r.removeFromTop(juce::roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

		g.setColour(this->findColour(juce::ResizableWindow::backgroundColourId));
		g.fillRect(r.removeFromTop(1));
	}
	else {
		auto textColour = (textColourToUse == nullptr ? this->findColour(juce::PopupMenu::textColourId)
			: *textColourToUse);

		auto r = area.reduced(1);

		if (isHighlighted && isActive) {
			g.setColour(this->findColour(juce::PopupMenu::highlightedBackgroundColourId));
			g.fillRect(r);

			g.setColour(this->findColour(juce::PopupMenu::highlightedTextColourId));
		}
		else {
			g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
		}

		r.reduce(juce::jmin(5, area.getWidth() / 20), 0);

		auto font = this->getPopupMenuFont();

		auto maxFontHeight = (float)r.getHeight() / 1.3f;

		if (font.getHeight() > maxFontHeight)
			font.setHeight(maxFontHeight);

		g.setFont(font);

		auto iconArea = r.removeFromLeft(juce::roundToInt(maxFontHeight)).toFloat();

		if (icon != nullptr) {
			icon->drawWithin(g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
			r.removeFromLeft(juce::roundToInt(maxFontHeight * 0.5f));
		}
		else if (isTicked) {
			auto tick = this->getTickShape(1.0f);
			g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
		}

		if (hasSubMenu) {
			auto arrowH = 0.6f * this->getPopupMenuFont().getAscent();

			auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
			auto halfH = static_cast<float> (r.getCentreY());

			juce::Path path;
			path.startNewSubPath(x, halfH - arrowH * 0.5f);
			path.lineTo(x + arrowH * 0.6f, halfH);
			path.lineTo(x, halfH + arrowH * 0.5f);

			g.strokePath(path, juce::PathStrokeType(2.0f));
		}

		r.removeFromRight(3);
		g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

		if (shortcutKeyText.isNotEmpty()) {
			auto f2 = font;
			f2.setHeight(f2.getHeight() * 0.75f);
			f2.setHorizontalScale(0.95f);
			g.setFont(f2);

			g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
		}
	}
}
