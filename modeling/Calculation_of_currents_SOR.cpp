#include "Calculation_of_currents_SOR.h"


Calculation_of_currents_SOR::Calculation_of_currents_SOR(int width, int height) : Wind(L"Current distribution", 500, 500), width(width), height(height)
{
	num_equations = width * (height + 1); // counting the number of equations

	equations = new double* [num_equations]; // create arr for matrix
	for (int i = 0; i < num_equations; i++)
		equations[i] = new double[5]{ 0. };

	voltage = new double[num_equations] {0.};
	currents = new double[num_equations] {0.};

	BitmapMemory = (void*) new UINT32[width * height];

	relax_p = (8 - sqrt(64 - 16 * pow(cos(3.14 / height) + cos(3.14 / width), 2))) / pow(cos(3.14 / height) + cos(3.14 / width), 2); // Value of relaxation parameter 
	tolerance = 1.e-10; // Tolerance.
	break_difference = 1.e-15;	
}

void Calculation_of_currents_SOR::Culculation(Cell** arr_of_cells, double potential_difference, bool is_draw_wind)
{
	for (int i = 0; i < num_equations; i++)
		for (int j = 0; j < 5; j++)
			equations[i][j] = 0.;

	for (int i = 0; i < num_equations; i++) // filling the matrix with coefficients
	{
		int i_y = i / (width);  // definition of the x-y coordinates of the contour, not the cell
		int i_x = i - (width)*i_y;

		if (i_x > 0) // left
		{
			if (i_y < height) equations[i][1] -= arr_of_cells[i_y][i_x].resistances / 2.;
			if (i_y > 0) equations[i][1] -= arr_of_cells[i_y - 1][i_x].resistances / 2.;
		}
		else
		{
			if (i_y < height) equations[i][2] += arr_of_cells[i_y][i_x].resistances / 2.;
			if (i_y > 0) equations[i][2] += arr_of_cells[i_y - 1][i_x].resistances / 2.;
		}
		if (i_y > 0) //upper
		{
			if (i_x != width - 1) equations[i][0] = -(arr_of_cells[i_y - 1][i_x].resistances + arr_of_cells[i_y - 1][i_x + 1].resistances) / 2.;
			else equations[i][0] = -dielectric;
		}
		if (i_x < width - 1) // right
		{
			if (i_y < height) equations[i][3] -= arr_of_cells[i_y][i_x + 1].resistances / 2.;
			if (i_y > 0) equations[i][3] -= arr_of_cells[i_y - 1][i_x + 1].resistances / 2.;
		}
		if (i_y < height) // bottom
		{
			if (i_x != width - 1) equations[i][4] = -(arr_of_cells[i_y][i_x].resistances + arr_of_cells[i_y][i_x + 1].resistances) / 2.;
			else equations[i][4] = -dielectric;
		}
		// self
		{
			equations[i][2] += -equations[i][1] - equations[i][0] - equations[i][3] - equations[i][4];
		}
		// free members
		{
			if (i_x == width - 1) voltage[i] = potential_difference / (height + 1.);
		}
	}

	static bool First_time = true;

	if (First_time)
	{
		for (int i = 0; i < width; i++)
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

			if (i - width >= 0) S += equations[i][0] * currents[i - width];
			if (i - 1 >= 0) S += equations[i][1] * currents[i - 1];
			S += equations[i][2] * currents[i];
			if (i + 1 < num_equations) S += equations[i][3] * currents[i + 1];
			if (i + width < num_equations) S += equations[i][4] * currents[i + width];

			currents[i] += relax_p * (voltage[i] - S) / equations[i][2];
		}


		for (int i = 0; i < num_equations; i++)
		{
			double S = 0.;

			if (i - width >= 0) S -= equations[i][0] * currents[i - width];
			if (i - 1 >= 0)S -= equations[i][1] * currents[i - 1];
			S -= equations[i][2] * currents[i];
			if (i + 1 < num_equations) S -= equations[i][3] * currents[i + 1];
			if (i + width < num_equations) S -= equations[i][4] * currents[i + width];


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
			//	for (int i = 0; i < height; i++) // calculation of currents flowing in nodes
			//	{
			//		for (int j = 0; j < width; j++)
			//		{
			//			if (j == 0)
			//			{
			//				arr_of_cells[i][j].currents = (abs(currents[i * width]) +
			//					abs(currents[(i + 1) * width]) +
			//					abs(currents[i * width] - currents[(i + 1) * width])) / 2;
			//			}
			//			else
			//			{
			//				arr_of_cells[i][j].currents = (abs(currents[i * width + j - 1] - currents[i * width + j]) +
			//					abs(currents[(i + 1) * width + j - 1] - currents[(i + 1) * width + j]) +
			//					abs(currents[i * width + j - 1] - currents[(i + 1) * width + j - 1]) +
			//					abs(currents[i * width + j] - currents[(i + 1) * width + j])) / 2;
			//			}
			//		}
			//	}
			//	if (is_draw_wind) Draw_wind(arr_of_cells);
			//}
		}
	}


	for (int i = 0; i < height; i++) // calculation of currents flowing in nodes
	{
		for (int j = 0; j < width; j++)
		{
			if (j == 0)
			{
				arr_of_cells[i][j].currents = (abs(currents[i * width]) +
					abs(currents[(i + 1) * width]) +
					abs(currents[i * width] - currents[(i + 1) * width])) / 2;
			}
			else
			{
				arr_of_cells[i][j].currents = (abs(currents[i * width + j - 1] - currents[i * width + j]) +
					abs(currents[(i + 1) * width + j - 1] - currents[(i + 1) * width + j]) +
					abs(currents[i * width + j - 1] - currents[(i + 1) * width + j - 1]) +
					abs(currents[i * width + j] - currents[(i + 1) * width + j])) / 2;
			}
		}
	}

	if (is_draw_wind) Draw_wind(arr_of_cells);
}

void Calculation_of_currents_SOR::Draw_wind(Cell** arr_of_cells)
{
	double min_c = 99999.;
	double max_c = 0.;

	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			if (arr_of_cells[i][j].currents > max_c) max_c = arr_of_cells[i][j].currents;
			if (arr_of_cells[i][j].currents < min_c) min_c = arr_of_cells[i][j].currents;
		}
	}

	//std::cout << "\n\nmin of cur: " << min_c << '\n' << "max of cur: " << max_c << std::endl;

	for (int i = 0; i < height; i++) // filling the pixel array, lower current darker pixel, higher current lighter pixel
	{
		for (int j = 0; j < width; j++)
		{
			int intens = (arr_of_cells[i][j].currents - min_c) * 255 / (max_c - min_c);
			SetPixel(BitmapMemory, j, i, width, RGB(intens, intens, intens));
		}
	}

	Wind.Draw(BitmapMemory, width, height); // sending an array of pixels for rendering
}

Calculation_of_currents_SOR::~Calculation_of_currents_SOR() // memory release
{
	delete[] BitmapMemory;

	for (int i = 0; i < num_equations; i++)
		delete[] equations[i];

	delete[] equations;

	delete[] voltage;
	delete[] currents;
}
