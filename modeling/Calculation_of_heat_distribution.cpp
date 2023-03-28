#include "Calculation_of_heat_distribution.h"
#include <iostream>
#include <vector>
#include <thread>

Calculation_of_heat_distribution::Calculation_of_heat_distribution(int width, int height) : Wind(L"Heat distribution", 500, 500), width(width), height(height)
{
	BitmapMemory = (void*) new UINT32[width * height];
}

enum Direction_heat_flow
{
	DHF_UP,
	DHF_DOWN,
	DHF_LEFT,
	DHF_RIGHT
};

enum Type_heat_exchange
{
	THE_NON,
	THE_FLOW,
	THE_STATIC,
	THE_RADIATION,
	THE_CONVECTION
};

double Temprture_flow_from (Cell** arr_of_cell, int i, int j, Direction_heat_flow DHF) // K/c, degrees per second, T_f = Q/(cqv) -> Q = dT/sum(Ri) -> Ri = lin_size / (term_conduct * area)
{
	double sum_revers_termal_condactiviti = 1 / arr_of_cell[i][j].thermal_conductivity;

	switch (DHF)
	{
	case DHF_UP:
		sum_revers_termal_condactiviti += 1 / arr_of_cell[i - 1][j].thermal_conductivity;
		break;
	case DHF_DOWN:
		sum_revers_termal_condactiviti += 1 / arr_of_cell[i + 1][j].thermal_conductivity;
		break;
	case DHF_LEFT:
		sum_revers_termal_condactiviti += 1 / arr_of_cell[i][j - 1].thermal_conductivity;
		break;
	case DHF_RIGHT:
		sum_revers_termal_condactiviti += 1 / arr_of_cell[i][j + 1].thermal_conductivity;
		break;
	}

	double sum_termal_resistance = sum_revers_termal_condactiviti / (arr_of_cell[i][j].linear_size * 2);

	double termal_flow = 0;

	switch (DHF)
	{
	case DHF_UP:
		termal_flow = (arr_of_cell[i - 1][j].old_temperature - arr_of_cell[i][j].old_temperature) / sum_termal_resistance;
		break;
	case DHF_DOWN:
		termal_flow = (arr_of_cell[i + 1][j].old_temperature - arr_of_cell[i][j].old_temperature) / sum_termal_resistance;
		break;
	case DHF_LEFT:
		termal_flow = (arr_of_cell[i][j - 1].old_temperature - arr_of_cell[i][j].old_temperature) / sum_termal_resistance;
		break;
	case DHF_RIGHT:
		termal_flow = (arr_of_cell[i][j + 1].old_temperature - arr_of_cell[i][j].old_temperature) / sum_termal_resistance;
		break;
	}

	return termal_flow / (arr_of_cell[i][j].heat_capacity * arr_of_cell[i][j].density * arr_of_cell[i][j].volume);
}

double Static_temprture_wall (Cell** arr_of_cell, int i, int j, double temprture) // K/c, degrees per second, T_f = Q/(cqv) -> Q = dT/sum(Ri) -> Ri = lin_size / (term_conduct * area)
{
	double sum_revers_termal_condactiviti = 1 / arr_of_cell[i][j].thermal_conductivity;

	double sum_termal_resistance = sum_revers_termal_condactiviti / (arr_of_cell[i][j].linear_size * 2);

	double termal_flow = (temprture - arr_of_cell[i][j].old_temperature) / sum_termal_resistance;

	return termal_flow / (arr_of_cell[i][j].heat_capacity * arr_of_cell[i][j].density * arr_of_cell[i][j].volume);
}

void Calculation_of_heat_distribution::Culculation(Cell** arr_of_cells, double delta_time, bool is_draw_wind) //a custom option for heat distribution
{
	for (int i = 0; i < height; i++) 
	{
		for (int j = 0; j < width; j++)
		{
			double cheng_temprture_per_second = 0.;

			{ // up
				Type_heat_exchange THE = THE_NON;

				if (i > 0)
				{
					THE = THE_FLOW;
				}
				else
				{
					THE = THE_STATIC;
				}

				switch (THE)
				{
				case THE_FLOW:
					cheng_temprture_per_second += Temprture_flow_from(arr_of_cells, i, j, DHF_UP);
					break;
				case THE_STATIC:
					cheng_temprture_per_second += Static_temprture_wall(arr_of_cells, i, j, 1273);
					break;
				}
			}

			{ // down
				Type_heat_exchange THE = THE_NON;

				if (i < height - 1)
				{
					THE = THE_FLOW;
				}
				else
				{
					THE = THE_STATIC;
				}

				switch (THE)
				{
				case THE_FLOW:
					cheng_temprture_per_second += Temprture_flow_from(arr_of_cells, i, j, DHF_DOWN);
					break;
				case THE_STATIC:
					cheng_temprture_per_second += Static_temprture_wall(arr_of_cells, i, j, 273);
					break;
				}
			}

			{ // left
				Type_heat_exchange THE = THE_NON;

				if (j > 0)
				{
					THE = THE_FLOW;
				}
				else
				{
					THE = THE_NON;
				}

				switch (THE)
				{
				case THE_FLOW:
					cheng_temprture_per_second += Temprture_flow_from(arr_of_cells, i, j, DHF_LEFT);
					break;
				case THE_STATIC:
					cheng_temprture_per_second += Static_temprture_wall(arr_of_cells, i, j, 1000);
					break;
				}
			}

			{ // right
				Type_heat_exchange THE = THE_NON;

				if (j < width - 1)
				{
					THE = THE_FLOW;
				}
				else
				{
					THE = THE_NON;
				}

				switch (THE)
				{
				case THE_FLOW:
					cheng_temprture_per_second += Temprture_flow_from(arr_of_cells, i, j, DHF_RIGHT);
					break;
				case THE_STATIC:
					cheng_temprture_per_second += Static_temprture_wall(arr_of_cells, i, j, 0);
					break;
				}
			}

			arr_of_cells[i][j].new_temperature = arr_of_cells[i][j].old_temperature + cheng_temprture_per_second * delta_time;
		}
	}

	if (is_draw_wind) Draw_wind(arr_of_cells);
}

void Calculation_of_heat_distribution::Draw_wind(Cell** arr_of_cells)
{
	double min_t = 99999;
	double max_t = 0;

	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			if (arr_of_cells[i][j].new_temperature > max_t) max_t = arr_of_cells[i][j].new_temperature;
			if (arr_of_cells[i][j].new_temperature < min_t) min_t = arr_of_cells[i][j].new_temperature;
		}
	}

	//std::cout << "\n\nmin of temp: " << min_t << '\n' << "max of temp: " << max_t << std::endl;

	for (int i = 0; i < height; i++) // filling the pixel array, lower temperature darker pixel, higher temperature lighter pixel
	{
		for (int j = 0; j < width; j++)
		{
			int intens = (arr_of_cells[i][j].new_temperature - min_t) * 255 / (max_t - min_t);
			SetPixel(BitmapMemory, j, i, width, RGB(intens, intens, intens));
		}
	}

	Wind.Draw(BitmapMemory, width, height); // sending an array of pixels for rendering
}

Calculation_of_heat_distribution::~Calculation_of_heat_distribution()
{
	delete[] BitmapMemory;
}
