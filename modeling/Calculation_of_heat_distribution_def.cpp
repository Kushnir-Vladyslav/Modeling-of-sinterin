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

double SUM_FOR_MNK(int pow_x, int pow_y)
{
	double rez = 0;
	for (int i = 0; i < height_sample; i++)
		for (int j = 0; j < width_sample; j++)
			rez += pow(i * linear_cell_size, pow_y) * pow(j * linear_cell_size, pow_x);
	return rez;
}

double SUM_FOR_MNK_WHIT_Y(Cell** sample, int pow_x, int pow_y)
{
	double rez = 0;
	for (int i = 0; i < height_sample; i++)
		for (int j = 0; j < width_sample; j++)
			rez += pow(i * linear_cell_size, pow_y) * pow(j * linear_cell_size, pow_x) * sample[i][j].old_temperature;
	return rez;
}

#define VAR_AND_SUM_FOR_MNK(NAME, pow_X, pow_Y) double NAME = SUM_FOR_MNK(pow_X, pow_Y)

#define FILL_MNK_MATRIX(NAME, ITR_I, ITR_J) MNK_MATRIX[ITR_I][ITR_J] = NAME

#define FILL_NMK_FREE_MEM(ITR, pow_X, pow_Y) free_members[ITR] = SUM_FOR_MNK_WHIT_Y(sample, pow_X, pow_Y)

