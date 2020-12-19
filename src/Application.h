#pragma once

#include <stack>
#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>

class State;

class Application {
private:
	bool _running;
	sf::Clock _deltaClock;

	sf::RenderWindow _window;

	std::stack<std::unique_ptr<State> > _states;

public:
	Application();
	~Application();

	bool isRunning() const;

	void run();
	void update(float dt);
	void render(float dt);
};