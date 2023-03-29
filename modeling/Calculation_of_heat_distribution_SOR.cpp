#include "Calculation_of_heat_distribution_SOR.h"
#include <iostream>
#include <vector>
#include <thread>

Calculation_of_heat_distribution_SOR::Calculation_of_heat_distribution_SOR(bool draw_window)
{
	if (draw_window)
		Wind = new Wind_wrap(L"Heat distribution", width_window, height_window);
	else
		Wind = nullptr;

	num_equations = (width_sample + 1) * (height_sample + 1); // counting the number of equations

	equations = new double* [num_equations]; // create arr for matrix
	for (int i = 0; i < num_equations; i++)
		equations[i] = new double[5]{ 0. };

	potential = new double[num_equations] {0.};
	heat_flow = new double[num_equations] {0.};

	BitmapMemory = (void*) new UINT32[width_sample * height_sample];

	relax_p = (8 - sqrt(64 - 16 * pow(cos(3.14 / height_sample) + cos(3.14 / width_sample), 2))) / pow(cos(3.14 / height_sample) + cos(3.14 / width_sample), 2); // Value of relaxation parameter 
	tolerance = 1.e-10; // Tolerance.
	break_difference = 1.e-15;
}

enum Direction_heat_flow
{
	DHF_UP,
	DHF_DOWN,
	DHF_LEFT,
	DHF_RIGHT
};

