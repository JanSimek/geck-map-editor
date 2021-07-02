cmake_minimum_required (VERSION 3.8)

project (geck-mapper 
    LANGUAGES CXX
    VERSION 0.1
)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to Release as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(cmake/CPM.cmake)

CPMAddPackage("gh:gabime/spdlog@1.8.2")
CPMAddPackage("gh:jarro2783/cxxopts@2.2.1")
CPMAddPackage("gh:SFML/SFML#2.5.1")
CPMAddPackage("gh:ocornut/imgui#docking")
CPMAddPackage(
    GITHUB_REPOSITORY "eliasdaler/imgui-sfml"
    VERSION "2.3"
    OPTIONS "IMGUI_DIR ${imgui_SOURCE_DIR}"
    OPTIONS "IMGUI_SFML_IMGUI_DEMO ON" # not including demo breaks build
)

add_subdirectory(src)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
