#include "terrgen.h"

bool terrgen::get(bool* buffer, uint32_t x, uint32_t y) const
{
	return buffer[(y * _width + x) % _size];
}

bool* terrgen::get_neighbors(bool* buffer, uint32_t x, uint32_t y)
{
	bool* res = new bool[8];

	res[0] = get(buffer, x - 1, y - 1);
	res[1] = get(buffer, x + 0, y - 1);
	res[2] = get(buffer, x + 1, y - 1);

	res[3] = get(buffer, x - 1, y + 0);
	res[4] = get(buffer, x + 1, y + 0);

	res[5] = get(buffer, x - 1, y + 1);
	res[6] = get(buffer, x + 0, y + 1);
	res[7] = get(buffer, x + 1, y + 1);

	return res;
}

void terrgen::set(uint32_t x, uint32_t y, bool value)
{
	_phase_grid[(y * _width + x) % _size] = value;
}

void terrgen::flip_all()
{
	for (int i = 0; i < _size; i++)
		_phase_grid[i] ^= true;
}

void terrgen::operate(const rule_func& rules)
{
	bool* buffer = new bool[_size];
	for (int i = 0; i < _size; i++)
		buffer[i] = _phase_grid[i];

	for (int x = 0; x < _width; x++)
	{
		for (int y = 0; y < _height; y++)
		{
			auto neighbors = get_neighbors(buffer, x, y);
			int count = 0;
			for (int i = 0; i < 8; i++)
				count += neighbors[i];
			set(x, y, rules(get(buffer, x, y), count));
			delete[] neighbors;
		}
	}

	delete[] buffer;
	_iterations++;
}

//patterns
void terrgen::scatter(int a, int b)
{
	std::uniform_int_distribution<int> distrib(a, b);
	for (int i = 0; i < _size; i++)
		_phase_grid[i] = (bool)std::max(0, distrib(_engine));
}

void terrgen::spots_large(bool less)
{
	auto rules = [](bool state, int count) { return count <= 4; };
	auto c = less ? 40 : 10;

	scatter(0, 1);
	for (int i = 0; i < c; i++)
		operate(rules);
}

void terrgen::spots_small()
{
	auto rules = [](bool state, int count) { return count <= 6; };
	auto c = 9;

	scatter(0, 4);
	for (int i = 0; i < c; i++)
		operate(rules);
}

void terrgen::cavern(int steps, int a, int b)
{
	auto rules = [](bool state, int count)
	{
		if (count > 4)
			return true;
		if (count < 4)
			return false;
		return state;
	};

	scatter(a, b);
	for (int i = 0; i < steps; i++)
		operate(rules);
}

void terrgen::terrain()
{
	auto rules = [](bool state, int count) { return count > 5; };

	auto c1 = 20, c2 = 5, c3 = 10, c4 = 5;
	c1 += std::uniform_int_distribution<int>(-c1 / 4, c1 / 4)(_engine);
	c2 += std::uniform_int_distribution<int>(-c2 / 4, c2 / 4)(_engine);
	c3 += std::uniform_int_distribution<int>(-c3 / 4, c3 / 4)(_engine);
	c4 += std::uniform_int_distribution<int>(-c4 / 4, c4 / 4)(_engine);

	scatter();
	for (int i = 0; i < c1; i++)
		operate(rules);
	flip_all();
	for (int i = 0; i < c2; i++)
		operate(rules);
	flip_all();
	for (int i = 0; i < c3; i++)
		operate(rules);
	flip_all();
	for (int i = 0; i < c4; i++)
		operate(rules);
	if (std::uniform_int_distribution<int>(0, 1)(_engine))
		flip_all();
}

void terrgen::craters(int steps)
{
	auto rules = [](bool state, int count) { return count > 5; };

	scatter();
	for (int i = 0; i < steps; i++)
		operate(rules);
}

//phases
void terrgen::fertilize()
{
	terrain();
	for (int i = 0; i < _size; i++)
		_uni_grid[i] = _phase_grid[i];
}

void terrgen::vegetate()
{
	terrain();
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i] && _uni_grid[i] == 1)
			_uni_grid[i] = 2;
}

void terrgen::diversify()
{
	auto apply = [this](uint8_t state, uint8_t mod = 3)
	{
		for (int i = 0; i < _size; i++)
			if (_phase_grid[i] && _uni_grid[i] == state)
				_uni_grid[i] += mod;
	};

	spots_large();
	apply(0);
	scatter(-24, 1);
	apply(0, 6);

	spots_large();
	apply(1);

	cavern();
	apply(2);
	scatter(-2, 1);
	apply(2);
}

void terrgen::irrigate()
{
	spots_large(true);
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i] && (_uni_grid[i] % 3))
			_uni_grid[i] = 7;
}

terrgen::terrgen(uint32_t width, uint32_t height, uint32_t seed)
	: _width(width), _height(height), _size(width * height), _seed(seed)
{
	_phase_grid = new bool[_size];
	_uni_grid = new uint8_t[_size];
	_engine = std::minstd_rand(_seed);
	_iterations = 0;
	_phases = 0;
	std::memset(_phase_grid, 0, sizeof(bool) * _size);
	std::memset(_uni_grid, 0, sizeof(uint8_t) * _size);
}

terrgen::~terrgen()
{
	delete[] _phase_grid;
	delete[] _uni_grid;
}

void terrgen::generate(int phases)
{
	_seed = _engine();
	_iterations = 0;
	_phases = phases;

	fertilize();
	if (phases < 2)
		return;
	vegetate();
	if (phases < 3)
		return;
	diversify();
	if (phases < 4)
		return;
	irrigate();
}

uint8_t terrgen::get(uint32_t x, uint32_t y) const
{
	return _uni_grid[(y * _width + x) % _size];
}

uint32_t terrgen::width() const
{
	return _width;
}

uint32_t terrgen::height() const
{
	return _height;
}

uint32_t terrgen::iterations() const
{
	return _iterations;
}

uint32_t terrgen::phases() const
{
	return _phases;
}

uint32_t terrgen::seed() const
{
	return _seed;
}
