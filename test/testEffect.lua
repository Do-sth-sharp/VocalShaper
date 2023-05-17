-- Create A Stereo Track
AC.addMixerTrack(-1, -1);
AC.addMixerTrackInputFromDevice(0, 0, 0);
AC.addMixerTrackInputFromDevice(0, 0, 1);
AC.addMixerTrackOutput(0, 0, 0);
AC.addMixerTrackOutput(0, 1, 1);

-- Set Gain And Add Effect Plugin
AC.setMixerTrackGain(0, 5);
AC.addMixerPlugin(0, 0, "VST3-Experiverb-31f1abe0-a118c70b");
AC.addMixerPlugin(0, 1, "VST3-Clone-deb2f263-f74aeabb");

-- Remove Effect Plugin
AC.removeMixerPlugin(0, 0);

-- Effect Plugin Window
AC.setMixerPluginWindow(0, 0, true);
AC.setMixerPluginWindow(0, 0, false);

-- Effect Plugin Bypass
AC.setMixerPluginBypass(0, 0, true);
AC.setMixerPluginBypass(0, 0, false);

-- Track Audio Send
AC.addMixerTrackSend(0, 0, 1, 0);
AC.addMixerTrackSend(0, 1, 1, 1);
