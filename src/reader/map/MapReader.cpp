#include "MapReader.h"

#include <spdlog/spdlog.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>    // std::fill

#include "../../editor/Object.h"
#include "../../editor/Tile.h"

#include "../../format/map/Map.h"
#include "../../format/map/MapObject.h"
#include "../../format/pro/Pro.h"
#include "../../reader/lst/LstReader.h"
#include "../../reader/pro/ProReader.h"
#include "../../util/FileHelper.h"

#include "../../format/msg/Msg.h"

#include "../../writer/map/MapWriter.h"

namespace geck {

// TODO: namespace Map...

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
    unsigned int typeId = PID >> 24;

    const auto data_path = FileHelper::getInstance().fallout2DataPath();
    auto listFile = data_path;

    switch ((Object::OBJECT_TYPE)typeId) {
        using enum Object::OBJECT_TYPE;
        case ITEM:
            listFile /= "proto/items/items.lst";
            break;
        case CRITTER:
            listFile /= "proto/critters/critters.lst";
            break;
        case SCENERY:
            listFile /= "proto/scenery/scenery.lst";
            break;
        case WALL:
            listFile /= "proto/walls/walls.lst";
            break;
        case TILE:
            listFile /= "proto/tiles/tiles.lst";
            break;
        case MISC:
            listFile /= "proto/misc/misc.lst";
            break;
        default:
            throw std::runtime_error{"Wrong PID: " + std::to_string(PID)};
    }

    LstReader lst_reader;
    auto lst = lst_reader.openFile(listFile);

    unsigned int index = 0x00000FFF & PID;

    if (index > lst->list().size()) {
        throw std::runtime_error{"LST size < PID: " + std::to_string(PID)};
    }

    std::string protoName = lst->list().at(index - 1);

    std::filesystem::path proFilename;
    switch (static_cast<Object::OBJECT_TYPE>(typeId)) {
        using enum Object::OBJECT_TYPE;
        case ITEM:
            proFilename /= "proto/items";
            break;
        case CRITTER:
            proFilename /= "proto/critters";
            break;
        case SCENERY:
            proFilename /= "proto/scenery";
            break;
        case WALL:
            proFilename /= "proto/walls";
            break;
        case TILE:
            proFilename /= "proto/tiles";
            break;
        case MISC:
            proFilename /= "proto/misc";
            break;
    };

    proFilename /= protoName;

    if (!proFilename.empty()) {
        ProReader pro_reader;
        return pro_reader.openFile(data_path / proFilename);
    } else {
        throw std::runtime_error{"Couldn't load PRO file " + protoName};
    }
}

