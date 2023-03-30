#include "Calculation_of_heat_distribution.h"
#include <iostream>
#include <vector>
#include <thread>

Calculation_of_heat_distribution::Calculation_of_heat_distribution(bool draw_window)
{
	if (draw_window)
		Wind = new Wind_wrap(L"Heat distribution", width_window, height_window);
	else
		Wind = nullptr;

	BitmapMemory = (void *) new UINT32[width_sample * height_sample];
}

enum Direction_heat_flow
{
	DHF_UP,
	DHF_DOWN,
	DHF_LEFT,
	DHF_RIGHT
};

double Temprture_flow_from (Cell** sample, int i, int j, Direction_heat_flow DHF) // K/c, degrees per second, T_f = Q/(cqv) -> Q = dT/sum(Ri) -> Ri = lin_size / (term_conduct * area)
{
	double sum_revers_termal_condactiviti = 1 / sample[i][j].thermal_conductivity;

	switch (DHF)
	{
	case DHF_UP:
		sum_revers_termal_condactiviti += 1 / sample[i - 1][j].thermal_conductivity;
		break;
	case DHF_DOWN:
		sum_revers_termal_condactiviti += 1 / sample[i + 1][j].thermal_conductivity;
		break;
	case DHF_LEFT:
		sum_revers_termal_condactiviti += 1 / sample[i][j - 1].thermal_conductivity;
		break;
	case DHF_RIGHT:
		sum_revers_termal_condactiviti += 1 / sample[i][j + 1].thermal_conductivity;
		break;
	}

	double sum_termal_resistance = sum_revers_termal_condactiviti / (sample[i][j].linear_size * 2);

	double termal_flow = 0;

	switch (DHF)
	{
	case DHF_UP:
		termal_flow = (sample[i - 1][j].old_temperature - sample[i][j].old_temperature) / sum_termal_resistance;
		break;
	case DHF_DOWN:
		termal_flow = (sample[i + 1][j].old_temperature - sample[i][j].old_temperature) / sum_termal_resistance;
		break;
	case DHF_LEFT:
		termal_flow = (sample[i][j - 1].old_temperature - sample[i][j].old_temperature) / sum_termal_resistance;
		break;
	case DHF_RIGHT:
		termal_flow = (sample[i][j + 1].old_temperature - sample[i][j].old_temperature) / sum_termal_resistance;
		break;
	}

	return termal_flow / (sample[i][j].heat_capacity * sample[i][j].density * sample[i][j].volume);
}

double Static_temprture_wall (Cell** sample, int i, int j, double temprture) // K/c, degrees per second, T_f = Q/(cqv) -> Q = dT/sum(Ri) -> Ri = lin_size / (term_conduct * area)
{
	double sum_revers_termal_condactiviti = 1 / sample[i][j].thermal_conductivity;

	double sum_termal_resistance = sum_revers_termal_condactiviti / (sample[i][j].linear_size * 2);

	double termal_flow = (temprture - sample[i][j].old_temperature) / sum_termal_resistance;

	return termal_flow / (sample[i][j].heat_capacity * sample[i][j].density * sample[i][j].volume);
}

void Calculation_of_heat_distribution::Culculation(Cell** sample, double delta_time, bool is_draw_wind) //a custom option for heat distribution
{
	for (int i = 0; i < height_sample; i++) 
	{
		for (int j = 0; j < width_sample; j++)
		{
			double cheng_temprture_per_second = 0.;

			{ // up
				if (i > 0)
				{
					cheng_temprture_per_second += Temprture_flow_from(sample, i, j, DHF_UP);
				}
				else
				{
					switch (UP_Type_heat_exchange)
					{
					case THE_STATIC:
						cheng_temprture_per_second += Static_temprture_wall(sample, i, j, UP_Static_temperature);
						break;
					}
				}
			}

			{ // down
				if (i < height_sample - 1)
				{
					cheng_temprture_per_second += Temprture_flow_from(sample, i, j, DHF_DOWN);
				}
				else
				{
					switch (DOWN_Type_heat_exchange)
					{
					case THE_STATIC:
						cheng_temprture_per_second += Static_temprture_wall(sample, i, j, DOWN_Static_temperature);
						break;
					}
				}
			}

			{ // left
				if (j > 0)
				{
					cheng_temprture_per_second += Temprture_flow_from(sample, i, j, DHF_LEFT);
				}
				else
				{
					switch (LEFT_Type_heat_exchange)
					{
					case THE_STATIC:
						cheng_temprture_per_second += Static_temprture_wall(sample, i, j, LEFT_Static_temperature);
						break;
					}
				}
			}

			{ // right
				if (j < width_sample - 1)
				{
					cheng_temprture_per_second += Temprture_flow_from(sample, i, j, DHF_RIGHT);
				}
				else
				{
					switch (RIGHT_Type_heat_exchange)
					{
					case THE_STATIC:
						cheng_temprture_per_second += Static_temprture_wall(sample, i, j, RIGHT_Static_temperature);
						break;
					}
				}
			}

			sample[i][j].new_temperature = sample[i][j].old_temperature + cheng_temprture_per_second * delta_time;
		}
	}

	if (is_draw_wind) Draw_wind(sample);
}

void Calculation_of_heat_distribution::Draw_wind(Cell** sample)
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

Calculation_of_heat_distribution::~Calculation_of_heat_distribution()
{
	delete		Wind;
	delete[]	BitmapMemory;
}
