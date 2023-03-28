#pragma once

#include "Wind_wrap.h"
#include "Cell.h"

class Calculation_of_currents_Gauss
{
public:
	Calculation_of_currents_Gauss(int width, int height);

	void Culculation(Cell** arr_of_cells, double voltage, bool is_draw_wind = true);
	void Draw_wind(Cell** arr_of_cells);

	~Calculation_of_currents_Gauss();
private:
	Wind_wrap Wind;
	void* BitmapMemory;

	double dielectric = INT32_MAX / 3;

	int num_equations;
	double** equations;

	double* voltage;

	int width;
	int height;
};

