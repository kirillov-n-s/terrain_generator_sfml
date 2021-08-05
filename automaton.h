#pragma once
#include <random>

class automaton
{
	bool* _phase_grid;
	uint8_t* _uni_grid;
	uint32_t _iterations = 0;
	uint32_t _phases = 0;

	std::mt19937 _engine;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;

	bool get(bool* buffer, uint32_t x, uint32_t y) const;
	bool* get_neighbors(bool* buffer, uint32_t x, uint32_t y);
	void set(uint32_t x, uint32_t y, bool value);
	void flip_all();

	void randomize();
	void operate();
	void generate();

	void landmasses();
	void wastes();
	void vegetation();
	void arctic();

public:
	automaton(uint32_t width, uint32_t height);
	~automaton();

	void terraform(int phases = 1);
	void clear();

	uint8_t get(uint32_t x, uint32_t y) const;

	uint32_t width() const;
	uint32_t height() const;
	uint32_t iterations() const;
	uint32_t phases() const;
};
