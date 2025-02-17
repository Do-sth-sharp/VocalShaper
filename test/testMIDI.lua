-- Add Note
AC.midiAddNote(0x1234, 0, 0.0, 1.0, 1, 60, 127, "la");
AC.midiAddNote(0x1234, 0, 1.0, 1.5, 1, 61, 127, "la");

-- Set Note Data
AC.midiSetNoteTime(0x1234, 0, 1, 0.5, 1.0);
AC.midiSetNoteChannel(0x1234, 0, 1, 2);
AC.midiSetNotePitch(0x1234, 0, 1, 48);
AC.midiSetNoteVelocity(0x1234, 0, 1, 90);
AC.midiSetNoteLyrics(0x1234, 0, 1, "");

-- Remove Note
AC.midiRemoveNote(0x1234, 0, 1);
