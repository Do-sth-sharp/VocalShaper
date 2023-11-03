-- Create A Stereo Track
AC.addMixerTrack(-1, -1);
AC.addMixerTrackInputFromDevice(0, 0, 0);
AC.addMixerTrackInputFromDevice(0, 0, 1);
AC.addMixerTrackOutput(0, 0, 0);
AC.addMixerTrackOutput(0, 1, 1);

-- Set Gain And Add Effect Plugin
AC.setMixerTrackGain(0, 5);
AC.addEffect(0, 0, "VST3-Experiverb-31f1abe0-a118c70b");
AC.addEffect(0, 1, "VST3-Clone-deb2f263-f74aeabb");

-- Remove Effect Plugin
AC.removeEffect(0, 0);

-- Effect Plugin Window
AC.setEffectWindow(0, 0, true);
AC.setEffectWindow(0, 0, false);

-- Effect Plugin Bypass
AC.setEffectBypass(0, 0, true);
AC.setEffectBypass(0, 0, false);

-- Track Audio Send
AC.addMixerTrackSend(0, 0, 1, 0);
AC.addMixerTrackSend(0, 1, 1, 1);

-- Effect Plugin Param
AC.listEffectParam(0, 0);
AC.echoEffectParamValue(0, 0, 0);
AC.echoEffectParamDefaultValue(0, 0, 0);
AC.setEffectParamValue(0, 0, 0, 1);

-- Track MIDI Input From Device
AC.addMixerTrackMidiInput(0);
AC.removeMixerTrackMidiInput(0);

-- Effect Plugin MIDI Channel
AC.setEffectMIDIChannel(0, 0, 0);

-- Effect Plugin Param MIDI CC
AC.setEffectMIDICCIntercept(0, 0, true);
AC.echoEffectParamCC(0, 0, 0);
AC.echoEffectCCParam(0, 0, 17);
AC.removeEffectParamCCConnection(0, 0, 17);

-- Track MIDI Output
AC.addMixerTrackMidiOutput(0);
