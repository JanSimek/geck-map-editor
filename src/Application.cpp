#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "Application.h"
#include "state/EditorState.h"

Application::Application() 
    : _running(false), 
    _window(sf::VideoMode(1280, 960), "GECK - Fallout 2 map editor")
{
    _window.setFramerateLimit(60);
    ImGui::SFML::Init(_window);

    _states.push(std::make_unique<EditorState>());
}

Application::~Application()
{
    _window.close();
    ImGui::SFML::Shutdown();
}

bool Application::isRunning() const
{
	return _running;
}

void Application::update(float dt)
{
    sf::Event event;
    while (_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);

        if (event.type == sf::Event::Closed) {
            _running = false;
        }
    }

    if (!_states.empty()) {
        _states.top()->update(dt);
    }

    ImGui::SFML::Update(_window, _deltaClock.getElapsedTime());
}

void Application::render(float dt)
{
    _window.clear();

    if (!_states.empty()) {
        _states.top()->render(dt);
    }

    ImGui::SFML::Render(_window);
    
    _window.display();
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
