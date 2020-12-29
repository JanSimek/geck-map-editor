#pragma once

#include <stack>
#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include "ui/IconsFontAwesome5.h"

namespace geck {

class StateMachine;

struct AppData {
    std::shared_ptr<sf::RenderWindow> window;
    std::shared_ptr<StateMachine> stateMachine;
    std::string dataPath;
    std::string mapName;
};

class Application {
private:
    const std::string FONT_DIR = "fonts/";
    const std::string FONT_MAIN = FONT_DIR + "SourceSansPro-SemiBold.ttf";
    const std::string FONT_ICON = FONT_DIR + FONT_ICON_FILE_NAME_FAR;

    void initUI();

    bool _running;
	sf::Clock _deltaClock;

    std::shared_ptr<sf::RenderWindow> _window;
    std::shared_ptr<StateMachine> _stateMachine;
    std::shared_ptr<AppData> _appData;
public:
    Application(std::string dataPath, std::string mapName);
    ~Application();

    bool isRunning() const;

    void run();
    void update(float dt);
    void render(float dt);
};

}
