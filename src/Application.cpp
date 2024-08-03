#include "Application.h"

#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Window/Event.hpp>
#include <portable-file-dialogs.h>

#include "imgui_internal.h"
#include "state/EditorState.h"
#include "state/LoadingState.h"
#include "state/StateMachine.h"
#include "state/loader/MapLoader.h"
#include "util/ResourceManager.h"
#include "ui/util.h"

namespace geck {

Application::Application(const std::filesystem::path& resourcePath, const std::filesystem::path& mapPath)
    : _running(false)
    , _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 960), "Gecko"))
    , _stateMachine(std::make_shared<StateMachine>())
    , _appData(std::make_shared<AppData>(AppData{ _window, _stateMachine })) {

    _window->setVerticalSyncEnabled(true);

    ResourceManager::getInstance().addDataPath(resourcePath);

    //sf::Image icon;
    // VSF -> loadFromMemory icon.loadFromFile(data_path / "icon.png");
    //_window->setIcon(600, 600, icon.getPixelsPtr());

    initUI();

    // TODO: show configuration window if no map is selected
    loadMap(mapPath);
}

void Application::loadMap(const std::filesystem::path& mapPath) {

    auto loading_state = std::make_unique<LoadingState>(_appData);
    loading_state->addLoader(std::make_unique<MapLoader>(mapPath, -1, [this](auto map) {
        _appData->stateMachine->push(std::make_unique<EditorState>(_appData, std::move(map)), true);
    }));

    _stateMachine->push(std::move(loading_state));
}

Application::~Application() {
    _window->close();
    ImGui::SFML::Shutdown();
}

void Application::initUI() {
    if (!ImGui::SFML::Init(*_window)) {
        throw std::runtime_error{ "Error initializing SFML-ImGui" };
    }

    constexpr auto scale_factor = 1.0f; // default

    ImGui::GetStyle().ScaleAllSizes(scale_factor);

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale_factor;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
/*
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
*/
    io.Fonts->Clear();

    constexpr float font_size = 20.0f;
    constexpr float icon_size = 16.0f;

    // icon font - merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset = ImVec2(0, 1);
    icons_config.GlyphMinAdvanceX = 13.0f; // to make the icon monospaced

    // default UI font
    std::filesystem::path main_font = RESOURCES_DIR / FONT_MAIN;
    io.Fonts->AddFontFromFileTTF(main_font.string().c_str(), font_size);
    // TODO: VFS io.Fonts->AddFontFromMemoryTTF();

    std::filesystem::path icon_font = RESOURCES_DIR / FONT_ICON;
    io.Fonts->AddFontFromFileTTF(icon_font.string().c_str(), icon_size, &icons_config, icons_ranges);
    // TODO: VFS io.Fonts->AddFontFromMemoryTTF();

    io.Fonts->Build();

    if (!ImGui::SFML::UpdateFontTexture()) {
        spdlog::error("Unable to load custom ImGui font");
    }

    ImGui::SetupImGuiStyle();
}

void Application::update(float dt) {
    sf::Event event{};
    while (_window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*_window, event);

        // don't pass mouse and keyboard presses to states when an ImGui widget is active
        auto& io = ImGui::GetIO();
        if ((io.WantCaptureMouse && (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseWheelScrolled))
            || (io.WantCaptureKeyboard && event.type == sf::Event::KeyPressed)) {
            continue;
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

        if (!_stateMachine->top().isRunning())
            _running = false;
    } else {
        _running = false;
    }

    ImGui::SFML::Update(*_window, _deltaClock.getElapsedTime());
}

/**
 * @brief Creates an invisible window where other IMGUI windows can be docked into
 */
void geck::Application::renderDockingUI() {
    const bool enableDocking = ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable;
    if (enableDocking) {
        ImGuiDockNodeFlags window_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), window_flags);
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

} // namespace geck
