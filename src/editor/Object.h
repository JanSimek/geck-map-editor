#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

namespace geck {

struct MapObject;

class Object {
private:
    // TODO: not here -> component
    std::string _frm;

    sf::Sprite _sprite;

    std::shared_ptr<MapObject> mapObject;

public:
    const MapObject& getMapObject() const;
    void setMapObject(std::shared_ptr<MapObject> newMapObject);

    void setSprite(sf::Sprite sprite);
    const sf::Sprite& getSprite() const;
};

} // namespace geck
