#include <JuceHeader.h>

/** To Fix Symbol Export Error Of juce::ScopedMessageBox */
#include <juce_gui_basics/detail/juce_ScopedMessageBoxInterface.h>
#include <juce_gui_basics/detail/juce_ScopedMessageBoxImpl.h>
//juce_gui_basics/windows/juce_ScopedMessageBox.cpp
#include "juce_ScopedMessageBox.cpp"

/** To Fix Symbol Export Error Of XmlElement::TextFormat::TextFormat() */
namespace juce {
	XmlElement::TextFormat::TextFormat() {}
}

/** To Fix Symbol Export Error Of juce::dsp::Panner<float> */
//juce_dsp/processors/juce_Panner.cpp
#include "juce_Panner.cpp"

/**
 * To Fix Symbol Export Error Of
 * juce::AAXClientExtensions::getPluginIDForMainBusConfig and
 * juce::AAXClientExtensions::getPageFileName.
 */
//juce_audio_processors/utilities/juce_AAXClientExtensions.cpp
#include "juce_AAXClientExtensions.cpp"

/** To Fix Symbol Export Of juce::AudioProcessorParameterGroup */
//juce_audio_processors/processors/juce_AudioProcessorParameterGroup.cpp
#include "juce_AudioProcessorParameterGroup.cpp"
