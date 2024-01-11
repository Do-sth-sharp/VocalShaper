AC.addMixerTrack(-1, -1);
AC.addMixerTrackOutput(0, 0, 0);  AC.addMixerTrackOutput(0, 1, 1);
AC.addInstr(0, -1, "VST3-MONSTER Piano v2-2022.07-d77ff23b-efa9463a");
AC.addInstrOutput(0, 0, 0, 0);
AC.addInstrOutput(0, 1, 0, 1);
AC.addSequencerTrack(-1, -1);
AC.addSequencerTrackMidiOutputToInstr(0, 0);
AC.addMIDISource();
AC.addSequencerSourceInstance(0, 0, 0, 100, 0);
AC.addRecorderSourceInstance(0, 0, 0);

AC.setInstrWindow(0, true);