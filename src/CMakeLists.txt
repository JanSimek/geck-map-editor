add_executable (GECK 
	main.cpp 
	
	Application.h 
	Application.cpp 
	
	state/State.h
	state/EditorState.h
	state/EditorState.cpp
)

target_link_libraries(geck-mapper PRIVATE CONAN_PKG::imgui-sfml)