#include "KeyMappingEditorComponent.h"
#include "../../Utils.h"

class KeyMappingEditorComponent::ChangeKeyButton final : public juce::Button {
public:
	ChangeKeyButton(KeyMappingEditorComponent& kec, juce::CommandID command,
		const juce::String& keyName, int keyIndex)
		: Button(keyName),
		owner(kec),
		commandID(command),
		keyNum(keyIndex) {
		this->setWantsKeyboardFocus(false);
		this->setTriggeredOnMouseDown(this->keyNum >= 0);

		this->setTooltip(keyIndex < 0 ? TRANS("Adds a new key-mapping")
			: TRANS("Click to change this key-mapping"));
	};

	void paintButton(juce::Graphics& g, bool /*isOver*/, bool /*isDown*/) override {
		this->getLookAndFeel().drawKeymapChangeButton(g, this->getWidth(), this->getHeight(), *this,
			this->keyNum >= 0 ? this->getName() : juce::String());
	};

	void clicked() override {
		if (this->keyNum >= 0) {
			juce::Component::SafePointer<ChangeKeyButton> button(this);
			juce::PopupMenu m;

			m.addItem(TRANS("Change this key-mapping"),
				[button] {
					if (button != nullptr)
						button.getComponent()->assignNewKey();
				});

			m.addSeparator();

			m.addItem(TRANS("Remove this key-mapping"),
				[button] {
					if (button != nullptr)
						button->owner.getMappings().removeKeyPress(button->commandID,
							button->keyNum);
				});

			m.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this));
		}
		else {
			this->assignNewKey();  // + button pressed..
		}
	};

	using juce::Button::clicked;

	void fitToContent(const int h) noexcept {
		if (this->keyNum < 0)
			this->setSize(h, h);
		else
			this->setSize(juce::jlimit(h * 4, h * 8, 6 + juce::Font((float)h * 0.6f).getStringWidth(this->getName())), h);
	};

	//==============================================================================
	class KeyEntryWindow final : public juce::AlertWindow {
	public:
		KeyEntryWindow(KeyMappingEditorComponent& kec)
			: AlertWindow(TRANS("New key-mapping"),
				TRANS("Please press a key combination now..."),
				juce::MessageBoxIconType::NoIcon),
			owner(kec) {
			this->addButton(TRANS("OK"), 1);
			this->addButton(TRANS("Cancel"), 0);

			// (avoid return + escape keys getting processed by the buttons..)
			for (auto* child : this->getChildren())
				child->setWantsKeyboardFocus(false);

			this->setWantsKeyboardFocus(true);
			this->grabKeyboardFocus();
		};

		bool keyPressed(const juce::KeyPress& key) override {
			this->lastPress = key;
			juce::String message(TRANS("Key") + ": " + this->owner.getDescriptionForKeyPress(key));

			auto previousCommand = this->owner.getMappings().findCommandForKeyPress(key);

			if (previousCommand != 0)
				message << "\n\n("
				<< TRANS("Currently assigned to \"CMDN\"")
				.replace("CMDN", TRANS(this->owner.getCommandManager().getNameOfCommand(previousCommand)))
				<< ')';

			this->setMessage(message);
			return true;
		};

		bool keyStateChanged(bool) override {
			return true;
		};

		juce::KeyPress lastPress;

	private:
		KeyMappingEditorComponent& owner;

		JUCE_DECLARE_NON_COPYABLE(KeyEntryWindow)
	};

	void setNewKey(const juce::KeyPress& newKey, bool dontAskUser) {
		if (newKey.isValid()) {
			auto previousCommand = this->owner.getMappings().findCommandForKeyPress(newKey);

			if (previousCommand == 0 || dontAskUser) {
				this->owner.getMappings().removeKeyPress(newKey);

				if (this->keyNum >= 0)
					this->owner.getMappings().removeKeyPress(this->commandID, this->keyNum);

				this->owner.getMappings().addKeyPress(this->commandID, newKey, this->keyNum);
			}
			else {
				auto options = juce::MessageBoxOptions::makeOptionsOkCancel(juce::MessageBoxIconType::WarningIcon,
					TRANS("Change key-mapping"),
					TRANS("This key is already assigned to the command \"CMDN\"")
					.replace("CMDN", this->owner.getCommandManager().getNameOfCommand(previousCommand))
					+ "\n\n"
					+ TRANS("Do you want to re-assign it to this new command instead?"),
					TRANS("Re-assign"),
					TRANS("Cancel"),
					this);
				this->messageBox = juce::AlertWindow::showScopedAsync(options, [this, newKey](int result) {
					if (result != 0)
						this->setNewKey(newKey, true);
					});
			}
		}
	};

	static void keyChosen(int result, ChangeKeyButton* button) {
		if (button != nullptr && button->currentKeyEntryWindow != nullptr) {
			if (result != 0) {
				button->currentKeyEntryWindow->setVisible(false);
				button->setNewKey(button->currentKeyEntryWindow->lastPress, false);
			}

			button->currentKeyEntryWindow.reset();
		}
	};

	void assignNewKey() {
		this->currentKeyEntryWindow.reset(new KeyEntryWindow(this->owner));
		this->currentKeyEntryWindow->enterModalState(
			true, juce::ModalCallbackFunction::forComponent(ChangeKeyButton::keyChosen, this));
	};

