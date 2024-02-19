﻿#pragma once

enum class ActionType : unsigned int {
	ActionAddMixerTrack = 0x0001,
	ActionAddMixerTrackSend,
	ActionAddMixerTrackInputFromDevice,
	ActionAddMixerTrackOutput,
	ActionAddEffect,
	ActionAddEffectAdditionalInput,/**< deprecated */
	ActionAddInstr,
	ActionAddInstrOutput,
	ActionAddInstrMidiInput,
	ActionAddMixerTrackMidiInput,
	ActionAddMixerTrackMidiOutput,
	ActionAddAudioSourceThenLoad,
	ActionAddAudioSourceThenInit,
	ActionAddMidiSourceThenLoad,
	ActionAddMidiSourceThenInit,
	ActionAddSequencerTrack,
	ActionAddSequencerTrackMidiOutputToMixer,
	ActionAddSequencerTrackMidiOutputToInstr,
	ActionAddSequencerTrackOutput,
	ActionAddSequencerSourceInstance,
	ActionAddRecorderSourceInstance,
	ActionAddMixerTrackSideChainBus,

	ActionCloneSource = 0x0101,
	ActionSave,
	ActionReloadSource,

	ActionRemoveMixerTrack = 0x0201,
	ActionRemoveMixerTrackSend,
	ActionRemoveMixerTrackInputFromDevice,
	ActionRemoveMixerTrackOutput,
	ActionRemoveEffect,
	ActionRemoveEffectAdditionalInput,/**< deprecated */
	ActionRemoveInstr,
	ActionRemoveInstrOutput,
	ActionRemoveInstrMidiInput,
	ActionRemoveInstrParamCCConnection,
	ActionRemoveEffectParamCCConnection,
	ActionRemoveMixerTrackMidiInput,
	ActionRemoveMixerTrackMidiOutput,
	ActionRemoveSource,
	ActionRemoveSequencerTrack,
	ActionRemoveSequencerTrackMidiOutputToMixer,
	ActionRemoveSequencerTrackMidiOutputToInstr,
	ActionRemoveSequencerTrackOutput,
	ActionRemoveSequencerSourceInstance,
	ActionRemoveRecorderSourceInstance,
	ActionRemoveMixerTrackSideChainBus,

	ActionSetMixerTrackGain = 0x0301,
	ActionSetMixerTrackPan,
	ActionSetMixerTrackSlider,
	ActionSetEffectBypass,
	ActionSetInstrBypass,
	ActionSetInstrMidiChannel,
	ActionSetEffectMidiChannel,
	ActionSetInstrParamValue,
	ActionSetEffectParamValue,
	ActionSetInstrParamConnectToCC,
	ActionSetEffectParamConnectToCC,
	ActionSetInstrMidiCCIntercept,
	ActionSetEffectMidiCCIntercept,
	ActionSetSequencerTrackBypass,
	ActionSetSourceSynthesizer,
	ActionSetSourceName,
	ActionSetSourceSynthDst,
	ActionSetEffectBypassByPtr,
	ActionSetInstrBypassByPtr,
	ActionSetInstrMidiChannelByPtr,
	ActionSetEffectMidiChannelByPtr,
	ActionSetInstrMidiCCInterceptByPtr,
	ActionSetEffectMidiCCInterceptByPtr,
	ActionSetInstrMidiOutputByPtr,
	ActionSetEffectMidiOutputByPtr,
	ActionSetInstrParamConnectToCCByPtr,
	ActionSetEffectParamConnectToCCByPtr,
	ActionSetMixerTrackName,
	ActionSetMixerTrackColor,
	ActionSetMixerTrackMute
};
