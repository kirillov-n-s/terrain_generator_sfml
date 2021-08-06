#include "automaton.h"

bool automaton::get(bool* buffer, uint32_t x, uint32_t y) const
{
	return buffer[(y * _width + x) % _size];
}

bool* automaton::get_neighbors(bool* buffer, uint32_t x, uint32_t y)
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

void automaton::set(uint32_t x, uint32_t y, bool value)
{
	_phase_grid[(y * _width + x) % _size] = value;
}

void automaton::flip_all()
{
	for (int i = 0; i < _size; i++)
		_phase_grid[i] ^= true;
}

void automaton::operate(const rule_func& rules)
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
			set(x, y, rules(count));
			delete[] neighbors;
		}
	}
	delete[] buffer;
	_iterations++;
}

//patterns
void automaton::scatter(int a, int b)
{
	std::uniform_int_distribution<int> distrib(a, b);
	for (int i = 0; i < _size; i++)
		_phase_grid[i] = (bool)std::max(0, distrib(_engine));
}

void automaton::spots_large(bool less)
{
	//rule funcion
	auto rules = [](int count) { return count <= 4; };

	//count of operations
	auto c = less ? 40 : 10;

	scatter(0, 1);
	for (int i = 0; i < c; i++)
		operate(rules);
}

void automaton::spots_small()
{
	//rule funcion
	auto rules = [](int count) { return count <= 6; };

	//count of operations
	auto c = 9;

	scatter(0, 4);
	for (int i = 0; i < c; i++)
		operate(rules);
}

void automaton::terrain()
{
	//rule function
	auto rules = [](int count) { return count > 5; };

	//counts of each stage's operations (base + fluctuation)
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

void automaton::craters(int a, int b)
{
	//rule function
	auto rules = [](int count) { return count > 5; };

	//count of operations
	auto c = std::uniform_int_distribution<int>(a, b)(_engine);

	scatter();
	for (int i = 0; i < c; i++)
		operate(rules);
}

//phases
void automaton::fertilize()
{
	terrain();
	for (int i = 0; i < _size; i++)
		_uni_grid[i] = _phase_grid[i];
}

void automaton::vegetate()
{
	terrain();
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i] && _uni_grid[i] == 1)
			_uni_grid[i] = 2;
}

void automaton::populate()
{
	auto apply = [this](int state, int mod = 3)
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

	spots_large();
	apply(2);
	scatter(-2, 1);
	apply(2);
}

void automaton::irrigate()
{
	spots_large(true);
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i] && (_uni_grid[i] % 3))
			_uni_grid[i] = 7;
}

automaton::automaton(uint32_t width, uint32_t height, uint32_t seed)
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

automaton::~automaton()
{
	delete[] _phase_grid;
	delete[] _uni_grid;
}

void automaton::terraform(int phases)
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
	populate();
	if (phases < 4)
		return;
	irrigate();
}

uint8_t automaton::get(uint32_t x, uint32_t y) const
{
	return _uni_grid[(y * _width + x) % _size];
}

uint32_t automaton::width() const
{
	return _width;
}

uint32_t automaton::height() const
{
	return _height;
}

uint32_t automaton::iterations() const
{
	return _iterations;
}

uint32_t automaton::phases() const
{
	return _phases;
}

uint32_t automaton::seed() const
{
	return _seed;
}
