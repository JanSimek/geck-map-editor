#include "ProReader.h"

#include "../../format/pro/Pro.h"
#include "../../format/lst/Lst.h"
#include "../../reader/lst/LstReader.h"
#include "../../util/FileHelper.h"
#include "../../util/io.h"

namespace geck {

std::unique_ptr<Pro> ProReader::read() {
    using namespace geck::io;

    auto pro = std::make_unique<Pro>();

    int32_t PID = read_be_u32();
    int32_t type = (PID & 0x0F000000) >> 24;

    int32_t messageId = read_be_u32();
    pro->setMessageId(messageId);

    read_be_u32(); //    _FID            = stream.int32();

    read_be_u32(); //    _lightDistance  = stream.uint32();
    read_be_u32(); //    _lightIntencity = stream.uint32();
    read_be_u32(); //    _flags          = stream.uint32();

    switch (static_cast<Pro::OBJECT_TYPE>(type)) {
        case Pro::OBJECT_TYPE::TILE:
        case Pro::OBJECT_TYPE::MISC:
            break;
        default:
            read_be_u32(); //_flagsExt = stream.uint32();
            break;
    }

    switch (static_cast<Pro::OBJECT_TYPE>(type)) {
        case Pro::OBJECT_TYPE::ITEM:
        case Pro::OBJECT_TYPE::CRITTER:
        case Pro::OBJECT_TYPE::SCENERY:
        case Pro::OBJECT_TYPE::WALL:
            read_be_u32(); //_SID = stream.int32();
            break;
        case Pro::OBJECT_TYPE::TILE:
        case Pro::OBJECT_TYPE::MISC:
            break;
    }

    switch (static_cast<Pro::OBJECT_TYPE>(type)) {
        case Pro::OBJECT_TYPE::ITEM: {
            uint32_t subtypeId = read_be_u32();
            pro->setObjectSubtypeId(subtypeId);

            read_be_u32(); //            _materialId    = stream.uint32();
            read_be_u32(); //            _containerSize = stream.uint32();
            read_be_u32(); //            _weight        = stream.uint32();
            read_be_u32(); //            _basePrice     = stream.uint32();
            read_be_u32(); //            _inventoryFID  = stream.int32();
            read_be_u8();  //            _soundId       = stream.uint8();

            switch ((Pro::ITEM_TYPE)subtypeId) {
                case Pro::ITEM_TYPE::ARMOR: {
                    read_be_u32(); // _armorClass = stream.uint32();
                    // Damage resist
                    for (unsigned int i = 0; i != 7; ++i) {
                        read_be_u32();
                        //                        _damageResist.at(i) = stream.uint32();
                    }
                    // Damage threshold
                    for (unsigned int i = 0; i != 7; ++i) {
                        read_be_u32();
                        //                        _damageThreshold.at(i) = stream.uint32();
                    }
                    read_be_u32(); //_perk           = stream.int32();
                    read_be_u32(); //_armorMaleFID   = stream.int32();
                    read_be_u32(); //_armorFemaleFID = stream.int32();
                    break;
                }
                case Pro::ITEM_TYPE::CONTAINER: {
                    read_be_u32(); // stream.uint32(); // max size
                    read_be_u32(); // stream.uint32(); // containter flags
                    break;
                }
                case Pro::ITEM_TYPE::DRUG: {
                    read_be_u32(); // stream.uint32(); // Stat0
                    read_be_u32(); // stream.uint32(); // Stat1
                    read_be_u32(); // stream.uint32(); // Stat2
                    read_be_u32(); // stream.uint32(); // Stat0 ammount
                    read_be_u32(); // stream.uint32(); // Stat1 ammount
                    read_be_u32(); // stream.uint32(); // Stat2 ammount
                    // first delayed effect
                    read_be_u32(); // stream.uint32(); // delay in game minutes
                    read_be_u32(); // stream.uint32(); // Stat0 ammount
                    read_be_u32(); // stream.uint32(); // Stat1 ammount
                    read_be_u32(); // stream.uint32(); // Stat2 ammount
                    // second delayed effect
                    read_be_u32(); // stream.uint32(); // delay in game minutes
                    read_be_u32(); // stream.uint32(); // Stat0 ammount
                    read_be_u32(); // stream.uint32(); // Stat1 ammount
                    read_be_u32(); // stream.uint32(); // Stat2 ammount
                    read_be_u32(); // stream.uint32(); // addiction chance
                    read_be_u32(); // stream.uint32(); // addiction perk
                    read_be_u32(); // stream.uint32(); // addiction delay
                    break;
                }
                case Pro::ITEM_TYPE::WEAPON:
                    read_be_u32(); //_weaponAnimationCode  = stream.uint32();
                    read_be_u32(); //_weaponDamageMin      = stream.uint32();
                    read_be_u32(); //_weaponDamageMax      = stream.uint32();
                    read_be_u32(); //_weaponDamageType     = stream.uint32();
                    read_be_u32(); //_weaponRangePrimary   = stream.uint32();
                    read_be_u32(); //_weaponRangeSecondary = stream.uint32();
                    read_be_u32(); // stream.uint32(); // Proj PID
                    read_be_u32(); //_weaponMinimumStrenght     = stream.uint32();
                    read_be_u32(); //_weaponActionCostPrimary   = stream.uint32();
                    read_be_u32(); //_weaponActionCostSecondary = stream.uint32();
                    read_be_u32(); // stream.uint32(); // Crit Fail
                    read_be_u32(); //_perk = stream.int32();
                    read_be_u32(); //_weaponBurstRounds  = stream.uint32();
                    read_be_u32(); //_weaponAmmoType     = stream.uint32();
                    read_be_u32(); //_weaponAmmoPID      = stream.uint32();
                    read_be_u32(); //_weaponAmmoCapacity = stream.uint32();
                    read_be_u8();  //_soundId = stream.uint8();
                    break;
                case Pro::ITEM_TYPE::AMMO:
                    break;
                case Pro::ITEM_TYPE::MISC:
                    break;
                case Pro::ITEM_TYPE::KEY:
                    break;
            }
            break;
        }
        case Pro::OBJECT_TYPE::CRITTER: {
            read_be_i32(); //_critterHeadFID = stream.int32();

            read_be_u32(); // stream.uint32(); // ai packet number
            read_be_u32(); // stream.uint32(); // team number
            read_be_u32(); //_critterFlags = stream.uint32();

            // S P E C I A L
            for (unsigned int i = 0; i != 7; ++i) {
                read_be_u32(); //_critterStats.at(i) = stream.uint32();
            }
            read_be_u32(); //_critterHitPointsMax = stream.uint32();
            read_be_u32(); //_critterActionPoints = stream.uint32();
            read_be_u32(); //_critterArmorClass   = stream.uint32();
            read_be_u32(); // stream.uint32(); // Unused
            read_be_u32(); //_critterMeleeDamage    = stream.uint32();
            read_be_u32(); // _critterCarryWeightMax = stream.uint32();
            read_be_u32(); //_critterSequence       = stream.uint32();
            read_be_u32(); //_critterHealingRate    = stream.uint32();
            read_be_u32(); //_critterCriticalChance = stream.uint32();
            read_be_u32(); // stream.uint32(); // Better criticals

            // Damage threshold
            for (unsigned int i = 0; i != 7; ++i) {
                read_be_u32(); //_damageThreshold.at(i) = stream.uint32();
            }
            // Damage resist
            for (unsigned int i = 0; i != 9; ++i) {
                read_be_u32(); //_damageResist.at(i) = stream.uint32();
            }

            read_be_u32(); //_critterAge = stream.uint32(); // age
            read_be_u32(); //_critterGender = stream.uint32(); // sex

            for (unsigned int i = 0; i != 7; ++i) {
                read_be_u32(); //_critterStatsBonus.at(i) = stream.uint32();
            }

            read_be_u32(); // stream.uint32(); // Bonus Health points
            read_be_u32(); // stream.uint32(); // Bonus Action points
            read_be_u32(); // stream.uint32(); // Bonus Armor class
            read_be_u32(); // stream.uint32(); // Bonus Unused
            read_be_u32(); // stream.uint32(); // Bonus Melee damage
            read_be_u32(); // stream.uint32(); // Bonus Carry weight
            read_be_u32(); // stream.uint32(); // Bonus Sequence
            read_be_u32(); // stream.uint32(); // Bonus Healing rate
            read_be_u32(); // stream.uint32(); // Bonus Critical chance
            read_be_u32(); // stream.uint32(); // Bonus Better criticals

            // Bonus Damage threshold
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();

            // Bonus Damage resistance
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();
            read_be_u32();

            read_be_u32(); // Bonus age
            read_be_u32(); // Bonus sex

            for (unsigned int i = 0; i != 18; ++i) {
                read_be_u32(); //_critterSkills.at(i) = stream.uint32();
            }

            read_be_u32(); // body type
            read_be_u32(); // experience for kill
            read_be_u32(); // kill type
            read_be_u32(); // damage type
            break;
        }
        case Pro::OBJECT_TYPE::SCENERY: {
            uint32_t subtypeId = read_be_u32(); // _subtypeId  = stream.uint32();
            pro->setObjectSubtypeId(subtypeId);

            read_be_u32(); //_materialId = stream.uint32();
            read_be_u8();  // _soundId    = stream.uint8();

            switch ((Pro::SCENERY_TYPE)subtypeId) {
                case Pro::SCENERY_TYPE::DOOR: {
                    read_be_u32(); // stream.uint32(); // walk thru flag
                    read_be_u32(); // stream.uint32(); // unknown
                    break;
                }
                case Pro::SCENERY_TYPE::STAIRS: {
                    read_be_u32(); // stream.uint32(); // DestTile && DestElevation
                    read_be_u32(); // stream.uint32(); // DestElevation
                    break;
                }
                case Pro::SCENERY_TYPE::ELEVATOR: {
                    read_be_u32(); // stream.uint32(); // Elevator type
                    read_be_u32(); // stream.uint32(); // Elevator level
                    break;
                }
                case Pro::SCENERY_TYPE::LADDER_BOTTOM:
                case Pro::SCENERY_TYPE::LADDER_TOP: {
                    read_be_u32(); // stream.uint32(); // DestTile && DestElevation
                    break;
                }
                case Pro::SCENERY_TYPE::GENERIC: {
                    read_be_u32(); // stream.uint32(); // unknown
                    break;
                }
            }

            break;
        }
        case Pro::OBJECT_TYPE::WALL: {
            read_be_u32(); //_materialId = stream.uint32();
            break;
        }
        case Pro::OBJECT_TYPE::TILE: {
            read_be_u32(); //_materialId = stream.uint32();
            break;
        }
        case Pro::OBJECT_TYPE::MISC: {
            read_be_u32(); // stream.uint32(); // unknown
            break;
        }
    }
    return pro;
}

std::unique_ptr<Pro> ProReader::loadPro(const std::filesystem::path& dataPath, unsigned int PID) {
    unsigned int typeId = PID >> 24;

    auto listFile = dataPath;

    switch (static_cast<Pro::OBJECT_TYPE>(typeId)) {
        case Pro::OBJECT_TYPE::ITEM:
            listFile /= "proto/items/items.lst";
            break;
        case Pro::OBJECT_TYPE::CRITTER:
            listFile /= "proto/critters/critters.lst";
            break;
        case Pro::OBJECT_TYPE::SCENERY:
            listFile /= "proto/scenery/scenery.lst";
            break;
        case Pro::OBJECT_TYPE::WALL:
            listFile /= "proto/walls/walls.lst";
            break;
        case Pro::OBJECT_TYPE::TILE:
            listFile /= "proto/tiles/tiles.lst";
            break;
        case Pro::OBJECT_TYPE::MISC:
            listFile /= "proto/misc/misc.lst";
            break;
        default:
            throw std::runtime_error{ "Wrong PID: " + std::to_string(PID) };
    }

    LstReader lst_reader;
    auto lst = lst_reader.openFile(listFile);

    unsigned int index = 0x00000FFF & PID;

    if (index > lst->list().size()) {
        throw std::runtime_error{ "LST size < PID: " + std::to_string(PID) };
    }

    std::string protoName = lst->list().at(index - 1);

    std::filesystem::path proFilename;
    switch (static_cast<Pro::OBJECT_TYPE>(typeId)) {
        case Pro::OBJECT_TYPE::ITEM:
            proFilename /= "proto/items";
            break;
        case Pro::OBJECT_TYPE::CRITTER:
            proFilename /= "proto/critters";
            break;
        case Pro::OBJECT_TYPE::SCENERY:
            proFilename /= "proto/scenery";
            break;
        case Pro::OBJECT_TYPE::WALL:
            proFilename /= "proto/walls";
            break;
        case Pro::OBJECT_TYPE::TILE:
            proFilename /= "proto/tiles";
            break;
        case Pro::OBJECT_TYPE::MISC:
            proFilename /= "proto/misc";
            break;
    };
    proFilename /= protoName;
    if (!proFilename.empty()) {
        return openFile(dataPath / proFilename);
    } else {
        throw std::runtime_error{ "Couldn't load PRO file " + protoName };
    }
}
} // namespace geck
