#include "application.h"

void application::draw(uint32_t x, uint32_t y)
{
	_cell.setPosition({ (float)(x * _cell_dim), (float)(y * _cell_dim) });
	_cell.setFillColor(_terrgen->get(x, y));
	_window->draw(_cell);
}

void application::update(int phases)
{
	std::chrono::system_clock clock;
	auto then = clock.now();
	_terrgen->generate(phases);
	_time = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - then).count();
}

void application::handle_events()
{
	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window->close();

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Num1:
				update(1);
				break;
			case sf::Keyboard::Num2:
				update(2);
				break;
			case sf::Keyboard::Num3:
				update(3);
				break;
			case sf::Keyboard::Num4:
				update(4);
				break;
			case sf::Keyboard::Escape:
				_window->close();
				break;
			}
		}
	}
}

void application::render()
{
	_window->clear(sf::Color::Black);
	for (int x = 0; x < _terrgen->width(); x++)
		for (int y = 0; y < _terrgen->height(); y++)
			draw(x, y);
	_window->display();
}

application::application(terrgen* terrgen, const std::string& title, uint32_t dimension)
	: _terrgen(terrgen), _title(title)
{
	_cell_dim = dimension;
	auto width = _terrgen->width() * _cell_dim;
	auto height = _terrgen->height() * _cell_dim;

	_window = new sf::RenderWindow(sf::VideoMode(width, height), _title);
	_window->setVerticalSyncEnabled(true);

	_cell = sf::RectangleShape({ (float)_cell_dim, (float)_cell_dim });
	//_cell.setOutlineColor(sf::Color::Black);
	//_cell.setOutlineThickness(-1.f);
}

application::~application()
{
	delete _window;
}

void application::run()
{
	while (_window->isOpen())
	{
		std::chrono::system_clock clock;
		auto then = clock.now();
	
		handle_events();
		render();

		uint64_t fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - then).count();
		std::string log = "[FPS: " + std::to_string(fps)
			+ "] [Time: " + std::to_string(_time)
			+ "] [Iterations: " + std::to_string(_terrgen->iterations())
			+ "] [Phases: " + std::to_string(_terrgen->phases())
			+ "] [Seed: " + std::to_string(_terrgen->seed()) + "]";
		_window->setTitle(_title + " " + log);
	}
}

/*switch (_light)
	{
	case 0:
		light = { 0xff, 0xda, 0xb9 };
		break;
	case 1:
		light = { 0xbc, 0xff, 0xb9 };
		break;
	case 2:
		light = { 0x96, 0xba, 0xff };
		break;
	case 3:
		light = { 0xd9, 0xa5, 0xb3 };
		break;
	case 4:
		light = { 0xed, 0x8e, 0x7c };
		break;
	case 5:
		light = { 0xe1, 0xe8, 0xeb };
		break;
	case 6:
		light = { 0xec, 0xa3, 0xf5 };
		break;
	case 7:
		light = { 0xd7, 0x97, 0x71 };
		break;
	}

	switch (_dark)
	{
	case 0:
		dark = { 0x54, 0x43, 0x68 };
		break;
	case 1:
		dark = { 0x32, 0x52, 0x88 };
		break;
	case 2:
		dark = { 0x00, 0x36, 0x38 };
		break;
	case 3:
		dark = { 0x7b, 0x11, 0x3a };
		break;
	case 4:
		dark = { 0x4a, 0x39, 0x33 };
		break;
	case 5:
		dark = { 0x08, 0x20, 0x32 };
		break;
	}*/

	/*sf::Color sand = { 0xff, 0xda, 0xb9 },
		earth = { 0xb0, 0x5b, 0x3b },
		rock = { 0x73, 0x40, 0x46 },
		forest = { 0x5d, 0x82, 0x33 },
		snow = { 0xe1, 0xe8, 0xeb },
		tundra = { 0xc9, 0xe4, 0xc5 };*/
