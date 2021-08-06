#pragma once
#include <chrono>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "terrgen.h"

class application
{
	terrgen* _terrgen;

	std::string _title;
	sf::RenderWindow* _window;

	uint32_t _cell_dim;
	sf::RectangleShape _cell;
	void draw(uint32_t x, uint32_t y);

	uint64_t _time;
	void update(int phases);
	void handle_events();
	void render();

public:
	application(terrgen* terrgen, const std::string& title = "", uint32_t dimension = 10u, uint32_t framerate_limit = 16u);
	~application();

	void run();
};
