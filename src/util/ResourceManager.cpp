#include "ResourceManager.h"

#include <filesystem>
#include <stdexcept>

#include "../reader/frm/FrmReader.h"
#include "../reader/pal/PalReader.h"
#include "../reader/lst/LstReader.h"

#include "../format/frm/Direction.h"
#include "../format/frm/Frame.h"
#include "../format/lst/Lst.h"
#include "../format/pro/Pro.h"

namespace geck {

bool ResourceManager::exists(const std::string& filename) {
    return _textures.find(filename) != _textures.end() || _resources.find(filename) != _resources.end();
}

void ResourceManager::insertTexture(const std::string& filename) {
    if (exists(filename)) {
        return;
    }

    // lowercase file extension
    std::string extension = [&]() {
        std::string s = std::filesystem::path(filename).extension().string();
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }();

    if (extension.rfind(".frm", 0) == 0) { // frm, frm0, frmX..

        FrmReader frm_reader;
        loadResource(filename, frm_reader);
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
        auto frm = getResource<Frm>(filename); // TODO: check extension?

        if (frm == nullptr) {
            throw std::runtime_error{ "Texture " + _dataPath.string() + "/" + filename + " does not exist" };
        }

        auto texture = std::make_unique<sf::Texture>();
        //        texture->create(frame.width(), frame.height());
        //        texture->update(&frame.rgba[

        PalReader pal_reader;
        auto pal = loadResource("color.pal", pal_reader); // TODO: custom pal0]);

        texture->loadFromImage(imageFromFrm(frm, pal));

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

template <class T, typename Key>
T* ResourceManager::getResource(const Key& id) {

    static_assert(std::is_base_of<IFile, T>::value, "T must derive from IFile");

    auto itemIt = _resources.find(id);
    if (itemIt != _resources.end()) {
        auto resource = dynamic_cast<T*>(itemIt->second.get());
        if (resource == nullptr) {
            throw std::runtime_error{ "Requested file type does not match type in the cache: " + id };
        }
        return resource;
    }

    // TODO: load on-demand
    spdlog::error("Resource {} not found in resource cache", id);

    return nullptr;
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
std::string ResourceManager::FIDtoFrmName(unsigned int FID) {

    /*const*/ auto baseId = FID & 0x00FFFFFF; // FIXME? 0x00000FFF;
    /*const*/ auto type = static_cast<Frm::FRM_TYPE>(FID >> 24);

    if (type == Frm::FRM_TYPE::CRITTER) {
        baseId = FID & 0x00000FFF;
        type = static_cast<Frm::FRM_TYPE>((FID & 0x0F000000) >> 24); // FIXME: WTF?
    }

    // TODO: EditorState::_showScrlBlk
    if (type == Frm::FRM_TYPE::MISC && baseId == 1) {
        static const std::string SCROLL_BLOCKERS_PATH("art/misc/scrblk.frm");
        // Map scroll blockers
        return SCROLL_BLOCKERS_PATH;
    }

    if (type > Frm::FRM_TYPE::INVENTORY) {
        throw std::runtime_error{ "Invalid FRM_TYPE" };
    }

    // TODO: art/$/
    static struct TypeArtListDecription {
        const std::string prefixPath;
        const std::string lstFilePath;
    } const frmTypeDescription[] = {
        { "art/items/", "art/items/items.lst" },
        { "art/critters/", "art/critters/critters.lst" },
        { "art/scenery/", "art/scenery/scenery.lst" },
        { "art/walls/", "art/walls/walls.lst" },
        { "art/tiles/", "art/tiles/tiles.lst" },
        { "art/misc/", "art/misc/misc.lst" },
        { "art/intrface/", "art/intrface/intrface.lst" },
        { "art/inven/", "art/inven/inven.lst" },
    };

    const auto& typeArtDescription = frmTypeDescription[static_cast<size_t>(type)];

    const auto& lst = getResource<Lst>(typeArtDescription.lstFilePath);

    if (baseId >= lst->list().size()) {
        throw std::runtime_error{ "LST " + typeArtDescription.lstFilePath + " size " + std::to_string(lst->list().size()) + " <= frmID: " + std::to_string(baseId) + ", frmType: " + std::to_string((unsigned)type) };
    }

    std::string frm_name = lst->list().at(baseId);

    if (type == Frm::FRM_TYPE::CRITTER) {
        return typeArtDescription.prefixPath + frm_name.substr(0, 6) + Frm::STANDING_ANIMATION_SUFFIX;
    }
    return typeArtDescription.prefixPath + frm_name;
}

} // namespace geck
