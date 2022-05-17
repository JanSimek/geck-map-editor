cmake_minimum_required (VERSION 3.22)

project (
    geck-mapper # this will be the ${PROJECT_NAME}
    VERSION 0.1 # this will provide ${PROJECT_VERSION}
    DESCRIPTION "A map editor for Fallout 2"
    LANGUAGES CXX
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

##################################################
# dependencies

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

add_subdirectory("external/portable-file-dialogs")
add_subdirectory("external/spdlog")
add_subdirectory("external/cxxopts")

add_library(imgui INTERFACE)
add_library(imgui::imgui ALIAS imgui)
target_sources(imgui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/misc/cpp/imgui_stdlib.cpp")
target_include_directories(imgui INTERFACE "${CMAKE_SOURCE_DIR}/external/imgui" "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/misc/cpp")

set(BUILD_SHARED_LIBS TRUE CACHE BOOL "build shared SFLM-Imgui (static build requires static SFML libraries)" FORCE)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui" CACHE STRING "path to the imgui library sources" FORCE)
add_subdirectory("external/imgui-sfml")

# TODO: option to use system libraries instead

#find_package(imgui REQUIRED)
#find_package(ImGui-SFML REQUIRED)

add_subdirectory(src)

find_package(Catch2 3 QUIET)

if (Catch2_FOUND)
    add_subdirectory(tests)
    enable_testing()
endif()

# Enable verbose compiler warnings
add_compile_options(
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic>
)

##################################################
# show build configuration overview

message("")

message("${PROJECT_NAME} ${PROJECT_VERSION}
         compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
        generator | ${CMAKE_GENERATOR}
       build type | ${CMAKE_BUILD_TYPE}
         cxxflags | ${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}
        build dir | ${CMAKE_BINARY_DIR}
   install prefix | ${CMAKE_INSTALL_PREFIX}
")

##################################################
# install dependencies

# import for things like MS Visual Studio redistribution
include(InstallRequiredSystemLibraries)

#install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION})