std::string MapReader::FIDtoFrmName(unsigned int FID) {
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

    if (type > FRM_TYPE::INVENTORY) {
        throw std::runtime_error{"Invalid FRM_TYPE"};
    }

    // TODO: art/$/
    static struct TypeArtListDecription {
        const std::string prefixPath;
        const std::string lstFilePath;
    } const frmTypeDescription[] = {
        {"art/items/", "art/items/items.lst"},          {"art/critters/", "art/critters/critters.lst"},
        {"art/scenery/", "art/scenery/scenery.lst"},    {"art/walls/", "art/walls/walls.lst"},
        {"art/tiles/", "art/tiles/tiles.lst"},          {"art/misc/", "art/misc/misc.lst"},
        {"art/intrface/", "art/intrface/intrface.lst"}, {"art/inven/", "art/inven/inven.lst"},
    };

    const auto& typeArtDescription = frmTypeDescription[static_cast<size_t>(type)];

    const auto lst = lst_frm.at(type).get();

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

MapReader::MapReader()
{
    spdlog::info("Initializing MapReader");

    std::unordered_map<FRM_TYPE, std::string> frm_lists ({
        { FRM_TYPE::ITEM,       "art/items/items.lst" },
        { FRM_TYPE::CRITTER,    "art/critters/critters.lst" },
        { FRM_TYPE::SCENERY,    "art/scenery/scenery.lst" },
        { FRM_TYPE::WALL,       "art/walls/walls.lst" },
        { FRM_TYPE::TILE,       "art/tiles/tiles.lst" },
        { FRM_TYPE::MISC,       "art/misc/misc.lst" },
        { FRM_TYPE::INTERFACE,  "art/intrface/intrface.lst" },
        { FRM_TYPE::INVENTORY,  "art/inven/inven.lst" },
    });

    const auto data_path = FileHelper::getInstance().fallout2DataPath();
    LstReader lst_reader;
    for (const auto& lst : frm_lists) {
        lst_frm.emplace(std::make_pair(lst.first, std::move(lst_reader.openFile(data_path / lst.second ))));
    }
}


std::unique_ptr<MapObject> MapReader::readMapObject() {
    auto object = std::make_unique<MapObject>();

    object->unknown0 = read_be_u32();
    object->position = read_be_i32();
    object->x = read_be_u32();
    object->y = read_be_u32();
    object->sx = read_be_u32();
    object->sy = read_be_u32();
    object->frame_number = read_be_u32();
    object->direction = read_be_u32();
    object->frm_pid = read_be_u32();
    object->flags = read_be_u32();
    object->elevation = read_be_u32();
    object->pro_pid = read_be_u32();
    object->critter_index = read_be_u32();
    object->light_radius = read_be_u32();
    object->light_intensity = read_be_u32();
    object->outline_color = read_be_u32();
    object->map_scripts_pid = read_be_i32();
    object->script_id = read_be_i32();
    object->objects_in_inventory = read_be_u32();
    object->max_inventory_size = read_be_u32();
    object->unknown10 = read_be_u32();
    object->unknown11 = read_be_u32();

    uint32_t objectTypeId = object->pro_pid >> 24;
    uint32_t objectId = 0x00FFFFFF & object->pro_pid;

    switch (static_cast<Object::OBJECT_TYPE>(objectTypeId)) {
        using enum Object::OBJECT_TYPE;
        case ITEM:
            {
                uint32_t subtype_id = loadPro(object->pro_pid)->objectSubtypeId();
                switch (static_cast<Object::ITEM_TYPE>(subtype_id)) {
                    using enum Object::ITEM_TYPE;
                    case AMMO: // ammo
                    case MISC: // charges - have strangely high values, or negative.
                        object->ammo = read_be_u32(); // bullets
                        break;
                    case KEY:
                        object->keycode = read_be_u32(); // keycode = -1 in all maps. saves only? ignore for now
                        break;
                    case WEAPON:
                        object->ammo = read_be_u32();  // ammo
                        object->ammo_pid = read_be_u32();  // ammo pid
                        break;
                    case ARMOR:
                    case CONTAINER:
                    case DRUG:
                        break;
                    default:
                        throw std::runtime_error{"Unknown item type " + std::to_string(objectTypeId)};
                }
            }
            break;
        case CRITTER:
            {
                object->player_reaction = read_be_u32();  // reaction to player - saves only
                object->current_mp = read_be_u32();  // stream.uint32(); //current mp - saves only
                object->combat_results = read_be_u32();  // stream.uint32(); //combat results - saves only
                object->dmg_last_turn = read_be_u32();  // stream.uint32(); //damage last turn - saves only
                object->ai_packet = read_be_u32();  // AI packet - is it different from .pro? well, it can be
                object->group_id = read_be_u32();  // team - always 1? saves only?
                object->who_hit_me = read_be_u32();  // who hit me - saves only
                object->current_hp = read_be_u32();  // hit points - saves only, otherwise = value from .pro
                object->current_rad = read_be_u32();  // rad - always 0 - saves only
                object->current_poison = read_be_u32();  // poison - always 0 - saves only
            }
            break;

        case SCENERY:
            {
                uint32_t subtype_id = loadPro(object->pro_pid)->objectSubtypeId();
                switch (static_cast<Object::SCENERY_TYPE>(subtype_id)) {
                    using enum Object::SCENERY_TYPE;
                    case LADDER_TOP:
                    case LADDER_BOTTOM:
                        object->map = read_be_u32();
                        object->elevhex = read_be_u32();
                        // hex = elevhex & 0xFFFF;
                        // elev = ((elevhex >> 28) & 0xf) >> 1;
                        break;
                    case STAIRS:
                        // looks like for ladders and stairs map and elev+hex fields in the different order
                        object->elevhex = read_be_u32();
                        object->map = read_be_u32();
                        // hex = elevhex & 0xFFFF;
                        // elev = ((elevhex >> 28) & 0xf) >> 1;
                        break;
                    case ELEVATOR:
                        object->elevtype = read_be_u32();  // elevator type - sometimes -1
                        object->elevlevel = read_be_u32();  // current level - sometimes -1
                        break;
                    case DOOR:
                        object->walkthrough = read_be_u32();  // != 0 -> is opened;
                        break;
                    case GENERIC:
                        break;
                    default:
                        throw std::runtime_error{"Unknown scenery type: " + std::to_string(subtype_id)};
                }
            }
            break;
        case WALL:
        case TILE:
            break;
        case MISC:

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
                    object->exit_map = read_be_i32();
                    object->exit_position = read_be_i32();
                    object->exit_elevation = read_be_i32();
                    object->exit_orientation = read_be_i32();
                    break;
            }
            break;
        default:
            spdlog::error("Unknown object type {}", objectTypeId);
            break;
//            throw std::runtime_error{"Unknown object type: " + std::to_string(objectTypeId)};
    }

    return std::move(object);
}

