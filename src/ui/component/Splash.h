#pragma once

#include <JuceHeader.h>

class Splash final : public juce::Component {
public:
	Splash();
	~Splash() override = default;

	void paint(juce::Graphics& g) override;
	void mouseDown(const juce::MouseEvent& e) override;

	void ready();
	void showMessage(const juce::String& message);

	void closeSplash();

private:
	bool isReady = false;
	juce::String mesStr, platStr, verStr, relStr, fraStr;
	std::unique_ptr<juce::Image> logo;
	juce::Typeface::Ptr titleType, subTitleType, textType;

	juce::String illustStr;
	juce::Colour picBackGroundColor;
	std::unique_ptr<juce::Image> relImg;
	juce::Point<double> imgCentrePoint;
	double imgScale = 1.0;

	class CloseTimer final : public juce::Timer {
		Splash* splash = nullptr;

	public:
		CloseTimer(Splash* splash) :Timer(), splash(splash) {};
		void timerCallback() override {
			if (this->isTimerRunning()) {
				this->stopTimer();
			}

			if (this->splash) {
				this->splash->closeSplash();
			}
		};
	};
	class HideTimer final : public juce::Timer {
		Splash* splash = nullptr;

	public:
		HideTimer(Splash* splash) :Timer(), splash(splash) {};
		void timerCallback() override {
			if (this->isTimerRunning()) {
				this->stopTimer();
			}

			if (this->splash && this->splash->isVisible()) {
				this->splash->setVisible(false);
			}
		};
	};

	std::unique_ptr<CloseTimer> closeTimer;
	std::unique_ptr<HideTimer> hideTimer;

	juce::ComponentAnimator fader;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Splash)
};

