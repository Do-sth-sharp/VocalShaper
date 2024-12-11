#include "MainGraph.h"
#include "../misc/AudioLock.h"
#include "../uiCallback/UICallback.h"

void MainGraph::removeIllegalAudioI2SrcConnections() {
	this->audioI2SrcConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, -1);
}
