#include "OutputConfigComponent.h"
#include "ConfigPropertyComponents.h"
#include "../misc/ConfigManager.h"
#include "../Utils.h"
#include "../../audioCore/AC_API.h"

OutputConfigComponent::OutputConfigComponent() {
	/** Meta */
	this->metaLabel = std::make_unique<juce::Label>(
		TRANS("Meta Data"), TRANS("Meta Data"));
	this->addAndMakeVisible(this->metaLabel.get());

	this->metaList = std::make_unique<juce::TableListBox>(
		TRANS("Meta Data"), nullptr);
	this->metaList->getHeader().addColumn(
		TRANS("key"), 1, 30, 30, -1,
		juce::TableHeaderComponent::ColumnPropertyFlags::visible |
		juce::TableHeaderComponent::ColumnPropertyFlags::resizable |
		juce::TableHeaderComponent::ColumnPropertyFlags::sortable);
	this->metaList->getHeader().addColumn(
		TRANS("value"), 2, 30, 30, -1,
		juce::TableHeaderComponent::ColumnPropertyFlags::visible |
		juce::TableHeaderComponent::ColumnPropertyFlags::resizable);
	this->addAndMakeVisible(this->metaList.get());

	/** Property Panel */
	this->properties = std::make_unique<juce::PropertyPanel>(TRANS("Output Config"));
	this->addAndMakeVisible(this->properties.get());

	/** Formats Selector */
	this->formatSelectorLabel = std::make_unique<juce::Label>(
		TRANS("Audio Format"), TRANS("Audio Format"));
	this->addAndMakeVisible(this->formatSelectorLabel.get());

	auto audioFormats = quickAPI::getAudioFormatsSupported(true);
	this->formatSelector = std::make_unique<juce::ComboBox>(TRANS("Audio Format"));
	this->formatSelector->onChange = [this] {
		this->currentFormatChanged(
			this->formatSelector->getText().trimCharactersAtStart("*"));
		};
	this->formatSelector->addItemList(audioFormats, 1);
	this->formatSelector->setSelectedItemIndex(0);
	this->addAndMakeVisible(this->formatSelector.get());
}

int OutputConfigComponent::getHeightPrefered() const {
	auto screenSize = utils::getScreenSize(this);
	return screenSize.getHeight() * 0.55;
}

void OutputConfigComponent::resized() {
	auto screenSize = utils::getScreenSize(this);

	/** Sizes */
	int itemHeight = screenSize.getHeight() * 0.04;
	int metaAreaHeight = screenSize.getHeight() * 0.35;
	int labelWidth = std::min(200, screenSize.getWidth() / 3);

	/** Format Selector */
	juce::Rectangle<int> formatRect(
		0, 0, this->getWidth(), itemHeight);
	this->formatSelectorLabel->setBounds(formatRect.withWidth(labelWidth));
	this->formatSelector->setBounds(formatRect.withTrimmedLeft(labelWidth));

	/** Property Panel */
	juce::Rectangle<int> propertyRect =
		this->getLocalBounds().withTrimmedTop(itemHeight).withTrimmedBottom(metaAreaHeight);
	this->properties->setBounds(propertyRect);
	
	/** Meta */
	juce::Rectangle<int> metaRect(
		0, this->getHeightPrefered() - metaAreaHeight,
		this->getWidth(), metaAreaHeight);
	this->metaLabel->setBounds(metaRect.withWidth(labelWidth));
	this->metaList->setBounds(metaRect.withTrimmedLeft(labelWidth));
	this->metaList->setHeaderHeight(itemHeight);
	this->metaList->getHeader().setColumnWidth(1, labelWidth);
	this->metaList->getHeader().setColumnWidth(2, labelWidth);
	this->metaList->setRowHeight(itemHeight);
}

void OutputConfigComponent::currentFormatChanged(const juce::String& format) {
	/** Properties */
	this->properties->clear();

	juce::String className = "output" + format;
	auto possibleBitDepthTemp = quickAPI::getFormatPossibleBitDepthsForExtension(format);
	juce::StringArray possibleBitDepth;
	for (auto i : possibleBitDepthTemp) {
		possibleBitDepth.add(juce::String{ i });
	}
	auto possibleQualities = quickAPI::getFormatQualityOptionsForExtension(format);

	auto bitDepthUpdateCallback = [format](const juce::var& data) {
		quickAPI::setFormatBitsPerSample(format, data.toString().getIntValue());
		return true;
		};
	auto bitDepthValueCallback = [format]()->const juce::var {
		return juce::String{ quickAPI::getFormatBitsPerSample(format) };
		};
	auto qualityUpdateCallback = [format](const juce::var& data) {
		quickAPI::setFormatQualityOptionIndex(format, data);
		return true;
		};
	auto qualityValueCallback = [format]()->const juce::var {
		return quickAPI::getFormatQualityOptionIndex(format);
		};

	juce::Array<juce::PropertyComponent*> props;
	props.add(new ConfigWhiteSpaceProp{});
	props.add(new ConfigLabelProp{ "Switch audio formats to see different options." });
	props.add(new ConfigChoiceProp{ className, "bit-depth", possibleBitDepth,
		ConfigChoiceProp::ValueType::NameVal, bitDepthUpdateCallback, bitDepthValueCallback });
	props.add(new ConfigChoiceProp{ className, "quality", possibleQualities,
		ConfigChoiceProp::ValueType::IndexVal, qualityUpdateCallback, qualityValueCallback });
	this->properties->addProperties(props);

	/** Meta */
	auto metaData = quickAPI::getFormatMetaData(format);
	this->metaModel = std::make_unique<MetaDataModel>(metaData,
		[this] { this->addMeta(); },
		[this](const juce::String& key, const juce::String& value) { this->editMeta(key, value); },
		[this](const juce::String& key) { this->removeMeta(key); });
	this->metaList->setModel(this->metaModel.get());
	this->metaList->updateContent();

	/** Resize Components */
	this->properties->resized();
	this->resized();
}

