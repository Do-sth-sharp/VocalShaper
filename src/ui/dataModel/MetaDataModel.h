#pragma once

#include <JuceHeader.h>

class MetaDataModel final : public juce::TableListBoxModel {
public:
	using AddCallback = std::function<void(void)>;
	using EditCallback = std::function<void(const juce::String&, const juce::String&)>;
	using RemoveCallback = std::function<void(const juce::String&)>;

public:
	MetaDataModel(const juce::StringPairArray& data,
		const AddCallback& addCallback,
		const EditCallback& editCallback,
		const RemoveCallback& removeCallback);

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber,
		int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId,
		int width, int height, bool rowIsSelected) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	void backgroundClicked(const juce::MouseEvent& event) override;
	void sortOrderChanged(int newSortColumnId, bool isForwards) override;
	void deleteKeyPressed(int lastRowSelected) override;

	void set(const juce::String& key, const juce::String& value);
	void remove(const juce::String& key);
	const juce::StringPairArray getData() const;

private:
	juce::LookAndFeel& lookAndFeel;
	juce::StringPairArray data;
	const AddCallback addCallback;
	const EditCallback editCallback;
	const RemoveCallback removeCallback;
	bool sortOrder = true;

	juce::PopupMenu createMenu(bool onRow) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetaDataModel)
};
