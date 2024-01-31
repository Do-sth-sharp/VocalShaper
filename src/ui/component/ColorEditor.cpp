#include "ColorEditor.h"
#include "../misc/RCManager.h"
#include "../Utils.h"

ColorEditorContent::ColorEditorContent(
	const Callback& callback,
	const juce::Colour& defaultColor)
	: callback(callback) {

}

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
