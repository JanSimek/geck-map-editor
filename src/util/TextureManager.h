#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace geck {

class TextureManager {
private:
    std::string _dataPath = "";
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _resources;

    std::unique_ptr<sf::Texture> loadTextureFRM(const std::string& filename, uint32_t orientation = 0);

public:
    bool exists(const std::string& filename);
    void insert(const std::string& filename, uint32_t orientation = 0);
    const sf::Texture& get(const std::string& filename) const;

    void setDataPath(const std::string& path);
};

}  // namespace geck
