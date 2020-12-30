cmake_minimum_required (VERSION 3.8)

project (geck-mapper 
    LANGUAGES CXX
    VERSION 0.1
)

set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} 
    imgui-sfml/2.1@bincrafters/stable
    cxxopts/2.2.1
#    spdlog/1.8.2
)

include(cmake/Conan.cmake)
run_conan()

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME Format.cmake
  VERSION 1.6
  GITHUB_REPOSITORY TheLartians/Format.cmake
  OPTIONS # enable cmake formatting (optional)
          "FORMAT_CHECK_CMAKE OFF"
          # path to exclude (optional, supports regular expressions)
          "CMAKE_FORMAT_EXCLUDE cmake/CPM.cmake"
)

add_subdirectory(src)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
