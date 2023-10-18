﻿#include "ActionAdd.h"

#include "../AudioCore.h"
#include "../Utils.h"

ActionAddPluginBlackList::ActionAddPluginBlackList(const juce::String& plugin)
	: plugin(plugin) {}

bool ActionAddPluginBlackList::doAction() {
	if (AudioCore::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin black list while searching plugin.");
		return false;
	}

	AudioCore::getInstance()->addToPluginBlackList(this->plugin);
	this->output("Add to plugin black list.");
	return true;
}

ActionAddPluginSearchPath::ActionAddPluginSearchPath(const juce::String& path)
	: path(path) {}

bool ActionAddPluginSearchPath::doAction() {
	if (AudioCore::getInstance()->pluginSearchThreadIsRunning()) {
		this->output("Don't change plugin search path while searching plugin.");
		return false;
	}

	AudioCore::getInstance()->addToPluginSearchPath(this->path);
	this->output("Add to plugin search path.");
	return true;
}

ActionAddMixerTrack::ActionAddMixerTrack(int index, int type)
	: index(index), type(type) {}

bool ActionAddMixerTrack::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		juce::AudioChannelSet trackType = utils::getChannelSet(
			static_cast<utils::TrackType>(this->type));

		graph->insertTrack(this->index, trackType);

		this->output("Add Mixer Track: [" + juce::String(this->type) + "]" + trackType.getDescription() + "\n"
			+ "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrack::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeTrack(this->index);

		this->output(juce::String{ "Undo Add Mixer Track.\n" }
			+ "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackSend::ActionAddMixerTrackSend(
	int src, int srcc, int dst, int dstc)
	: src(src), srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionAddMixerTrackSend::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackSend::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2TrkConnection(
			this->src, this->dst, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackInputFromDevice::ActionAddMixerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: srcc(srcc), dst(dst), dstc(dstc) {}

bool ActionAddMixerTrackInputFromDevice::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("[Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackInputFromDevice::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioI2TrkConnection(
			this->dst, this->srcc, this->dstc);

		this->output("Undo [Device] " + juce::String(this->srcc) + " - " + juce::String(this->dst) + ", " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

ActionAddMixerTrackOutput::ActionAddMixerTrackOutput(
	int src, int srcc, int dstc)
	: src(src), srcc(srcc), dstc(dstc) {}

bool ActionAddMixerTrackOutput::doAction() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output(juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}

bool ActionAddMixerTrackOutput::undo() {
	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->removeAudioTrk2OConnection(
			this->src, this->srcc, this->dstc);

		this->output("Undo " + juce::String(this->src) + ", " + juce::String(this->srcc) + " - " + "[Device] " + juce::String(this->dstc) + "\n");
		return true;
	}
	return false;
}