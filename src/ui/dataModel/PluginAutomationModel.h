#pragma once

#include <JuceHeader.h>
#include "../misc/PluginType.h"
#include "../../audioCore/AC_API.h"

class PluginAutomationModel final : public juce::TableListBoxModel {
public:
	PluginAutomationModel() = delete;
	PluginAutomationModel(
		quickAPI::PluginHolder plugin, PluginType type);

	int getNumRows() override;
	void paintRowBackground(juce::Graphics&, int rowNumber,
		int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics&, int rowNumber, int columnId,
		int width, int height, bool rowIsSelected) override;
	void cellClicked(int rowNumber, int columnId,
		const juce::MouseEvent&) override;
	void backgroundClicked(const juce::MouseEvent&) override;
	void sortOrderChanged(int newSortColumnId, bool isForwards) override;
	void deleteKeyPressed(int lastRowSelected) override;

	void update();

private:
	const quickAPI::PluginHolder plugin;
	const PluginType type;

	/** Param Index, Param Name, CC Channel, CC Channel Name */
	using TempItem = std::tuple<int, juce::String, int, juce::String>;
	juce::Array<TempItem> listTemp;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAutomationModel);
};
