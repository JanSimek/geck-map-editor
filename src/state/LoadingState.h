#pragma once

#include "../Application.h"
#include "State.h"
#include "loader/Loader.h"

#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <memory>

namespace geck {

class LoadingState : public State {
private:
    std::shared_ptr<AppData> _appData;

    sf::Clock clock; // starts the clock

    sf::Font font;
    sf::Text progressText;
    sf::Text loadingText;

    std::vector<std::unique_ptr<Loader>> _loaders;

public:
    LoadingState(const std::shared_ptr<AppData>& appData)
        : _appData(appData) {

        const std::filesystem::path font_path = Application::RESOURCES_DIR / Application::FONT_MAIN;

        if (!font.loadFromFile(font_path.string())) {
            spdlog::error("Could not open font {}", font_path.string());
            return;
        }

        loadingText.setFont(font);
        loadingText.setString("Loading");
        loadingText.setCharacterSize(32); // in pixels, not points!
        loadingText.setFillColor(sf::Color::White);
        loadingText.setStyle(sf::Text::Bold);

        progressText.setFont(font);
        progressText.setString("Loading");
        progressText.setCharacterSize(24); // in pixels, not points!
        progressText.setFillColor(sf::Color::White);
    }

    void addLoader(std::unique_ptr<Loader> loader) {
        _loaders.emplace_back(std::move(loader));
    }

    void init() override {
        clock.restart();

        for (const auto& loader : _loaders) {
            loader->init();
        }
    }

    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Q: // Ctrl+Q
                    if (event.key.control)
                        quit();
                    break;
                case sf::Keyboard::Escape:
                    quit();
                    break;
                default:
                    break;
            }
        }
    }

    void update(const float dt) override {

        for (const auto& loader : _loaders) {

            if (!loader->isDone()) {
                loadingText.setString(loader->status());
                centerText(loadingText, -loadingText.getLocalBounds().height * 2);

                progressText.setString(loader->progress());
                centerText(progressText);
            } else {
                loader->onDone();
            }
        }
    }

    void centerText(sf::Text& text, float yOffset = 0) {
        float centerX = _appData->window->getView().getCenter().x - text.getGlobalBounds().width / 2;
        float centerY = _appData->window->getView().getCenter().y + yOffset;
        text.setPosition(sf::Vector2f(centerX, centerY));
    }

    void render(const float dt) override {
        _appData->window->clear(sf::Color::Black);

        _appData->window->draw(loadingText);
        _appData->window->draw(progressText);
    }

    //    bool isRunning() const override {
    //        return _loaders.empty() || _quit;
    //    }
};

} // namespace geck
