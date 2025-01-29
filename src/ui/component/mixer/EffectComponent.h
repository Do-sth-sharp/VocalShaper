#pragma once

#include <JuceHeader.h>

class EffectComponent final
	: public juce::Component,
	public juce::SettableTooltipClient,
	public juce::DragAndDropTarget {
public:
	EffectComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void update(int type, int track, int index);

	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

	bool isInterestedInDragSource(
		const SourceDetails& dragSourceDetails) override;
	void itemDragEnter(const SourceDetails& dragSourceDetails) override;
	void itemDragExit(const SourceDetails& dragSourceDetails) override;
	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
	int type = -1, track = -1, index = -1;
	juce::String name;
	bool valid = false;
	bool editorOpened = false;
	bool drop = false;

	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;

	void bypass();
	void editorShow();
	void showMenu();
	void showAddMenu();
	void startDrag();

	void addEffect(const juce::PluginDescription& plugin);
	void replaceEffect(const juce::PluginDescription& plugin);

	void preDrop();
	void endDrop();

	juce::var getDragSourceDescription() const;
	juce::String createToolTip() const;
	juce::PopupMenu createMenu(
		const std::function<void(const juce::PluginDescription&)>& addCallback,
		const std::function<void(const juce::PluginDescription&)>& editCallback) const;
	juce::PopupMenu createAddMenu(
		const std::function<void(const juce::PluginDescription&)>& callback) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectComponent)
};
