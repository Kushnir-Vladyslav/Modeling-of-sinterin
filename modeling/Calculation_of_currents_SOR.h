#pragma once

#include "Wind_wrap.h"
#include "Cell.h"

class Calculation_of_currents_SOR
{
public:
	Calculation_of_currents_SOR(int width, int height);

	void Culculation(Cell** arr_of_cells, double voltage, bool is_draw_wind = true);
	void Draw_wind(Cell** arr_of_cells);

	~Calculation_of_currents_SOR();
private:
	Wind_wrap Wind;
	void* BitmapMemory;

	double dielectric = INT32_MAX / 3;

	int num_equations;
	double** equations;

	double* voltage;
	double* currents;

	int width;
	int height;

	double relax_p; // Value of relaxation parameter 
	double tolerance; // Tolerance.
	double break_difference;
};