// TODO: split
std::unique_ptr<Map> MapReader::read() {

    auto map = std::make_unique<Map>();
    auto map_file = std::make_unique<Map::MapFile>();

    // 19 or 20
    auto version = read_be_u32();

    if (version == 19) {
        throw std::runtime_error{"Fallout 1 maps are not supported yet"};
    }

    if (version != 20) {
        throw std::runtime_error{"Unknown map version " + std::to_string(version)};
    }

    map_file->header.version = version;

    std::string filename = read_str(16);
    map_file->header.filename = filename;

    spdlog::info("Loading map {} from {}", filename, path.string());

    map_file->header.player_default_position = read_be_u32();
    map_file->header.player_default_elevation = read_be_u32();
    map_file->header.player_default_orientation = read_be_u32();

    uint32_t num_local_vars = read_be_u32();  // num local vars
    map_file->header.num_local_vars = num_local_vars;
    map_file->header.script_id = read_be_i32(); // Script id

    uint32_t flags = read_be_u32();  // map flags
    map_file->header.flags = flags;

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

//    skip<4>();  // map darkness (unused)
    map_file->header.darkness = read_be_u32();

    uint32_t num_global_vars = read_be_u32();  // num global vars
    map_file->header.num_global_vars = num_global_vars;

    map_file->header.map_id = read_be_u32();                             // map id
    map_file->header.timestamp = read_be_u32();                             // timestamp

    skip<4 * 44>();  // skipping to the end of a MAP header

    for (uint32_t i = 0; i < num_local_vars; ++i) {
        map_file->map_local_vars.emplace_back(read_be_u32());
    }
    for (uint32_t i = 0; i < num_global_vars; ++i) {
        map_file->map_global_vars.emplace_back(read_be_u32());
    }

    for (auto elevation = 0; elevation < elevations; ++elevation) {
        spdlog::info("Loading tiles at elevation {}", elevation);
        for (auto i = 0U; i < Map::TILES_PER_ELEVATION; ++i) {
            uint16_t roof = read_be_u16();
            uint16_t floor = read_be_u16();

            Tile tile(floor, roof);
            map_file->tiles[elevation].push_back(tile);
        }
    }

    // TODO: remove
    map->setTiles(std::move(map_file->tiles));

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
    spdlog::info("Loading map scripts");
    for (unsigned i = 0; i < Map::SCRIPT_SECTIONS; i++) {
        uint32_t count = read_be_u32(); // total number of scripts in section
        map_file->scripts_in_section[i] = count;

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

                int32_t pid = read_be_i32();

                map_script.pid = pid;
                map_script.next_script = read_be_u32();  // next script. unused

                switch ((pid & 0xFF000000) >> 24) {
                    case 0:
                        break;
                    case 1: // spatial script
                        map_script.timer = read_be_u32();
                        map_script.spatial_radius = read_be_u32();
                        break;
                    case 2: // timer script
                        map_script.timer = read_be_u32();
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
                map_script.flags = read_be_u32();  // flags
                map_script.script_id = read_be_i32();  // scriptId (?)
                map_script.unknown5 = read_be_u32();  // unknown 5
                map_script.script_oid = read_be_u32();  // oid == object->OID
                map_script.local_var_offset = read_be_u32();  // local var offset
                map_script.local_var_count = read_be_u32();  // loal var cnt
                map_script.unknown9 = read_be_u32();  // unknown 9
                map_script.unknown10 = read_be_u32();  // unknown 10
                map_script.unknown11 = read_be_u32();  // unknown 11
                map_script.unknown12 = read_be_u32();  // unknown 12
                map_script.unknown13 = read_be_u32();  // unknown 13
                map_script.unknown14 = read_be_u32();  // unknown 14
                map_script.unknown15 = read_be_u32();  // unknown 15
                map_script.unknown16 = read_be_u32();  // unknown 16

                if (j < count) {
                    map_file->map_scripts[i].push_back(map_script);
                }

                if ((j % 16) == 15) {

                    // TODO: write after the batch
                    // number of scripts in this batch (sequence)
                    uint32_t cur_check = read_be_u32();

                    spdlog::info("Current script check for sequence " + std::to_string(i) + " is " + std::to_string(cur_check) + ", j = " + std::to_string(j));

                    check += cur_check;

                    // TODO: check, klamath does not read this last value
                    read_be_u32(); // uknown
                }
            }
            if (check != count) {
                throw std::runtime_error{"Error reading scripts: check is incorrect"};
            }
        }
    }

    // OBJECTS SECTION
    uint32_t total_objects = read_be_u32();  // objects total

    spdlog::info("Loading {} map objects", total_objects);
    for (auto elev = 0; elev < elevations; ++elev) {
        auto objectsOnElevation = read_be_u32();

        spdlog::info("Loading {} map objects on elevation {}", objectsOnElevation, elev);
        for (size_t j = 0; j != objectsOnElevation; ++j) {

            std::unique_ptr<MapObject> object = readMapObject();

            if (object->objects_in_inventory > 0) {

                object->inventory.reserve(object->objects_in_inventory);

                for (size_t i = 0; i < object->objects_in_inventory; ++i) {
                    uint32_t amount = read_be_u32();
                    std::unique_ptr<MapObject> subobject = readMapObject();
                    subobject->amount = amount;

                    object->inventory.push_back(std::move(subobject));
                }
            }
            map_file->map_objects[elev].push_back(std::move(object));
        }

        if (objectsOnElevation != map_file->map_objects[elev].size()) {
            throw std::runtime_error{"Object count doesn't match: " + std::to_string(objectsOnElevation) + " vs " +
                                     std::to_string(map_file->map_objects[elev].size())};
        }
    }

//    std::unordered_map<int, std::vector<std::unique_ptr<Object> > > objects;
//    for (size_t elev = 0; elev < map_file->map_objects.size(); elev++) {
//        for (size_t i = 0; i < map_file->map_objects[elev].size(); i++) {

//            auto mapobject = objectToMapObject(map_file->map_objects[elev][i]);

//            objects[elev].push_back(std::move(mapobject));
//        }
//    }

    map->setMapFile(std::move(map_file));

    return map;
}

}  // namespace geck
