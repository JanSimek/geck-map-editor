#include "ProReader.h"

#include "../../editor/MapObject.h"
#include "../../format/pro/Pro.h"
#include "../../util/io.h"

namespace geck {

std::unique_ptr<Pro> ProReader::read(std::istream& stream) {
    using namespace geck::io;

    auto pro = std::make_unique<Pro>();

    int32_t PID = read_be_u32(stream);
    int32_t type = (PID & 0x0F000000) >> 24;

    read_be_u32(stream);  //    _messageId      = stream.uint32();

    read_be_u32(stream);  //    _FID            = stream.int32();

    read_be_u32(stream);  //    _lightDistance  = stream.uint32();
    read_be_u32(stream);  //    _lightIntencity = stream.uint32();
    read_be_u32(stream);  //    _flags          = stream.uint32();

    switch ((MapObject::OBJECT_TYPE)type) {
        case MapObject::OBJECT_TYPE::TILE:
        case MapObject::OBJECT_TYPE::MISC:
            break;
        default:
            read_be_u32(stream);  //_flagsExt = stream.uint32();
            break;
    }

    switch ((MapObject::OBJECT_TYPE)type) {
        case MapObject::OBJECT_TYPE::ITEM:
        case MapObject::OBJECT_TYPE::CRITTER:
        case MapObject::OBJECT_TYPE::SCENERY:
        case MapObject::OBJECT_TYPE::WALL:
            read_be_u32(stream);  //_SID = stream.int32();
            break;
        case MapObject::OBJECT_TYPE::TILE:
        case MapObject::OBJECT_TYPE::MISC:
            break;
    }

    switch ((MapObject::OBJECT_TYPE)type) {
            //    {
        case MapObject::OBJECT_TYPE::ITEM: {
            uint32_t subtypeId = read_be_u32(stream);  //            _subtypeId     = stream.uint32();
            pro->setObjectSubtypeId(subtypeId);

            read_be_u32(stream);  //            _materialId    = stream.uint32();
            read_be_u32(stream);  //            _containerSize = stream.uint32();
            read_be_u32(stream);  //            _weight        = stream.uint32();
            read_be_u32(stream);  //            _basePrice     = stream.uint32();
            read_be_u32(stream);  //            _inventoryFID  = stream.int32();
            read_be_u8(stream);   //            _soundId       = stream.uint8();

            switch ((MapObject::ITEM_TYPE)subtypeId) {
                case MapObject::ITEM_TYPE::ARMOR: {
                    read_be_u32(stream);  // _armorClass = stream.uint32();
                    // Damage resist
                    for (unsigned int i = 0; i != 7; ++i) {
                        read_be_u32(stream);
                        //                        _damageResist.at(i) = stream.uint32();
                    }
                    // Damage threshold
                    for (unsigned int i = 0; i != 7; ++i) {
                        read_be_u32(stream);
                        //                        _damageThreshold.at(i) = stream.uint32();
                    }
                    read_be_u32(stream);  //_perk           = stream.int32();
                    read_be_u32(stream);  //_armorMaleFID   = stream.int32();
                    read_be_u32(stream);  //_armorFemaleFID = stream.int32();
                    break;
                }
                case MapObject::ITEM_TYPE::CONTAINER: {
                    read_be_u32(stream);  // stream.uint32(); // max size
                    read_be_u32(stream);  // stream.uint32(); // containter flags
                    break;
                }
                case MapObject::ITEM_TYPE::DRUG: {
                    read_be_u32(stream);  // stream.uint32(); // Stat0
                    read_be_u32(stream);  // stream.uint32(); // Stat1
                    read_be_u32(stream);  // stream.uint32(); // Stat2
                    read_be_u32(stream);  // stream.uint32(); // Stat0 ammount
                    read_be_u32(stream);  // stream.uint32(); // Stat1 ammount
                    read_be_u32(stream);  // stream.uint32(); // Stat2 ammount
                    // first delayed effect
                    read_be_u32(stream);  // stream.uint32(); // delay in game minutes
                    read_be_u32(stream);  // stream.uint32(); // Stat0 ammount
                    read_be_u32(stream);  // stream.uint32(); // Stat1 ammount
                    read_be_u32(stream);  // stream.uint32(); // Stat2 ammount
                    // second delayed effect
                    read_be_u32(stream);  // stream.uint32(); // delay in game minutes
                    read_be_u32(stream);  // stream.uint32(); // Stat0 ammount
                    read_be_u32(stream);  // stream.uint32(); // Stat1 ammount
                    read_be_u32(stream);  // stream.uint32(); // Stat2 ammount
                    read_be_u32(stream);  // stream.uint32(); // addiction chance
                    read_be_u32(stream);  // stream.uint32(); // addiction perk
                    read_be_u32(stream);  // stream.uint32(); // addiction delay
                    break;
                }
                case MapObject::ITEM_TYPE::WEAPON:
                    read_be_u32(stream);  //_weaponAnimationCode  = stream.uint32();
                    read_be_u32(stream);  //_weaponDamageMin      = stream.uint32();
                    read_be_u32(stream);  //_weaponDamageMax      = stream.uint32();
                    read_be_u32(stream);  //_weaponDamageType     = stream.uint32();
                    read_be_u32(stream);  //_weaponRangePrimary   = stream.uint32();
                    read_be_u32(stream);  //_weaponRangeSecondary = stream.uint32();
                    read_be_u32(stream);  // stream.uint32(); // Proj PID
                    read_be_u32(stream);  //_weaponMinimumStrenght     = stream.uint32();
                    read_be_u32(stream);  //_weaponActionCostPrimary   = stream.uint32();
                    read_be_u32(stream);  //_weaponActionCostSecondary = stream.uint32();
                    read_be_u32(stream);  // stream.uint32(); // Crit Fail
                    read_be_u32(stream);  //_perk = stream.int32();
                    read_be_u32(stream);  //_weaponBurstRounds  = stream.uint32();
                    read_be_u32(stream);  //_weaponAmmoType     = stream.uint32();
                    read_be_u32(stream);  //_weaponAmmoPID      = stream.uint32();
                    read_be_u32(stream);  //_weaponAmmoCapacity = stream.uint32();
                    read_be_u8(stream);   //_soundId = stream.uint8();
                    break;
                case MapObject::ITEM_TYPE::AMMO:
                    break;
                case MapObject::ITEM_TYPE::MISC:
                    break;
                case MapObject::ITEM_TYPE::KEY:
                    break;
            }
            break;
        }
        case MapObject::OBJECT_TYPE::CRITTER: {
            read_be_u32(stream);  //_critterHeadFID = stream.int32();

            read_be_u32(stream);  // stream.uint32(); // ai packet number
            read_be_u32(stream);  // stream.uint32(); // team number
            read_be_u32(stream);  //_critterFlags = stream.uint32();

            for (unsigned int i = 0; i != 7; ++i) {
                read_be_u32(stream);  //_critterStats.at(i) = stream.uint32();
            }
            read_be_u32(stream);  //_critterHitPointsMax = stream.uint32();
            read_be_u32(stream);  //_critterActionPoints = stream.uint32();
            read_be_u32(stream);  //_critterArmorClass   = stream.uint32();
            read_be_u32(stream);  // stream.uint32(); // Unused
            read_be_u32(stream);  //_critterMeleeDamage    = stream.uint32();
            read_be_u32(stream);  // _critterCarryWeightMax = stream.uint32();
            read_be_u32(stream);  //_critterSequence       = stream.uint32();
            read_be_u32(stream);  //_critterHealingRate    = stream.uint32();
            read_be_u32(stream);  //_critterCriticalChance = stream.uint32();
            read_be_u32(stream);  // stream.uint32(); // Better criticals

            // Damage threshold
            for (unsigned int i = 0; i != 7; ++i) {
                read_be_u32(stream);  //_damageThreshold.at(i) = stream.uint32();
            }
            // Damage resist
            for (unsigned int i = 0; i != 9; ++i) {
                read_be_u32(stream);  //_damageResist.at(i) = stream.uint32();
            }

            read_be_u32(stream);  //_critterAge = stream.uint32(); // age
            read_be_u32(stream);  //_critterGender = stream.uint32(); // sex

            for (unsigned int i = 0; i != 7; ++i) {
                read_be_u32(stream);  //_critterStatsBonus.at(i) = stream.uint32();
            }

            read_be_u32(stream);  // stream.uint32(); // Bonus Health points
            read_be_u32(stream);  // stream.uint32(); // Bonus Action points
            read_be_u32(stream);  // stream.uint32(); // Bonus Armor class
            read_be_u32(stream);  // stream.uint32(); // Bonus Unused
            read_be_u32(stream);  // stream.uint32(); // Bonus Melee damage
            read_be_u32(stream);  // stream.uint32(); // Bonus Carry weight
            read_be_u32(stream);  // stream.uint32(); // Bonus Sequence
            read_be_u32(stream);  // stream.uint32(); // Bonus Healing rate
            read_be_u32(stream);  // stream.uint32(); // Bonus Critical chance
            read_be_u32(stream);  // stream.uint32(); // Bonus Better criticals

            // Bonus Damage threshold
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);

            // Bonus Damage resistance
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);
            read_be_u32(stream);

            read_be_u32(stream);  // Bonus age
            read_be_u32(stream);  // Bonus sex

            for (unsigned int i = 0; i != 18; ++i) {
                read_be_u32(stream);  //_critterSkills.at(i) = stream.uint32();
            }

            read_be_u32(stream);  // body type
            read_be_u32(stream);  // experience for kill
            read_be_u32(stream);  // kill type
            read_be_u32(stream);  // damage type
            break;
        }
        case MapObject::OBJECT_TYPE::SCENERY: {
            uint32_t subtypeId = read_be_u32(stream);  // _subtypeId  = stream.uint32();
            pro->setObjectSubtypeId(subtypeId);

            read_be_u32(stream);  //_materialId = stream.uint32();
            read_be_u8(stream);   // _soundId    = stream.uint8();

            switch ((MapObject::SCENERY_TYPE)subtypeId) {
                case MapObject::SCENERY_TYPE::DOOR: {
                    read_be_u32(stream);  // stream.uint32(); // walk thru flag
                    read_be_u32(stream);  // stream.uint32(); // unknown
                    break;
                }
                case MapObject::SCENERY_TYPE::STAIRS: {
                    read_be_u32(stream);  // stream.uint32(); // DestTile && DestElevation
                    read_be_u32(stream);  // stream.uint32(); // DestElevation
                    break;
                }
                case MapObject::SCENERY_TYPE::ELEVATOR: {
                    read_be_u32(stream);  // stream.uint32(); // Elevator type
                    read_be_u32(stream);  // stream.uint32(); // Elevator level
                    break;
                }
                case MapObject::SCENERY_TYPE::LADDER_BOTTOM:
                case MapObject::SCENERY_TYPE::LADDER_TOP: {
                    read_be_u32(stream);  // stream.uint32(); // DestTile && DestElevation
                    break;
                }
                case MapObject::SCENERY_TYPE::GENERIC: {
                    read_be_u32(stream);  // stream.uint32(); // unknown
                    break;
                }
            }

            break;
        }
        case MapObject::OBJECT_TYPE::WALL: {
            read_be_u32(stream);  //_materialId = stream.uint32();
            break;
        }
        case MapObject::OBJECT_TYPE::TILE: {
            read_be_u32(stream);  //_materialId = stream.uint32();
            break;
        }
        case MapObject::OBJECT_TYPE::MISC: {
            read_be_u32(stream);  // stream.uint32(); // unknown
            break;
        }
    }
    //    }
    return pro;
}

}  // namespace geck
