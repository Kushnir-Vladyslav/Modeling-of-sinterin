#pragma once
#include "Wind_wrap.h"
#include "Cell.h"

class Calculation_of_heat_distribution
{
public:
	Calculation_of_heat_distribution(int width, int height);

	void Culculation(Cell** arr_of_cells, double delta_time, bool is_draw_wind = true);
	void Draw_wind(Cell** arr_of_cells);

	~Calculation_of_heat_distribution();
private:
	Wind_wrap Wind;
	void* BitmapMemory;

	int width;
	int height;
};

