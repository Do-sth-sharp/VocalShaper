-- Plugin
AC.searchPlugin();
AC.clearPlugin();

-- Play
AC.play();
AC.pause();
AC.stop();
AC.rewind();
AC.setPlayPosition(2.0);
AC.startRecord();
AC.stopRecord();

-- Label
AC.addLabelTempo(1.0, 120.0);
AC.addLabelBeat(2.0, 3, 4);

AC.setLabelTime(0, 0.5);
AC.setLabelTempo(0, 80.0);
AC.setLabelBeat(1, 4, 4);

AC.removeLabel(0);

-- Render
AC.renderNow("./", "test", ".wav", { 0, 1, 2 }, {}, 24, 0);

-- Project
AC.newProject("C:/Music/vsp4/test/");
AC.save("testProj");
AC.load("C:/Music/vsp4/test/testProj.vsp4");

-- Source Save Config
AC.setAudioSaveBitsPerSample(".wav", 32);
AC.setAudioSaveMetaData(".wav", { ["bwav description"] = "test description", ["bwav origination date"] = "2023-08-26", ["bwav origination time"] = "00:42:00" });
AC.setAudioSaveQualityOptionIndex(".wav", 0);
