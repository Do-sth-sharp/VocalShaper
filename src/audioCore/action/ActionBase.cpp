#include "ActionBase.h"
#include "ActionDispatcher.h"

void ActionBase::output(const juce::String& mes) {
	ActionDispatcher::getInstance()->outputInternal(mes);
}

void ActionBase::error(const juce::String& mes) {
	ActionDispatcher::getInstance()->errorInternal(mes);
}
