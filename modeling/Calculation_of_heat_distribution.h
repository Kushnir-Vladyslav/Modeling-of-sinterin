#pragma once
#include "Wind_wrap.h"
#include "Cell.h"
#include "data.h"

class Calculation_of_heat_distribution
{
public:
	Calculation_of_heat_distribution(bool draw_window = false);

	void Culculation(Cell** sample, double delta_time, bool is_draw_wind = true);
	void Draw_wind(Cell** sample);

	~Calculation_of_heat_distribution();
private:
	Wind_wrap* Wind;
	void* BitmapMemory;
};

