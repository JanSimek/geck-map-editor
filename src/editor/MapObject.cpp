#include "MapObject.h"

namespace geck {

size_t MapObject::amount() const
{
    return _amount;
}

void MapObject::setAmount(const size_t &amount)
{
    _amount = amount;
}

void MapObject::setInventory_size(const size_t &inventory_size)
{
    _inventory_size = inventory_size;
}

uint32_t MapObject::PID() const
{
    return _PID;
}

void MapObject::setPID(const uint32_t &PID)
{
    _PID = PID;
}

unsigned int MapObject::objectSubtypeId() const
{
    return _objectSubtypeId;
}

void MapObject::setObjectSubtypeId(unsigned int objectSubtypeId)
{
    _objectSubtypeId = objectSubtypeId;
}

int32_t MapObject::hexPosition() const
{
    return _hexPosition;
}

void MapObject::setHexPosition(const int32_t &hexPosition)
{
    _hexPosition = hexPosition;
}

std::string MapObject::frm() const
{
    return _frm;
}

void MapObject::setFrm(const std::string &frm)
{
    _frm = frm;
}

uint32_t MapObject::FID() const
{
    return _FID;
}

void MapObject::setFID(const uint32_t &FID)
{
    _FID = FID;
}

size_t MapObject::inventory_size() const
{
    return _inventory_size;
}

}