private:
	KeyMappingEditorComponent& owner;
	const juce::CommandID commandID;
	const int keyNum;
	std::unique_ptr<KeyEntryWindow> currentKeyEntryWindow;
	juce::ScopedMessageBox messageBox;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeKeyButton)
};

//==============================================================================
class KeyMappingEditorComponent::ItemComponent final : public Component {
public:
	ItemComponent(KeyMappingEditorComponent& kec, juce::CommandID command)
		: owner(kec), commandID(command) {
		this->setInterceptsMouseClicks(false, true);

		const bool isReadOnly = this->owner.isCommandReadOnly(this->commandID);

		auto keyPresses = this->owner.getMappings().getKeyPressesAssignedToCommand(this->commandID);

		for (int i = 0; i < juce::jmin((int)maxNumAssignments, keyPresses.size()); ++i)
			this->addKeyPressButton(this->owner.getDescriptionForKeyPress(keyPresses.getReference(i)), i, isReadOnly);

		this->addKeyPressButton("Change Key Mapping", -1, isReadOnly);
	};

	void addKeyPressButton(const juce::String& desc, const int index, const bool isReadOnly) {
		auto* b = new ChangeKeyButton(this->owner, this->commandID, desc, index);
		this->keyChangeButtons.add(b);

		b->setEnabled(!isReadOnly);
		b->setVisible(this->keyChangeButtons.size() <= (int)maxNumAssignments);
		this->addChildComponent(b);
	};

	void paint(juce::Graphics& g) override {
		g.setFont((float)this->getHeight() * 0.7f);
		g.setColour(this->owner.findColour(KeyMappingEditorComponent::textColourId));

		g.drawFittedText(TRANS(this->owner.getCommandManager().getNameOfCommand(this->commandID)),
			4, 0, juce::jmax(40, this->getChildComponent(0)->getX() - 5), this->getHeight(),
			juce::Justification::centredLeft, true);
	};

	void resized() override {
		int x = this->getWidth() - 4;

		for (int i = this->keyChangeButtons.size(); --i >= 0;) {
			auto* b = this->keyChangeButtons.getUnchecked(i);

			b->fitToContent(this->getHeight() - 2);
			b->setTopRightPosition(x, 1);
			x = b->getX() - 5;
		}
	};

private:
	std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override {
		return this->createIgnoredAccessibilityHandler(*this);
	};

	KeyMappingEditorComponent& owner;
	juce::OwnedArray<ChangeKeyButton> keyChangeButtons;
	const juce::CommandID commandID;

	enum { maxNumAssignments = 3 };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ItemComponent)
};

//==============================================================================
class KeyMappingEditorComponent::MappingItem final : public juce::TreeViewItem {
public:
	MappingItem(KeyMappingEditorComponent& kec, juce::CommandID command)
		: owner(kec), commandID(command)
	{};

	juce::String getUniqueName() const override { return juce::String((int)this->commandID) + "_id"; };
	bool mightContainSubItems() override { return false; };
	int getItemHeight() const override {
		auto screenSize = utils::getScreenSize(&(this->owner));
		return screenSize.getHeight() * 0.03;
	};
	std::unique_ptr<Component> createItemComponent() override { return std::make_unique<ItemComponent>(this->owner, this->commandID); };
	juce::String getAccessibilityName() override { return TRANS(this->owner.getCommandManager().getNameOfCommand(this->commandID)); };

private:
	KeyMappingEditorComponent& owner;
	const juce::CommandID commandID;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingItem)
};


//==============================================================================
class KeyMappingEditorComponent::CategoryItem final : public juce::TreeViewItem {
public:
	CategoryItem(KeyMappingEditorComponent& kec, const juce::String& name)
		: owner(kec), categoryName(name)
	{};

	juce::String getUniqueName() const override { return this->categoryName + "_cat"; };
	bool mightContainSubItems() override { return true; };
	int getItemHeight() const override {
		auto screenSize = utils::getScreenSize(&(this->owner));
		return screenSize.getHeight() * 0.03;
	};
	juce::String getAccessibilityName() override { return this->categoryName; };

	void paintItem(juce::Graphics& g, int width, int height) override {
		g.setFont(juce::Font((float)height * 0.7f, juce::Font::bold));
		g.setColour(this->owner.findColour(KeyMappingEditorComponent::textColourId));

		g.drawText(TRANS(this->categoryName), 2, 0, width - 2, height, juce::Justification::centredLeft, true);
	};

