#pragma once

#include "Wind_wrap.h"
#include "Cell.h"

class Culculation_of_currents
{
public:
	Culculation_of_currents(int width, int height);

	void Culculation(Cell** arr_of_cells, double voltage, bool is_draw_wind = true);
	void Draw_wind(Cell** arr_of_cells);

	~Culculation_of_currents();
private:
	Wind_wrap Wind;
	void* BitmapMemory;

	double dielectric = INT32_MAX / 3;

	int num_equations;
	double** equations;

	double* currents;

	int width;
	int height;
};

