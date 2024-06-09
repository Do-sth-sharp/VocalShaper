# VocalShaper [![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/works-on-my-machine.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/built-with-love.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/made-with-c-plus-plus.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/fixed-bugs.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/open-source.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/powered-by-black-magic.svg)](https://github.com/FangCunWuChang/for-the-badge/)
  
A JUCE-based Open Source DAW. 
> [!IMPORTANT]
> Under development, not released.  

## Supported Technologies
### Audio Plugins
- VST
- VST3
- LV2
- AU (TODO)

### Audio Drivers
- WASAPI
- DirectSound
- ASIO
- CoreAudio (TODO)

### Sequence File Formats
- mid
- musicxml (TODO)
- ufdata (TODO)
- json (OpenSVIP model file)(TODO)

### Audio File Formats
- wav
- bwf
- flac
- mp3
- ogg
- aiff / aif
- wma
- wmv
- asf
- wm
- aac (TODO)
- m4a (TODO)
- 3gp (TODO)

### Others
- MIDI IO
- MIDI CC
- MMC
- Mackie Control (TODO)

## Get Source Code
### Windows (MSVC)
```
git clone https://github.com/Do-sth-sharp/VocalShaper.git
cd VocalShaper
git checkout develop
git submodule update --init --recursive
cd vcpkg
.\bootstrap-vcpkg.bat
vcpkg install --x-manifest-root=../scripts/vcpkg-manifest --x-install-root=./installed --triplet=x64-windows
cd ..
```

### Windows (MinGW)
```
git clone https://github.com/Do-sth-sharp/VocalShaper.git
cd VocalShaper
git checkout develop
git submodule update --init --recursive
cd vcpkg
.\bootstrap-vcpkg.bat
vcpkg install --x-manifest-root=../scripts/vcpkg-manifest --x-install-root=./installed --triplet=x64-mingw-dynamic
cd ..
```

## Compile
### Windows Debug (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-windows -B out/build/x64-Debug-MSVC .
ninja -C out/build/x64-Debug-MSVC -j 8 VocalShaper
```

### Windows Release (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-windows -B out/build/x64-Release-MSVC .
ninja -C out/build/x64-Release-MSVC -j 8 VocalShaper
```

### Windows Debug (MinGW)
```
set CC=gcc
set CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-mingw-dynamic -B out/build/x64-Debug-MinGW .
ninja -C out/build/x64-Debug-MinGW -j 8 VocalShaper
```

### Windows Release (MinGW)
```
set CC=gcc
set CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-mingw-dynamic -B out/build/x64-Release-MinGW .
ninja -C out/build/x64-Release-MinGW -j 8 VocalShaper
