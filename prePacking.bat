del /f /s /q .\app\data\audio\blackPlugins.txt
del /f /s /q .\app\data\audio\pluginPaths.txt
del /f /s /q .\app\data\audio\plugins.xml

del /f /s /q .\app\data\audio\deadPlugins\*
rmdir /s /q .\app\data\audio\deadPlugins

del /f /s /q .\app\*.mid
del /f /s /q .\app\*.wav
del /f /s /q .\app\*.mp3
del /f /s /q .\app\*.flac
del /f /s /q .\app\*.ogg
del /f /s /q .\app\*.pkf
del /f /s /q .\app\*.vshl.dmp
del /f /s /q .\app\*.vsml.dmp
del /f /s /q .\app\*.vsll.dmp

del /f /s /q .\app\synth-engines\*.iobj
del /f /s /q .\app\synth-engines\*.ipdb
del /f /s /q .\app\synth-engines\*.exp
del /f /s /q .\app\synth-engines\*.ilk
del /f /s /q .\app\synth-engines\*.lib

del /f /s /q .\app\*.iobj
del /f /s /q .\app\*.ipdb
del /f /s /q .\app\*.exp
del /f /s /q .\app\*.ilk
del /f /s /q .\app\*.lib

del /f /s /q .\app\protoc.exe
del /f /s /q .\app\libprotoc.dll
del /f /s /q .\app\libprotocd.dll
del /f /s /q .\app\protoc.pdb
del /f /s /q .\app\libprotoc.pdb
del /f /s /q .\app\libprotocd.pdb