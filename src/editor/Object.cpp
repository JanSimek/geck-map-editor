#include "Object.h"

namespace geck {

const MapObject& Object::getMapObject() const
{
    return *mapObject; //.get();
}

void Object::setMapObject(std::shared_ptr<MapObject> newMapObject)
{
    mapObject = newMapObject;
}

void Object::setSprite(sf::Sprite sprite)
{
    _sprite = sprite;
}

const sf::Sprite &Object::getSprite() const
{
    return _sprite;
}

}  // namespace geck
