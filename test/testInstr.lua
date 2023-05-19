-- Add Instrument Plugin
AC.addSequencerPlugin(0, "VST3-MONSTER Piano v2-2022.07-d77ff23b-efa9463a");
AC.addSequencerPlugin(1, "VST3-ACE Bridge-60483eb8-ca199f24");
AC.addSequencerPlugin(2, "VST-VOCALOID5 VSTi-b099a95-56355632");
AC.addSequencerPlugin(3, "VST-synthesizer-v-plugin64-c0552195-53796e56");
AC.addSequencerPlugin(4, "VST3-MONSTER Guitar v2.2022.09-e47046df-d16e2f22");

-- Set Audio Output
AC.addSequencerPluginOutput(0, 0, 0, 0);
AC.addSequencerPluginOutput(0, 1, 0, 1);

-- Remove Instrument Plugin
AC.removeSequencerPlugin(0);

-- Instrument Plugin Window
AC.setSequencerPluginWindow(0, true);
AC.setSequencerPluginWindow(0, false);

-- Instrument Plugin Bypass
AC.setSequencerPluginBypass(0, true);
AC.setSequencerPluginBypass(0, false);

-- Instrument Plugin MIDI Input From Device
AC.addSequencerPluginMidiInput(0);
AC.removeSequencerPluginMidiInput(0);

-- Instrument Plugin MIDI Channel
AC.setSequencerPluginMIDIChannel(0, 0);

-- Instrument Plugin Param
AC.listSequencerPluginParam(0);
