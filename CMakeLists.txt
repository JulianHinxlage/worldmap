cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory("tridot")

#general settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/tridot)
    set(CMAKE_DEBUG_POSTFIX "")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL 1)
endif()

#world map
project(worldmap)
file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src tridot/src)
target_link_libraries(${PROJECT_NAME} PUBLIC tridot)
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E touch "tridot/worldmap.dll"
    COMMAND ${CMAKE_COMMAND} -E rename "tridot/worldmap.dll" "tridot/junk-worldmap.dll" 
)

add_dependencies(tridot-editor worldmap)