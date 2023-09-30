-- Add Instrument Plugin
AC.addInstr(0, -1, "VST3-MONSTER Piano v2-2022.07-d77ff23b-efa9463a");
AC.addInstr(1, -1, "VST3-ACE Bridge-60483eb8-ca199f24");
AC.addInstr(2, -1, "VST-VOCALOID5 VSTi-b099a95-56355632");
AC.addInstr(3, -1, "VST-synthesizer-v-plugin64-c0552195-53796e56");
AC.addInstr(4, -1, "VST3-MONSTER Guitar v2.2022.09-e47046df-d16e2f22");

-- Set Audio Output
AC.addInstrOutput(0, 0, 0, 0);
AC.addInstrOutput(0, 1, 0, 1);

-- Remove Instrument Plugin
AC.removeInstr(0);

-- Instrument Plugin Window
AC.setInstrWindow(0, true);
AC.setInstrWindow(0, false);

-- Instrument Plugin Bypass
AC.setInstrBypass(0, true);
AC.setInstrBypass(0, false);

-- Instrument Plugin MIDI Input From Device
AC.addInstrMidiInput(0);
AC.removeInstrMidiInput(0);

-- Instrument Plugin MIDI Channel
AC.setInstrMIDIChannel(0, 0);

-- Instrument Plugin Param
AC.listInstrParam(0);
AC.echoInstrParamValue(0, 2);
AC.echoInstrParamDefaultValue(0, 2);
AC.setInstrParamValue(0, 2, 0.5);

-- Instrument Plugin Param MIDI CC
AC.setInstrParamListenCC(0, 2);
AC.setInstrMIDICCIntercept(0, true);
AC.echoInstrParamCC(0, 2);
AC.echoInstrCCParam(0, 85);
AC.removeInstrParamCCConnection(0, 85);
