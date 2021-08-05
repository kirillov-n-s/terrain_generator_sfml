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

void automaton::randomize()
{
	int a = 0;
	int b = 8;
	std::uniform_int_distribution<int> distrib(a, b);
	for (int i = 0; i < _size; i++)
		_phase_grid[i] = (bool)distrib(_engine);
}

void automaton::operate()
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
			bool state = count > 5;
			set(x, y, state);
			delete[] neighbors;
		}
	}
	delete[] buffer;
	_iterations++;
}

void automaton::generate()
{
	randomize();

	auto c1 = std::uniform_int_distribution<int>(-3, 3)(_engine);
	auto c2 = std::uniform_int_distribution<int>(-1, 1)(_engine);
	auto c3 = std::uniform_int_distribution<int>(-2, 2)(_engine);
	auto c4 = std::uniform_int_distribution<int>(-1, 1)(_engine);

	for (int i = 0; i < 20 + c1; i++)
		operate();
	flip_all();
	for (int i = 0; i < 5 + c2; i++)
		operate();
	flip_all();
	for (int i = 0; i < 10 + c3; i++)
		operate();
	flip_all();
	for (int i = 0; i < 5 + c4; i++)
		operate();

	if (std::uniform_int_distribution<int>(0, 1)(_engine))
		flip_all();
}

void automaton::landmasses()
{
	generate();
	for (int i = 0; i < _size; i++)
		_uni_grid[i] = _phase_grid[i];
}

void automaton::wastes()
{
	generate();
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i] && _uni_grid[i])
			_uni_grid[i] = 3;
}

void automaton::vegetation()
{
	generate();
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i] && _uni_grid[i] == 1)
			_uni_grid[i] = 5;
}

void automaton::arctic()
{
	auto resize = [this]()
	{
		_size = _width * _height;
		delete[] _phase_grid;
		_phase_grid = new bool[_size];
		std::memset(_phase_grid, 0, sizeof(bool) * _size);
	};

	auto height = _height;
	_height /= 3;
	resize();

	generate();
	for (int i = 0; i < _size; i++)
		if (_phase_grid[i])
			switch (_uni_grid[i])
			{
			case 0:
				_uni_grid[i] = 6;
				break;
			case 1:
				_uni_grid[i] = 7;
				break;
			case 5:
				_uni_grid[i] = 8;
				break;
			}

	_height = height;
	resize();
}

automaton::automaton(uint32_t width, uint32_t height)
	: _width(width), _height(height), _size(width * height)
{
	_phase_grid = new bool[_size];
	_uni_grid = new uint8_t[_size];
	_engine = std::mt19937(std::random_device()()); 
	clear();
}

automaton::~automaton()
{
	delete[] _phase_grid;
	delete[] _uni_grid;
}

void automaton::terraform(int phases)
{
	_iterations = 0;
	_phases = phases;

	landmasses();

	if (phases < 2)
		return;
	wastes();
	if (phases < 3)
		return;
	vegetation();
	if (phases < 4)
		return;
	arctic();
}

void automaton::clear()
{
	std::memset(_phase_grid, 0, sizeof(bool) * _size);
	std::memset(_uni_grid, 0, sizeof(uint8_t) * _size);
	_iterations = 0;
	_phases = 0;
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
