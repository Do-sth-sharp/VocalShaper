#include "MainGraph.h"
#include "../uiCallback/UICallback.h"

void MainGraph::removeIllegalAudioI2TrkConnections() {
	this->audioI2TrkConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.source.channelIndex >= this->getTotalNumInputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
}

void MainGraph::removeIllegalAudioTrk2OConnections() {
	this->audioTrk2OConnectionList.removeIf(
		[this](const juce::AudioProcessorGraph::Connection& element) {
			if (element.destination.channelIndex >= this->getTotalNumOutputChannels()) {
				this->removeConnection(element);
				return true;
			}
			return false;
		});

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
}
