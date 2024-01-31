#include "ColorEditor.h"
#include "../misc/RCManager.h"
#include "../misc/ColorMap.h"
#include "../Utils.h"

class ColorHistory final : private juce::DeletedAtShutdown {
public:
	ColorHistory() = default;

	void add(const juce::Colour& color) {
		/** Remove If Already In List */
		this->colorList.removeFirstMatchingValue(color);

		/** Add At Head */
		this->colorList.insert(0, color);

		/** Limit List Size */
		if (this->colorList.size() > this->maxSize) {
			this->colorList.resize(this->maxSize);
		}
	};

	const juce::Array<juce::Colour>& getList() const {
		return this->colorList;
	};

private:
	const int maxSize = 24;
	juce::Array<juce::Colour> colorList;

public:
	static ColorHistory* getInstance();
	static void releaseInstance();

private:
	static ColorHistory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorHistory)
};

ColorHistory* ColorHistory::getInstance() {
	return ColorHistory::instance ? ColorHistory::instance
		: (ColorHistory::instance = new ColorHistory{});
}

void ColorHistory::releaseInstance() {
	if (ColorHistory::instance) {
		delete ColorHistory::instance;
		ColorHistory::instance = nullptr;
	}
}

ColorHistory* ColorHistory::instance = nullptr;

ColorEditorContent::ColorEditorContent(
	const Callback& callback,
	const juce::Colour& defaultColor)
	: callback(callback),
	historyList(ColorHistory::getInstance()->getList()),
	themeColors({ ColorMap::getInstance()->get("ThemeColorB2") }),
	themeAlertColors({ ColorMap::getInstance()->get("ThemeColorC1"),
		ColorMap::getInstance()->get("ThemeColorC2"), 
		ColorMap::getInstance()->get("ThemeColorC3"), 
		ColorMap::getInstance()->get("ThemeColorC4"), 
		ColorMap::getInstance()->get("ThemeColorC5"), 
		ColorMap::getInstance()->get("ThemeColorC6") }) {}

ColorEditor::ColorEditor(const Callback& callback,
	const juce::Colour& defaultColor)
	: DocumentWindow(TRANS("Color Editor"), juce::LookAndFeel::getDefaultLookAndFeel().findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton, true) {
	this->setUsingNativeTitleBar(true);

	/** Icon */
	auto icon = RCManager::getInstance()->loadImage(
		utils::getResourceFile("logo.png"));
	this->setIcon(icon);
	this->getPeer()->setIcon(icon);

	/** Content */
	this->setContentOwned(
		new ColorEditorContent{ callback, defaultColor }, false);

	/** Size */
	juce::MessageManager::callAsync(
		[comp = juce::Component::SafePointer(this)] {
			if (comp) {
				auto screenSize = utils::getScreenSize(comp);
				comp->centreWithSize(screenSize.getWidth() * 0.4,
					screenSize.getHeight() * 0.4);
			}
		}
	);
}

void ColorEditor::closeButtonPressed() {
	this->exitModalState();
}
