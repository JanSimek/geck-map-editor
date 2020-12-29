#include "MapReader.h"

#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <string>
#include <spdlog/spdlog.h>

#include "../../editor/Tile.h"
#include "../../editor/MapObject.h"

#include "../../util/io.h"
#include "../../format/map/Map.h"
#include "../../format/pro/Pro.h"
#include "../../reader/pro/ProReader.h"
#include "../../format/lst/Lst.h"
#include "../../reader/lst/LstReader.h"

namespace geck {

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

    // FIXME: data path
    std::string listFile = "resources/";

    switch ((MapObject::OBJECT_TYPE)typeId)
    {
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
            throw std::runtime_error{"loadPro(unsigned int PID) - wrong PID: " + std::to_string(PID)};
    }

    LstReader lst_reader;
    auto lst = lst_reader.read(listFile);

    unsigned int index = 0x00000FFF & PID;

    if (index > lst->list().size())
    {
        throw std::runtime_error{"loadPro(unsigned int PID) - LST size < PID: " + std::to_string(PID)};
    }

    std::string protoName = lst->list().at(index-1); // FIXME ? is it -1?

    std::string proFilename;
    switch ((MapObject::OBJECT_TYPE)typeId)
    {
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
        // FIXME: data path
        return pro_reader.read("resources/" + proFilename);
    } else {
        throw std::runtime_error{"Couldn't load PRO file " + protoName};
    }
}

enum class FRM_TYPE : char
{
    ITEM = 0,
    CRITTER,
    SCENERY,
    WALL,
    TILE,
    MISC,
    INTERFACE,
    INVENTORY
};

std::string FIDtoFrmName(unsigned int FID)
{
    /*const*/ auto baseId = FID & 0x00FFFFFF; // FIXME? 0x00000FFF;
    /*const*/ auto type = static_cast<FRM_TYPE>(FID >> 24);

    if (type == FRM_TYPE::CRITTER) {
        baseId = FID & 0x00000FFF;
        type = static_cast<FRM_TYPE>((FID & 0x0F000000) >> 24); // FIXME: WTF?
    }

    // TODO: _showScrlBlk
    if (type == FRM_TYPE::MISC && baseId == 1)
    {
        static const std::string SCROLL_BLOCKERS_PATH("art/misc/scrblk.frm");
        // Map scroll blockers
//        return SCROLL_BLOCKERS_PATH;
        return std::string();
    }

    static struct TypeArtListDecription
    {
        const std::string prefixPath;
        const std::string lstFilePath;
    } const frmTypeDescription[] =
    {
        { "art/items/", "art/items/items.lst" },
        { "art/critters/", "art/critters/critters.lst" },
        { "art/scenery/", "art/scenery/scenery.lst" },
        { "art/walls/", "art/walls/walls.lst" },
        { "art/tiles/", "art/tiles/tiles.lst" },
        { "art/misc/", "art/misc/misc.lst" },
        { "art/intrface/", "art/intrface/intrface.lst" },
        { "art/inven/", "art/inven/inven.lst" },
    };

    if (type > FRM_TYPE::INVENTORY) {
        throw std::runtime_error{"Invalid FRM_TYPE"};
    }

    const auto& typeArtDescription = frmTypeDescription[static_cast<size_t>(type)];

    LstReader lst_reader;
    // FIXME data path
    auto lst = lst_reader.read("resources/" + typeArtDescription.lstFilePath);

    if (baseId >= lst->list().size())
    {
        throw std::runtime_error{"LST size " + std::to_string(lst->list().size()) + " <= frmID: " + std::to_string(baseId) + ", frmType: " + std::to_string((unsigned)type)};
    }

    std::string frmName = lst->list().at(baseId);

    // FIXME: resources prefix
    if (type == FRM_TYPE::CRITTER) {
        // TODO: correct direction and posture
        return typeArtDescription.prefixPath + frmName.substr(0, 6) + "aa.frm";
    }
    return typeArtDescription.prefixPath + frmName;
}