void Calculation_of_heat_distribution_def::Culculation(Cell** sample, double delta_time, bool is_draw_wind) //a custom option for heat distribution
{
	static bool First_time = true;
	if (First_time) // Сount the values of the body of the matrix. They depend only on coordinates and are constant.
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
		VAR_AND_SUM_FOR_MNK(X5Y2, 5, 2);
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


		FILL_MNK_MATRIX(X2, 0, 0);
		FILL_MNK_MATRIX(XY, 0, 1);
		FILL_MNK_MATRIX(X3, 0, 2);
		FILL_MNK_MATRIX(XY2, 0, 3);
		FILL_MNK_MATRIX(X2Y, 0, 4);
		FILL_MNK_MATRIX(X4, 0, 5);
		FILL_MNK_MATRIX(XY3, 0, 6);
		FILL_MNK_MATRIX(X3Y, 0, 7);
		FILL_MNK_MATRIX(X2Y2, 0, 8);
		FILL_MNK_MATRIX(X5, 0, 9);
		FILL_MNK_MATRIX(XY4, 0, 10);
		FILL_MNK_MATRIX(X4Y, 0, 11);
		FILL_MNK_MATRIX(X3Y2, 0, 12);
		FILL_MNK_MATRIX(X2Y3, 0, 13);
		FILL_MNK_MATRIX(X, 0, 14);

		FILL_MNK_MATRIX(Y2, 1, 1);
		FILL_MNK_MATRIX(X2Y, 1, 2);
		FILL_MNK_MATRIX(Y3, 1, 3);
		FILL_MNK_MATRIX(XY2, 1, 4);
		FILL_MNK_MATRIX(X3Y, 1, 5);
		FILL_MNK_MATRIX(Y4, 1, 6);
		FILL_MNK_MATRIX(X2Y2, 1, 7);
		FILL_MNK_MATRIX(XY3, 1, 8);
		FILL_MNK_MATRIX(X4Y, 1, 9);
		FILL_MNK_MATRIX(Y5, 1, 10);
		FILL_MNK_MATRIX(X3Y2, 1, 11);
		FILL_MNK_MATRIX(X2Y3, 1, 12);
		FILL_MNK_MATRIX(XY4, 1, 13);
		FILL_MNK_MATRIX(Y, 1, 14);

		FILL_MNK_MATRIX(X4, 2, 2);
		FILL_MNK_MATRIX(X2Y2, 2, 3);
		FILL_MNK_MATRIX(X3Y, 2, 4);
		FILL_MNK_MATRIX(X5, 2, 5);
		FILL_MNK_MATRIX(X2Y3, 2, 6);
		FILL_MNK_MATRIX(X4Y, 2, 7);
		FILL_MNK_MATRIX(X3Y2, 2, 8);
		FILL_MNK_MATRIX(X6, 2, 9);
		FILL_MNK_MATRIX(X2Y4, 2, 10);
		FILL_MNK_MATRIX(X5Y, 2, 11);
		FILL_MNK_MATRIX(X4Y2, 2, 12);
		FILL_MNK_MATRIX(X3Y3, 2, 13);
		FILL_MNK_MATRIX(X2, 2, 14);

		FILL_MNK_MATRIX(Y4, 3, 3);
		FILL_MNK_MATRIX(XY3, 3, 4);
		FILL_MNK_MATRIX(X3Y2, 3, 5);
		FILL_MNK_MATRIX(Y5, 3, 6);
		FILL_MNK_MATRIX(X2Y3, 3, 7);
		FILL_MNK_MATRIX(XY4, 3, 8);
		FILL_MNK_MATRIX(X4Y2, 3, 9);
		FILL_MNK_MATRIX(Y6, 3, 10);
		FILL_MNK_MATRIX(X3Y3, 3, 11);
		FILL_MNK_MATRIX(X2Y4, 3, 12);
		FILL_MNK_MATRIX(XY5, 3, 13);
		FILL_MNK_MATRIX(Y2, 3, 14);

		FILL_MNK_MATRIX(X2Y2, 4, 4);
		FILL_MNK_MATRIX(X4Y, 4, 5);
		FILL_MNK_MATRIX(XY4, 4, 6);
		FILL_MNK_MATRIX(X3Y2, 4, 7);
		FILL_MNK_MATRIX(X2Y3, 4, 8);
		FILL_MNK_MATRIX(X5Y, 4, 9);
		FILL_MNK_MATRIX(XY5, 4, 10);
		FILL_MNK_MATRIX(X4Y2, 4, 11);
		FILL_MNK_MATRIX(X3Y3, 4, 12);
		FILL_MNK_MATRIX(X2Y4, 4, 13);
		FILL_MNK_MATRIX(XY, 4, 14);

		FILL_MNK_MATRIX(X6, 5, 5);
		FILL_MNK_MATRIX(X3Y3, 5, 6);
		FILL_MNK_MATRIX(X5Y, 5, 7);
		FILL_MNK_MATRIX(X4Y2, 5, 8);
		FILL_MNK_MATRIX(X7, 5, 9);
		FILL_MNK_MATRIX(X3Y4, 5, 10);
		FILL_MNK_MATRIX(X6Y, 5, 11);
		FILL_MNK_MATRIX(X5Y2, 5, 12);
		FILL_MNK_MATRIX(X4Y3, 5, 13);
		FILL_MNK_MATRIX(X3, 5, 14);

		FILL_MNK_MATRIX(Y6, 6, 6);
		FILL_MNK_MATRIX(X2Y4, 6, 7);
		FILL_MNK_MATRIX(XY5, 6, 8);
		FILL_MNK_MATRIX(X4Y3, 6, 9);
		FILL_MNK_MATRIX(Y7, 6, 10);
		FILL_MNK_MATRIX(X3Y4, 6, 11);
		FILL_MNK_MATRIX(X2Y5, 6, 12);
		FILL_MNK_MATRIX(XY6, 6, 13);
		FILL_MNK_MATRIX(Y3, 6, 14);

		FILL_MNK_MATRIX(X4Y2, 7, 7);
		FILL_MNK_MATRIX(X3Y3, 7, 8);
		FILL_MNK_MATRIX(X6Y, 7, 9);
		FILL_MNK_MATRIX(X2Y5, 7, 10);
		FILL_MNK_MATRIX(X5Y2, 7, 11);
		FILL_MNK_MATRIX(X4Y3, 7, 12);
		FILL_MNK_MATRIX(X3Y4, 7, 13);
		FILL_MNK_MATRIX(X2Y, 7, 14);

		FILL_MNK_MATRIX(X2Y4, 8, 8);
		FILL_MNK_MATRIX(X5Y2, 8, 9);
		FILL_MNK_MATRIX(XY6, 8, 10);
		FILL_MNK_MATRIX(X4Y3, 8, 11);
		FILL_MNK_MATRIX(X3Y4, 8, 12);
		FILL_MNK_MATRIX(X2Y5, 8, 13);
		FILL_MNK_MATRIX(XY2, 8, 14);

		FILL_MNK_MATRIX(X8, 9, 9);
		FILL_MNK_MATRIX(X4Y4, 9, 10);
		FILL_MNK_MATRIX(X7Y, 9, 11);
		FILL_MNK_MATRIX(X6Y2, 9, 12);
		FILL_MNK_MATRIX(X5Y3, 9, 13);
		FILL_MNK_MATRIX(X4, 9, 14);

		FILL_MNK_MATRIX(Y8, 10, 10);
		FILL_MNK_MATRIX(X3Y5, 10, 11);
		FILL_MNK_MATRIX(X2Y6, 10, 12);
		FILL_MNK_MATRIX(XY7, 10, 13);
		FILL_MNK_MATRIX(Y4, 10, 14);

		FILL_MNK_MATRIX(X6Y2, 11, 11);
		FILL_MNK_MATRIX(X5Y3, 11, 12);
		FILL_MNK_MATRIX(X4Y4, 11, 13);
		FILL_MNK_MATRIX(X3Y, 11, 14);

		FILL_MNK_MATRIX(X4Y4, 12, 12);
		FILL_MNK_MATRIX(X3Y5, 12, 13);
		FILL_MNK_MATRIX(X2Y2, 12, 14);

		FILL_MNK_MATRIX(X2Y6, 13, 13);
		FILL_MNK_MATRIX(XY3, 13, 14);

		FILL_MNK_MATRIX(Sum_of_1, 14, 14);

		for (int i = 0; i < 15; i++)
			for (int j = i + 1; j < 15; j++)
				MNK_MATRIX[j][i] = MNK_MATRIX[i][j];
		
		First_time = false;
	} 

	for (int i = 0; i < width_sample; i++)
	{
		sample[0][i].new_temperature = UP_Static_temperature;
		sample[int(height_sample) - 1][i].new_temperature = DOWN_Static_temperature;
	}

	for (int i = 0; i < 15; i++) // Fill the working matrix with previously calculated values.
		for (int j = 0; j < 15; j++)
			matrix_body[i][j] = MNK_MATRIX[i][j];

	FILL_NMK_FREE_MEM(0, 1, 0); // Fill in the column of free members.
	FILL_NMK_FREE_MEM(1, 0, 1);
	FILL_NMK_FREE_MEM(2, 2, 0);
	FILL_NMK_FREE_MEM(3, 0, 2);
	FILL_NMK_FREE_MEM(4, 1, 1);
	FILL_NMK_FREE_MEM(5, 3, 0);
	FILL_NMK_FREE_MEM(6, 0, 3);
	FILL_NMK_FREE_MEM(7, 2, 1);
	FILL_NMK_FREE_MEM(8, 1, 2);
	FILL_NMK_FREE_MEM(9, 4, 0);
	FILL_NMK_FREE_MEM(10, 0, 4);
	FILL_NMK_FREE_MEM(11, 3, 1);
	FILL_NMK_FREE_MEM(12, 2, 2);
	FILL_NMK_FREE_MEM(13, 1, 3);
	FILL_NMK_FREE_MEM(14, 0, 0);

	for (int i = 0; i < 15; i++) // direct passage of the solution of the matrix
	{
		free_members[i] /= matrix_body[i][i];
		for (int j = 14; j >= i; j--)
			matrix_body[i][j] /= matrix_body[i][i];

		for (int k = i + 1; k < 15; k++)
		{
			free_members[k] -= free_members[i] * matrix_body[k][i];
			for (int j = 14; j >= i; j--)
				matrix_body[k][j] -= matrix_body[i][j] * matrix_body[k][i];
		}
	}

	for (int i = 14; i >= 0; i--)
	{
		for (int j = i - 1; j >= 0; j--)
		{
			free_members[j] -= free_members[i] * matrix_body[i][i];
		}
	}

