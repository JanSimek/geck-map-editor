cmake_minimum_required (VERSION 3.8)

project (geck-mapper 
	LANGUAGES CXX
	VERSION 0.1
)

set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES} 
	imgui-sfml/2.1@bincrafters/stable
)

include(cmake/Conan.cmake)
run_conan()

add_subdirectory(src)
