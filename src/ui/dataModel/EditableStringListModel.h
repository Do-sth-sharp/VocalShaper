#pragma once

#include <JuceHeader.h>

class EditableStringListModel : public juce::ListBoxModel {
public:
	using AddCallback = std::function<void(int)>;
	using EditCallback = std::function<void(int, const juce::String&)>;
	using RemoveCallback = std::function<void(int, const juce::String&)>;

	enum MenuType : uint8_t {
		MenuNoItem = 0,
		MenuAddItem = 1,
		MenuEditItem = 2,
		MenuRemoveItem = 4,

		MenuNoEditItem = MenuAddItem | MenuRemoveItem,
		MenuAllItem = MenuAddItem | MenuEditItem | MenuRemoveItem
	};

public:
	EditableStringListModel(
		const juce::StringArray& list,
		const AddCallback& addCallback,
		const EditCallback& editCallback,
		const RemoveCallback& removeCallback,
		MenuType menuType);

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g,
		int width, int height, bool rowIsSelected) override;
	juce::String getNameForRow(int rowNumber) override;
	void listBoxItemClicked(int row, const juce::MouseEvent& event) override;
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent& event) override;
	void backgroundClicked(const juce::MouseEvent& event) override;
	void deleteKeyPressed(int lastRowSelected) override;
	juce::MouseCursor getMouseCursorForRow(int row) override;

	void insert(int index, const juce::String& value);
	void set(int index, const juce::String& value);
	void remove(int index);
	const juce::StringArray getData() const;
	bool contains(const juce::String& value) const;

private:
	juce::LookAndFeel& lookAndFeel;
	juce::StringArray list;
	const AddCallback addCallback;
	const EditCallback editCallback;
	const RemoveCallback removeCallback;
	MenuType menuType = MenuAllItem;

	juce::PopupMenu createMenu(bool onRow) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditableStringListModel)
};
