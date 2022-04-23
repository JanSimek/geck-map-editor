#include "ProReader.h"

#include "../../editor/Object.h"
#include "../../format/pro/Pro.h"
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

    switch ((Object::OBJECT_TYPE)type) {
        case Object::OBJECT_TYPE::TILE:
        case Object::OBJECT_TYPE::MISC:
            break;
        default:
            read_be_u32(); //_flagsExt = stream.uint32();
            break;
    }

    switch ((Object::OBJECT_TYPE)type) {
        case Object::OBJECT_TYPE::ITEM:
        case Object::OBJECT_TYPE::CRITTER:
        case Object::OBJECT_TYPE::SCENERY:
        case Object::OBJECT_TYPE::WALL:
            read_be_u32(); //_SID = stream.int32();
            break;
        case Object::OBJECT_TYPE::TILE:
        case Object::OBJECT_TYPE::MISC:
            break;
    }

    switch ((Object::OBJECT_TYPE)type) {
            //    {
        case Object::OBJECT_TYPE::ITEM: {
            uint32_t subtypeId = read_be_u32();
            pro->setObjectSubtypeId(subtypeId);

            read_be_u32(); //            _materialId    = stream.uint32();
            read_be_u32(); //            _containerSize = stream.uint32();
            read_be_u32(); //            _weight        = stream.uint32();
            read_be_u32(); //            _basePrice     = stream.uint32();
            read_be_u32(); //            _inventoryFID  = stream.int32();
            read_be_u8();  //            _soundId       = stream.uint8();

            switch ((Object::ITEM_TYPE)subtypeId) {
                case Object::ITEM_TYPE::ARMOR: {
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
                case Object::ITEM_TYPE::CONTAINER: {
                    read_be_u32(); // stream.uint32(); // max size
                    read_be_u32(); // stream.uint32(); // containter flags
                    break;
                }
                case Object::ITEM_TYPE::DRUG: {
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
                case Object::ITEM_TYPE::WEAPON:
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
                case Object::ITEM_TYPE::AMMO:
                    break;
                case Object::ITEM_TYPE::MISC:
                    break;
                case Object::ITEM_TYPE::KEY:
                    break;
            }
            break;
        }
        case Object::OBJECT_TYPE::CRITTER: {
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
        case Object::OBJECT_TYPE::SCENERY: {
            uint32_t subtypeId = read_be_u32(); // _subtypeId  = stream.uint32();
            pro->setObjectSubtypeId(subtypeId);

            read_be_u32(); //_materialId = stream.uint32();
            read_be_u8();  // _soundId    = stream.uint8();

            switch ((Object::SCENERY_TYPE)subtypeId) {
                case Object::SCENERY_TYPE::DOOR: {
                    read_be_u32(); // stream.uint32(); // walk thru flag
                    read_be_u32(); // stream.uint32(); // unknown
                    break;
                }
                case Object::SCENERY_TYPE::STAIRS: {
                    read_be_u32(); // stream.uint32(); // DestTile && DestElevation
                    read_be_u32(); // stream.uint32(); // DestElevation
                    break;
                }
                case Object::SCENERY_TYPE::ELEVATOR: {
                    read_be_u32(); // stream.uint32(); // Elevator type
                    read_be_u32(); // stream.uint32(); // Elevator level
                    break;
                }
                case Object::SCENERY_TYPE::LADDER_BOTTOM:
                case Object::SCENERY_TYPE::LADDER_TOP: {
                    read_be_u32(); // stream.uint32(); // DestTile && DestElevation
                    break;
                }
                case Object::SCENERY_TYPE::GENERIC: {
                    read_be_u32(); // stream.uint32(); // unknown
                    break;
                }
            }

            break;
        }
        case Object::OBJECT_TYPE::WALL: {
            read_be_u32(); //_materialId = stream.uint32();
            break;
        }
        case Object::OBJECT_TYPE::TILE: {
            read_be_u32(); //_materialId = stream.uint32();
            break;
        }
        case Object::OBJECT_TYPE::MISC: {
            read_be_u32(); // stream.uint32(); // unknown
            break;
        }
    }
    //    }
    return pro;
}

} // namespace geck
