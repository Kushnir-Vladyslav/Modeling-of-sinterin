#include "Culculation_of_currents.h"


Culculation_of_currents::Culculation_of_currents(int width, int height) : Wind(L"Current distribution", 500, 500), width(width), height(height)
{
	num_equations = width * (height + 1); // counting the number of equations

	equations = new double* [num_equations]; // create arr for matrix
	for (int i = 0; i < num_equations; i++)
		equations[i] = new double[width * 2 + 1]{ 0 };

	currents = new double[num_equations] {0};

	BitmapMemory = (void*) new UINT32[width * height];
}

void Culculation_of_currents::Culculation(Cell** arr_of_cells, double voltage, bool is_draw_wind)
{
	for (int i = 0; i < num_equations; i++) // filling the matrix with coefficients
	{
		int i_y = i / (width);  // definition of the x-y coordinates of the contour, not the cell
		int i_x = i - (width)*i_y;

		if (i_x > 0) // left
		{
			if (i_y < height) equations[i][width - 1] -= arr_of_cells[i_y][i_x].resistances / 2;
			if (i_y > 0) equations[i][width - 1] -= arr_of_cells[i_y - 1][i_x].resistances / 2;
		}
		else
		{
			if (i_y < height) equations[i][width] += arr_of_cells[i_y][i_x].resistances / 2;
			if (i_y > 0) equations[i][width] += arr_of_cells[i_y - 1][i_x].resistances / 2;
		}
		if (i_y > 0) //upper
		{
			if (i_x != width - 1) equations[i][0] = -(arr_of_cells[i_y - 1][i_x].resistances + arr_of_cells[i_y - 1][i_x + 1].resistances) / 2;
			else equations[i][0] = -dielectric;
		}
		if (i_x < width - 1) // right
		{
			if (i_y < height) equations[i][width + 1] -= arr_of_cells[i_y][i_x + 1].resistances / 2;
			if (i_y > 0) equations[i][width + 1] -= arr_of_cells[i_y - 1][i_x + 1].resistances / 2;
		}
		if (i_y < height) // bottom
		{
			if (i_x != width - 1) equations[i][width * 2] = -(arr_of_cells[i_y][i_x].resistances + arr_of_cells[i_y][i_x + 1].resistances) / 2;
			else equations[i][width * 2] = -dielectric;
		}
		// self
		{
			equations[i][width] += -equations[i][width - 1] - equations[i][0] - equations[i][width + 1] - equations[i][width * 2];
		}
		// free members
		{
			if (i_x == width - 1) currents[i] = voltage / (height + 1.);
		}
	}

	for (int i = 0; i < num_equations; i++) // direct passage of the solution of the matrix
	{
		currents[i] /= equations[i][width];					// make 1 in diagonl
		for (int j = width * 2; j > width; j--)									 // sing > not a >= because we never more use this num 
		{
			equations[i][j] /= equations[i][width];
		}

		for (int j = 1; j <= width && i + j < num_equations; j++) // sub line to bottom
		{
			currents[i + j] -= equations[i + j][width - j] * currents[i]; // sub of cur
			for (int k = width * 2; k > width; k--)									// sing > not a >= because we never more use this num 
			{
				equations[i + j][k - j] -= equations[i + j][width - j] * equations[i][k];
			}
		}

	}

	for (int i = num_equations - 1; i > 0; i--) // inverse passage of the matrix solution
	{

		for (int j = 1; j <= width && i - j >= 0; j++) // sub line to top
		{
			currents[i - j] -= currents[i] * equations[i - j][width + j];
			//equations[i - j][width + j] = 0;											//this is commented because we never use this num
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

void Culculation_of_currents::Draw_wind(Cell** arr_of_cells)
{
	double min_c = 99999;
	double max_c = 0;

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

Culculation_of_currents::~Culculation_of_currents() // memory release
{ 
	delete[] BitmapMemory;

	for (int i = 0; i < num_equations; i++)
		delete[] equations[i];

	delete[] equations;

	delete[] currents;
}
