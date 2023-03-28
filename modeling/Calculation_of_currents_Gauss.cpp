#include "Calculation_of_currents_Gauss.h"

Calculation_of_currents_Gauss::Calculation_of_currents_Gauss(bool draw_window)
{
	if (draw_window)
		Wind = new Wind_wrap(L"Current distribution", width_window, height_window);
	else
		Wind = nullptr;

	num_equations = width_sample * (height_sample + 1); // counting the number of equations

	equations = new double* [num_equations]; // create arr for matrix
	for (int i = 0; i < num_equations; i++)
		equations[i] = new double[width_sample * 2 + 1]{ 0 };

	voltage = new double[num_equations] {0};

	currents = new double[num_equations] {0};

	BitmapMemory = (void *) new UINT32[width_sample * height_sample];
}

void Calculation_of_currents_Gauss::Culculation(Cell** sample, bool is_draw_wind)
{
	for (int i = 0; i < num_equations; i++)
		for (int j = 0; j < width_sample * 2 + 1; j++)
			equations[i][j] = 0.;

	for (int i = 0; i < num_equations; i++)
		voltage[i] = 0.;

	for (int i = 0; i < num_equations; i++) // filling the matrix with coefficients
	{
		int i_y = i / (width_sample);  // definition of the x-y coordinates of the contour, not the cell
		int i_x = i - (width_sample)*i_y;

		if (i_x > 0) // left
		{
			if (i_y < height_sample) equations[i][width_sample - 1] -= sample[i_y][i_x].resistances / 2;
			if (i_y > 0) equations[i][width_sample - 1] -= sample[i_y - 1][i_x].resistances / 2;
		}
		else
		{
			if (i_y < height_sample) equations[i][width_sample] += sample[i_y][i_x].resistances / 2;
			if (i_y > 0) equations[i][width_sample] += sample[i_y - 1][i_x].resistances / 2;
		}
		if (i_y > 0) //upper
		{
			if (i_x != width_sample - 1) equations[i][0] = -(sample[i_y - 1][i_x].resistances + sample[i_y - 1][i_x + 1].resistances) / 2;
			else equations[i][0] = -dielectric;
		}
		if (i_x < width_sample - 1) // right
		{
			if (i_y < height_sample) equations[i][width_sample + 1] -= sample[i_y][i_x + 1].resistances / 2;
			if (i_y > 0) equations[i][width_sample + 1] -= sample[i_y - 1][i_x + 1].resistances / 2;
		}
		if (i_y < height_sample) // bottom
		{
			if (i_x != width_sample - 1) equations[i][width_sample * 2] = -(sample[i_y][i_x].resistances + sample[i_y][i_x + 1].resistances) / 2;
			else equations[i][width_sample * 2] = -dielectric;
		}
		// self
		{
			equations[i][width_sample] += -equations[i][width_sample - 1] - equations[i][0] - equations[i][width_sample + 1] - equations[i][width_sample * 2];
		}
		// free members
		{
			if (i_x == width_sample - 1) voltage[i] = potential_difference / (height_sample + 1.);
		}
	}

	for (int i = 0; i < num_equations; i++) // direct passage of the solution of the matrix
	{
		voltage[i] /= equations[i][width_sample];					// make 1 in diagonl
		for (int j = width_sample * 2; j > width_sample; j--)									 // sing > not a >= because we never more use this num 
		{
			equations[i][j] /= equations[i][width_sample];
		}

		for (int j = 1; j <= width_sample && i + j < num_equations; j++) // sub line to bottom
		{
			voltage[i + j] -= equations[i + j][width_sample - j] * voltage[i]; // sub of cur
			for (int k = width_sample * 2; k > width_sample; k--)									// sing > not a >= because we never more use this num 
			{
				equations[i + j][k - j] -= equations[i + j][width_sample - j] * equations[i][k];
			}
		}

	}


	for (int i = num_equations - 1; i > 0; i--) // inverse passage of the matrix solution
	{
		for (int j = 1; j <= width_sample && i - j >= 0; j++) // sub line to top
		{
			voltage[i - j] -= voltage[i] * equations[i - j][width_sample + j];
			//equations[i - j][width_sample + j] = 0;											//this is commented because we never use this num
		}
	}

	for (int i = 0; i < num_equations; i++)
		currents[i] = voltage[i];

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

void Calculation_of_currents_Gauss::Draw_wind(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_c = 99999;
	double max_c = 0;

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

Calculation_of_currents_Gauss::~Calculation_of_currents_Gauss() // memory release
{ 
	delete		Wind;
	delete[]	BitmapMemory;

	for (int i = 0; i < num_equations; i++)
		delete[] equations[i];

	delete[]	equations;
	delete[]	voltage;
	delete[]	currents;
}
