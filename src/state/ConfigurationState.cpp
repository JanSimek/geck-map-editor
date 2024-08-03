#include "ConfigurationState.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>

namespace geck {

ConfigurationState::ConfigurationState() {
}

void ConfigurationState::init() {
}

void ConfigurationState::handleEvent(const sf::Event& event) {
}

void ConfigurationState::update(const float dt) {
}

void ConfigurationState::render(const float dt) {
    ImGui::Begin("Configuration");

    std::string data_path = "";
    ImGui::InputText("Fallout 2 installation directory", &data_path);

    ImGui::End();
}

} // namespace geck
