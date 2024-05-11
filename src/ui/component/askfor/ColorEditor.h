#pragma once

#include <JuceHeader.h>

class ColorEditor;

class ColorEditorContent final : public juce::Component {
public:
	using Callback = std::function<void(const juce::Colour&)>;

public:
	ColorEditorContent() = delete;
	ColorEditorContent(ColorEditor* parent,
		const Callback& callback,
		const juce::Colour& defaultColor);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void selectColor(float hue, float sat, float bri);

private:
	class ColourSpaceView final : public juce::Component {
	public:
		ColourSpaceView(ColorEditorContent* parent);

		void setCurrentColor(float hue, float sat, float bri);

		void paint(juce::Graphics& g) override;

		void mouseDown(const juce::MouseEvent& e) override;
		void mouseDrag(const juce::MouseEvent& e) override;

		void resized() override;

	private:
		ColorEditorContent* const parent;
		float h = 0.f, s = 0.f, b = 0.f;
		juce::Image colours;

		class ColourSpaceMarker final : public juce::Component {
		public:
			ColourSpaceMarker();

			void paint(juce::Graphics& g) override;

			void setPos(float pX, float pY);

		private:
			float pX = 0.f, pY = 0.f;
		};

		ColourSpaceMarker marker;

		void updateMarker();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourSpaceView)
	};

	class HueSelectorComp final : public juce::Component {
	public:
		HueSelectorComp(ColorEditorContent* parent);

		void setCurrentColor(float hue, float sat, float bri);

		void paint(juce::Graphics& g) override;
		void resized() override;

		void mouseDown(const juce::MouseEvent& e) override;
		void mouseDrag(const juce::MouseEvent& e) override;

		void updateMarker();

	private:
		ColorEditorContent* const parent;
		float h = 0.f, s = 0.f, b = 0.f;

		struct HueSelectorMarker : public juce::Component {
		public:
			HueSelectorMarker();

			void paint(juce::Graphics& g) override;

			void setPos(float pY);

		private:
			float pY = 0.f;
		};

		HueSelectorMarker marker;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HueSelectorComp)
	};
	class ColorViewer final : public juce::Component {
	public:
		ColorViewer() = default;

		void setCurrentColor(float hue, float sat, float bri);

		void paint(juce::Graphics& g) override;

	private:
		float hue = 0.f, sat = 0.f, bri = 0.f;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorViewer)
	};

private:
	ColorEditor* const parent;
	const Callback callback;
	const juce::Array<juce::Colour> themeColors;
	const juce::Array<juce::Colour> themeAlertColors;
	const juce::Array<juce::Colour> historyList;

	std::unique_ptr<ColourSpaceView> colorSpace = nullptr;
	std::unique_ptr<HueSelectorComp> hueSelector = nullptr;
	std::unique_ptr<ColorViewer> colorViewer = nullptr;
	std::unique_ptr<juce::TextEditor> rEditor = nullptr,
		gEditor = nullptr, bEditor = nullptr, hexEditor = nullptr;

	std::unique_ptr<juce::TextButton> okButton = nullptr;

	juce::String commonTitle, historyTitle;
	juce::String rTitle, gTitle, bTitle, hexTitle;

	juce::Colour color;

	void updateColor(const juce::Colour& color, bool updateRGB = true, bool updateHex = true);
	void updateComponents(bool updateRGB = true, bool updateHex = true);

	int getThemeColorIndex(const juce::Point<int>& pos) const;
	int getThemeAlertColorIndex(const juce::Point<int>& pos) const;
	int getHistoryColorIndex(const juce::Point<int>& pos) const;

	void rgbValueChanged();
	void hexValueChanged();
	void okButtonPressed();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorEditorContent)
};

class ColorEditor final : public juce::DocumentWindow {
	using Callback = ColorEditorContent::Callback;

public:
	ColorEditor() = delete;
	ColorEditor(const Callback& callback,
		const juce::Colour& defaultColor);

	void closeButtonPressed() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorEditor)
};
