#pragma once
#include <random>
#include <functional>
#include <SFML/Graphics.hpp>

class terrgen
{
	const sf::Color* COLORS = new sf::Color[]
	{
		{ 0xff, 0xda, 0xb9 }, //sand
		{ 0xb0, 0x5b, 0x3b }, //soil
		{ 0x5d, 0x82, 0x33 }, //greenery

		{ 0x9e, 0x75, 0x40 }, //rock
		{ 0x44, 0x5c, 0x3c }, //swamp
		{ 0x1f, 0x44, 0x1e }, //tree

		{ 0x7f, 0x8b, 0x52 }, //succulent
		{ 0x32, 0x52, 0x88 }, //water
	};

	bool* _phase_grid;
	uint8_t* _uni_grid;

	uint32_t _iterations = 0;
	uint32_t _phases = 0;
	uint32_t _seed = 0;
	std::minstd_rand _engine;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;

	bool get(bool* buffer, uint32_t x, uint32_t y) const;
	bool* get_neighbors(bool* buffer, uint32_t x, uint32_t y);
	void set(uint32_t x, uint32_t y, bool value);
	void flip();

	using rule_func = std::function<bool(bool, int)>;
	void operate(const rule_func& rules);

	//patterns
	void scatter(int a = 0, int b = 8);
	void spots_large(bool less = false);
	void spots_small();
	void cavern(int steps = 10, int a = 0, int b = 1);
	void terrain();
	void craters(int steps = 15);

	//phases
	void fertilize();
	void vegetate();
	void diversify();
	void irrigate();

public:
	terrgen(uint32_t width, uint32_t height);
	~terrgen();

	void generate(int phases = 4, uint32_t seed = std::random_device()());

	sf::Color get(uint32_t x, uint32_t y) const;

	uint32_t width() const;
	uint32_t height() const;
	uint32_t iterations() const;
	uint32_t phases() const;
	uint32_t seed() const;
};