void OutputConfigComponent::addMeta() {
	juce::String format = this->formatSelector->getText().trimCharactersAtStart("*");

	auto addCallback =
		[format, parent = OutputConfigComponent::SafePointer{ this }]
		(const juce::String& key, const juce::String& value) {
		if (!parent) { return; }
		auto metaModel = parent->metaModel.get();
		if (!metaModel) { return; }

		metaModel->set(key, value);
		auto data = metaModel->getData();

		quickAPI::setFormatMetaData(format, data);
		parent->writeMetaConfig(format, data);

		parent->metaList->updateContent();
		};

	this->showMetaEditor(TRANS("Add"), quickAPI::getFormatPossibleMetaKeyForExtension(format),
		addCallback);
}

void OutputConfigComponent::editMeta(const juce::String& key, const juce::String& value) {
	juce::String format = this->formatSelector->getText().trimCharactersAtStart("*");

	auto editCallback =
		[format, parent = OutputConfigComponent::SafePointer{ this }]
		(const juce::String& key, const juce::String& value) {
		if (!parent) { return; }
		auto metaModel = parent->metaModel.get();
		if (!metaModel) { return; }

		metaModel->set(key, value);
		auto data = metaModel->getData();

		quickAPI::setFormatMetaData(format, data);
		parent->writeMetaConfig(format, data);

		parent->metaList->updateContent();
		parent->metaList->repaint();
		};

	this->showMetaEditor(TRANS("Edit"), quickAPI::getFormatPossibleMetaKeyForExtension(format),
		editCallback, true, key, value);
}

void OutputConfigComponent::removeMeta(const juce::String& key) {
	if (this->metaModel) {
		juce::String format = this->formatSelector->getText().trimCharactersAtStart("*");

		this->metaModel->remove(key);
		auto data = this->metaModel->getData();

		quickAPI::setFormatMetaData(format, data);
		this->writeMetaConfig(format, data);

		this->metaList->updateContent();
	}
}

bool OutputConfigComponent::writeMetaConfig(
	const juce::String& format, const juce::StringPairArray& data) {
	auto dataObject = std::make_unique<juce::DynamicObject>();
	auto& keys = data.getAllKeys();
	auto& values = data.getAllValues();
	for (int i = 0; i < data.size(); i++) {
		dataObject->setProperty(keys[i], values[i]);
	}

	auto dataVar = juce::var{ dataObject.release() };
	juce::String className = "output" + format;
	auto& config = ConfigManager::getInstance()->get(className);
	if (auto confObj = config.getDynamicObject()) {
		confObj->setProperty("meta", dataVar);
		return ConfigManager::getInstance()->saveConfig(className);
	}

	return false;
}

void OutputConfigComponent::showMetaEditor(
	const juce::String& title, const juce::StringArray& possibleKeys,
	const MetaEditorCallback& callback, bool keyLocked,
	const juce::String& defaultKey, const juce::String& defaultValue) {
	juce::String message = TRANS("Use the Enter key to quickly look up a key name.");
	auto window = new juce::AlertWindow{ title, message, juce::MessageBoxIconType::QuestionIcon };
	window->addButton(TRANS("OK"), 1);
	window->addButton(TRANS("Cancel"), 0);

	window->addTextEditor("key", defaultKey, TRANS("key"));
	window->addTextEditor("value", defaultValue, TRANS("value"));

	if (auto keyEditor = window->getTextEditor("key")) {
		keyEditor->setReadOnly(keyLocked);
		keyEditor->onReturnKey = [keyEditor, possibleKeys] {
			juce::String word = keyEditor->getText();
			auto possibleList = word.isEmpty()
				? possibleKeys : utils::searchKMP(possibleKeys, word);
			if (possibleList.isEmpty()) { return; }

			juce::PopupMenu menu;
			for (int i = 0; i < possibleList.size(); i++) {
				menu.addItem(i + 1, possibleList[i]);
			}
			int result = menu.showMenu(juce::PopupMenu::Options{}
				.withTargetComponent(keyEditor)
				.withDeletionCheck(*keyEditor)
				.withPreferredPopupDirection(
					juce::PopupMenu::Options::PopupDirection::downwards)
				.withStandardItemHeight(keyEditor->getHeight()));

			if (result > 0 && result <= possibleList.size()) {
				keyEditor->setText(possibleList[result - 1], false);
			}
			};
	}

	window->enterModalState(true, juce::ModalCallbackFunction::create(
		[callback, window, title](int result) {
			if (result != 1) { return; }

			auto keyEditor = window->getTextEditor("key");
			auto valueEditor = window->getTextEditor("value");
			if ((!keyEditor) || (!valueEditor)) { return; }
			
			juce::String key = keyEditor->getText();
			juce::String value = valueEditor->getText();
			if (key.isEmpty() || value.isEmpty()) {
				juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
					title, TRANS("The key or value must not be empty!"));
				return;
			}

			callback(key, value);
		}
	), true);
}
