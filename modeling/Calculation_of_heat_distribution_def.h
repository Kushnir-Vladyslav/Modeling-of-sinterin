#pragma once
#include "Wind_wrap.h"
#include "Cell.h"
#include "data.h"

class Calculation_of_heat_distribution_def
{
public:
	Calculation_of_heat_distribution_def(bool draw_window = false);

	void Culculation(Cell** sample, double delta_time, bool is_draw_wind = true);
	void Draw_wind(Cell** sample);

	~Calculation_of_heat_distribution_def();
private:
	double MNK_MATRIX[15][15];

	double matrix_body[15][15];
	double free_members[15];

	Wind_wrap* Wind;
	void* BitmapMemory;
};

