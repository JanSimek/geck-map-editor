#include "TextureManager.h"

#include <filesystem>
#include <stdexcept>

#include "../reader/frm/FrmReader.h"
#include "../reader/pal/PalReader.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"
#include "../format/frm/Frm.h"
//#include "../format/pal/Pal.h"

namespace geck {

bool TextureManager::exists(const std::string& filename) {
    return _resources.find(filename) != _resources.end() || _imagesToLoad.find(filename) != _imagesToLoad.end();
}

void TextureManager::insert(const std::string& filename, uint32_t orientation) {
    if (exists(_dataPath / filename)) {
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
        loadTextureFRM(_dataPath / filename, orientation);
    } else {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(_dataPath / filename)) {  // default to SFML's implementation
            throw std::runtime_error{"Failed to load texture " + _dataPath.string() + "/" + filename + ", extension: " + extension};
        }
        _resources.insert(std::make_pair(_dataPath / filename, std::move(texture)));
    }
}

const sf::Texture& TextureManager::get(const std::string& filename) {
    auto found = _resources.find(_dataPath / filename);

    if (found == _resources.end()) {
        auto image = _imagesToLoad.find(_dataPath / filename);
        if (image == _imagesToLoad.end()) {
            throw std::runtime_error{"Texture " + _dataPath.string() + "/" + filename + " does not exist"};
        }

        auto texture = std::make_unique<sf::Texture>();
        //    texture->create(frame.width(), frame.height());
        //    texture->update(&pixels[0]);

        texture->loadFromImage(image->second);

        auto loaded = _resources.insert(std::make_pair(_dataPath / filename, std::move(texture)));

        if (loaded.second) {
            return *loaded.first->second; // uh huh
        } else {
            throw std::runtime_error{"Couldn't load " + _dataPath.string() + "/" + filename + " from image"};
        }
    } else {
        // TODO
    }

    return *found->second;
}

void TextureManager::setDataPath(const std::string& path) {
    _dataPath = path;
}

void TextureManager::loadTextureFRM(const std::string& filename, uint32_t orientation) {
    FrmReader frm_reader;
    auto frm = frm_reader.openFile(filename);

    int totalOrientations = frm->orientations().size() - 1;

    // FIXME: block.frm on cave6.map
    if (orientation > totalOrientations) {
        spdlog::error("Cannot load orientation {} because FRM {} has only {} orientations. Using orientation 0 instead.", orientation, filename, totalOrientations);
        orientation = 0;
    }

    // FIXME: using just the first frame for now
    geck::Frame frame = frm->orientations().at(orientation).frames().front();

    const auto& colors = _pal->palette();
    const auto& colorIndexes = frame.data();

    constexpr int RGBA = 4;  // RGBA of SFML texture
    size_t pixelCount = frame.width() * frame.height() * RGBA;
    sf::Uint8 pixels[pixelCount];

    for (size_t i = 0; i < pixelCount; i += RGBA) {
        uint8_t paletteIndex = colorIndexes[i / RGBA];

        geck::Rgb color = colors[paletteIndex];
        constexpr uint8_t white = 255;
        constexpr uint8_t opaque_alpha = 255;

        if (color.r == white && color.g == white && color.b == white) {
            // transparent
            pixels[i] = 0;
            pixels[i + 1] = 0;
            pixels[i + 2] = 0;
            pixels[i + 3] = 0;
        } else {
            constexpr int brightness = 4;  // brightness modifier
            pixels[i] = color.r * brightness;
            pixels[i + 1] = color.g * brightness;
            pixels[i + 2] = color.b * brightness;
            pixels[i + 3] = opaque_alpha;
        }
    }

    sf::Image image{};
    image.create(frame.width(), frame.height(), pixels);

    _imagesToLoad.insert(std::make_pair(_dataPath / filename, std::move(image)));
}

}  // namespace geck
