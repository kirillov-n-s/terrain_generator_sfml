#pragma once
#include <random>
#include <functional>

class automaton
{
	bool* _phase_grid;
	uint8_t* _uni_grid;

	uint32_t _iterations = 0;
	uint32_t _phases = 0;
	uint32_t _seed;
	std::minstd_rand _engine;

	uint32_t _width;
	uint32_t _height;
	uint32_t _size;

	bool get(bool* buffer, uint32_t x, uint32_t y) const;
	bool* get_neighbors(bool* buffer, uint32_t x, uint32_t y);
	void set(uint32_t x, uint32_t y, bool value);
	void flip_all();

	//generic operate
	using rule_func = std::function<bool(int)>;
	void operate(const rule_func& rules);

	//patterns
	void scatter(int a = 0, int b = 8);
	void spots_large(bool less = false);
	void spots_small();
	void terrain();
	void craters(int a = 12, int b = 17);

	//phases
	void fertilize();
	void vegetate();
	void populate();
	void irrigate();

public:
	automaton(uint32_t width, uint32_t height, uint32_t seed = std::random_device()());
	~automaton();

	void terraform(int phases = 1);

	uint8_t get(uint32_t x, uint32_t y) const;

	uint32_t width() const;
	uint32_t height() const;

	uint32_t iterations() const;
	uint32_t phases() const;
	uint32_t seed() const;
};
