#pragma once
#include "Wind_wrap.h"
#include "Cell.h"
#include "data.h"

enum What_draw
{
	DR_NOTHING,
	DR_RESISTANCES,
	DR_DENSITY,
	DR_HEAT_CAPACITY,
	DR_THERMAL_CONDUCTIVITY
};

class Calculation_of_effective_properties
{
public:
	Calculation_of_effective_properties(bool draw_window = false);

	void Array_initialization(Cell** sample); // primary filling of all parameters of arrays, distribution of materials, and, accordingly, effective properties
																									
	void Calculation_all_of_effective_properties(Cell** sample, What_draw what_draw = DR_NOTHING); // so far, all methods assume that each cell has only one material

	void Culculation_resistances(Cell** sample, bool is_draw_wind = false);
	void Draw_wind_resistances(Cell** sample);

	void Culculation_density(Cell** sample, bool is_draw_wind = false);
	void Draw_wind_density(Cell** sample);

	void Culculation_heat_capacity(Cell** sample, bool is_draw_wind = false);
	void Draw_wind_heat_capacity(Cell** sample);

	void Culculation_thermal_conductivity(Cell** sample, bool is_draw_wind = false);
	void Draw_wind_thermal_conductivity(Cell** sample);

	void Test_filling_resistances(Cell** sample, bool is_draw_wind = true);

	~Calculation_of_effective_properties();
private:
	Wind_wrap* Wind;
	void* BitmapMemory;
};

