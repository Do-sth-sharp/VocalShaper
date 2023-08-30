cmake_minimum_required (VERSION 3.15)
project (VocalShaper VERSION 0.0.1 LANGUAGES CXX C)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release"  CACHE STRING "Choose the type of build." FORCE)
endif (NOT DEFINED CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 20)# Using C++20
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)# Using C17
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")# Using /MD and /MDd on MSVC

set (JUCE_USE_WINRT_MIDI OFF)# Windows 10.0.14393.0 or later
set (JUCE_ASIO ON)
set (JUCE_ASIO_DEBUGGING ON)
set (JUCE_WINRT_MIDI_LOGGING ON)
set (JUCE_PLUGINHOST_VST ON)
set (JUCE_PLUGINHOST_VST3 ON)
set (JUCE_PLUGINHOST_LV2 ON)
set (JUCE_PLUGINHOST_ARA OFF)
set (JUCE_CUSTOM_VST3_SDK ON)
set (JUCE_USE_MP3AUDIOFORMAT ON)
set (JUCE_USE_LAME_AUDIO_FORMAT ON)

set (ASIO_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/asiosdk_2.3.3_2019-06-14/common")
set (VST_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vst2-sdk-for-vst3sdk")
set (VST3_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk")

set (SYNTH_ENGINE_DEMO_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/app/synth-engines")
set (LAME_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/app")

add_subdirectory (LUCE-cpp)
add_subdirectory (SynthEngineDemo)
add_subdirectory (cmake-lame)

# Mackie Control
set (MACKIECONTROL_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libMackieControl/src")
file (GLOB_RECURSE MACKIECONTROL_SRC "${MACKIECONTROL_SRC_PATH}/*.cpp")

# DMDA
set (DMDA_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libDMDA")
set (DMDA_SRC "${DMDA_SRC_PATH}/DMDA.cpp")

# Plugin Searcher Target
file (GLOB_RECURSE PLUGINSEARCHER_SRC "./pluginSearcher/*.cpp")
add_executable (PluginSearcher ${PLUGINSEARCHER_SRC})
if (MSVC)
    target_compile_definitions (PluginSearcher PRIVATE "JUCE_DLL=1" "JUCE_DISABLE_DLL_ALLOCATORS=1")
endif (MSVC)
target_compile_definitions (PluginSearcher PRIVATE "LUCE_DLL_BUILD=1")
target_link_libraries (PluginSearcher PRIVATE libJUCE)
add_dependencies (PluginSearcher libJUCE)

set_target_properties (PluginSearcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/app")

# Main Target
file (GLOB_RECURSE VOCALSHAPER_SRC "./src/*.cpp" "./src/*.rc")
add_executable (VocalShaper ${VOCALSHAPER_SRC} ${MACKIECONTROL_SRC} ${DMDA_SRC})
target_include_directories (VocalShaper PRIVATE ${MACKIECONTROL_SRC_PATH} ${DMDA_SRC_PATH})
target_compile_definitions (VocalShaper PRIVATE "LUCE_DLL=1")
target_link_libraries (VocalShaper PRIVATE libLUCE)
add_dependencies (VocalShaper libLUCE PluginSearcher SynthEngineDemo lame)

set_target_properties (VocalShaper PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/app")
