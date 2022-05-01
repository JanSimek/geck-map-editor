#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../format/IFile.h"
#include "../format/pal/Pal.h"
#include "../format/frm/Frm.h"

namespace geck {

class ResourceManager {
private:
    std::filesystem::path _dataPath;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;

    std::unordered_map<std::string, std::unique_ptr<IFile>> _resources;

    bool _initialized = false;
    std::unique_ptr<Pal> _pal;

    ResourceManager(){}; // private constructor for singleton

    const sf::Image imageFromFrm(Frm* frm, Pal* pal);

public:
    // Singleton
    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&) = delete;

    static ResourceManager& getInstance() {
        static ResourceManager instance; // Guaranteed to be destroyed.
        return instance;
    }

    template <class T>
    T* get(const std::string& filepath);
    void add(const std::string& filepath, std::unique_ptr<IFile> file);

    bool exists(const std::string& filename);
    void insert(const std::string& filename);

    const sf::Texture& texture(const std::string& filename);

    void setDataPath(const std::filesystem::path& path);
};

} // namespace geck
