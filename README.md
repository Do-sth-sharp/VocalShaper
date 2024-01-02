# VocalShaper [![forthebadge](https://github.com/BraveUX/for-the-badge/blob/master/src/images/badges/works-on-my-machine.svg)](https://github.com/BraveUX/for-the-badge/)
[![forthebadge](https://github.com/BraveUX/for-the-badge/blob/master/src/images/badges/built-with-love.svg)](https://github.com/BraveUX/for-the-badge/)
[![forthebadge](https://github.com/BraveUX/for-the-badge/blob/master/src/images/badges/made-with-c-plus-plus.svg)](https://github.com/BraveUX/for-the-badge/)
[![forthebadge](https://github.com/BraveUX/for-the-badge/blob/master/src/images/badges/fixed-bugs.svg)](https://github.com/BraveUX/for-the-badge/)
[![forthebadge](https://github.com/BraveUX/for-the-badge/blob/master/src/images/badges/open-source.svg)](https://github.com/BraveUX/for-the-badge/)
[![forthebadge](https://github.com/BraveUX/for-the-badge/blob/master/src/images/badges/powered-by-black-magic.svg)](https://github.com/BraveUX/for-the-badge/)
  
A JUCE-based Open Source DAW. **Under development, not released.**  

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

## Compile
### Windows Debug (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -B out/build/x64-Debug .
ninja -C out/build/x64-Debug -j 8 VocalShaper
```

### Windows Release (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -B out/build/x64-Release .
ninja -C out/build/x64-Release -j 8 VocalShaper
```
