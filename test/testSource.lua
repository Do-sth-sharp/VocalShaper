-- Test SeqSource
AC.addSequencerTrack(-1, -1);
AC.removeSequencerTrack(0);

-- Link Source MIDI Output
AC.addSequencerTrackMidiOutputToInstr(0, 0);
AC.addSequencerTrackMidiOutputToMixer(0, 0);

-- Link Source Audio Output
AC.addSequencerTrackOutput(0, 0, 0, 0);
AC.addSequencerTrackOutput(0, 1, 0, 1);

-- Source Bypass
AC.setSequencerTrackBypass(0, true);
AC.setSequencerTrackBypass(0, false);

-- Add Source
AC.addAudioSource();
AC.addMIDISource();

-- Remove Source
AC.removeSource(0);

-- Load Source
AC.loadSource(0, "./test.wav");
AC.loadSourceAsync(0, "./test.wav");

-- Save Source
AC.saveSource(0, "./test.wav");
AC.saveSourceAsync(0, "./test.wav");

-- Add Instance
AC.addSequencerSourceInstance(0, 0, 0, 10, 0);

-- Remove Instance
AC.removeSequencerSourceInstance(0, 0);
