#include "Calculation_of_heat_distribution_def.h"
#include <iostream>
#include <vector>
#include <thread>

Calculation_of_heat_distribution_def::Calculation_of_heat_distribution_def(bool draw_window)
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

double SUM_FOR_MNK(int pow_x, int pow_y)
{
	double rez = 0;
	for (int i = 0; i < height_sample; i++)
		for (int j = 0; j < width_sample; j++)
			rez += pow(i * linear_cell_size, pow_y) * pow(j * linear_cell_size, pow_x);
	return rez;
}

#define VAR_AND_SUM_FOR_MNK(NAME, pow_X, pow_Y) double NAME = SUM_FOR_MNK(pow_X, pow_Y)

void Calculation_of_heat_distribution_def::Culculation(Cell** sample, double delta_time, bool is_draw_wind) //a custom option for heat distribution
{
	static bool First_time = true;
	if (First_time)
	{
		VAR_AND_SUM_FOR_MNK(X2, 2, 0);
		VAR_AND_SUM_FOR_MNK(XY, 1, 1);
		VAR_AND_SUM_FOR_MNK(X3, 3, 0);
		VAR_AND_SUM_FOR_MNK(XY2, 1, 2);
		VAR_AND_SUM_FOR_MNK(X2Y, 2, 1);
		VAR_AND_SUM_FOR_MNK(X4, 4, 0);
		VAR_AND_SUM_FOR_MNK(XY3, 1, 3);
		VAR_AND_SUM_FOR_MNK(X3Y, 3, 1);
		VAR_AND_SUM_FOR_MNK(X2Y2, 2, 2);
		VAR_AND_SUM_FOR_MNK(X5, 5, 0);
		VAR_AND_SUM_FOR_MNK(XY4, 1, 4);
		VAR_AND_SUM_FOR_MNK(X4Y, 4, 1);
		VAR_AND_SUM_FOR_MNK(X3Y2, 3, 2);
		VAR_AND_SUM_FOR_MNK(X2Y3, 2, 3);
		VAR_AND_SUM_FOR_MNK(X, 1, 0);

		VAR_AND_SUM_FOR_MNK(Y2, 0, 2);
		VAR_AND_SUM_FOR_MNK(Y3, 0, 3);
		VAR_AND_SUM_FOR_MNK(Y4, 0, 4);
		VAR_AND_SUM_FOR_MNK(Y5, 0, 5);
		VAR_AND_SUM_FOR_MNK(Y, 0, 1);

		VAR_AND_SUM_FOR_MNK(X6, 6, 0);
		VAR_AND_SUM_FOR_MNK(X2Y4, 2, 4);
		VAR_AND_SUM_FOR_MNK(X5Y, 5, 1);
		VAR_AND_SUM_FOR_MNK(X4Y2, 4, 2);
		VAR_AND_SUM_FOR_MNK(X3Y3, 3, 3);

		VAR_AND_SUM_FOR_MNK(Y6, 0, 6);
		VAR_AND_SUM_FOR_MNK(XY5, 1, 5);

		VAR_AND_SUM_FOR_MNK(X7, 7, 0);
		VAR_AND_SUM_FOR_MNK(X3Y4, 3, 4);
		VAR_AND_SUM_FOR_MNK(X6Y, 6, 1);
		VAR_AND_SUM_FOR_MNK(X4Y3, 4, 3);

		VAR_AND_SUM_FOR_MNK(Y7, 0, 7);
		VAR_AND_SUM_FOR_MNK(X2Y5, 2, 5);
		VAR_AND_SUM_FOR_MNK(XY6, 1, 6);

		VAR_AND_SUM_FOR_MNK(X8, 8, 0);
		VAR_AND_SUM_FOR_MNK(X4Y4, 4, 4);
		VAR_AND_SUM_FOR_MNK(X7Y, 7, 1);
		VAR_AND_SUM_FOR_MNK(X6Y2, 6, 2);
		VAR_AND_SUM_FOR_MNK(X5Y3, 5, 3);

		VAR_AND_SUM_FOR_MNK(Y8, 0, 8);
		VAR_AND_SUM_FOR_MNK(X3Y5, 3, 5);
		VAR_AND_SUM_FOR_MNK(X2Y6, 2, 6);
		VAR_AND_SUM_FOR_MNK(XY7, 1, 7);

		VAR_AND_SUM_FOR_MNK(Sum_of_1, 0, 0);

		MNK_MATRIX[0][0] = X2;
		MNK_MATRIX[0][1] = XY;
		MNK_MATRIX[0][2] = X3;
		MNK_MATRIX[0][3] = XY3;
		MNK_MATRIX[0][4] = X2Y;
		MNK_MATRIX[0][5] = X4;
		MNK_MATRIX[0][6] = XY3;
		MNK_MATRIX[0][7] = X3Y;
		MNK_MATRIX[0][8] = X2Y2;
		MNK_MATRIX[0][9] = X5;
		MNK_MATRIX[0][10] = XY4;
		MNK_MATRIX[0][11] = X4Y;
		MNK_MATRIX[0][12] = X3Y2;
		MNK_MATRIX[0][13] = X2Y3;
		MNK_MATRIX[0][14] = X;

		MNK_MATRIX[1][1] = Y2;
		MNK_MATRIX[1][2] = X2Y;
		MNK_MATRIX[1][3] = Y3;
		MNK_MATRIX[1][4] = XY2;
		MNK_MATRIX[1][5] = X3Y;
		MNK_MATRIX[1][6] = Y4;
		MNK_MATRIX[1][7] = X2Y2;
		MNK_MATRIX[1][8] = XY3;
		MNK_MATRIX[1][9] = X4Y;
		MNK_MATRIX[1][10] = Y5;
		MNK_MATRIX[1][11] = X3Y2;
		MNK_MATRIX[1][12] = X2Y3;
		MNK_MATRIX[1][13] = XY4;
		MNK_MATRIX[1][14] = Y;


		First_time = false;
	}

	if (is_draw_wind) Draw_wind(sample);
}

void Calculation_of_heat_distribution_def::Draw_wind(Cell** sample)
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

Calculation_of_heat_distribution_def::~Calculation_of_heat_distribution_def()
{
	delete		Wind;
	delete[]	BitmapMemory;
}
