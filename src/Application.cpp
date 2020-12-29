#include "Application.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "ui/util.h"
#include "state/StateMachine.h"
#include "state/EditorState.h"

namespace geck {

Application::Application(std::string dataPath, std::string mapName)
    : _running(false), 
    _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 960), "GECK::Mapper")),
    _stateMachine(std::make_shared<StateMachine>()),
    _appData(std::make_shared<AppData>(AppData{_window, _stateMachine, dataPath, mapName }))
{
    initUI();

    _stateMachine->push(std::make_unique<EditorState>(_appData));
}

Application::~Application()
{
    _window->close();
    ImGui::SFML::Shutdown();
}

void Application::initUI()
{
    ImGui::SFML::Init(*_window, false);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    // default UI font
    std::string main_font = _appData->dataPath + FONT_MAIN;
    io.Fonts->AddFontFromFileTTF(main_font.c_str(), 18.0f);

    // icon font - merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset = ImVec2(0,1);

    std::string icon_font = _appData->dataPath + FONT_ICON;
    io.Fonts->AddFontFromFileTTF(icon_font.c_str(), 16.0f, &icons_config, icons_ranges );

    ImGui::SFML::UpdateFontTexture();

    ImGui::SetupImGuiStyle(false, 1.f);
}

void Application::update(float dt)
{
    sf::Event event;
    while (_window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);

        if (!_stateMachine->empty()) {
            _stateMachine->top().handleEvent(event);
        }

        if (event.type == sf::Event::Closed) {
            _running = false;
        }
    }

    if (!_stateMachine->empty()) {
        _stateMachine->top().update(dt);

        if(_stateMachine->top().quit())
            _running = false;
    }

    ImGui::SFML::Update(*_window, _deltaClock.getElapsedTime());
}

void Application::render(float dt)
{
    _window->clear(sf::Color::Black);

    if (!_stateMachine->empty()) {
        _stateMachine->top().render(dt);
    }

    ImGui::SFML::Render(*_window);
    
    _window->display();
}

void Application::run()
{
    _running = true;

    float dt = 0.f;
    while (_running) {

        update(dt);
        render(dt);

        dt = _deltaClock.restart().asSeconds();
    }
}

bool Application::isRunning() const
{
    return _running;
}

}
