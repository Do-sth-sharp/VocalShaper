#include "OutputConfigComponent.h"
#include "ConfigPropertyComponents.h"
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
	return screenSize.getHeight() * 0.5;
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
	props.add(new ConfigChoiceProp{ className, "bit-depth", possibleBitDepth,
		ConfigChoiceProp::ValueType::NameVal, bitDepthUpdateCallback, bitDepthValueCallback });
	props.add(new ConfigChoiceProp{ className, "quality", possibleQualities,
		ConfigChoiceProp::ValueType::IndexVal, qualityUpdateCallback, qualityValueCallback });
	this->properties->addProperties(props);

	/** TODO Meta */

	/** Resize Components */
	this->properties->resized();
	this->resized();
}
