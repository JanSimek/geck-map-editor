#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../format/pal/Pal.h"

namespace geck {

class TextureManager {
private:
    std::filesystem::path _dataPath;
    std::unordered_map<std::string, sf::Image> _imagesToLoad;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _resources;

    void loadTextureFRM(const std::string& filename, uint32_t orientation = 0);

    TextureManager() {};

    bool _initialized = false;
    std::unique_ptr<Pal> _pal;
public:
    // Singleton
    TextureManager(TextureManager const&) = delete;
    void operator=(TextureManager const&) = delete;

    static TextureManager& getInstance() {
        static TextureManager instance;  // Guaranteed to be destroyed.
        return instance;
    }

    bool exists(const std::string& filename);
    void insert(const std::string& filename, uint32_t orientation = 0);
    const sf::Texture& get(const std::string& filename);

    void setDataPath(const std::string& path);
};

}  // namespace geck
