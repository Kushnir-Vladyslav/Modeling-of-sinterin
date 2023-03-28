#pragma once

#include "Wind_wrap.h"
#include "Cell.h"
#include "data.h"

class Calculation_of_currents_SOR
{
public:
	Calculation_of_currents_SOR(bool draw_window = false);

	void Culculation(Cell** sample, bool is_draw_wind = true);
	void Draw_wind(Cell** sample);

	~Calculation_of_currents_SOR();
private:
	Wind_wrap* Wind;
	void* BitmapMemory;

	int num_equations;
	double** equations;

	double* voltage;
	double* currents;

	double relax_p; // Value of relaxation parameter 
	double tolerance; // Tolerance.
	double break_difference;
};

