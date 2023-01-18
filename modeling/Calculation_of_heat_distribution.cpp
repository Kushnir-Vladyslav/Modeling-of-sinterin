#include "Calculation_of_heat_distribution.h"
#include <iostream>

Calculation_of_heat_distribution::Calculation_of_heat_distribution(int width, int height) : Wind(L"Heat distribution", 500, 500), width(width), height(height)
{
	BitmapMemory = (void*) new UINT32[width * height];
}

void Calculation_of_heat_distribution::Culculation(Cell** arr_of_cells, double delta_time, bool is_draw_wind)
{
	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			double trmprture_conductivity = arr_of_cells[i][j].thermal_conductivity / (arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density);
			//if (arr_of_cells[i][j].material == &Porosity) trmprture_conductivity = 0;
			double grad_x = (((j > 0) ? arr_of_cells[i][j - 1].old_temperature : arr_of_cells[i][j].old_temperature)  // left
							- 2 * arr_of_cells[i][j].old_temperature
							+ ((j < width - 1) ? arr_of_cells[i][j + 1].old_temperature : arr_of_cells[i][j].old_temperature)) // right
							/ pow(arr_of_cells[i][j].linear_size, 2);

			double grad_y = (((i > 0) ? arr_of_cells[i - 1][j].old_temperature : 1000) //top
							- 2 * arr_of_cells[i][j].old_temperature 
							+ (( i < height - 1) ? arr_of_cells[i + 1][j].old_temperature : 0)) // botom
							/ pow(arr_of_cells[i][j].linear_size, 2);

			double internal_heat_release = pow(arr_of_cells[i][j].currents, 2) * arr_of_cells[i][j].resistances / arr_of_cells[i][j].linear_size;

			//if (i == height / 2 && j == width / 2) internal_heat_release = 1000;

			internal_heat_release /= (arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density * arr_of_cells[i][j].volume);

			if (arr_of_cells[i][j].material == &Porosity)
			{ 
				//Sleep(100);
			}

			//arr_of_cells[i][j].new_temperature = arr_of_cells[i][j].old_temperature  + (trmprture_conductivity * (grad_x + grad_y) + internal_heat_release)* delta_time;
			arr_of_cells[i][j].new_temperature = arr_of_cells[i][j].old_temperature + (trmprture_conductivity * (grad_x + grad_y)) * delta_time;



			//double heat_flow_from_left = 0;
			//if(j > 0) 
			//	heat_flow_from_left = (arr_of_cells[i][j - 1].old_temperature - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i][j - 1].thermal_conductivity + 1 / arr_of_cells[i][j].thermal_conductivity);
			//else 
			//	heat_flow_from_left = (arr_of_cells[i][j].old_temperature - arr_of_cells[i][j].old_temperature ) /
			//	(1 / arr_of_cells[i][j].thermal_conductivity);

			//double heat_flow_from_right = 0;
			//if(j < width - 1)
			//	heat_flow_from_right = (arr_of_cells[i][j + 1].old_temperature - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i][j + 1].thermal_conductivity + 1 / arr_of_cells[i][j].thermal_conductivity);
			//else
			//	heat_flow_from_right = (arr_of_cells[i][j].old_temperature - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i][j].thermal_conductivity);

			//double heat_flow_from_up = 0;
			//if (i > 0)
			//	heat_flow_from_up = (arr_of_cells[i - 1][j].old_temperature - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i - 1][j].thermal_conductivity + 1 / arr_of_cells[i][j].thermal_conductivity); 
			//else
			//	heat_flow_from_up = (1000 - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i][j].thermal_conductivity);

			//double heat_flow_from_down = 0;
			//if(i < height - 1) heat_flow_from_down = (arr_of_cells[i + 1][j].old_temperature - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i + 1][j].thermal_conductivity + 1 / arr_of_cells[i][j].thermal_conductivity);
			//else
			//	heat_flow_from_down = (0 - arr_of_cells[i][j].old_temperature) /
			//	(1 / arr_of_cells[i][j].thermal_conductivity);

			//double sun_of_heat_flow = heat_flow_from_up + heat_flow_from_down + heat_flow_from_left + heat_flow_from_right;

			//double temperature_flow = sun_of_heat_flow * pow(arr_of_cells[i][j].linear_size, 2) *  2 / (arr_of_cells[i][j].linear_size * arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density * arr_of_cells[i][j].volume);

			//double internal_heat_release = 0;
			//	
			////if (i == height / 2 && j == width / 2)
			//{
			//	internal_heat_release = pow(arr_of_cells[i][j].currents, 2) * arr_of_cells[i][j].resistances / arr_of_cells[i][j].linear_size;

			//	internal_heat_release /= (arr_of_cells[i][j].heat_capacity * arr_of_cells[i][j].density * arr_of_cells[i][j].volume);
			//}

			//arr_of_cells[i][j].new_temperature = arr_of_cells[i][j].old_temperature + (sun_of_heat_flow) * delta_time;

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
