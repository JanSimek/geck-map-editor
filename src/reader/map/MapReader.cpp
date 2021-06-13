#include "MapReader.h"

#include <spdlog/spdlog.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>    // std::fill

#include <arpa/inet.h> // htonl

#include "../../editor/MapObject.h"
#include "../../editor/Tile.h"

#include "../../format/lst/Lst.h"
#include "../../format/map/Map.h"
#include "../../format/pro/Pro.h"
#include "../../reader/lst/LstReader.h"
#include "../../reader/pro/ProReader.h"
#include "../../util/FileHelper.h"
#include "../../util/io.h"

#include "../../format/msg/Msg.h"

namespace geck {

// TODO: namespace Map...

// Header
struct MapHeader {
    uint32_t version;
    std::string filename;

    uint32_t default_position; // player position
    uint32_t default_elevation; // map elevation where the map is loaded
    uint32_t default_orientation; // player elevation

    uint32_t num_local_vars;
    int32_t script_id;
    uint32_t flags;
    uint32_t darkness;
    uint32_t num_global_vars;
    uint32_t map_id;
    uint32_t timestamp;
};

//// Global and local variables
//std::vector<uint32_t> map_local_vars;
//std::vector<uint32_t> map_global_vars;

//// Tiles
//std::map<int, std::vector<Tile> > tiles;

// Scripts
struct MapScript {
    uint32_t pid;
    uint32_t next_script; // unused
    uint32_t timer; // only if PID = 1 or 2
    uint32_t spatial_radius; // only if PID = 1
    uint32_t flags;
    uint32_t script_id;
    uint32_t unknown5;
    uint32_t script_oid;
    uint32_t local_var_offset;
    uint32_t local_var_count;
    uint32_t unknown9;
    uint32_t unknown10;
    uint32_t unknown11;
    uint32_t unknown12;
    uint32_t unknown13;
    uint32_t unknown14;
    uint32_t unknown15;
    uint32_t unknown16;
};

MapScript empty_script = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//std::vector<MapScript> map_scripts[MapReader::SCRIPT_SECTIONS];
//std::array<int, MapReader::SCRIPT_SECTIONS> scripts_in_section;

// TODO: default values?
// scenery, walls, items, containers, keys and critters
struct Object {
    uint32_t unknown0; // falltergeist OID ?
    int32_t position;
    uint32_t x;
    uint32_t y;
    int32_t sx;
    int32_t sy;
    uint32_t frame_number;
    uint32_t orientation;
    uint32_t frm_pid; // FID
    uint32_t flags;
    uint32_t elevation;
    uint32_t pro_pid; // PID
    int32_t critter_index; // combat id
    uint32_t light_radius;
    uint32_t light_intensity;
    uint32_t outline_color;
    int32_t map_scripts_pid; // SID
    int32_t script_id;

    // Inventory
    uint32_t objects_in_inventory;
    uint32_t max_inventory_size;
    std::vector<Object> inventory;
    uint32_t amount;

    uint32_t unknown10; // unknown12 ?
    uint32_t unknown11; // unknown13 ?

    // Extra fields for critters
    uint32_t player_reaction;  // reaction to player - saves only
    uint32_t current_mp;  // current mp - saves only
    uint32_t combat_results;  // combat results - saves only
    uint32_t dmg_last_turn;  // damage last turn - saves only
    uint32_t ai_packet;  // AI packet - is it different from .pro? well, it can be
    uint32_t group_id;  // team - always 1? saves only?
    uint32_t who_hit_me;  // who hit me - saves only
    uint32_t current_hp;  // hit points - saves only, otherwise = value from .pro
    uint32_t current_rad;  // rad - always 0 - saves only
    uint32_t current_poison;

    // Extra fields for ammo
    // Extra fields for misc items
    uint32_t ammo; // charges for misc items

    // Extra fields for keys
    uint32_t keycode;

    // Extra fields for weapons
    // + ammo above
    uint32_t ammo_pid;

    // Extra fields for ladders
    // Extra fields for stairs
    uint32_t elevhex; // elevation + hex
    uint32_t map;

    // Extra fields for portals/doors
    uint32_t walkthrough;

    // Extra fields for elevators
    uint32_t elevtype;
    uint32_t elevlevel;

    // Extra fields for exit grids
    uint32_t exit_map;
    uint32_t exit_position;
    uint32_t exit_elevation;
    uint32_t exit_orientation;
};

//std::unordered_map<int, std::vector<Object>> map_objects;


struct F2Map {

    MapHeader header;

    // Global and local variables
    std::vector<uint32_t> map_local_vars;
    std::vector<uint32_t> map_global_vars;

    // Tiles
    std::map<int, std::vector<Tile> > tiles;

    // Scripts
    std::vector<MapScript> map_scripts[MapReader::SCRIPT_SECTIONS];
    std::array<int, MapReader::SCRIPT_SECTIONS> scripts_in_section;

