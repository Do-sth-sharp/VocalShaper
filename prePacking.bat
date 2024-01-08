%Remove Unused Files%
del /f /s /q %1\data\audio\blackPlugins.txt
del /f /s /q %1\data\audio\pluginPaths.txt
del /f /s /q %1\data\audio\plugins.xml

del /f /s /q %1\data\audio\deadPlugins\*
rmdir /s /q %1\data\audio\deadPlugins

del /f /s /q %1\*.vshl.dmp
del /f /s /q %1\*.vsml.dmp
del /f /s /q %1\*.vsll.dmp

del /f /s /q %1\synth-engines\*.iobj
del /f /s /q %1\synth-engines\*.ipdb
del /f /s /q %1\synth-engines\*.exp
del /f /s /q %1\synth-engines\*.ilk
del /f /s /q %1\synth-engines\*.lib
del /f /s /q %1\synth-engines\*.a

del /f /s /q %1\*.iobj
del /f /s /q %1\*.ipdb
del /f /s /q %1\*.exp
del /f /s /q %1\*.ilk
del /f /s /q %1\*.lib
del /f /s /q %1\*.a
