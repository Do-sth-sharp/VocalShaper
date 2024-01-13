#include "PluginTreeModel.h"
#include "../misc/DragSourceType.h"
#include "../Utils.h"

class PluginItemModel final : public juce::TreeViewItem {
public:
	PluginItemModel() = delete;
	PluginItemModel(const juce::PluginDescription& plugin) : plugin(plugin) {};

	bool mightContainSubItems() override { return false; };
	juce::String getUniqueName() const override { return this->plugin.createIdentifierString(); };

	int getItemWidth() const override {
		if (auto view = this->getOwnerView()) {
			/** Size */
			auto screenSize = utils::getScreenSize(view);
			int paddingWidth = screenSize.getWidth() * 0.004;
			float textHeight = screenSize.getHeight() * 0.0175f;

			/** Font */
			juce::Font textFont(textHeight);
			return textFont.getStringWidth(this->plugin.name) + paddingWidth * 2;
		}
		return 0;
	};
	int getItemHeight() const override {
		if (auto view = this->getOwnerView()) {
			auto screenSize = utils::getScreenSize(view);
			int paddingHeight = screenSize.getHeight() * 0.004;
			int textHeight = screenSize.getHeight() * 0.0175;
			return textHeight + paddingHeight * 2;
		}
		return 0;
	}

	bool canBeSelected() const override { return true; };

	void paintItem(juce::Graphics& g, int width, int height) override {
		if (auto view = this->getOwnerView()) {
			auto screenSize = utils::getScreenSize(view);
			auto& laf = view->getLookAndFeel();

			/** Size */
			int paddingWidth = screenSize.getWidth() * 0.004;
			int paddingHeight = screenSize.getHeight() * 0.004;
			float textHeight = screenSize.getHeight() * 0.0175f;

			/** Color */
			juce::Colour backgroundColor = laf.findColour(
				juce::Label::ColourIds::backgroundColourId);
			juce::Colour textColor = laf.findColour(
				juce::Label::ColourIds::textColourId);

			/** Font */
			juce::Font textFont(textHeight);

			/** Background */
			g.setColour(backgroundColor);
			g.fillAll();

			/** Text */
			juce::Rectangle<int> textRect(
				paddingWidth, paddingHeight,
				width - paddingWidth * 2, textHeight);
			g.setColour(textColor);
			g.setFont(textFont);
			g.drawFittedText(this->plugin.name, textRect, juce::Justification::centredLeft, 1, 1.f);
		}
	};

	juce::String getTooltip() override {
		return this->plugin.name + "\n"
			+ TRANS("Description:") + " " + this->plugin.descriptiveName + "\n"
			+ TRANS("Format:") + " " + this->plugin.pluginFormatName + "\n"
			+ TRANS("Category:") + " " + this->plugin.category + "\n"
			+ TRANS("Manufacturer:") + " " + this->plugin.manufacturerName + "\n"
			+ TRANS("Version:") + " " + this->plugin.version + "\n"
			+ TRANS("File:") + " " + this->plugin.fileOrIdentifier + "\n"
			+ TRANS("Last File Mod Time:") + " " + this->plugin.lastFileModTime.formatted("%Y-%m-%d %H:%M:S") + "\n"
			+ TRANS("Last Info Update Time:") + " " + this->plugin.lastInfoUpdateTime.formatted("%Y-%m-%d %H:%M:S") + "\n"
			+ TRANS("Is Instrument:") + " " + (this->plugin.isInstrument ? TRANS("Yes") : TRANS("No")) + "\n"
			+ TRANS("Input Channels:") + " " + juce::String{ this->plugin.numInputChannels } + "\n"
			+ TRANS("Output Channels:") + " " + juce::String{ this->plugin.numOutputChannels } + "\n";
	};

	juce::var getDragSourceDescription() override {
		auto object = std::make_unique<juce::DynamicObject>();

		object->setProperty("type", (int)DragSourceType::Plugin);
		object->setProperty("name", this->plugin.name);
		object->setProperty("format", this->plugin.pluginFormatName);
		object->setProperty("instrument", this->plugin.isInstrument);
		object->setProperty("id", this->plugin.createIdentifierString());

		return juce::var{ object.release() };
	};

	bool isInterestedInFileDrag(const juce::StringArray&) { return false; };
	bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails&) { return false; };

private:
	const juce::PluginDescription plugin;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginItemModel)
};

class PluginClassModel final : public juce::TreeViewItem {
public:
	PluginClassModel() = delete;
	PluginClassModel(const juce::String& name,
		const juce::Array<juce::PluginDescription>& plugins)
		: name(name) {
		for (auto& i : plugins) {
			this->addSubItem(new PluginItemModel{ i });
		}
	};

	bool mightContainSubItems() override { return true; };
	juce::String getUniqueName() const override { return this->name; }

	int getItemWidth() const override {
		if (auto view = this->getOwnerView()) {
			/** Size */
			auto screenSize = utils::getScreenSize(view);
			int paddingWidth = screenSize.getWidth() * 0.005;
			float textHeight = screenSize.getHeight() * 0.02f;

			/** Font */
			juce::Font textFont(textHeight);
			return textFont.getStringWidth(this->name) + paddingWidth * 2;
		}
		return 0;
	};
	int getItemHeight() const override {
		if (auto view = this->getOwnerView()) {
			auto screenSize = utils::getScreenSize(view);
			int paddingHeight = screenSize.getHeight() * 0.005;
			int textHeight = screenSize.getHeight() * 0.02;
			return textHeight + paddingHeight * 2;
		}
		return 0;
	}

	bool canBeSelected() const override { return false; };

	void paintItem(juce::Graphics& g, int width, int height) override {
		if (auto view = this->getOwnerView()) {
			auto screenSize = utils::getScreenSize(view);
			auto& laf = view->getLookAndFeel();

			/** Size */
			int paddingWidth = screenSize.getWidth() * 0.005;
			int paddingHeight = screenSize.getHeight() * 0.005;
			float textHeight = screenSize.getHeight() * 0.02f;

			/** Color */
			juce::Colour backgroundColor = laf.findColour(
				juce::TreeView::ColourIds::backgroundColourId);
			juce::Colour textColor = laf.findColour(
				juce::Label::ColourIds::textColourId);

			/** Font */
			juce::Font textFont(textHeight);

			/** Background */
			g.setColour(backgroundColor);
			g.fillAll();

			/** Text */
			juce::Rectangle<int> textRect(
				paddingWidth, paddingHeight,
				width - paddingWidth * 2, textHeight);
			g.setColour(textColor);
			g.setFont(textFont);
			g.drawFittedText(this->name, textRect, juce::Justification::centredLeft, 1, 1.f);
		}
	};

	bool isInterestedInFileDrag(const juce::StringArray&) { return false; };
	bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails&) { return false; };

private:
	const juce::String name;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginClassModel)
};

PluginTreeModel::PluginTreeModel(
	const juce::Array<PluginClass>& plugins) {
	for (auto& [name, list] : plugins) {
		this->addSubItem(new PluginClassModel{ name, list });
	}
}

bool PluginTreeModel::mightContainSubItems() { return true; }

bool PluginTreeModel::canBeSelected() const { return false; }

bool PluginTreeModel::isInterestedInFileDrag(
	const juce::StringArray&) { return false; };

bool PluginTreeModel::isInterestedInDragSource(
	const juce::DragAndDropTarget::SourceDetails&) { return false; };
