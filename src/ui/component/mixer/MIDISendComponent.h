#pragma once

#include <JuceHeader.h>

class MIDISendComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	MIDISendComponent();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int type, int track, int slot);

	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

	const juce::String getSendName() const;

private:
	int type = -1, track = -1, slot = -1;
	juce::String name;
	bool valid = false;

	void showAddMenu();
	void showMenu();
	void startDrag();

	void addSend(int type, int index);
	void removeSend();

	juce::var getDragSourceDescription() const;
	juce::String createToolTip() const;
	juce::PopupMenu createMenu(
		const std::function<void(int, int)>& addCallback,
		const std::function<void()>& removeCallback) const;
	juce::PopupMenu createAddMenu(
		const std::function<void(int, int)>& addCallback) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDISendComponent)
};

