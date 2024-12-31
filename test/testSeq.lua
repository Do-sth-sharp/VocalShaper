-- Add Instrument Plugin
AC.addInstr(0, "VST3-MONSTER Piano v2-2022.07-d77ff23b-efa9463a", false);
AC.addInstr(1, "VST3-ACE Bridge-60483eb8-ca199f24", false);
AC.addInstr(2, "VST-VOCALOID5 VSTi-b099a95-56355632", false);
AC.addInstr(3, "VST-synthesizer-v-plugin64-c0552195-53796e56", false);
AC.addInstr(4, "VST3-MONSTER Guitar v2.2022.09-e47046df-d16e2f22", false);

-- Remove Instrument Plugin
AC.removeInstr(0);

-- Instrument Plugin Bypass
AC.setInstrBypass(0, true);
AC.setInstrBypass(0, false);

-- Instrument Offline
AC.setInstrOffline(0, true);
AC.setInstrOffline(0, false);

-- Instrument Plugin MIDI Channel
AC.setInstrMIDIChannel(0, 0);

-- Instrument Plugin Param
AC.setInstrParamValue(0, 2, 0.5);

-- Instrument Plugin Param MIDI CC
AC.setInstrMIDICCIntercept(0, true);
AC.setInstrParamConnectToCC(0, 0, 85);

-- Init Source
AC.initAudio(0, "New Audio", 48000, 2, 2);
AC.initMIDI(0, "New MIDI");

-- Load And Save Source
AC.loadAudio(0, "test.wav");
AC.saveAudio(0, "test.wav");
AC.loadMIDI(0, "test.mid", true);
AC.saveMIDI(0, "test.mid");

-- Seq Block
AC.addSequencerBlock(0, 0, 300, 0);
AC.setSequencerBlockTime(0, 0, 0, 301, 0);
AC.removeSequencerBlock(0, 0);

-- Current MIDI Track
AC.setCurrentMIDITrack(1, 1);
