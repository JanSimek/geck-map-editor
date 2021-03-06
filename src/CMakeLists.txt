add_executable (geck-mapper 
    main.cpp

    Application.h
    Application.cpp

    util/io.h
    util/TextureManager.h
    util/TextureManager.cpp
    util/FileHelper.h
    util/FileHelper.cpp

    # States
    state/StateMachine.h
    state/StateMachine.cpp
    state/State.h
    state/EditorState.h
    state/EditorState.cpp

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
    ImGui-SFML::ImGui-SFML
    cxxopts
    spdlog
)

target_compile_features(geck-mapper PRIVATE cxx_std_17)
set_target_properties(geck-mapper PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
