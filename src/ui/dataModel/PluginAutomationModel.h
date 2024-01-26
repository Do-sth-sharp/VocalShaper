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
	juce::LookAndFeel& lookAndFeel;
	const quickAPI::PluginHolder plugin;
	const PluginType type;

	/** Param Index, Param Name, CC Channel, CC Channel Name */
	using TempItem = std::tuple<int, juce::String, int, juce::String>;
	juce::Array<TempItem> listTemp;

	int sortColumn = 0;
	bool sortForwards = false;

	class SortComparator final {
	public:
		SortComparator() = delete;
		SortComparator(int column, bool forwards)
			: column(column), forwards(forwards) {};

		int compareElements(TempItem& first, TempItem& second) const {
			int r = this->forwards ? 1 : -1;
			switch (this->column) {
			case 1: {
				return (std::get<0>(first) - std::get<0>(second)) * r;
				break;
			}
			case 2: {
				return (std::get<2>(first) - std::get<2>(second)) * r;
				break;
			}
			}
			return 0;
		}

	private:
		const int column;
		const bool forwards;

		JUCE_LEAK_DETECTOR(SortComparator)
	};

	void addItem();
	void editItem(int itemIndex);
	void removeItem(int itemIndex);

	void showItemMenu(int itemIndex);
	void showBackgroundMenu();

	juce::PopupMenu createMenu(bool isItem);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAutomationModel);
};
