cmake_minimum_required (VERSION 3.15)
project (VocalShaper VERSION 0.0.2 LANGUAGES CXX C)

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
if (MSVC)
    add_compile_definitions ("_CRT_SECURE_NO_WARNINGS")
endif (MSVC)

if (WIN32)
	set (COMPILE_SYS_DEF "WIN32=1")
elseif (UNIX AND NOT APPLE)
	set (COMPILE_SYS_DEF "LINUX=1")
elseif (APPLE)
	set (COMPILE_SYS_DEF "DARWIN=1")
endif ()

# Packages
find_package (juce-host-dev-kit CONFIG REQUIRED)
find_package (SynthEngineDemo CONFIG REQUIRED)
find_package (Lua REQUIRED)
find_package (cmake-lame CONFIG REQUIRED)
find_package (vsp4 CONFIG REQUIRED)
find_package (flowui CONFIG REQUIRED)
find_package (sigar CONFIG REQUIRED)

set (OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set (ENGINES_DIR "${OUTPUT_DIR}/synth-engines")

# App Configs
file (GLOB_RECURSE APP_CONFIG_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/app/*")
set (APP_CONFIG_OUTPUT "")
foreach (ITEM ${APP_CONFIG_SRC})
    string (REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/app" "${OUTPUT_DIR}" ITEM_TARGET ${ITEM})
	list (APPEND APP_CONFIG_OUTPUT ${ITEM_TARGET})

	cmake_path (CONVERT ${ITEM} TO_NATIVE_PATH_LIST ITEM_INPUT)
	cmake_path (CONVERT ${ITEM_TARGET} TO_NATIVE_PATH_LIST ITEM_OUTPUT)

    add_custom_command (
		OUTPUT ${ITEM_TARGET}
		COMMAND copy ${ITEM_INPUT} ${ITEM_OUTPUT}
		DEPENDS ${ITEM}
		COMMENT "Copying application config file: ${ITEM}"
		VERBATIM)
endforeach (ITEM ${APP_CONFIG_SRC})
add_custom_target (app_config_copy
	DEPENDS ${APP_CONFIG_OUTPUT}
	COMMENT "Copy application configs"
	VERBATIM)

# RemixIcon
file (GLOB_RECURSE REMIXICON_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/RemixIcon/icons/*.svg")
set (REMIXICON_OUTPUT "")
foreach (ITEM ${REMIXICON_SRC})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/RemixIcon/icons" "${OUTPUT_DIR}/RemixIcon" ITEM_TARGET ${ITEM})
	list (APPEND REMIXICON_OUTPUT ${ITEM_TARGET})

	cmake_path (CONVERT ${ITEM} TO_NATIVE_PATH_LIST ITEM_INPUT)
	cmake_path (CONVERT ${ITEM_TARGET} TO_NATIVE_PATH_LIST ITEM_OUTPUT)

    add_custom_command (
		OUTPUT ${ITEM_TARGET}
		COMMAND copy ${ITEM_INPUT} ${ITEM_OUTPUT}
		DEPENDS ${ITEM}
		COMMENT "Copying icon file: ${ITEM}"
		VERBATIM)
endforeach (ITEM ${REMIXICON_SRC})
add_custom_target (remix_icon_copy
	DEPENDS ${REMIXICON_OUTPUT}
	COMMENT "Copy RemixIcon files"
	VERBATIM)

# Engines
function (engine_get_pdb var vst3_path)
	get_filename_component (vst3_dir ${vst3_path} DIRECTORY)
	get_filename_component (vst3_name ${vst3_path} NAME_WE)
	set (${var} "${vst3_dir}/${vst3_name}.pdb" PARENT_SCOPE)
endfunction (engine_get_pdb var vst3_path)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	get_target_property (ENGINE_PATH SynthEngineDemo::SynthEngineDemo IMPORTED_LOCATION_DEBUG)
	engine_get_pdb (ENGINE_PDB_PATH ${ENGINE_PATH})
else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	get_target_property (ENGINE_PATH SynthEngineDemo::SynthEngineDemo IMPORTED_LOCATION_RELEASE)
	set (ENGINE_PDB_PATH "")
endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
set (ENGINE_LIST ${ENGINE_LIST} "${ENGINE_PATH}")
if (MSVC)
	set (ENGINE_LIST ${ENGINE_LIST} "${ENGINE_PDB_PATH}")
endif (MSVC)

set (ENGINE_OUTPUT "")
foreach (ITEM ${ENGINE_LIST})
	get_filename_component (ITEM_NAME ${ITEM} NAME)
	set (ITEM_TARGET "${ENGINES_DIR}/${ITEM_NAME}")
	list (APPEND ENGINE_OUTPUT ${ITEM_TARGET})

	cmake_path (CONVERT ${ITEM} TO_NATIVE_PATH_LIST ITEM_INPUT)
	cmake_path (CONVERT ${ITEM_TARGET} TO_NATIVE_PATH_LIST ITEM_OUTPUT)

    add_custom_command (
		OUTPUT ${ITEM_TARGET}
		COMMAND copy ${ITEM_INPUT} ${ITEM_OUTPUT}
		DEPENDS ${ITEM}
		COMMENT "Copying engine file: ${ITEM}"
		VERBATIM)
endforeach (ITEM ${ENGINE_LIST})
add_custom_target (engines_copy
	DEPENDS ${ENGINE_OUTPUT}
	COMMENT "Copy engines"
	VERBATIM)
add_dependencies (engines_copy
	SynthEngineDemo::SynthEngineDemo
)

# lame
function (lame_get_pdb var bin_path)
	get_filename_component (bin_dir ${bin_path} DIRECTORY)
	get_filename_component (bin_name ${bin_path} NAME_WE)
	set (${var} "${bin_dir}/${bin_name}.pdb" PARENT_SCOPE)
endfunction (lame_get_pdb var bin_path)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	get_target_property (LAME_PATH cmake-lame::lame IMPORTED_LOCATION_DEBUG)
	lame_get_pdb (LAME_PDB_PATH ${LAME_PATH})
else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	get_target_property (LAME_PATH cmake-lame::lame IMPORTED_LOCATION_RELEASE)
	set (LAME_PDB_PATH "")
endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
set (LAME_LIST ${LAME_LIST} "${LAME_PATH}")
if (MSVC)
	set (LAME_LIST ${LAME_LIST} "${LAME_PDB_PATH}")
endif (MSVC)

set (LAME_OUTPUT "")
foreach (ITEM ${LAME_LIST})
	get_filename_component (ITEM_NAME ${ITEM} NAME)
	set (ITEM_TARGET "${OUTPUT_DIR}/${ITEM_NAME}")
	list (APPEND LAME_OUTPUT ${ITEM_TARGET})

	cmake_path (CONVERT ${ITEM} TO_NATIVE_PATH_LIST ITEM_INPUT)
	cmake_path (CONVERT ${ITEM_TARGET} TO_NATIVE_PATH_LIST ITEM_OUTPUT)

    add_custom_command (
		OUTPUT ${ITEM_TARGET}
		COMMAND copy ${ITEM_INPUT} ${ITEM_OUTPUT}
		DEPENDS ${ITEM}
		COMMENT "Copying lame executable file: ${ITEM}"
		VERBATIM)
endforeach (ITEM ${LAME_LIST})
add_custom_target (lame_copy
	DEPENDS ${LAME_OUTPUT}
	COMMENT "Copy lame executable file"
	VERBATIM)
add_dependencies (lame_copy
	cmake-lame::lame
)

# Plugin Searcher Target
file (GLOB_RECURSE PLUGINSEARCHER_SRC CONFIGURE_DEPENDS "./pluginSearcher/*.cpp")
add_executable (PluginSearcher ${PLUGINSEARCHER_SRC})
target_compile_definitions (PluginSearcher PRIVATE
	"PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
	"PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
	"PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
)
if (NOT MSVC)
	target_compile_options (PluginSearcher PRIVATE
		-pthread -fPIE)
endif (NOT MSVC)
if (MINGW)
	target_link_options (PluginSearcher PRIVATE
		-static -static-libgcc -static-libstdc++)
endif (MINGW)
if (MSVC)
	target_link_libraries (PluginSearcher PRIVATE juce-host-dev-kit::juce-full)
else (MSVC)
	target_link_libraries (PluginSearcher PRIVATE juce-host-dev-kit::juce-full-static)
endif (MSVC)
if (MSVC)
	target_link_options (PluginSearcher PRIVATE 
		/SUBSYSTEM:WINDOWS
	)
endif (MSVC)

set_target_properties (PluginSearcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# File Registrar Target
file (GLOB_RECURSE FILEREGISTRAR_SRC CONFIGURE_DEPENDS "./fileRegistrar/*.cpp")
add_executable (FileRegistrar ${FILEREGISTRAR_SRC})
target_compile_definitions (FileRegistrar PRIVATE
	"PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
	"PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
	"PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
)
if (NOT MSVC)
	target_compile_options (FileRegistrar PRIVATE
		-pthread -fPIE)
endif (NOT MSVC)
if (MINGW)
	target_link_options (FileRegistrar PRIVATE
		-static -static-libgcc -static-libstdc++)
endif (MINGW)
if (MSVC)
	target_link_libraries (FileRegistrar PRIVATE juce-host-dev-kit::juce-core)
else (MSVC)
	target_link_libraries (FileRegistrar PRIVATE juce-host-dev-kit::juce-core-static)#To avoid "[[deprecated]] __attribute__ ((visibility ("default")))" failure.
endif (MSVC)
if (MSVC)
	target_link_options (FileRegistrar PRIVATE 
		/SUBSYSTEM:WINDOWS
	)
endif (MSVC)

set_target_properties (FileRegistrar PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# Main Target
file (GLOB_RECURSE VOCALSHAPER_SRC CONFIGURE_DEPENDS "./src/*.cpp" "./src/*.c" "./src/*.rc")
add_executable (VocalShaper ${VOCALSHAPER_SRC} "./JuceFix/fix.cpp")
target_include_directories (VocalShaper PRIVATE ${LUA_INCLUDE_DIR} "./sigar/include" "./src")
target_compile_definitions (VocalShaper PRIVATE ${COMPILE_SYS_DEF})
target_compile_definitions (VocalShaper PRIVATE
	"PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
	"PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
	"PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
)
target_link_libraries (VocalShaper PRIVATE
	flowui::flowui
	${LUA_LIBRARIES}
	vsp4::vsp4
	sigar::sigar
)
if (NOT MSVC)
	target_compile_options (VocalShaper PRIVATE
		-pthread -fPIE
		-msse3 -mavx2 -mavx512f)
endif (NOT MSVC)
if (MINGW)
	target_link_options (VocalShaper PRIVATE
		-static -static-libgcc -static-libstdc++)
endif (MINGW)
if (MSVC)
	target_link_libraries (VocalShaper PRIVATE juce-host-dev-kit::juce-full)
else (MSVC)
	target_link_libraries (VocalShaper PRIVATE juce-host-dev-kit::juce-full-static)
endif (MSVC)
if (WIN32)
    target_link_libraries (VocalShaper PRIVATE Dbghelp)
endif (WIN32)
if (MSVC)
	target_link_options (VocalShaper PRIVATE 
		/SUBSYSTEM:WINDOWS
	)
endif (MSVC)

add_dependencies (VocalShaper
	PluginSearcher FileRegistrar
	app_config_copy remix_icon_copy engines_copy lame_copy
)

set_target_properties (VocalShaper PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
