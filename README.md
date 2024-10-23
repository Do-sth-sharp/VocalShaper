# VocalShaper [![Build Artifacts](https://github.com/Do-sth-sharp/VocalShaper/actions/workflows/buildArtifacts.yml/badge.svg)](https://github.com/Do-sth-sharp/VocalShaper/actions/workflows/buildArtifacts.yml)

[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/built-with-love.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/made-with-c-plus-plus.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/fixed-bugs.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/open-source.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/powered-by-black-magic.svg)](https://github.com/FangCunWuChang/for-the-badge/)
[![forthebadge](https://github.com/FangCunWuChang/for-the-badge/blob/master/src/images/badges/works-on-my-machine.svg)](https://github.com/FangCunWuChang/for-the-badge/)
  
A JUCE-based Open Source DAW. This DAW is lightweight and can meet most daily usage needs.  
> [!IMPORTANT]
> Under development, not released.  

## Supported Technologies
### Audio Plugins
- VST
- VST3
- LV2
- ARA
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

### Windows (Clang-LLVM)
```
git clone https://github.com/Do-sth-sharp/VocalShaper.git
cd VocalShaper
git checkout develop
git submodule update --init --recursive
cd vcpkg
.\bootstrap-vcpkg.bat
vcpkg install --x-manifest-root=../scripts/vcpkg-manifest --x-install-root=./installed --triplet=x64-windows-llvm
cd ..
```

## Compile
### Windows Debug (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake --preset "Ninja Debug x64"
cmake --build build/ninja-debug-x64 --target VocalShaper
```

### Windows Release (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake --preset "Ninja Release x64"
cmake --build build/ninja-release-x64 --target VocalShaper
```
### Windows Debug (Clang-LLVM)
```
cmake --preset "Ninja Debug Win64 LLVM"
cmake --build build/ninja-debug-win64-llvm --target VocalShaper
```

### Windows Release (Clang-LLVM)
```
cmake --preset "Ninja Release Win64 LLVM"
cmake --build build/ninja-release-win64-llvm --target VocalShaper
```
