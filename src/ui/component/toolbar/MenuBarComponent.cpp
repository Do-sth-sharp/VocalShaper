#include "MenuBarComponent.h"

MenuBarComponent::MenuBarComponent(MenuBarModel* model)
	: model(model) {
	/** More Button */
	this->moreButton = std::make_unique<juce::TextButton>("...");
	this->moreButton->setWantsKeyboardFocus(false);
	this->moreButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->moreButton->setConnectedEdges(
		juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->moreButton->onClick = [this] {
		this->showMoreMenu();
	};
	this->addChildComponent(this->moreButton.get());

	/** Update Buttons */
	this->updateButtons();
}

void MenuBarComponent::setModel(MenuBarModel* model) {
	this->model = model;
	this->updateButtons();
}

void MenuBarComponent::resized() {
	this->updateSize();
}

void MenuBarComponent::updateSize() {
	/** Button Height */
	int buttonHeight = this->getHeight();

	/** More Button Size */
	int moreButtonWidth = this->moreButton->getBestWidthForHeight(buttonHeight);

	/** Buttons Total Width */
	int buttonTotalWidth = 0;
	for (int i = 0; i < this->buttons.size(); i++) {
		auto button = this->buttons.getUnchecked(i);

		int buttonWidth = button->getBestWidthForHeight(buttonHeight);
		button->setBounds(buttonTotalWidth, 0, buttonWidth, buttonHeight);
		buttonTotalWidth += buttonWidth;
	}

	/** Show Buttons */
	bool buttonOverflow = buttonTotalWidth > this->getWidth();
	this->buttonShownNum = 0;
	buttonTotalWidth = 0;
	for (int i = 0; i < this->buttons.size(); i++) {
		auto button = this->buttons.getUnchecked(i);

		if (buttonOverflow) {
			if ((this->buttonShownNum < i)
				|| ((buttonTotalWidth + button->getWidth()) >
				(this->getWidth() - moreButtonWidth))) {
				button->setVisible(false);
				continue;
			}
		}

		button->setVisible(true);
		this->buttonShownNum++;
		buttonTotalWidth += button->getWidth();
	}

	/** More Button Shown */
	moreButtonWidth = std::max(
		moreButtonWidth, this->getWidth() - buttonTotalWidth);
	this->moreButton->setBounds(
		this->getWidth() - moreButtonWidth, 0,
		moreButtonWidth, buttonHeight);
	this->moreButton->setVisible(
		this->buttonShownNum < this->buttons.size());
}

void MenuBarComponent::updateButtons() {
	/** Clear Buttons */
	this->clearButtons();
	if (!this->model) { return; }

	/** Create Buttons */
	auto textArray = this->model->getMenuBarNames();
	for (int i = 0; i < textArray.size(); i++) {
		if (auto button = this->buttons.add(new juce::TextButton{ textArray.getReference(i) })) {
			button->setWantsKeyboardFocus(false);
			button->setMouseCursor(juce::MouseCursor::PointingHandCursor);
			button->setConnectedEdges(
				juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
			button->onClick = [i, button, model = this->model] {
				auto menu = model->getMenuForIndex(i, button->getButtonText());
				menu.showAt(button);
			};
			this->addChildComponent(button);
		}
	}

	/** Update Size */
	this->updateSize();
}

void MenuBarComponent::clearButtons() {
	this->buttons.clear();
	this->moreButton->setVisible(false);
	this->buttonShownNum = 0;
}

void MenuBarComponent::showMoreMenu() {
	auto menu = this->getMoreMenu();
	menu.showAt(this->moreButton.get());
}

juce::PopupMenu MenuBarComponent::getMoreMenu() {
	juce::PopupMenu menu;

	for (int i = this->buttonShownNum; i < this->buttons.size(); i++) {
		auto& text = this->buttons.getUnchecked(i)->getButtonText();
		menu.addSubMenu(text, this->model->getMenuForIndex(i, text));
	}

	return menu;
}
