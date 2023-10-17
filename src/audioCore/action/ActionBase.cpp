#include "ActionBase.h"
#include "ActionDispatcher.h"

void ActionBase::output(const juce::String& mes) {
	ActionDispatcher::getInstance()->outputInternal(mes);
}
