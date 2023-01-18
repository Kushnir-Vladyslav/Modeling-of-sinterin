#pragma once
#include "Wind_wrap.h"
#include "Cell.h"

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
	Calculation_of_effective_properties(int width, int height);

	void Array_initialization(Cell** arr_of_cells, int width, int height, double linear_cell_size); // primary filling of all parameters of arrays, distribution of materials, and, accordingly, effective properties
																									
	void Calculation_all_of_effective_properties(Cell** arr_of_cells, What_draw what_draw = DR_NOTHING); // so far, all methods assume that each cell has only one material

	void Culculation_resistances(Cell** arr_of_cells, bool is_draw_wind = false);
	void Draw_wind_resistances(Cell** arr_of_cells);

	void Culculation_density(Cell** arr_of_cells, bool is_draw_wind = false);
	void Draw_wind_density(Cell** arr_of_cells);

	void Culculation_heat_capacity(Cell** arr_of_cells, bool is_draw_wind = false);
	void Draw_wind_heat_capacity(Cell** arr_of_cells);

	void Culculation_thermal_conductivity(Cell** arr_of_cells, bool is_draw_wind = false);
	void Draw_wind_thermal_conductivity(Cell** arr_of_cells);

	void Test_filling_resistances(Cell** arr_of_cells, bool is_draw_wind = true);

	~Calculation_of_effective_properties();
private:
	Wind_wrap Wind;
	void* BitmapMemory;

	double dielectric = INT32_MAX / 3;

	int width;
	int height;
};

