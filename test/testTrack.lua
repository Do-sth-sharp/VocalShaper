-- Add Track
AC.addTrack(0, -1, -1);-- 0: Track, 1: AuxTrack
AC.addTrack(1, -1, -1);
AC.addTrack(1, -1, -1);

-- Remove Track
AC.removeTrack(1, 0);-- 0: Track, 1: AuxTrack

-- Add Input
AC.addTrackAudioInput(0, 0, 0, 0);
AC.addTrackAudioInput(0, 0, 1, 1);

AC.addTrackMIDIInput(0, 0);

-- Remove Send
AC.removeTrackAudioSendOnSlot(0, 0, 0);

-- Add Send
AC.addTrackAudioSend(0, 0, 0, 2, 0, 0, 0); -- 0: Device, 1: MasterTrack, 2: AuxTrack
AC.addTrackAudioSend(0, 0, 0, 2, 0, 1, 1);

AC.addTrackMIDISend(0, 0, 0, 2, 0);

AC.addTrackMIDISend(1, 0, 0, 1, 0);

-- Mute Solo
AC.setTrackSolo(0, 0, true); -- 0: Track, 1: AuxTrack, 2: MasterTrack
AC.setTrackSolo(0, 0, false);
AC.setTrackMute(0, 0, true);
AC.setTrackMute(0, 0, false);
AC.setTrackMute(1, 0, true);
AC.setTrackMute(1, 0, false);

-- Recording
AC.setTrackRecording(0, 0x01); -- 0xn1: MIDINewTrackMode, 0xn2: MIDIInsertMode, 0xn3: MIDICoverMode, 0x1n: AudioInsertMode, 0x2n: AudioCoverMode

-- Input Monitoring
AC.setTrackInputMonitoring(0, true);

-- Gain Pan Fader
AC.setTrackGain(0, 0, 1.0); -- 0: Track, 1: AuxTrack, 2: MasterTrack
AC.setTrackPan(0, 0, 0.2);
AC.setTrackFader(0, 0, 0.6);

-- Side Chain Bus
AC.addTrackSideChainBus(1, 0); -- 1: AuxTrack, 2: MasterTrack
AC.removeTrackSideChainBus(1, 0);
