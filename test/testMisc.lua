-- Plugin
AC.searchPlugin();
AC.clearPlugin();

-- Play
AC.play();
AC.pause();
AC.stop();
AC.rewind();
AC.setPlayPosition();
AC.startRecord();
AC.stopRecord();

-- Render
AC.renderNow("./", "test", ".wav", { 0, 1, 2 }, {}, 24, 0);

-- Project
AC.newProject("C:/Music/vsp4/test/");
AC.save("testProj");
AC.load("C:/Music/vsp4/test/testProj.vsp4");
