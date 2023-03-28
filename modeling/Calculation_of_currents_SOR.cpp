#include "Calculation_of_currents_SOR.h"


Calculation_of_currents_SOR::Calculation_of_currents_SOR(bool draw_window)
{
	if (draw_window)
		Wind = new Wind_wrap(L"Current distribution", width_window, height_window);
	else
		Wind = nullptr;

	num_equations = width_sample * (height_sample + 1); // counting the number of equations

	equations = new double* [num_equations]; // create arr for matrix
	for (int i = 0; i < num_equations; i++)
		equations[i] = new double[5]{ 0. };

	voltage = new double[num_equations] {0.};
	currents = new double[num_equations] {0.};

	BitmapMemory = (void *) new UINT32[width_sample * height_sample];

	relax_p = (8 - sqrt(64 - 16 * pow(cos(3.14 / height_sample) + cos(3.14 / width_sample), 2))) / pow(cos(3.14 / height_sample) + cos(3.14 / width_sample), 2); // Value of relaxation parameter 
	tolerance = 1.e-10; // Tolerance.
	break_difference = 1.e-15;	
}

void Calculation_of_currents_SOR::Culculation(Cell** sample, bool is_draw_wind)
{
	for (int i = 0; i < num_equations; i++)
		for (int j = 0; j < 5; j++)
			equations[i][j] = 0.;

	for (int i = 0; i < num_equations; i++) // filling the matrix with coefficients
	{
		int i_y = i / (width_sample);  // definition of the x-y coordinates of the contour, not the cell
		int i_x = i - (width_sample)*i_y;

		if (i_x > 0) // left
		{
			if (i_y < height_sample) equations[i][1] -= sample[i_y][i_x].resistances / 2.;
			if (i_y > 0) equations[i][1] -= sample[i_y - 1][i_x].resistances / 2.;
		}
		else
		{
			if (i_y < height_sample) equations[i][2] += sample[i_y][i_x].resistances / 2.;
			if (i_y > 0) equations[i][2] += sample[i_y - 1][i_x].resistances / 2.;
		}
		if (i_y > 0) //upper
		{
			if (i_x != width_sample - 1) equations[i][0] = -(sample[i_y - 1][i_x].resistances + sample[i_y - 1][i_x + 1].resistances) / 2.;
			else equations[i][0] = -dielectric;
		}
		if (i_x < width_sample - 1) // right
		{
			if (i_y < height_sample) equations[i][3] -= sample[i_y][i_x + 1].resistances / 2.;
			if (i_y > 0) equations[i][3] -= sample[i_y - 1][i_x + 1].resistances / 2.;
		}
		if (i_y < height_sample) // bottom
		{
			if (i_x != width_sample - 1) equations[i][4] = -(sample[i_y][i_x].resistances + sample[i_y][i_x + 1].resistances) / 2.;
			else equations[i][4] = -dielectric;
		}
		// self
		{
			equations[i][2] += -equations[i][1] - equations[i][0] - equations[i][3] - equations[i][4];
		}
		// free members
		{
			if (i_x == width_sample - 1) voltage[i] = potential_difference / (height_sample + 1.);
		}
	}

	static bool First_time = true;

	if (First_time)
	{
		for (int i = 0; i < width_sample; i++)
			currents[i] = voltage[i];
		First_time = false;
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

			if (i - width_sample >= 0) S += equations[i][0] * currents[i - width_sample];
			if (i - 1 >= 0) S += equations[i][1] * currents[i - 1];
			S += equations[i][2] * currents[i];
			if (i + 1 < num_equations) S += equations[i][3] * currents[i + 1];
			if (i + width_sample < num_equations) S += equations[i][4] * currents[i + width_sample];

			currents[i] += relax_p * (voltage[i] - S) / equations[i][2];
		}


		for (int i = 0; i < num_equations; i++)
		{
			double S = 0.;

			if (i - width_sample >= 0) S -= equations[i][0] * currents[i - width_sample];
			if (i - 1 >= 0)S -= equations[i][1] * currents[i - 1];
			S -= equations[i][2] * currents[i];
			if (i + 1 < num_equations) S -= equations[i][3] * currents[i + 1];
			if (i + width_sample < num_equations) S -= equations[i][4] * currents[i + width_sample];


			if (error < abs(S + voltage[i])) error = abs(S + voltage[i]);
		}
		if (abs(error) <= tolerance || abs(error - last_error) <= break_difference)
		{
			solved = true;
		}
		else
		{
			last_error = error;
			//{
			//	for (int i = 0; i < height_sample; i++) // calculation of currents flowing in nodes
			//	{
			//		for (int j = 0; j < width_sample; j++)
			//		{
			//			if (j == 0)
			//			{
			//				sample[i][j].currents = (abs(currents[i * width_sample]) +
			//					abs(currents[(i + 1) * width_sample]) +
			//					abs(currents[i * width_sample] - currents[(i + 1) * width_sample])) / 2;
			//			}
			//			else
			//			{
			//				sample[i][j].currents = (abs(currents[i * width_sample + j - 1] - currents[i * width_sample + j]) +
			//					abs(currents[(i + 1) * width_sample + j - 1] - currents[(i + 1) * width_sample + j]) +
			//					abs(currents[i * width_sample + j - 1] - currents[(i + 1) * width_sample + j - 1]) +
			//					abs(currents[i * width_sample + j] - currents[(i + 1) * width_sample + j])) / 2;
			//			}
			//		}
			//	}
			//	if (is_draw_wind) Draw_wind(sample);
			//}
		}
	}


	for (int i = 0; i < height_sample; i++) // calculation of currents flowing in nodes
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (j == 0)
			{
				sample[i][j].currents = (abs(currents[i * width_sample]) +
					abs(currents[(i + 1) * width_sample]) +
					abs(currents[i * width_sample] - currents[(i + 1) * width_sample])) / 2;
			}
			else
			{
				sample[i][j].currents = (abs(currents[i * width_sample + j - 1] - currents[i * width_sample + j]) +
					abs(currents[(i + 1) * width_sample + j - 1] - currents[(i + 1) * width_sample + j]) +
					abs(currents[i * width_sample + j - 1] - currents[(i + 1) * width_sample + j - 1]) +
					abs(currents[i * width_sample + j] - currents[(i + 1) * width_sample + j])) / 2;
			}
		}
	}

	if (is_draw_wind) Draw_wind(sample);
}

void Calculation_of_currents_SOR::Draw_wind(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_c = 99999.;
	double max_c = 0.;

	for (int i = 0; i < height_sample; i++) // minmax search
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (sample[i][j].currents > max_c) max_c = sample[i][j].currents;
			if (sample[i][j].currents < min_c) min_c = sample[i][j].currents;
		}
	}

	//std::cout << "\n\nmin of cur: " << min_c << '\n' << "max of cur: " << max_c << std::endl;

	for (int i = 0; i < height_sample; i++) // filling the pixel array, lower current darker pixel, higher current lighter pixel
	{
		for (int j = 0; j < width_sample; j++)
		{
			int intens = (sample[i][j].currents - min_c) * 255 / (max_c - min_c);
			SetPixel(BitmapMemory, j, i, width_sample, RGB(intens, intens, intens));
		}
	}

	Wind->Draw(BitmapMemory, width_sample, height_sample); // sending an array of pixels for rendering
}

Calculation_of_currents_SOR::~Calculation_of_currents_SOR() // memory release
{
	delete		Wind;
	delete[]	BitmapMemory;

	for (int i = 0; i < num_equations; i++)
		delete[] equations[i];

	delete[]	equations;

	delete[]	voltage;
	delete[]	currents;
}
