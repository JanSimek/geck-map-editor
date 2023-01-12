#pragma once

#include <memory>
#include <stack>
#include <filesystem>
#include <atomic>
#include <shared_mutex>

#include <SFML/Graphics/RenderWindow.hpp>
#include "ui/IconsFontAwesome6.h"

namespace geck {

class StateMachine;

struct AppData {
    std::shared_ptr<sf::RenderWindow> window;
    std::shared_ptr<StateMachine> stateMachine;
};

class Application {
public:
    inline static const std::string FONT_DIR = "fonts/";
    inline static const std::string FONT_MAIN = FONT_DIR + "SourceSansPro-SemiBold.ttf";
    inline static const std::string FONT_ICON = FONT_DIR + FONT_ICON_FILE_NAME_FAS;

    Application(const std::filesystem::path& dataPath, const std::filesystem::path& mapPath);
    ~Application();

    bool isRunning() const;

    void run();
    void update(float dt);
    void render(float dt);

private:
    void initUI();

    bool _running;
    sf::Clock _deltaClock;

    std::shared_ptr<sf::RenderWindow> _window;
    std::shared_ptr<StateMachine> _stateMachine;
    std::shared_ptr<AppData> _appData;

    void renderDockingUI();

    void loadMap(const std::filesystem::path& mapPath);
};

} // namespace geck
