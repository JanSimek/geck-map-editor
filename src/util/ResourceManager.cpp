#include "ResourceManager.h"

#include <filesystem>
#include <stdexcept>

#include "../reader/frm/FrmReader.h"
#include "../reader/pal/PalReader.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"

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
        std::string s = std::filesystem::path(filename).extension().string();
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }();

    if (extension.rfind(".frm", 0) == 0) { // frm, frm0, frmX..
        if (!_initialized) {
            PalReader pal_reader;
            _pal = pal_reader.openFile((_dataPath / "color.pal").string()); // TODO: custom .pal
            _initialized = true;
        }
        FrmReader frm_reader;
        auto frm = frm_reader.openFile((_dataPath / filename).string());

        add(filename, std::move(frm));
    } else {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile((_dataPath / filename).string())) { // default to SFML's implementation
            throw std::runtime_error{ "Failed to load texture " + _dataPath.string() + "/" + filename + ", extension: " + extension };
        }
        _textures.insert(std::make_pair(filename, std::move(texture)));
    }
}

const sf::Texture& ResourceManager::texture(const std::string& filename) {
    const auto& found = _textures.find(filename);

    if (found == _textures.end()) {
        auto frm = get<Frm>(filename); // TODO: check extension?

        if (frm == nullptr) {
            throw std::runtime_error{ "Texture " + _dataPath.string() + "/" + filename + " does not exist" };
        }

        auto texture = std::make_unique<sf::Texture>();
        //        texture->create(frame.width(), frame.height());
        //        texture->update(&frame.rgba[0]);

        texture->loadFromImage(imageFromFrm(frm, _pal.get()));

        auto loaded = _textures.insert(std::make_pair(filename, std::move(texture)));

        if (loaded.second) {
            return *loaded.first->second; // uh huh
        } else {
            throw std::runtime_error{ "Couldn't load " + _dataPath.string() + "/" + filename + " from image" };
        }
    }

    return *found->second;
}

void ResourceManager::setDataPath(const std::filesystem::path& path) {
    _dataPath = path;
}

template <class T>
T* ResourceManager::get(const std::string& filepath) {
    auto itemIt = _resources.find(filepath);
    if (itemIt != _resources.end()) {
        auto resource = dynamic_cast<T*>(itemIt->second.get());
        if (resource == nullptr) {
            throw std::runtime_error{ "Requested file type does not match type in the cache: " + filepath };
        }
        return resource;
    }

    // TODO: load on-demand
    spdlog::error("Resource {} not found in resource cache", filepath);

    return nullptr;
}

void ResourceManager::add(const std::string& filepath, std::unique_ptr<IFile> file) {
    _resources.insert({ filepath, std::move(file) });
}

const sf::Image ResourceManager::imageFromFrm(Frm* frm, Pal* pal) {

    spdlog::debug("Stitching {} texture from {} directions", frm->filename(), frm->directions().size());

    auto colors = pal->palette();

    // find maximum width and height
    unsigned maxWidth = frm->maxFrameWidth();
    unsigned maxHeight = frm->maxFrameHeight();

    sf::Image image{};
    image.create(frm->width(), frm->height(), { 0, 0, 0, 0 });

    int yOffset = 0;
    for (const auto& direction : frm->directions()) {
        int xOffset = 0;

        for (const Frame& frame : direction.frames()) {

            for (int x = 0; x < frame.width(); x++) {
                for (int y = 0; y < frame.height(); y++) {

                    uint8_t paletteIndex = frame.index(x, y);
                    geck::Rgb color = colors[paletteIndex];

                    constexpr uint8_t white = 255;
                    constexpr uint8_t opaque_alpha = 255;

                    uint8_t r, g, b, a;
                    if (color.r == white && color.g == white && color.b == white) {
                        // transparent
                        r = 0;
                        g = 0;
                        b = 0;
                        a = 0;
                    } else {
                        constexpr int brightness = 4; // brightness modifier
                        r = color.r * brightness;
                        g = color.g * brightness;
                        b = color.b * brightness;
                        a = opaque_alpha;
                    }

                    image.setPixel(
                        maxWidth * xOffset + x,
                        maxHeight * yOffset + y,
                        { r, g, b, a });
                }
            }
            xOffset++;
        }
        yOffset++;
    }

    return image;
}

} // namespace geck
