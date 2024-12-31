-- Add Effect Plugin
AC.addEffect(0, 0, 0, "VST3-Experiverb-31f1abe0-a118c70b"); -- 0: Track, 1: AuxTrack, 2: MasterTrack
AC.addEffect(0, 0, 1, "VST3-Clone-deb2f263-f74aeabb");

-- Remove Effect Plugin
AC.removeEffect(0, 0, 0);

-- Effect Plugin Bypass
AC.setEffectBypass(0, 0, 0, true);
AC.setEffectBypass(0, 0, 0, false);

-- Effect Plugin Param
AC.setEffectParamValue(0, 0, 0, 0, 1);

-- Effect Plugin MIDI Channel
AC.setEffectMIDIChannel(0, 0, 0, 0);

-- Effect Plugin Param MIDI CC
AC.setEffectMIDICCIntercept(0, 0, 0, true);
AC.setEffectParamConnectToCC(0, 0, 0, 0, 17);
