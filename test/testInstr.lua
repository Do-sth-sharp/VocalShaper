-- Add Instrument Plugin
AC.addSequencerPlugin(0, "VST3-MONSTER Piano v2-2022.07-d77ff23b-efa9463a");
AC.addSequencerPlugin(1, "VST3-ACE Bridge-60483eb8-ca199f24");

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