    // Objects sorted by elevation
    std::unordered_map<int, std::vector<Object>> map_objects;
};

void write_be_32(std::ofstream& stream, uint32_t value) {
    uint32_t be_value = htonl(value); // convert to big endian
    stream.write(reinterpret_cast<const char *>(&be_value), sizeof(be_value));
//    stream.put(value >> 24);
//    stream.put(value >> 16);
//    stream.put(value >> 8);
//    stream.put(value);
}

void write_be_16(std::ofstream& stream, uint16_t value) {
    uint16_t be_value = htons(value); // convert to big endian
    stream.write(reinterpret_cast<const char *>(&be_value), sizeof(be_value));
}

void write_script(const MapScript& script, std::ofstream& stream) {
    write_be_32(stream, script.pid);
    write_be_32(stream, script.next_script);

    // TODO: enum 5 types of scripts ?
    switch ((script.pid & 0xFF000000) >> 24) {
        case 0:
            break;
        case 1: // spatial script
            write_be_32(stream, script.timer);
            write_be_32(stream, script.spatial_radius);
            break;
        case 2: // timer script
            write_be_32(stream, script.timer);
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        default:
//            spdlog::debug("Unknown script PID = " + std::to_string((pid & 0xFF000000) >> 24));
            break;
    }

    write_be_32(stream, script.flags);
    write_be_32(stream, script.script_id);
    write_be_32(stream, script.unknown5);
    write_be_32(stream, script.script_oid);
    write_be_32(stream, script.local_var_offset);
    write_be_32(stream, script.local_var_count);
    write_be_32(stream, script.unknown9);
    write_be_32(stream, script.unknown10);
    write_be_32(stream, script.unknown11);
    write_be_32(stream, script.unknown12);
    write_be_32(stream, script.unknown13);
    write_be_32(stream, script.unknown14);
    write_be_32(stream, script.unknown15);
    write_be_32(stream, script.unknown16);
}

void write_object(const Object& obj, std::ofstream& out) {

    write_be_32(out, obj.unknown0);
    write_be_32(out, obj.position);
    write_be_32(out, obj.x);
    write_be_32(out, obj.y);
    write_be_32(out, obj.sx);
    write_be_32(out, obj.sy);
    write_be_32(out, obj.frame_number);
    write_be_32(out, obj.orientation);
    write_be_32(out, obj.frm_pid);
    write_be_32(out, obj.flags);
    write_be_32(out, obj.elevation);
    write_be_32(out, obj.pro_pid);
    write_be_32(out, obj.critter_index);
    write_be_32(out, obj.light_radius);
    write_be_32(out, obj.light_intensity);
    write_be_32(out, obj.outline_color);
    write_be_32(out, obj.map_scripts_pid);
    write_be_32(out, obj.script_id);
    write_be_32(out, obj.objects_in_inventory);
    write_be_32(out, obj.max_inventory_size);
    write_be_32(out, obj.unknown10);
    write_be_32(out, obj.unknown11);

    uint32_t objectTypeId = obj.pro_pid >> 24;
    uint32_t objectId = 0x00FFFFFF & obj.pro_pid;

    MapReader map_reader;

    switch ((MapObject::OBJECT_TYPE)objectTypeId) {
        case MapObject::OBJECT_TYPE::ITEM:
            {
                uint32_t subtype_id = map_reader.loadPro(obj.pro_pid)->objectSubtypeId();
                switch ((MapObject::ITEM_TYPE)subtype_id) {
                    case MapObject::ITEM_TYPE::AMMO: // ammo
                    case MapObject::ITEM_TYPE::MISC: // charges - have strangely high values, or negative.
                        write_be_32(out, obj.ammo); // bullets
                        break;
                    case MapObject::ITEM_TYPE::KEY:
                        write_be_32(out, obj.keycode); // keycode = -1 in all maps. saves only? ignore for now
                        break;
                    case MapObject::ITEM_TYPE::WEAPON:
                        write_be_32(out, obj.ammo);  // ammo
                        write_be_32(out, obj.ammo_pid); // ammo pid
                        break;
                    case MapObject::ITEM_TYPE::ARMOR:
                    case MapObject::ITEM_TYPE::CONTAINER:
                    case MapObject::ITEM_TYPE::DRUG:
                        break;
                    default:
                        throw std::runtime_error{"Unknown item type " + std::to_string(objectTypeId)};
                }
            }
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            write_be_32(out, obj.player_reaction);  // reaction to player - saves only
            write_be_32(out, obj.current_mp);  // current mp - saves only
            write_be_32(out, obj.combat_results);  // combat results - saves only
            write_be_32(out, obj.dmg_last_turn);  // damage last turn - saves only
            write_be_32(out, obj.ai_packet);  // AI packet - is it different from .pro? well, it can be
            write_be_32(out, obj.group_id);  // team - always 1? saves only?
            write_be_32(out, obj.who_hit_me);  // who hit me - saves only
            write_be_32(out, obj.current_hp);  // hit points - saves only, otherwise = value from .pro
            write_be_32(out, obj.current_rad);  // rad - always 0 - saves only
            write_be_32(out, obj.current_poison);  // poison - always 0 - saves only

            break;

        case MapObject::OBJECT_TYPE::SCENERY:
            {
                uint32_t subtype_id = map_reader.loadPro(obj.pro_pid)->objectSubtypeId();
                switch ((MapObject::SCENERY_TYPE)subtype_id) {
                    case MapObject::SCENERY_TYPE::LADDER_TOP:
                    case MapObject::SCENERY_TYPE::LADDER_BOTTOM:
                        write_be_32(out, obj.map);
                        write_be_32(out, obj.elevhex);
                        // hex = elevhex & 0xFFFF;
                        // elev = ((elevhex >> 28) & 0xf) >> 1;
                        break;
                    case MapObject::SCENERY_TYPE::STAIRS:
                        // looks like for ladders and stairs map and elev+hex fields in the different order
                        write_be_32(out, obj.elevhex);
                        write_be_32(out, obj.map);
                        // hex = elevhex & 0xFFFF;
                        // elev = ((elevhex >> 28) & 0xf) >> 1;
                        break;
                    case MapObject::SCENERY_TYPE::ELEVATOR:
                        write_be_32(out, obj.elevtype);  // elevator type - sometimes -1
                        write_be_32(out, obj.elevlevel);  // current level - sometimes -1
                        break;
                    case MapObject::SCENERY_TYPE::DOOR:
                        write_be_32(out, obj.walkthrough);  // != 0 -> is opened;
                        break;
                    case MapObject::SCENERY_TYPE::GENERIC:
                        break;
                    default:
                        throw std::runtime_error{"Unknown scenery type: " + std::to_string(subtype_id)};
                }
            }
            break;
        case MapObject::OBJECT_TYPE::WALL:
        case MapObject::OBJECT_TYPE::TILE:
            break;
        case MapObject::OBJECT_TYPE::MISC:

            switch (objectId) {
                case 12:
                    break;
                // Exit Grids
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                default:
                    write_be_32(out, obj.exit_map);
                    write_be_32(out, obj.exit_position);
                    write_be_32(out, obj.exit_elevation);
                    write_be_32(out, obj.exit_orientation);
                    break;
            }
            break;
        default:
            throw std::runtime_error{"Unknown object type: " + std::to_string(objectTypeId)};
    }

    if (obj.objects_in_inventory > 0) {

        for (const auto& invobj : obj.inventory) {
            write_be_32(out, invobj.amount);
            write_object(invobj, out);
        }
    }
}

void saveMap(F2Map map) {
    std::ofstream out("test.map", std::ofstream::binary);

    write_be_32(out, map.header.version);

    out.write(map.header.filename.c_str(), map.header.filename.size());

    write_be_32(out, map.header.default_position);
    write_be_32(out, map.header.default_elevation);
    write_be_32(out, map.header.default_orientation);


    write_be_32(out, map.header.num_local_vars);
    write_be_32(out, map.header.script_id);

    write_be_32(out, map.header.flags);
    write_be_32(out, map.header.darkness);
    write_be_32(out, map.header.num_global_vars);
    write_be_32(out, map.header.map_id);
    write_be_32(out, map.header.timestamp);

    for (int i = 0; i < 44; i++) {
        write_be_32(out, 0); // unused bytes
    }

    for (const auto& var : map.map_local_vars) {
        write_be_32(out, var);
    }

    for (const auto& var : map.map_global_vars) {
        write_be_32(out, var);
    }

    for (const auto& elevation : map.tiles) {
        for (const auto& tile : elevation.second) {
            write_be_16(out, tile.getRoof());
            write_be_16(out, tile.getFloor());
        }
    }

//    for (auto& script_section : map_scripts) {
//        if (script_section.size() % 16 != 0) {
//            std::fill(script_section.begin(), script_section.end() + 16 - script_section.size() % 16, empty_script);
//        }
//    }

    for (const auto& script_section : map.map_scripts) {

        uint32_t number_of_scripts = script_section.size();
        write_be_32(out, number_of_scripts);

        if (number_of_scripts == 0)
            continue;

//        std::fill(number_of_scripts /*+ 1*/, number_of_scripts + 16 - number_of_scripts % 16, empty_script);
//        for (const auto& script : script_section) {

        int current_sequence = 0;
        uint32_t check = 0;
        int scripts_in_section = (script_section.size() % 16 == 0 ? script_section.size() : script_section.size() + 16 - number_of_scripts % 16);
        for (int i = 0; i < scripts_in_section; i++) {

            if (i < script_section.size()) {
                write_script(script_section.at(i), out);
                current_sequence++;
                check++;
            } else {
                write_script(empty_script, out);
            }

            if (i % 16 == 15) { // check after every batch
                write_be_32(out, current_sequence);
                write_be_32(out, 0); // unknown
                current_sequence = 0;
            }
        }

        if (check != number_of_scripts) {
            spdlog::error("Check {} doesn't match number of scripts {} in section", check, number_of_scripts);
        }

//        // fill the remainder up to a multiple of 16 with empty scripts
//        int multiple_of_16 = (number_of_scripts % 16 == 0 ? 0 : 16 - number_of_scripts % 16);
//        for (int i = 0; i < multiple_of_16; i++) {
//            write_script(empty_script, out);
//        }
    }

    // Objects

    // write total number of objects
    write_be_32(out, 1);

    // for elevation ...
    for (size_t elev = 0; elev < map.map_objects.size(); elev++) {
        auto objectsOnElevation = map.map_objects[elev].size();
        write_be_32(out, objectsOnElevation);

        for (const auto& obj : map.map_objects[elev]) {
            write_object(obj, out);
        }
    }
}

MapReader::ScriptType MapReader::fromPid(uint32_t val) {
    unsigned int pid = (val & 0xff000000) >> 24;

    switch (pid) {
        case 0:
            return ScriptType::system;
        case 1:
            return ScriptType::spatial;
        case 2:
            return ScriptType::timer;
        case 3:
            return ScriptType::item;
        case 4:
        default:
            return ScriptType::unknown;
    }
}

std::unique_ptr<Pro> MapReader::loadPro(unsigned int PID) {
    using namespace io;

    unsigned int typeId = PID >> 24;

    const std::string data_path = FileHelper::getInstance().path();
    std::string listFile = data_path;

    switch ((MapObject::OBJECT_TYPE)typeId) {
        case MapObject::OBJECT_TYPE::ITEM:
            listFile += "proto/items/items.lst";
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            listFile += "proto/critters/critters.lst";
            break;
        case MapObject::OBJECT_TYPE::SCENERY:
            listFile += "proto/scenery/scenery.lst";
            break;
        case MapObject::OBJECT_TYPE::WALL:
            listFile += "proto/walls/walls.lst";
            break;
        case MapObject::OBJECT_TYPE::TILE:
            listFile += "proto/tiles/tiles.lst";
            break;
        case MapObject::OBJECT_TYPE::MISC:
            listFile += "proto/misc/misc.lst";
            break;
        default:
            throw std::runtime_error{"Wrong PID: " + std::to_string(PID)};
    }

    LstReader lst_reader;
    auto lst = lst_reader.read(listFile);

    unsigned int index = 0x00000FFF & PID;

    if (index > lst->list().size()) {
        throw std::runtime_error{"LST size < PID: " + std::to_string(PID)};
    }

    std::string protoName = lst->list().at(index - 1); // - 1);  // FIXME ? is it -1?

    std::string proFilename;
    switch ((MapObject::OBJECT_TYPE)typeId) {
        case MapObject::OBJECT_TYPE::ITEM:
            proFilename += "proto/items/" + protoName;
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            proFilename += "proto/critters/" + protoName;
            break;
        case MapObject::OBJECT_TYPE::SCENERY:
            proFilename += "proto/scenery/" + protoName;
            break;
        case MapObject::OBJECT_TYPE::WALL:
            proFilename += "proto/walls/" + protoName;
            break;
        case MapObject::OBJECT_TYPE::TILE:
            proFilename += "proto/tiles/" + protoName;
            break;
        case MapObject::OBJECT_TYPE::MISC:
            proFilename += "proto/misc/" + protoName;
            break;
    }
    if (!proFilename.empty()) {
        ProReader pro_reader;
        return pro_reader.read(data_path + proFilename);
    } else {
        throw std::runtime_error{"Couldn't load PRO file " + protoName};
    }
}

enum class FRM_TYPE : char { ITEM = 0, CRITTER, SCENERY, WALL, TILE, MISC, INTERFACE, INVENTORY };

std::string FIDtoFrmName(unsigned int FID) {
    /*const*/ auto baseId = FID & 0x00FFFFFF;  // FIXME? 0x00000FFF;
    /*const*/ auto type = static_cast<FRM_TYPE>(FID >> 24);

    if (type == FRM_TYPE::CRITTER) {
        baseId = FID & 0x00000FFF;
        type = static_cast<FRM_TYPE>((FID & 0x0F000000) >> 24);  // FIXME: WTF?
    }

    // TODO: EditorState::_showScrlBlk
    if (type == FRM_TYPE::MISC && baseId == 1) {
        static const std::string SCROLL_BLOCKERS_PATH("art/misc/scrblk.frm");
        // Map scroll blockers
        return SCROLL_BLOCKERS_PATH;
    }

    static struct TypeArtListDecription {
        const std::string prefixPath;
        const std::string lstFilePath;
    } const frmTypeDescription[] = {
        {"art/items/", "art/items/items.lst"},          {"art/critters/", "art/critters/critters.lst"},
        {"art/scenery/", "art/scenery/scenery.lst"},    {"art/walls/", "art/walls/walls.lst"},
        {"art/tiles/", "art/tiles/tiles.lst"},          {"art/misc/", "art/misc/misc.lst"},
        {"art/intrface/", "art/intrface/intrface.lst"}, {"art/inven/", "art/inven/inven.lst"},
    };

    if (type > FRM_TYPE::INVENTORY) {
        throw std::runtime_error{"Invalid FRM_TYPE"};
    }

    const auto& typeArtDescription = frmTypeDescription[static_cast<size_t>(type)];

    const std::string data_path = FileHelper::getInstance().path();

    LstReader lst_reader;
    auto lst = lst_reader.read(data_path + typeArtDescription.lstFilePath);

    if (baseId >= lst->list().size()) {
        throw std::runtime_error{"LST " + typeArtDescription.lstFilePath + " size " + std::to_string(lst->list().size()) + " <= frmID: " +
                                 std::to_string(baseId) + ", frmType: " + std::to_string((unsigned)type)};
    }

    std::string frmName = lst->list().at(baseId);

    if (type == FRM_TYPE::CRITTER) {
        // TODO: correct direction and posture
        return typeArtDescription.prefixPath + frmName.substr(0, 6) + "aa.frm";
    }
    return typeArtDescription.prefixPath + frmName;
}

std::unique_ptr<MapObject> MapReader::readObject(std::istream& stream) {
    using namespace io;

    auto object = std::make_unique<MapObject>();
    read_be_u32(stream);                          // object->setOID(stream.uint32());
    object->setHexPosition(read_be_i32(stream));  // object->setHexPosition(stream.int32());
    read_be_u32(stream);                          // object->setX(stream.uint32());
    read_be_u32(stream);                          // object->setY(stream.uint32());
    read_be_u32(stream);                          // object->setSx(stream.uint32());
    read_be_u32(stream);                          // object->setSy(stream.uint32());
    read_be_u32(stream);                          // object->setFrameNumber(stream.uint32());

    uint32_t orientation = read_be_u32(stream);                          // object->setOrientation(stream.uint32());
    object->setOrientation(orientation);

    uint32_t FID = read_be_u32(stream);           //

    object->setFID(FID);

    //    if (static_cast<FRM_TYPE>(FID >> 24) != FRM_TYPE::CRITTER) {
    //        std::cout << "Object FRM: " << FIDtoFrmName(FID) << std::endl;
    object->setFrm(FIDtoFrmName(FID));
    //    }

    // TODO:
    //    object->setFrmTypeId(FID >> 24);
    //    object->setFrmId(0x00FFFFFF & FID);
    read_be_u32(stream);  // object->setFlags(stream.uint32());
    read_be_u32(stream);  // elevation

    uint32_t PID = read_be_u32(stream);  // stream.uint32();
    uint32_t objectTypeId = PID >> 24;
    //    object->setObjectTypeId(PID >> 24);

    uint32_t objectId = 0x00FFFFFF & PID;
    //    object->setObjectId(0x00FFFFFF & PID);
    read_be_u32(stream);  // object->setCombatId(stream.uint32());
    read_be_u32(stream);  // object->setLightRadius(stream.uint32());
    read_be_u32(stream);  // object->setLightIntensity(stream.uint32());
    read_be_u32(stream);  // object->setOutline(stream.uint32());

    int32_t SID = read_be_i32(stream);
    if (SID != -1) {
        //        for (auto& script : _scripts)
        //        {
        //            // TODO: comparing PID to SID? If this is not bug, need better name for PID
        //            if (script.PID() == SID)
        //            {
        //                object->setMapScriptId(script.scriptId());
        //            }
        //        }
    }

    SID = read_be_i32(stream);
    if (SID != -1) {
        //        object->setScriptId(SID);
    }

    object->setInventory_size(read_be_u32(stream));  // object->setInventorySize(stream.uint32());
    read_be_u32(stream);                             // object->setMaxInventorySize(stream.uint32());
    read_be_u32(stream);                             // object->setUnknown12(stream.uint32());
    read_be_u32(stream);                             // object->setUnknown13(stream.uint32());

    switch ((MapObject::OBJECT_TYPE)objectTypeId) {
        case MapObject::OBJECT_TYPE::ITEM:
            object->setObjectSubtypeId(loadPro(PID)->objectSubtypeId());

            switch ((MapObject::ITEM_TYPE)object->objectSubtypeId()) {
                case MapObject::ITEM_TYPE::AMMO:
                    read_be_u32(stream);  // object->setAmmo(stream.uint32()); // bullets
                    break;
                case MapObject::ITEM_TYPE::KEY:
                    read_be_i32(stream);  // stream.uint32(); // keycode = -1 in all maps. saves only? ignore for now
                    break;
                case MapObject::ITEM_TYPE::MISC:
                    read_be_i32(stream);  // object->setAmmo(stream.uint32()); //charges - have strangely high values,
                                          // or negative.
                    break;
                case MapObject::ITEM_TYPE::WEAPON:
                    read_be_u32(stream);  // object->setAmmo(stream.uint32()); // ammo
                    read_be_u32(stream);  // object->setAmmoPID(stream.uint32()); // ammo pid
                    break;
                case MapObject::ITEM_TYPE::ARMOR:
                case MapObject::ITEM_TYPE::CONTAINER:
                case MapObject::ITEM_TYPE::DRUG:
                    break;
                default:
                    throw std::runtime_error{"Unknown item type " + std::to_string(objectTypeId)};
            }
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            {
                read_be_u32(stream);  // stream.uint32(); //reaction to player - saves only
                read_be_u32(stream);  // stream.uint32(); //current mp - saves only
                read_be_u32(stream);  // stream.uint32(); //combat results - saves only
                read_be_u32(stream);  // stream.uint32(); //damage last turn - saves only
                read_be_u32(stream);  // object->setAIPacket(stream.uint32()); // AI packet - is it different from .pro?
                                      // well, it can be
                read_be_u32(stream);  // stream.uint32(); // team - always 1? saves only?
                read_be_u32(stream);  // stream.uint32(); // who hit me - saves only
                read_be_u32(stream);  // stream.uint32(); // hit points - saves only, otherwise = value from .pro
                read_be_u32(stream);  // stream.uint32(); // rad - always 0 - saves only
                read_be_u32(stream);  // stream.uint32(); // poison - always 0 - saves only

                //            object->setFrmId(FID & 0x00000FFF);
                //            object->setObjectID1((FID & 0x0000F000) >> 12);
                //            object->setObjectID2((FID & 0x00FF0000) >> 16);
                //            object->setFrmTypeId((FID & 0x0F000000) >> 24);
                //            object->setObjectID3((FID & 0xF0000000) >> 28);

            }
            break;

        case MapObject::OBJECT_TYPE::SCENERY:

            // TODO
            //            object->setObjectSubtypeId(callback(PID)->subtypeId());
            //            std::unique_ptr<Pro> proFile2 = loadPro(PID);

            uint32_t elevhex;  // elev+hex
            uint32_t hex;
            uint32_t elev;
            int32_t map;
            //            std::cout << "SCENERY SUBTYPE = " << loadPro(PID)->objectSubtypeId() << std::endl;

            object->setObjectSubtypeId(loadPro(PID)->objectSubtypeId());
            switch ((MapObject::SCENERY_TYPE)object->objectSubtypeId()) {
                case MapObject::SCENERY_TYPE::LADDER_TOP:
                case MapObject::SCENERY_TYPE::LADDER_BOTTOM:
                    read_be_u32(stream);  // map = stream.int32();
                    read_be_u32(stream);  // elevhex = stream.uint32();  // elev+hex
                    //                    hex = elevhex & 0xFFFF;
                    //                    elev = ((elevhex >> 28) & 0xf) >> 1;
                    //                    object->setExitMap(map); // map id
                    //                    object->setExitPosition(hex);
                    //                    object->setExitElevation(elev);
                    break;
                case MapObject::SCENERY_TYPE::STAIRS:
                    // looks like for ladders and stairs map and elev+hex fields in the different order
                    read_be_u32(stream);  // elevhex = stream.uint32();  // elev+hex
                    read_be_u32(stream);  // map = stream.int32();
                    //                    hex = elevhex & 0xFFFF;
                    //                    elev = ((elevhex >> 28) & 0xf) >> 1;
                    //                    object->setExitMap(map); // map id
                    //                    object->setExitPosition(hex);
                    //                    object->setExitElevation(elev);
                    break;
                case MapObject::SCENERY_TYPE::ELEVATOR:
                    read_be_u32(stream);  // object->setElevatorType(stream.uint32()); // elevator type - sometimes -1
                    read_be_u32(stream);  // object->setElevatorLevel(stream.uint32()); // current level - sometimes -1
                    break;
                case MapObject::SCENERY_TYPE::DOOR:
                    read_be_u32(stream);  // object->setOpened(stream.uint32() != 0);// is opened;
                    break;
                case MapObject::SCENERY_TYPE::GENERIC:
                    break;
                default:
                    throw std::runtime_error{"Unknown scenery type: " + std::to_string(object->objectSubtypeId())};
            }
            break;
        case MapObject::OBJECT_TYPE::WALL:
        case MapObject::OBJECT_TYPE::TILE:
            break;
        case MapObject::OBJECT_TYPE::MISC:

            switch (objectId) {
                case 12:
                    break;
                // Exit Grids
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                    read_be_i32(stream);  // object->setExitMap(stream.int32());
                    read_be_i32(stream);  // object->setExitPosition(stream.int32());
                    read_be_i32(stream);  // object->setExitElevation(stream.int32());
                    read_be_i32(stream);  // object->setExitOrientation(stream.int32());
                    break;
                default:
                    read_be_u32(stream);  // stream.uint32();
                    read_be_u32(stream);  // stream.uint32();
                    read_be_u32(stream);  // stream.uint32();
                    read_be_u32(stream);  // stream.uint32();
                    break;
            }
            break;
        default:
            throw std::runtime_error{"Unknown object type: " + std::to_string(objectTypeId)};
    }
    return object;
}

std::unique_ptr<MapObject> objectToMapObject(const Object& object) {

    auto mapobject = std::make_unique<MapObject>();
    mapobject->setHexPosition(object.position);
    mapobject->setOrientation(object.orientation);
    mapobject->setFID(object.frm_pid); // FID ?????

    mapobject->setFrm(FIDtoFrmName(mapobject->FID()));

    mapobject->setAmount(object.amount);

    uint32_t PID = object.pro_pid; // PID?????
    mapobject->setPID(PID);

    uint32_t objectTypeId = PID >> 24;
    uint32_t objectId = 0x00FFFFFF & PID;

    int32_t SID = object.script_id; // SID ????
    if (SID != -1) {
    }

    if (SID != -1) {
    }

    mapobject->setInventory_size(object.max_inventory_size);  // object->setInventorySize(stream.uint32());

    MapReader map_reader;

    switch ((MapObject::OBJECT_TYPE)objectTypeId) {
        case MapObject::OBJECT_TYPE::ITEM:
            {
                mapobject->setObjectSubtypeId(map_reader.loadPro(PID)->objectSubtypeId());
            }
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            break;

        case MapObject::OBJECT_TYPE::SCENERY:
            //            object->setObjectSubtypeId(callback(PID)->subtypeId());
            //            std::unique_ptr<Pro> proFile2 = loadPro(PID);

            uint32_t elevhex;  // elev+hex
            uint32_t hex;
            uint32_t elev;
            int32_t map;
            //            std::cout << "SCENERY SUBTYPE = " << loadPro(PID)->objectSubtypeId() << std::endl;

            mapobject->setObjectSubtypeId(map_reader.loadPro(PID)->objectSubtypeId());
            switch ((MapObject::SCENERY_TYPE)mapobject->objectSubtypeId()) {
                case MapObject::SCENERY_TYPE::LADDER_TOP:
                case MapObject::SCENERY_TYPE::LADDER_BOTTOM:
                    break;
                case MapObject::SCENERY_TYPE::STAIRS:
                    break;
                case MapObject::SCENERY_TYPE::ELEVATOR:
                    break;
                case MapObject::SCENERY_TYPE::DOOR:
                    break;
                case MapObject::SCENERY_TYPE::GENERIC:
                    break;
                default:
                    throw std::runtime_error{ "Unknown scenery type: " + std::to_string(mapobject->objectSubtypeId()) };
            }
            break;
        case MapObject::OBJECT_TYPE::WALL:
        case MapObject::OBJECT_TYPE::TILE:
            break;
        case MapObject::OBJECT_TYPE::MISC:

            switch (objectId) {
                case 12:
                    break;
                // Exit Grids
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                    break;
                default:
                    break;
            }
            break;
        default:
            throw std::runtime_error{"Unknown object type: " + std::to_string(objectTypeId)};
    }
    return mapobject;
}

Object readMapObject(std::istream& stream) {
    using namespace io;

    Object object;

    object.unknown0 = read_be_u32(stream);
    object.position = read_be_i32(stream);
    object.x = read_be_u32(stream);
    object.y = read_be_u32(stream);
    object.sx = read_be_u32(stream);
    object.sy = read_be_u32(stream);
    object.frame_number = read_be_u32(stream);
    object.orientation = read_be_u32(stream);
    object.frm_pid = read_be_u32(stream);
    object.flags = read_be_u32(stream);
    object.elevation = read_be_u32(stream);
    object.pro_pid = read_be_u32(stream);
    object.critter_index = read_be_u32(stream);
    object.light_radius = read_be_u32(stream);
    object.light_intensity = read_be_u32(stream);
    object.outline_color = read_be_u32(stream);
    object.map_scripts_pid = read_be_i32(stream);
    object.script_id = read_be_i32(stream);
    object.objects_in_inventory = read_be_u32(stream);
    object.max_inventory_size = read_be_u32(stream);
    object.unknown10 = read_be_u32(stream);
    object.unknown11 = read_be_u32(stream);

    uint32_t objectTypeId = object.pro_pid >> 24;
    uint32_t objectId = 0x00FFFFFF & object.pro_pid;

    MapReader map_reader;

    switch ((MapObject::OBJECT_TYPE)objectTypeId) {
        case MapObject::OBJECT_TYPE::ITEM:
            {
                uint32_t subtype_id = map_reader.loadPro(object.pro_pid)->objectSubtypeId();
                switch ((MapObject::ITEM_TYPE)subtype_id) {
                    case MapObject::ITEM_TYPE::AMMO: // ammo
                    case MapObject::ITEM_TYPE::MISC: // charges - have strangely high values, or negative.
                        object.ammo = read_be_u32(stream); // bullets
                        break;
                    case MapObject::ITEM_TYPE::KEY:
                        object.keycode = read_be_u32(stream); // keycode = -1 in all maps. saves only? ignore for now
                        break;
                    case MapObject::ITEM_TYPE::WEAPON:
                        object.ammo = read_be_u32(stream);  // ammo
                        object.ammo_pid = read_be_u32(stream);  // ammo pid
                        break;
                    case MapObject::ITEM_TYPE::ARMOR:
                    case MapObject::ITEM_TYPE::CONTAINER:
                    case MapObject::ITEM_TYPE::DRUG:
                        break;
                    default:
                        throw std::runtime_error{"Unknown item type " + std::to_string(objectTypeId)};
                }
            }
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            {
                object.player_reaction = read_be_u32(stream);  // reaction to player - saves only
                object.current_mp = read_be_u32(stream);  // stream.uint32(); //current mp - saves only
                object.combat_results = read_be_u32(stream);  // stream.uint32(); //combat results - saves only
                object.dmg_last_turn = read_be_u32(stream);  // stream.uint32(); //damage last turn - saves only
                object.ai_packet = read_be_u32(stream);  // AI packet - is it different from .pro? well, it can be
                object.group_id = read_be_u32(stream);  // team - always 1? saves only?
                object.who_hit_me = read_be_u32(stream);  // who hit me - saves only
                object.current_hp = read_be_u32(stream);  // hit points - saves only, otherwise = value from .pro
                object.current_rad = read_be_u32(stream);  // rad - always 0 - saves only
                object.current_poison = read_be_u32(stream);  // poison - always 0 - saves only

                Msg message("resources/text/english/game/pro_crit.msg");
                int msgId = map_reader.loadPro(object.pro_pid)->messageId();

                spdlog::warn("Message: {}", message.message(msgId).text);
                spdlog::warn("Description: {}", message.message(msgId+1).text);
            }
            break;

        case MapObject::OBJECT_TYPE::SCENERY:
            {
                uint32_t subtype_id = map_reader.loadPro(object.pro_pid)->objectSubtypeId();
                switch ((MapObject::SCENERY_TYPE)subtype_id) {
                    case MapObject::SCENERY_TYPE::LADDER_TOP:
                    case MapObject::SCENERY_TYPE::LADDER_BOTTOM:
                        object.map = read_be_u32(stream);
                        object.elevhex = read_be_u32(stream);
                        // hex = elevhex & 0xFFFF;
                        // elev = ((elevhex >> 28) & 0xf) >> 1;
                        break;
                    case MapObject::SCENERY_TYPE::STAIRS:
                        // looks like for ladders and stairs map and elev+hex fields in the different order
                        object.elevhex = read_be_u32(stream);
                        object.map = read_be_u32(stream);
                        // hex = elevhex & 0xFFFF;
                        // elev = ((elevhex >> 28) & 0xf) >> 1;
                        break;
                    case MapObject::SCENERY_TYPE::ELEVATOR:
                        object.elevtype = read_be_u32(stream);  // elevator type - sometimes -1
                        object.elevlevel = read_be_u32(stream);  // current level - sometimes -1
                        break;
                    case MapObject::SCENERY_TYPE::DOOR:
                        object.walkthrough = read_be_u32(stream);  // != 0 -> is opened;
                        break;
                    case MapObject::SCENERY_TYPE::GENERIC:
                        break;
                    default:
                        throw std::runtime_error{"Unknown scenery type: " + std::to_string(subtype_id)};
                }
            }
            break;
        case MapObject::OBJECT_TYPE::WALL:
        case MapObject::OBJECT_TYPE::TILE:
            break;
        case MapObject::OBJECT_TYPE::MISC:

            switch (objectId) {
                case 12:
                    break;
                // Exit Grids
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                default:
                    object.exit_map = read_be_i32(stream);
                    object.exit_position = read_be_i32(stream);
                    object.exit_elevation = read_be_i32(stream);
                    object.exit_orientation = read_be_i32(stream);
                    break;
//                default:
//                    read_be_u32(stream);  // stream.uint32();
//                    read_be_u32(stream);  // stream.uint32();
//                    read_be_u32(stream);  // stream.uint32();
//                    read_be_u32(stream);  // stream.uint32();
//                    break;
            }
            break;
        default:
            throw std::runtime_error{"Unknown object type: " + std::to_string(objectTypeId)};
    }

    return object;
}

std::unique_ptr<Map> geck::MapReader::read(std::istream& stream) {
    using namespace io;

    F2Map f2map;

    // 19 or 20
    auto version = read_be_u32(stream);
    f2map.header.version = version;

    if (version == 19) {
        throw std::runtime_error{"Fallout 1 maps are not supported yet"};
    }

    if (version != 20) {
        throw std::runtime_error{"Unknown map version " + std::to_string(version)};
    }

    auto map = std::make_unique<Map>();

    std::string filename = read_str(stream, 16);
    f2map.header.filename = filename;

    spdlog::info("Loading map " + filename);

    f2map.header.default_position = read_be_u32(stream);  // player position
    f2map.header.default_elevation = read_be_u32(stream);  // player elevation
    f2map.header.default_orientation = read_be_u32(stream);  // player orientation

    uint32_t num_local_vars = read_be_u32(stream);  // num local vars
    f2map.header.num_local_vars = num_local_vars;
    f2map.header.script_id = read_be_i32(stream); // Script id

    uint32_t flags = read_be_u32(stream);  // map flags
    f2map.header.flags = flags;

    bool elevation_low = (flags & 0x2) == 0;
    bool elevation_medium = (flags & 0x4) == 0;
    bool elevation_high = (flags & 0x8) == 0;

    int elevations = 0;
    if (elevation_low)
        elevations++;
    if (elevation_medium)
        elevations++;
    if (elevation_high)
        elevations++;
    spdlog::info("Map has " + std::to_string(elevations) + " elevation(s)");

//    skip<4>(stream);  // map darkness (unused)
    f2map.header.darkness = read_be_u32(stream);

    uint32_t num_global_vars = read_be_u32(stream);  // num global vars
    f2map.header.num_global_vars = num_global_vars;

    f2map.header.map_id = read_be_u32(stream);                             // map id
    f2map.header.timestamp = read_be_u32(stream);                             // timestamp

    skip<4 * 44>(stream);  // skipping to the end of a MAP header

    for (uint32_t i = 0; i < num_local_vars; ++i) {
        f2map.map_local_vars.emplace_back(read_be_u32(stream));
    }
    for (uint32_t i = 0; i < num_global_vars; ++i) {
        f2map.map_global_vars.emplace_back(read_be_u32(stream));
    }

    for (auto elevation = 0; elevation < elevations; ++elevation) {
        for (auto i = 0U; i < Map::TILES_PER_ELEVATION; ++i) {
            uint16_t roof = read_be_u16(stream);
            uint16_t floor = read_be_u16(stream);

            Tile tile(floor, roof);
            f2map.tiles[elevation].push_back(tile);
        }
    }
    map->setTiles(std::move(f2map.tiles));

    /*
     * 1. count... 16
     *
     * 2. count
     * 3. count
     * 4. count
     * 5. count
     */

    // SCRIPTS SECTION
    // Each section contains 16 slots for scripts
    // FIXME: Five types of scripts - create an enum
    for (unsigned i = 0; i < SCRIPT_SECTIONS; i++) {
        uint32_t count = read_be_u32(stream); // total number of scripts in section
        f2map.scripts_in_section[i] = count;

        spdlog::info("Script group " + std::to_string(i) + " has " + std::to_string(count) + " scripts");

        if (count > 0) {
            uint32_t loop = count;
            // find the next multiple of 16 higher than the count
            if (count % 16 > 0) {
                loop += 16 - count % 16;
            }

            uint32_t check = 0;
            for (unsigned j = 0; j < loop; j++) {

                MapScript map_script;

                int32_t pid = read_be_i32(stream);

                map_script.pid = pid;
                map_script.next_script = read_be_u32(stream);  // next script. unused

                switch ((pid & 0xFF000000) >> 24) {
                    case 0:
                        break;
                    case 1: // spatial script
                        map_script.timer = read_be_u32(stream);
                        map_script.spatial_radius = read_be_u32(stream);
                        break;
                    case 2: // timer script
                        map_script.timer = read_be_u32(stream);
                        break;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                        break;
                    default:
                        spdlog::debug("Unknown script PID = " + std::to_string((pid & 0xFF000000) >> 24));
                        break;
                }
                map_script.flags = read_be_u32(stream);  // flags
                map_script.script_id = read_be_i32(stream);  // scriptId (?)
                map_script.unknown5 = read_be_u32(stream);  // unknown 5
                map_script.script_oid = read_be_u32(stream);  // oid == object->OID
                map_script.local_var_offset = read_be_u32(stream);  // local var offset
                map_script.local_var_count = read_be_u32(stream);  // loal var cnt
                map_script.unknown9 = read_be_u32(stream);  // unknown 9
                map_script.unknown10 = read_be_u32(stream);  // unknown 10
                map_script.unknown11 = read_be_u32(stream);  // unknown 11
                map_script.unknown12 = read_be_u32(stream);  // unknown 12
                map_script.unknown13 = read_be_u32(stream);  // unknown 13
                map_script.unknown14 = read_be_u32(stream);  // unknown 14
                map_script.unknown15 = read_be_u32(stream);  // unknown 15
                map_script.unknown16 = read_be_u32(stream);  // unknown 16

                if (j < count) {
                    f2map.map_scripts[i].push_back(map_script);
                }

                if ((j % 16) == 15) {

                    // TODO: write after the batch
                    // number of scripts in this batch (sequence)
                    uint32_t cur_check = read_be_u32(stream);

                    spdlog::info("Current script check for sequence " + std::to_string(i) + " is " + std::to_string(cur_check) + ", j = " + std::to_string(j));

                    check += cur_check;
                    read_be_u32(stream); // uknown
                }
            }
            if (check != count) {
                throw std::runtime_error{"Error reading scripts: check is incorrect"};
            }
        }
    }

    // OBJECTS SECTION
    uint32_t total_objects = read_be_u32(stream);  // objects total

    for (auto elev = 0; elev < elevations; ++elev) {
        auto objectsOnElevation = read_be_u32(stream);
        for (size_t j = 0; j != objectsOnElevation; ++j) {
//            auto object = readObject(stream);
            Object object = readMapObject(stream);
//            if (object.inventory_size() > 0) {
            if (object.objects_in_inventory > 0) {

                object.inventory.reserve(object.objects_in_inventory);

                for (size_t i = 0; i < object.objects_in_inventory; ++i) {
                    uint32_t amount = read_be_u32(stream);
                    Object subobject = readMapObject(stream);
                    subobject.amount = amount;

                    object.inventory.push_back(subobject);
                }
            }
//            objects[elev].push_back(std::move(object));
            f2map.map_objects[elev].push_back(object);
        }

//        if (objectsOnElevation != objects[elev].size()) {
        if (objectsOnElevation != f2map.map_objects[elev].size()) {
            throw std::runtime_error{"Object count doesn't match: " + std::to_string(objectsOnElevation) + " vs " +
                                     std::to_string(f2map.map_objects[elev].size())};
        }
    }


    std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > > objects;

    for (size_t elev = 0; elev < f2map.map_objects.size(); elev++) {
        for (size_t i = 0; i < f2map.map_objects[elev].size(); i++) {

            auto mapobject = objectToMapObject(f2map.map_objects[elev][i]);

            objects[elev].push_back(std::move(mapobject));
        }
    }

    saveMap(f2map);

    map->setObjects(std::move(objects));

    return map;
}

}  // namespace geck
