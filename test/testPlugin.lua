AC.addMixerTrack(-1, -1);
AC.addMixerTrackInputFromDevice(0, 0, 0);
AC.addMixerTrackInputFromDevice(0, 0, 1);
AC.addMixerTrackOutput(0, 0, 0);
AC.addMixerTrackOutput(0, 1, 1);

AC.setMixerTrackGain(0, 5);
AC.addMixerPlugin(0, 0, "VST3-Experiverb-31f1abe0-a118c70b");
AC.addMixerPlugin(0, 1, "VST3-Clone-deb2f263-f74aeabb");

AC.removeMixerPlugin(0, 0);

AC.setMixerPluginWindow(0, 0, true);
AC.setMixerPluginWindow(0, 0, false);

AC.setMixerPluginBypass(0, 0, true);
AC.setMixerPluginBypass(0, 0, false);
