cmake_minimum_required (VERSION 3.8)

project (geck-mapper 
    LANGUAGES CXX
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to Release as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(cmake/CPM.cmake)

CPMAddPackage("gh:samhocevar/portable-file-dialogs#0.1.0")
CPMAddPackage("gh:gabime/spdlog@1.8.2")
CPMAddPackage("gh:jarro2783/cxxopts@2.2.1")

if(NOT WIN32) # imgui-sfml from CPMAddPackage fails to compile with imgui on Windows
    CPMAddPackage("gh:SFML/SFML#2.5.1")
    CPMAddPackage("gh:ocornut/imgui#docking")
    add_library(imgui INTERFACE)
    add_library(imgui::imgui ALIAS imgui)
    CPMAddPackage(
        GITHUB_REPOSITORY "eliasdaler/imgui-sfml"
        VERSION "2.3"
        OPTIONS "IMGUI_DIR ${imgui_SOURCE_DIR}"
    )
else() # vcpkg
    find_package(imgui REQUIRED)
    find_package(SFML COMPONENTS system window graphics REQUIRED)
    find_package(ImGui-SFML REQUIRED)
endif()

add_subdirectory(src)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
