#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include <vfspp/VirtualFileSystem.hpp>

#include "format/IFile.h"
#include "format/pal/Pal.h"
#include "format/frm/Frm.h"
#include "format/pro/Pro.h"
#include "format/msg/Msg.h"

#include "reader/FileParser.h"
#include "reader/dat/DatReader.h"

namespace geck {

class ResourceManager {
private:
    vfspp::VirtualFileSystemPtr _vfs;

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;
    std::unordered_map<std::string, std::unique_ptr<IFile>> _resources;

    ResourceManager(); // private constructor for singleton

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
    Resource* loadResource(const std::filesystem::path& path, FileParser<Resource>& reader) {

        static_assert(std::is_base_of<IFile, Resource>::value, "Resource must derive from IFile");

        if (!exists(path.string())) {
            // vfspp adds / to the root by default
            std::filesystem::path vfsPath = "/" / path;
            vfspp::IFilePtr file = _vfs->OpenFile(vfspp::FileInfo(vfsPath.string()), vfspp::IFile::FileMode::Read);

            if (!file || !file->IsOpened()) {
                throw std::runtime_error{ "Failed to open file from VFS: " + vfsPath.string() };
            }
            std::vector<uint8_t> data(file->Size());
            file->Read(data, file->Size());
            _resources.emplace(path.string(), std::move(reader.openFile(path.string(), data)));
            _vfs->CloseFile(file);
        }

        return dynamic_cast<Resource*>(_resources.at(path.string()).get());
    }

    bool exists(const std::string& filename);
    void insertTexture(const std::string& filename);

    const sf::Texture& texture(const std::string& filename);

    std::string FIDtoFrmName(unsigned int FID);

    void addDataPath(const std::filesystem::path& path);
};

} // namespace geck
