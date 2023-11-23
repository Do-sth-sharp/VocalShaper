#include "MidiDebuggerComponent.h"
#include "../../audioCore/AC_API.h"

MidiDebuggerComponent::MidiDebuggerComponent()
	: FlowComponent(TRANS("MIDI Debugger")) {
	this->comp = quickAPI::getMidiDebugger();
	this->addAndMakeVisible(this->comp);
}

void MidiDebuggerComponent::resized() {
	if (this->comp) {
		this->comp->setBounds(this->getLocalBounds());
	}
}