void Calculation_of_heat_distribution_SOR::Culculation(Cell** sample, double delta_time, bool is_draw_wind) //a custom option for heat distribution
{
	for (int i = 0; i < num_equations; i++)
		for (int j = 0; j < 5; j++)
			equations[i][j] = 0.;

	for (int i = 0; i < num_equations; i++)
		potential[i] = 0.;

	for (int i = 0; i < num_equations; i++) // filling the matrix with coefficients
	{
		int i_y = i / (width_sample + 1);  // definition of the x-y coordinates of the contour, not the cell
		int i_x = i - (width_sample + 1)*i_y;

		// thermal resistance 
		if (i_y == 0) // up
		{
			equations[i][2] -= infinity_heat_resistor;
		}
		else
		{
			if (i_x == 0)
				equations[i][0] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x].thermal_conductivity);
			else if (i_x == width_sample)
				equations[i][0] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x - 1].thermal_conductivity);
			else
				equations[i][0] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x - 1].thermal_conductivity + 1. / sample[i_y - 1][i_x].thermal_conductivity);				
		}
		if (i_y == height_sample) // down
		{
			equations[i][2] -= infinity_heat_resistor;
		}
		else
		{
			if (i_x == 0)
				equations[i][4] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y][i_x].thermal_conductivity);
			else if (i_x == width_sample)
				equations[i][4] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y][i_x - 1].thermal_conductivity);
			else
				equations[i][4] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y][i_x - 1].thermal_conductivity + 1. / sample[i_y][i_x].thermal_conductivity);
		}
		if (i_x == 0) // left
		{
			equations[i][2] -= infinity_heat_resistor;
		}
		else
		{
			if (i_y == 0)
				equations[i][1] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y][i_x - 1].thermal_conductivity);
			else if (i_y == height_sample)
				equations[i][1] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x - 1].thermal_conductivity);
			else
				equations[i][1] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x - 1].thermal_conductivity + 1. / sample[i_y][i_x - 1].thermal_conductivity);
		}
		if (i_x == width_sample) // right
		{
			equations[i][2] -= infinity_heat_resistor;
		}
		else
		{
			if (i_y == 0)
				equations[i][3] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y][i_x].thermal_conductivity);
			else if (i_y == height_sample)
				equations[i][3] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x].thermal_conductivity);
			else
				equations[i][3] -= (1. / (2. * linear_cell_size)) * (1. / sample[i_y - 1][i_x].thermal_conductivity + 1. / sample[i_y][i_x].thermal_conductivity);
		}
		{ // self
			equations[i][2] -= equations[i][0] + equations[i][1] + equations[i][3] + equations[i][4];
		}

		// thermal potential
		if (i_y != 0) // up
		{
			if (i_x == 0)
			{
				switch (LEFT_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += LEFT_Static_temperature - sample[i_y - 1][i_x].old_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else if (i_x == width_sample)
			{
				switch (RIGHT_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += sample[i_y - 1][i_x - 1].old_temperature - RIGHT_Static_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else
			{
				potential[i] += sample[i_y - 1][i_x - 1].old_temperature - sample[i_y - 1][i_x].old_temperature;
			}
		}
		if (i_y != height_sample) // douwn
		{
			if (i_x == 0)
			{
				switch (LEFT_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += sample[i_y][i_x].old_temperature - LEFT_Static_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else if (i_x == width_sample)
			{
				switch (RIGHT_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += RIGHT_Static_temperature - sample[i_y][i_x - 1].old_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else
			{
				potential[i] += sample[i_y][i_x].old_temperature - sample[i_y][i_x - 1].old_temperature;
			}
		}
		if (i_x != 0) // left
		{
			if (i_y == 0)
			{
				switch (UP_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += sample[i_y][i_x - 1].old_temperature - UP_Static_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else if (i_y == width_sample)
			{
				switch (DOWN_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += DOWN_Static_temperature - sample[i_y - 1][i_x - 1].old_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else
			{
				potential[i] += sample[i_y][i_x - 1].old_temperature - sample[i_y - 1][i_x - 1].old_temperature;
			}
		}
		if (i_x != width_sample) // right
		{
			if (i_y == 0)
			{
				switch (UP_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += UP_Static_temperature - sample[i_y][i_x].old_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else if (i_y == width_sample)
			{
				switch (DOWN_Type_heat_exchange)
				{
				case THE_STATIC:
					potential[i] += sample[i_y - 1][i_x].old_temperature - DOWN_Static_temperature;
					break;
				case THE_NON:
					break;
				}
			}
			else
			{
				potential[i] += sample[i_y - 1][i_x].old_temperature - sample[i_y][i_x].old_temperature;
			}
		}
	}


	bool solved = false;

	double error = 0.;
	double last_error = 0.;

	while (!solved)
	{
		error = 0.;
		for (int i = 0; i < num_equations; i++)
		{
			double S = 0.;

			if (i - (width_sample + 1) >= 0) S += equations[i][0] * heat_flow[i - (width_sample + 1)];
			if (i - 1 >= 0) S += equations[i][1] * heat_flow[i - 1];
			S += equations[i][2] * heat_flow[i];
			if (i + 1 < num_equations) S += equations[i][3] * heat_flow[i + 1];
			if (i + (width_sample + 1) < num_equations) S += equations[i][4] * heat_flow[i + (width_sample + 1)];

			heat_flow[i] += relax_p * (potential[i] - S) / equations[i][2];
		}


		for (int i = 0; i < num_equations; i++)
		{
			double S = 0.;

			if (i - (width_sample + 1) >= 0) S += equations[i][0] * heat_flow[i - (width_sample + 1)];
			if (i - 1 >= 0) S += equations[i][1] * heat_flow[i - 1];
			S += equations[i][2] * heat_flow[i];
			if (i + 1 < num_equations) S += equations[i][3] * heat_flow[i + 1];
			if (i + (width_sample + 1) < num_equations) S += equations[i][4] * heat_flow[i + (width_sample + 1)];


			if (error < abs(potential[i] - S)) error = abs(potential[i] - S);
		}
		if (abs(error) <= tolerance || abs(error - last_error) <= break_difference)
		{
			solved = true;
		}
		else
		{
			last_error = error;
		}
	}

	for (int i = 0; i < height_sample; i++) // calculation of heat flowing between nodes
	{
		for (int j = 0; j < width_sample; j++)
		{
			sample[i][j].new_temperature = sample[i][j].old_temperature + delta_time * 
				(heat_flow[i * (width_sample + 1) + j] - heat_flow[i * (width_sample + 1) + j + 1] +
				heat_flow[(i + 1) * (width_sample + 1) + j + 1] - heat_flow[i * (width_sample + 1) + j + 1] +
				heat_flow[(i + 1) * (width_sample + 1) + j] - heat_flow[(i + 1) * (width_sample + 1) + j + 1] +
				heat_flow[(i + 1) * (width_sample + 1) + j] - heat_flow[i * (width_sample + 1) + j]) /
				(sample[i][j].heat_capacity * sample[i][j].density * sample[i][j].volume);
		}
	}

	if (is_draw_wind) Draw_wind(sample);
}

void Calculation_of_heat_distribution_SOR::Draw_wind(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_t = 99999;
	double max_t = 0;

	for (int i = 0; i < height_sample; i++) // minmax search
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (sample[i][j].new_temperature > max_t) max_t = sample[i][j].new_temperature;
			if (sample[i][j].new_temperature < min_t) min_t = sample[i][j].new_temperature;
		}
	}

	//std::cout << "\n\nmin of temp: " << min_t << '\n' << "max of temp: " << max_t << std::endl;

	for (int i = 0; i < height_sample; i++) // filling the pixel array, lower temperature darker pixel, higher temperature lighter pixel
	{
		for (int j = 0; j < width_sample; j++)
		{
			int intens = (sample[i][j].new_temperature - min_t) * 255 / (max_t - min_t);
			SetPixel(BitmapMemory, j, i, width_sample, RGB(intens, intens, intens));
		}
	}

	Wind->Draw(BitmapMemory, width_sample, height_sample); // sending an array of pixels for rendering
}

Calculation_of_heat_distribution_SOR::~Calculation_of_heat_distribution_SOR()
{
	delete		Wind;
	delete[]	BitmapMemory;
}
