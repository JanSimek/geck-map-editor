#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <SFML/Graphics.hpp>

namespace geck {

class TextureManager
{
private:
    std::string _dataPath = "";
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _resources;

    std::unique_ptr<sf::Texture> loadTextureFRM(const std::string& filename);

public:

    bool exists(const std::string& filename);
    void insert(const std::string& filename);
    const sf::Texture& get(const std::string &filename) const;
//    Resource& get(Identifier id);

    void setDataPath(const std::string& path);

};

}