#define mn(itr, pow_x, pow_y) free_members[itr] * pow(j * linear_cell_size, pow_x)* pow(i * linear_cell_size, pow_y)

	for (int i = 1; i < height_sample - 1; i++)
		for (int j = 0; j < width_sample; j++)
			sample[i][j].new_temperature = mn(0, 1, 0) + mn(1, 0, 1) + mn(2, 2, 0) + mn(3, 0, 2) + mn(4, 1, 1) + mn(5, 3, 0) + mn(6, 0, 3)
			+ mn(7, 2, 1) + mn(8, 1, 2) + mn(9, 4, 0) + mn(10, 0, 4) + mn(11, 3, 1) + mn(12, 2, 2) + mn(13, 1, 3) + mn(14, 0, 0);

	for (int i = 1; i < height_sample - 1; i++)
		for (int j = 0; j < width_sample; j++)
		{
			double lcp = sample[i][j].thermal_conductivity / (sample[i][j].heat_capacity * sample[i][j].density);
			double dx = 2 * free_members[2] + 6 * free_members[5] * j * linear_cell_size + 2 * free_members[7] * i * linear_cell_size +
				12 * free_members[9] * pow(j * linear_cell_size, 2) + 6 * free_members[11] * i * j * pow(linear_cell_size, 2) +
				2 * free_members[12] * pow(i * linear_cell_size, 2);
			double dy = 2 * free_members[3] + 6 * free_members[6] * i * linear_cell_size + 2 * free_members[8] * j * linear_cell_size +
				12 * free_members[10] * pow(i * linear_cell_size, 2) + 2 * free_members[12] * pow(j * linear_cell_size, 2) +
				6 * free_members[13] * i * j * pow(linear_cell_size, 2);

			//sample[i][j].new_temperature = lcp * (dx + dy) * delta_time;
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
