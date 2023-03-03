if exist .\app\libJUCE.dll del .\app\libJUCE.dll
if exist .\app\libLua.dll del .\app\libLua.dll
if exist .\app\libLUCE.dll del .\app\libLUCE.dll
mklink /H  .\app\libJUCE.dll .\out\build\x64-Debug\LUCE-cpp\libJUCE.dll
mklink /H  .\app\libLua.dll .\out\build\x64-Debug\LUCE-cpp\libLua.dll
mklink /H  .\app\libLUCE.dll .\out\build\x64-Debug\LUCE-cpp\libLUCE.dll