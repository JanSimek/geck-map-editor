#include "TileSelectionPanel.h"

#include <imgui-SFML.h>

#include "format/lst/Lst.h"
#include "format/map/Tile.h"
#include "ui/util.h"
#include "util/ResourceManager.h"

namespace geck {

TileSelectionPanel::TileSelectionPanel() {
    _title = "Tiles";

    const auto& lst = ResourceManager::getInstance().getResource<Lst, std::string>("art/tiles/tiles.lst");

    // selectable tiles

    // TODO: auto& tile : lst->list()
    for (int tile_id = 0; tile_id < lst->list().size(); tile_id++) {
        sf::Sprite tile_sprite;
        std::string texture_path = "art/tiles/" + lst->at(tile_id);
        tile_sprite.setTexture(ResourceManager::getInstance().texture(texture_path));
        _selectableTileSprites.push_back(tile_sprite);
    }
}

void TileSelectionPanel::render(float dt) {
    ImGui::Begin(ICON_FA_MAP " Tiles");

    const auto& lst = ResourceManager::getInstance().getResource<Lst, std::string>("art/tiles/tiles.lst");

    float content_width = ImGui::GetContentRegionAvail().x;
    const float item_width = Tile::TILE_WIDTH;
    int columns = content_width / item_width;
    if (columns == 0) {
        columns = 1;
    }
    ImGui::Columns(columns, nullptr, false);

    for (int tileIndex = 0; const auto& tile : _selectableTileSprites) {

        // hide reserved.frm and grid000.frm
        if (tileIndex < 2) {
            ++tileIndex;
            continue;
        }

        sf::Vector2 tileSize = tile.getTexture()->getSize();
        if (ImGui::ImageButton(lst->at(tileIndex).c_str(), tile, { (float) tileSize.x, (float) tileSize.y })) {
            tileClicked.emit(tileIndex);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Tile #: %d", tileIndex);
            ImGui::Text("File  : %s", lst->at(tileIndex).c_str());
            ImGui::EndTooltip();
        }
        ImGui::NextColumn();
        ++tileIndex;
    }

    ImGui::End();
}

} // namespace geck