	void itemOpennessChanged(bool isNowOpen) override {
		if (isNowOpen) {
			if (this->getNumSubItems() == 0)
				for (auto command : this->owner.getCommandManager().getCommandsInCategory(this->categoryName))
					if (this->owner.shouldCommandBeIncluded(command))
						this->addSubItem(new MappingItem(this->owner, command));
		}
		else {
			this->clearSubItems();
		}
	};

private:
	KeyMappingEditorComponent& owner;
	juce::String categoryName;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CategoryItem)
};

//==============================================================================
class KeyMappingEditorComponent::TopLevelItem final : public juce::TreeViewItem,
	private juce::ChangeListener {
public:
	TopLevelItem(KeyMappingEditorComponent& kec) : owner(kec) {
		this->setLinesDrawnForSubItems(false);
		this->owner.getMappings().addChangeListener(this);
	};

	~TopLevelItem() override {
		this->owner.getMappings().removeChangeListener(this);
	};

	bool mightContainSubItems() override { return true; };
	juce::String getUniqueName() const override { return "keys"; };

	void changeListenerCallback(juce::ChangeBroadcaster*) override {
		const OpennessRestorer opennessRestorer(*this);
		this->clearSubItems();

		for (auto category : this->owner.getCommandManager().getCommandCategories()) {
			int count = 0;

			for (auto command : this->owner.getCommandManager().getCommandsInCategory(category))
				if (this->owner.shouldCommandBeIncluded(command))
					++count;

			if (count > 0)
				this->addSubItem(new CategoryItem(this->owner, category));
		}
	};

private:
	KeyMappingEditorComponent& owner;
};

//==============================================================================
KeyMappingEditorComponent::KeyMappingEditorComponent(juce::KeyPressMappingSet& mappingManager,
	const bool showResetToDefaultButton)
	: mappings(mappingManager),
	resetButton(TRANS("reset to defaults")) {
	this->treeItem.reset(new TopLevelItem(*this));

	if (showResetToDefaultButton) {
		this->addAndMakeVisible(this->resetButton);

		this->resetButton.onClick = [this] {
			auto options = juce::MessageBoxOptions::makeOptionsOkCancel(juce::MessageBoxIconType::QuestionIcon,
				TRANS("Reset to defaults"),
				TRANS("Are you sure you want to reset all the key-mappings to their default state?"),
				TRANS("Reset"),
				{},
				this);
			this->messageBox = juce::AlertWindow::showScopedAsync(options, [this](int result) {
				if (result != 0)
					this->getMappings().resetToDefaultMappings();
				});
			};
	}

	this->addAndMakeVisible(this->tree);
	this->tree.setTitle("Key Mappings");
	this->tree.setColour(juce::TreeView::backgroundColourId, findColour(backgroundColourId));
	this->tree.setRootItemVisible(false);
	this->tree.setDefaultOpenness(true);
	this->tree.setRootItem(this->treeItem.get());
	this->tree.setIndentSize(12);
}

KeyMappingEditorComponent::~KeyMappingEditorComponent() {
	this->tree.setRootItem(nullptr);
}

//==============================================================================
void KeyMappingEditorComponent::setColours(juce::Colour mainBackground,
	juce::Colour textColour) {
	this->setColour(backgroundColourId, mainBackground);
	this->setColour(textColourId, textColour);
	this->tree.setColour(juce::TreeView::backgroundColourId, mainBackground);
}

void KeyMappingEditorComponent::parentHierarchyChanged() {
	this->treeItem->changeListenerCallback(nullptr);
}

void KeyMappingEditorComponent::resized() {
	auto screenSize = utils::getScreenSize(this);
	int buttonAreaHeight = screenSize.getHeight() * 0.04;
	int buttonPaddingHeight = buttonAreaHeight * 0.15;
	int buttonPaddingWidth = buttonAreaHeight * 0.2;

	int h = this->getHeight();
	if (this->resetButton.isVisible()) {
		h -= buttonAreaHeight;
		int x = this->getWidth() - buttonPaddingWidth;

		this->resetButton.changeWidthToFitText(buttonAreaHeight - buttonPaddingHeight * 2);
		this->resetButton.setTopRightPosition(x, h + buttonPaddingHeight);
	}

	this->tree.setBounds(0, 0, this->getWidth(), h);
}

//==============================================================================
bool KeyMappingEditorComponent::shouldCommandBeIncluded(const juce::CommandID commandID) {
	auto* ci = this->mappings.getCommandManager().getCommandForID(commandID);

	return ci != nullptr && (ci->flags & juce::ApplicationCommandInfo::hiddenFromKeyEditor) == 0;
}

bool KeyMappingEditorComponent::isCommandReadOnly(const juce::CommandID commandID) {
	auto* ci = this->mappings.getCommandManager().getCommandForID(commandID);

	return ci != nullptr && (ci->flags & juce::ApplicationCommandInfo::readOnlyInKeyEditor) != 0;
}

juce::String KeyMappingEditorComponent::getDescriptionForKeyPress(const juce::KeyPress& key) {
	return key.getTextDescription();
}
