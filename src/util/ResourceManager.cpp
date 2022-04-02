#include "ResourceManager.h"

#include <filesystem>
#include <stdexcept>

#include "../reader/frm/FrmReader.h"
#include "../reader/pal/PalReader.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"
#include "../format/frm/Frm.h"

namespace geck {

bool ResourceManager::exists(const std::string& filename) {
    return _textures.find(filename) != _textures.end() || _resources.find(filename) != _resources.end();
}

void ResourceManager::insert(const std::string& filename) {
    if (exists(filename)) {
        return;
    }

    // lowercase file extension
    std::string extension = [&]() -> std::string {
        std::string s = std::filesystem::path(filename).extension();
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }();

    if (extension.rfind(".frm", 0) == 0) {  // frm, frm0, frmX..
        if (!_initialized) {
            PalReader pal_reader;
            _pal = pal_reader.openFile(_dataPath / "color.pal");  // TODO: custom .pal
            _initialized = true;
        }
        FrmReader frm_reader;
        auto frm = frm_reader.openFile(_dataPath / filename);

        add(filename, std::move(frm));
    } else {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(_dataPath / filename)) {  // default to SFML's implementation
            throw std::runtime_error{"Failed to load texture " + _dataPath.string() + "/" + filename + ", extension: " + extension};
        }
        _textures.insert(std::make_pair(filename, std::move(texture)));
    }
}

const sf::Texture& ResourceManager::texture(const std::string& filename) {
    auto found = _textures.find(filename);

    if (found == _textures.end()) {
        auto frm = get<Frm>(filename); // TODO: check extension?

        if (frm == nullptr) {
            throw std::runtime_error{"Texture " + _dataPath.string() + "/" + filename + " does not exist"};
        }

        auto texture = std::make_unique<sf::Texture>();
//        texture->create(frame.width(), frame.height());
//        texture->update(&frame.rgba[0]);

        texture->loadFromImage(frm->image(_pal.get()));

        auto loaded = _textures.insert(std::make_pair(filename, std::move(texture)));

        if (loaded.second) {
            return *loaded.first->second; // uh huh
        } else {
            throw std::runtime_error{"Couldn't load " + _dataPath.string() + "/" + filename + " from image"};
        }
    }

    return *found->second;
}

void ResourceManager::setDataPath(const std::string& path) {
    _dataPath = path;
}

template<class T>
T* ResourceManager::get(const std::string &filepath)
{
    auto itemIt = _resources.find(filepath);
    if (itemIt != _resources.end()) {
        auto resource = dynamic_cast<T *>(itemIt->second.get());
        if (resource == nullptr) {
            throw std::runtime_error{"Requested file type does not match type in the cache: " + filepath};
        }
        return resource;
    }

    // TODO: load on-demand
    spdlog::error("Resource {} not found in resource cache", filepath);

    return nullptr;
}

void ResourceManager::add(const std::string &filepath, std::unique_ptr<IFile> file)
{
    _resources.insert({ filepath, std::move(file) });
}

}  // namespace geck
