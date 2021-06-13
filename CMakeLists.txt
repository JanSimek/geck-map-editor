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

#set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES}
#    imgui-sfml/2.1@bincrafters/stable
#    cxxopts/2.2.1
#)

#include(cmake/Conan.cmake)
#run_conan()

include(cmake/CPM.cmake)

CPMAddPackage("gh:gabime/spdlog@1.8.2")
CPMAddPackage("gh:jarro2783/cxxopts@2.2.1")


add_subdirectory(src)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
