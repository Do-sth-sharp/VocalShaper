#include "ActionRemove.h"

#include "../AudioCore.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

ActionRemovePluginBlackList::ActionRemovePluginBlackList(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginBlackList::doAction() {
	if (AudioCore::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin black list while searching plugin.");
		return false;
	}

	AudioCore::getInstance()->removeFromPluginBlackList(this->path);
	
	this->output("Remove from plugin black list.");
	return true;
}

ActionRemovePluginSearchPath::ActionRemovePluginSearchPath(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginSearchPath::doAction() {
	if (AudioCore::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin search path while searching plugin.");
		return false;
	}

	AudioCore::getInstance()->removeFromPluginSearchPath(this->path);
	
	this->output("Remove from plugin search path.");
	return true;
}

ActionRemoveMixerTrack::ActionRemoveMixerTrack(int index)
	: index(index) {}

bool ActionRemoveMixerTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (this->index < 0 || this->index >= graph->getTrackNum()) { return false; }

		/** Save Connections */
		this->audioT2Trk = graph->getTrackInputFromTrackConnections(this->index);
		this->audioSrc2Trk = graph->getTrackInputFromSrcConnections(this->index);
		this->audioInstr2Trk = graph->getTrackInputFromInstrConnections(this->index);
		this->audioI2Trk = graph->getTrackInputFromDeviceConnections(this->index);
		this->audioTrk2T = graph->getTrackOutputToTrackConnections(this->index);
		this->audioTrk2O = graph->getTrackOutputToDeviceConnections(this->index);
		this->midiSrc2Trk = graph->getTrackMidiInputFromSrcConnections(this->index);
		this->midiI2Trk = graph->getTrackMidiInputFromDeviceConnections(this->index);
		this->midiTrk2O = graph->getTrackMidiOutputToDeviceConnections(this->index);

		/** Save Track State */
		auto track = graph->getTrackProcessor(this->index);
		if (!track) { return false; }
		auto state = track->serialize();

		auto statePtr = dynamic_cast<vsp4::MixerTrack*>(state.get());
		if (!statePtr) { return false; }
		statePtr->set_bypassed(graph->getTrackBypass(this->index));

		this->data.setSize(state->ByteSizeLong());
		state->SerializeToArray(this->data.getData(), this->data.getSize());

		/** Remove Track */
		graph->removeTrack(this->index);

		juce::String result;
		result += "Remove Mixer Track: " + juce::String(this->index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		return true;
	}
	return false;
}

bool ActionRemoveMixerTrack::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::MixerTrack>();
		if (!state->ParseFromArray(this->data.getData(), this->data.getSize())) {
			return false;
		}

		/** Add Track */
		graph->insertTrack(this->index,
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));
		
		/** Recover Track State */
		auto track = graph->getTrackProcessor(this->index);
		graph->setTrackBypass(this->index, state->bypassed());
		track->parse(state.get());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : this->audioT2Trk) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioSrc2Trk) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioInstr2Trk) {
			graph->setAudioInstr2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioI2Trk) {
			graph->setAudioI2TrkConnection(dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioTrk2T) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : this->audioTrk2O) {
			graph->setAudioTrk2OConnection(src, srcc, dstc);
		}

		for (auto [src, dst] : this->midiI2Trk) {
			graph->setMIDII2TrkConnection(dst);
		}

		for (auto [src, dst] : this->midiSrc2Trk) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		for (auto [src, dst] : this->midiTrk2O) {
			graph->setMIDITrk2OConnection(src);
		}

		juce::String result;
		result += "Undo Remove Mixer Track: " + juce::String(this->index) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		return true;
	}
	return false;
}