std::unique_ptr<MapObject> MapReader::readObject(std::istream &stream)
{
    using namespace io;

    auto object = std::make_unique<MapObject>();
    read_be_u32(stream); //object->setOID(stream.uint32());
    object->setHexPosition(read_be_i32(stream)); //object->setHexPosition(stream.int32());
    read_be_u32(stream); //object->setX(stream.uint32());
    read_be_u32(stream); //object->setY(stream.uint32());
    read_be_u32(stream); //object->setSx(stream.uint32());
    read_be_u32(stream); //object->setSy(stream.uint32());
    read_be_u32(stream); //object->setFrameNumber(stream.uint32());
    read_be_u32(stream); //object->setOrientation(stream.uint32());
    uint32_t FID = read_be_u32(stream); //

    object->setFID(FID);

//    if (static_cast<FRM_TYPE>(FID >> 24) != FRM_TYPE::CRITTER) {
//        std::cout << "Object FRM: " << FIDtoFrmName(FID) << std::endl;
        object->setFrm(FIDtoFrmName(FID));
//    }

    // TODO:
//    object->setFrmTypeId(FID >> 24);
//    object->setFrmId(0x00FFFFFF & FID);
    read_be_u32(stream); //object->setFlags(stream.uint32());
    read_be_u32(stream); // elevation

    uint32_t PID = read_be_u32(stream); //stream.uint32();
    uint32_t objectTypeId = PID >> 24;
//    object->setObjectTypeId(PID >> 24);

    uint32_t objectId = 0x00FFFFFF & PID;
//    object->setObjectId(0x00FFFFFF & PID);
    read_be_u32(stream); //object->setCombatId(stream.uint32());
    read_be_u32(stream); //object->setLightRadius(stream.uint32());
    read_be_u32(stream); //object->setLightIntensity(stream.uint32());
    read_be_u32(stream); //object->setOutline(stream.uint32());

    int32_t SID = read_be_i32(stream);
    if (SID != -1)
    {
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
    if (SID != -1)
    {
//        object->setScriptId(SID);
    }

    object->setInventory_size(read_be_u32(stream)); //object->setInventorySize(stream.uint32());
    read_be_u32(stream); //object->setMaxInventorySize(stream.uint32());
    read_be_u32(stream); //object->setUnknown12(stream.uint32());
    read_be_u32(stream); //object->setUnknown13(stream.uint32());

    switch ((MapObject::OBJECT_TYPE)objectTypeId)
    {
        case MapObject::OBJECT_TYPE::ITEM:
//            std::unique_ptr<Pro> proFile = ; //            object->setObjectSubtypeId(callback(PID)->subtypeId());
            object->setObjectSubtypeId(loadPro(PID)->objectSubtypeId());
            switch((MapObject::ITEM_TYPE)object->objectSubtypeId())
            {
                case MapObject::ITEM_TYPE::AMMO:
                    read_be_u32(stream); //object->setAmmo(stream.uint32()); // bullets
                    break;
                case MapObject::ITEM_TYPE::KEY:
                    read_be_u32(stream); //stream.uint32(); // keycode = -1 in all maps. saves only? ignore for now
                    break;
                case MapObject::ITEM_TYPE::MISC:
                    read_be_u32(stream); //object->setAmmo(stream.uint32()); //charges - have strangely high values, or negative.
                    break;
                case MapObject::ITEM_TYPE::WEAPON:
                    read_be_u32(stream); //object->setAmmo(stream.uint32()); // ammo
                    read_be_u32(stream); //object->setAmmoPID(stream.uint32()); // ammo pid
                    break;
                case MapObject::ITEM_TYPE::ARMOR:
                    break;
                case MapObject::ITEM_TYPE::CONTAINER:
                    break;
                case MapObject::ITEM_TYPE::DRUG:
                    break;
                default:
                    throw std::runtime_error{"Unknown item type " + std::to_string(objectTypeId)};
            }
            break;
        case MapObject::OBJECT_TYPE::CRITTER:
            read_be_u32(stream); //stream.uint32(); //reaction to player - saves only
            read_be_u32(stream); //stream.uint32(); //current mp - saves only
            read_be_u32(stream); //stream.uint32(); //combat results - saves only
            read_be_u32(stream); //stream.uint32(); //damage last turn - saves only
            read_be_u32(stream); //object->setAIPacket(stream.uint32()); // AI packet - is it different from .pro? well, it can be
            read_be_u32(stream); //stream.uint32(); // team - always 1? saves only?
            read_be_u32(stream); //stream.uint32(); // who hit me - saves only
            read_be_u32(stream); //stream.uint32(); // hit points - saves only, otherwise = value from .pro
            read_be_u32(stream); //stream.uint32(); // rad - always 0 - saves only
            read_be_u32(stream); //stream.uint32(); // poison - always 0 - saves only


//            object->setFrmId(FID & 0x00000FFF);
//            object->setObjectID1((FID & 0x0000F000) >> 12);
//            object->setObjectID2((FID & 0x00FF0000) >> 16);
//            object->setFrmTypeId((FID & 0x0F000000) >> 24);
//            object->setObjectID3((FID & 0xF0000000) >> 28);

            break;

        case MapObject::OBJECT_TYPE::SCENERY:

        //TODO
//            object->setObjectSubtypeId(callback(PID)->subtypeId());
//            std::unique_ptr<Pro> proFile2 = loadPro(PID);

            uint32_t elevhex;  // elev+hex
            uint32_t hex;
            uint32_t elev;
            int32_t map;
//            std::cout << "SCENERY SUBTYPE = " << loadPro(PID)->objectSubtypeId() << std::endl;

            object->setObjectSubtypeId(loadPro(PID)->objectSubtypeId());
            switch((MapObject::SCENERY_TYPE)object->objectSubtypeId())
            {
                case MapObject::SCENERY_TYPE::LADDER_TOP:
                case MapObject::SCENERY_TYPE::LADDER_BOTTOM:
                    read_be_u32(stream); //map = stream.int32();
                    read_be_u32(stream); //elevhex = stream.uint32();  // elev+hex
//                    hex = elevhex & 0xFFFF;
//                    elev = ((elevhex >> 28) & 0xf) >> 1;
//                    object->setExitMap(map); // map id
//                    object->setExitPosition(hex);
//                    object->setExitElevation(elev);
                    break;
                case MapObject::SCENERY_TYPE::STAIRS:
                    // looks like for ladders and stairs map and elev+hex fields in the different order
                    read_be_u32(stream); //elevhex = stream.uint32();  // elev+hex
                    read_be_u32(stream); //map = stream.int32();
//                    hex = elevhex & 0xFFFF;
//                    elev = ((elevhex >> 28) & 0xf) >> 1;
//                    object->setExitMap(map); // map id
//                    object->setExitPosition(hex);
//                    object->setExitElevation(elev);
                    break;
                case MapObject::SCENERY_TYPE::ELEVATOR:
                    read_be_u32(stream); //object->setElevatorType(stream.uint32()); // elevator type - sometimes -1
                    read_be_u32(stream); //object->setElevatorLevel(stream.uint32()); // current level - sometimes -1
                    break;
                case MapObject::SCENERY_TYPE::DOOR:
                    read_be_u32(stream); //object->setOpened(stream.uint32() != 0);// is opened;
                    break;
                case MapObject::SCENERY_TYPE::GENERIC:
                    break;
                default:
                    throw std::runtime_error{"Unknown scenery type: " + std::to_string(object->objectSubtypeId())};
            }
            break;
        case MapObject::OBJECT_TYPE::WALL:
            break;
        case MapObject::OBJECT_TYPE::TILE:
            break;
        case MapObject::OBJECT_TYPE::MISC:

            switch(objectId)
            {
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
                    read_be_i32(stream); //object->setExitMap(stream.int32());
                    read_be_i32(stream); //object->setExitPosition(stream.int32());
                    read_be_i32(stream); //object->setExitElevation(stream.int32());
                    read_be_i32(stream); //object->setExitOrientation(stream.int32());
                    break;
                default:
                    read_be_u32(stream); //stream.uint32();
                    read_be_u32(stream); //stream.uint32();
                    read_be_u32(stream); //stream.uint32();
                    read_be_u32(stream); //stream.uint32();
                    break;
            }
            break;
        default:
            throw std::runtime_error{"Unknown object type: " + std::to_string(objectTypeId)};
    }
    return object;
}

std::unique_ptr<Map> geck::MapReader::read(std::istream &stream)
{
    using namespace io;

    // 19 or 20
    auto version = read_be_u32(stream);

    if (version == 19) {
        throw std::runtime_error{"Fallout 1 maps are not supported yet"};
    }

    if (version != 20) {
        throw std::runtime_error{"Unknown map version " + std::to_string(version)};
    }

    auto map = std::make_unique<Map>();

    std::string filename = read_str(stream, 16);

    spdlog::info("Loading map " + filename);

    read_be_u32(stream); // player position
    read_be_u32(stream); // player elevation
    read_be_u32(stream); // player orientation

    uint32_t num_local_vars = read_be_u32(stream); // num local vars
    read_be_i32(stream);

    uint32_t flags = read_be_u32(stream); // map flags

    bool elevation_low = (flags & 0x2) == 0;
    bool elevation_medium = (flags & 0x4) == 0;
    bool elevation_high = (flags & 0x8) == 0;

    int elevations = 0;
    if (elevation_low) elevations++;
    if (elevation_medium) elevations++;
    if (elevation_high) elevations++;
    spdlog::info("Map has " + std::to_string(elevations) + " elevation(s)");

    skip<4>(stream);  // map darkness (unused)

    uint32_t num_global_vars = read_be_u32(stream); // num global vars
    read_be_u32(stream); // map id
    read_be_u32(stream); // timestamp

    skip<4*44>(stream);  // skipping to the end of a MAP header

    for (uint32_t i = 0; i < num_local_vars; ++i) {
        read_be_u32(stream);
    }
    for (uint32_t i = 0; i < num_global_vars; ++i) {
        read_be_u32(stream);
    }

    std::map<int, std::vector<Tile> > tiles;
    for (auto elevation = 0; elevation < elevations; ++elevation) {

        for (auto i = 0U; i < Map::TILES_PER_ELEVATION; ++i) {
            uint16_t roof = read_be_u16(stream);
            uint16_t floor = read_be_u16(stream);

            Tile tile(floor, roof);
            tiles[elevation].push_back(tile);
        }
    }
    map->setTiles(std::move(tiles));

    // SCRIPTS SECTION
    for (unsigned i = 0; i < 5; i++)
    {
        uint32_t count = read_be_u32(stream);

        if (count > 0)
        {
            uint32_t loop = count;
            if (count % 16 > 0 )
            {
                loop += 16 - count % 16;
            }

            uint32_t check = 0;
            for (unsigned j = 0; j < loop; j++)
            {
                int32_t pid = read_be_i32(stream);

                read_be_u32(stream); // next script. unused

                switch ((pid & 0xFF000000) >> 24)
                {
                    case 0:
                        break;
                    case 1:
                        read_be_u32(stream);
                        read_be_u32(stream);
                        break;
                    case 2:
                        read_be_u32(stream);
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
                        spdlog::error("Unknown script PID = " + std::to_string((pid & 0xFF000000) >> 24));
                        break;
                }
                read_be_u32(stream); //flags
                read_be_i32(stream); //scriptId (?)
                read_be_u32(stream); //unknown 5
                read_be_u32(stream); //oid == object->OID
                read_be_u32(stream); //local var offset
                read_be_u32(stream); //loal var cnt
                read_be_u32(stream); //unknown 9
                read_be_u32(stream); //unknown 10
                read_be_u32(stream); //unknown 11
                read_be_u32(stream); //unknown 12
                read_be_u32(stream); //unknown 13
                read_be_u32(stream); //unknown 14
                read_be_u32(stream); //unknown 15
                read_be_u32(stream); //unknown 16

                if (j < count)
                {
//                    _scripts.push_back(script);
                }

                if ((j % 16) == 15)
                {
                    check += read_be_u32(stream);
                    read_be_u32(stream);
                }
            }
            if (check != count)
            {
                throw std::runtime_error{"Error reading scripts: check is incorrect"};
            }
        }
    }

    //OBJECTS SECTION
    read_be_u32(stream); // objects total

    std::unordered_map<int, std::vector<std::unique_ptr<MapObject> > > objects;
    for (auto elev = 0; elev < elevations; ++elev)
    {
        auto objectsOnElevation = read_be_u32(stream);
        for (size_t j = 0; j != objectsOnElevation; ++j)
        {
            auto object = readObject(stream);
            if (object->inventory_size() > 0)
            {
                for (size_t i = 0; i < object->inventory_size(); ++i)
                {
                    uint32_t amount = read_be_u32(stream);
                    auto subobject = readObject(stream);
                    subobject->setAmount(amount);
                }
            }
            objects[elev].push_back(std::move(object));
        }

        if (objectsOnElevation != objects[elev].size()) {
            throw std::runtime_error{"Object count doesn't match: " + std::to_string(objectsOnElevation) + " vs " + std::to_string(objects[elev].size())};
        }
    }
    map->setObjects(std::move(objects));

    return map;
}

}
