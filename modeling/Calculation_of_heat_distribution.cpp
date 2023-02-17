#include "Calculation_of_heat_distribution.h"
#include <iostream>

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
					cheng_temprture_per_second += Static_temprture_wall(arr_of_cells, i, j, 1000);
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
					cheng_temprture_per_second += Static_temprture_wall(arr_of_cells, i, j, 0);
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

//void Calculation_of_heat_distribution::Culculation(Cell** arr_of_cells, double delta_time, bool is_draw_wind) //differential variant of heat distribution
//{
//	for (int i = 0; i < height; i++) // minmax search
//	{
//		for (int j = 0; j < width; j++)
//		{	
//			double thermal_condactivity_x = 0;
//
//			if (j == 0)
//			{
//				thermal_condactivity_x = 3 / (1 / arr_of_cells[i][j].thermal_conductivity + 2 / arr_of_cells[i][j + 1].thermal_conductivity);
//			}
//			else if (j == width - 1)
//			{
//				thermal_condactivity_x = 3 / (1 / arr_of_cells[i][j - 1].thermal_conductivity + 2 / arr_of_cells[i][j].thermal_conductivity);
//			}
//			else
//			{
//				thermal_condactivity_x = 4 / (1 / arr_of_cells[i][j - 1].thermal_conductivity + 2 / arr_of_cells[i][j].thermal_conductivity + 1 / arr_of_cells[i][j + 1].thermal_conductivity);
//			}
//
//			double temprture_conductivity_x = thermal_condactivity_x / (arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density);
//
//			double grad_x = (((j > 0) ? arr_of_cells[i][j - 1].old_temperature : arr_of_cells[i][j].old_temperature)  // left
//				- 2 * arr_of_cells[i][j].old_temperature
//				+ ((j < width - 1) ? arr_of_cells[i][j + 1].old_temperature : arr_of_cells[i][j].old_temperature)) // right
//				/ pow(arr_of_cells[i][j].linear_size, 2);
//
//			double thermal_condactivity_y = 0;
//
//			if (i == 0)
//			{
//				thermal_condactivity_y = 3 / (1 / arr_of_cells[i][j].thermal_conductivity + 2 / arr_of_cells[i + 1][j].thermal_conductivity);
//			}
//			else if (i == height - 1)
//			{
//				thermal_condactivity_y = 3 / (1 / arr_of_cells[i - 1][j].thermal_conductivity + 2 / arr_of_cells[i][j].thermal_conductivity);
//			}
//			else
//			{
//				thermal_condactivity_y = 4 / (1 / arr_of_cells[i - 1][j].thermal_conductivity + 2 / arr_of_cells[i][j].thermal_conductivity + 1 / arr_of_cells[i + 1][j].thermal_conductivity);
//			}
//
//			double temprture_conductivity_y = thermal_condactivity_y / (arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density);
//
//			double grad_y = (((i > 0) ? arr_of_cells[i - 1][j].old_temperature : 1000) //top
//				- 2 * arr_of_cells[i][j].old_temperature
//				+ ((i < height - 1) ? arr_of_cells[i + 1][j].old_temperature : 0)) // botom
//				/ pow(arr_of_cells[i][j].linear_size, 2);
//
//			double internal_heat_release = pow(arr_of_cells[i][j].currents, 2) * arr_of_cells[i][j].resistances / arr_of_cells[i][j].linear_size;
//
//			//if (i == height / 2 && j == width / 2) internal_heat_release = 1000;
//
//			internal_heat_release /= (arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density * arr_of_cells[i][j].volume);
//
//			//arr_of_cells[i][j].new_temperature = arr_of_cells[i][j].old_temperature  + (trmprture_conductivity * (grad_x + grad_y) + internal_heat_release)* delta_time;
//			arr_of_cells[i][j].new_temperature = arr_of_cells[i][j].old_temperature + (temprture_conductivity_x * grad_x + temprture_conductivity_y * grad_y) * delta_time;
//
//
//		}
//	}
//
//	if (is_draw_wind) Draw_wind(arr_of_cells);
//}


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
