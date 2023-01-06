cmake_minimum_required (VERSION 3.15)
project (LUCE-cpp VERSION 0.0.1 LANGUAGES CXX C)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release"  CACHE STRING "Choose the type of build." FORCE)
endif (NOT DEFINED CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 20)#使用C++20
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)#使用C17
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")#MSVC下使用/MD和/MDd

add_subdirectory (LUCE-cpp)

file (GLOB_RECURSE VOCALSHAPER_SRC "./src/*.cpp")
add_executable (VocalShaper ${VOCALSHAPER_SRC})
target_compile_definitions (VocalShaper PRIVATE "LUCE_DLL=1")
target_link_libraries (VocalShaper PRIVATE libLUCE)
add_dependencies (VocalShaper libLUCE)

set_target_properties (VocalShaper PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/app")

add_custom_command (TARGET VocalShaper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:libJUCE> $<TARGET_FILE_DIR:VocalShaper>
)
add_custom_command (TARGET VocalShaper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:libLua> $<TARGET_FILE_DIR:VocalShaper>
)
add_custom_command (TARGET VocalShaper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:libLUCE> $<TARGET_FILE_DIR:VocalShaper>
)
