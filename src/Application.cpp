#include "Application.h"

#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Window/Event.hpp>
#include <portable-file-dialogs.h>

#include "state/LoadingState.h"
#include "state/StateMachine.h"
#include "state/loader/MapLoader.h"
#include "ui/util.h"
#include "util/FileHelper.h"

namespace geck {

Application::Application(const std::filesystem::path& dataPath, const std::filesystem::path& mapPath)
    : _running(false),
      _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 960), "GECK::Mapper")),
      _stateMachine(std::make_shared<StateMachine>()),
      _appData(std::make_shared<AppData>(AppData{_window, _stateMachine, mapPath})) {

    sf::Image icon;
    icon.loadFromFile(FileHelper::getInstance().resourcesPath() / "icon.png");
    _window->setIcon(600, 600, icon.getPixelsPtr());

    initUI();

    loadMap();
}

void Application::loadMap() {

    auto loading_state = std::make_unique<LoadingState>(_appData);
    loading_state->addLoader(std::make_unique<MapLoader>(_appData->mapPath, 0)); // FIXME: default elevation

    _stateMachine->push(std::move(loading_state));
}

Application::~Application() {
    _window->close();
    ImGui::SFML::Shutdown();
}

void Application::initUI() {
    ImGui::SFML::Init(*_window, false);

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    io.Fonts->Clear();

    const std::filesystem::path resources_path = FileHelper::getInstance().resourcesPath();

    // default UI font
    std::filesystem::path main_font = resources_path / FONT_MAIN;
    io.Fonts->AddFontFromFileTTF(main_font.c_str(), 18.0f);

    // icon font - merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset = ImVec2(0, 1);

    std::filesystem::path icon_font = resources_path / FONT_ICON;
    io.Fonts->AddFontFromFileTTF(icon_font.c_str(), 16.0f, &icons_config, icons_ranges);

    ImGui::SFML::UpdateFontTexture();

    ImGui::SetupImGuiStyle(false, 1.f);
}

void Application::update(float dt) {
    sf::Event event;
    while (_window->pollEvent(event)) {
        if (!(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Unknown)) {
            ImGui::SFML::ProcessEvent(event); // FIXME: printscreen key crashes in an older version of SFML-IMGUI
        }

        if (!_stateMachine->empty()) {
            _stateMachine->top().handleEvent(event);
        }

        if (event.type == sf::Event::Closed) {
            _running = false;
        }
    }

    if (!_stateMachine->empty()) {
        _stateMachine->top().update(dt);

        if (_stateMachine->top().quit())
            _running = false;
    }

    ImGui::SFML::Update(*_window, _deltaClock.getElapsedTime());
}

/**
 * @brief Creates an invisible window where other IMGUI windows can be docked into
 */
void geck::Application::renderDockingUI()
{
    const bool enableDocking = ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable;
    if (enableDocking)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;
    //  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    //  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Window", nullptr, host_window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();
    }
}

void Application::render(float dt) {

    renderDockingUI(); // must be isDone before any other IMGUI rendering

    _window->clear(sf::Color::Black);

    if (!_stateMachine->empty()) {
        _stateMachine->top().render(dt);
    }

    ImGui::SFML::Render(*_window);

    _window->display();
}

void Application::run() {
    _running = true;

    float dt = 0.f;
    while (_running) {
        update(dt);
        render(dt);

        dt = _deltaClock.restart().asSeconds();
    }
}

bool Application::isRunning() const {
    return _running;
}

}  // namespace geck
