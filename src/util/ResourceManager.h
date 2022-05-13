#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../format/IFile.h"
#include "../format/pal/Pal.h"
#include "../format/frm/Frm.h"
#include "../format/pro/Pro.h"
#include "../format/msg/Msg.h"

#include "reader/FileParser.h"

namespace geck {

class ResourceManager {
private:
    std::filesystem::path _dataPath;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;
    std::unordered_map<std::string, std::unique_ptr<IFile>> _resources;

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

    template <class T, typename Key>
    T* getResource(const Key& filepath);

    template <class Resource>
    Resource* loadResource(const std::filesystem::path& basePath, FileParser<Resource>& reader) {

        static_assert(std::is_base_of<IFile, Resource>::value, "Resource must derive from IFile");

        if (!exists(basePath)) {
            _resources.emplace(basePath, std::move(reader.openFile(_dataPath / basePath)));
        }

        return dynamic_cast<Resource*>(_resources.at(basePath).get());
    }

    bool exists(const std::string& filename);
    void insertTexture(const std::string& filename);

    const sf::Texture& texture(const std::string& filename);

    std::string FIDtoFrmName(unsigned int FID);

    void setDataPath(const std::filesystem::path& path);
};

} // namespace geck
