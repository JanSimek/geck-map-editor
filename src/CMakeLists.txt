add_executable (geck-mapper 
    main.cpp

    Application.h
    Application.cpp

    util/io.h
    util/ResourceManager.h
    util/ResourceManager.cpp
    util/FileHelper.h
    util/FileHelper.cpp

    # States
    state/StateMachine.h
    state/StateMachine.cpp

    state/State.h

    state/EditorState.h
    state/EditorState.cpp

    state/LoadingState.h
    state/LoadingState.cpp
    state/loader/Loader.h
    state/loader/MapLoader.cpp
    state/loader/MapLoader.h

    # UI
    ui/util.h
    ui/IconsFontAwesome5.h

    editor/Tile.h
    editor/Tile.cpp

    editor/Object.h
    editor/Object.cpp

    editor/helper/ObjectHelper.h

    editor/HexagonGrid.h
    editor/HexagonGrid.cpp

    editor/Hex.h
    editor/Hex.cpp

    #
    # FILE FORMATS
    #

    format/IFile.h
    format/IFile.cpp

    # DAT
    format/dat/Dat.h
    format/dat/Dat.cpp
    format/dat/DatEntry.h
    format/dat/DatEntry.cpp

    # FRM
    format/frm/Frm.h
    format/frm/Frm.cpp
    format/frm/Direction.h
    format/frm/Direction.cpp
    format/frm/Frame.h
    format/frm/Frame.cpp

    # PAL
    format/pal/Pal.h
    format/pal/Pal.cpp

    # MAP
    format/map/Map.h
    format/map/Map.cpp
    format/map/MapScript.h
    format/map/MapObject.h

    # MSG
    format/msg/Msg.h
    format/msg/Msg.cpp

    # LST
    format/lst/Lst.h
    format/lst/Lst.cpp

    # PRO
    format/pro/Pro.h
    format/pro/Pro.cpp

    #
    # FILE WRITERS
    #

    # Writer superclass
    writer/FileWriter.h

    writer/map/MapWriter.h
    writer/map/MapWriter.cpp

    #
    # FILE LOADERS
    #

    # Parser superclass
    reader/FileParser.h

    reader/dat/DatReader.h
    reader/dat/DatReader.cpp

#    writer/dat/DatWriter.h
#    writer/dat/DatWriter.cpp

    reader/frm/FrmReader.h
    reader/frm/FrmReader.cpp

    reader/pal/PalReader.h
    reader/pal/PalReader.cpp

    reader/map/MapReader.h
    reader/map/MapReader.cpp

    reader/msg/MsgReader.h
    reader/msg/MsgReader.cpp

    reader/lst/LstReader.h
    reader/lst/LstReader.cpp

    reader/pro/ProReader.h
    reader/pro/ProReader.cpp
)

target_link_libraries(geck-mapper PRIVATE
    sfml-system sfml-graphics sfml-window
    imgui::imgui
    ImGui-SFML::ImGui-SFML
    portable_file_dialogs
    cxxopts
    spdlog::spdlog_header_only
)

target_include_directories(geck-mapper PRIVATE ${imgui_SOURCE_DIR})

# copy shared external libraries to the build directory for Windows builds
add_custom_command(
    TARGET geck-mapper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:geck-mapper> $<TARGET_FILE_DIR:geck-mapper>
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    TARGET geck-mapper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:geck-mapper>/resources
)