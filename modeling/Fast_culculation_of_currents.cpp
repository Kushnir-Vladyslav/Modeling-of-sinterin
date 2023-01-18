#include "Fast_culculation_of_currents.h"

Fast_culculation_of_currents::Fast_culculation_of_currents(int width, int height) : Wind(L"Current distribution", 500, 500), width(width), height(height)
{
	num_equations = (width / 2 + 1)* (height + 1); // counting the number of equations

	equations = new double* [num_equations]; // create arr for matrix
	for (int i = 0; i < num_equations; i++)
		equations[i] = new double[width + 5]{ 0 };

	currents = new double[num_equations] {0};

	BitmapMemory = (void*) new UINT32[width * height];
}

void Fast_culculation_of_currents::Culculation(Cell** arr_of_cells, double voltage, bool is_draw_wind)
{
	for (int i = 0; i < num_equations; i++) // filling the matrix with coefficients
	{
		int i_y = i / (width / 2 + 1);  // definition of the x-y coordinates of the contour, not the cell
		int i_x = i - (width / 2 + 1) * i_y;

		if (i_y > 0) // upper_left
		{
			if (i_y % 2 == 0) //парні рядки, здвинуті в право
			{
				if (i_x < width / 2) // в останньому контурі з ліва великий опір
				{
					equations[i][1] -= arr_of_cells[i_y - 1][i_x * 2 + 1].resistances;
				}
				else
				{
					equations[i][1] -= dielectric;
				}
			}
			else // не парні рядки, здвинуті в ліво
			{
				if (i_x > 0) // лівий не парний контур не мижує ні зким з ліва
				{
					if (i_x < width / 2) // в останньому окнтурі з ліва великий опір
					{
						equations[i][0] -= arr_of_cells[i_y - 1][i_x * 2].resistances;
					}
					else
					{
						equations[i][0] -= dielectric;
					}
				}
				else // не межує але треба додати до суми контуру
				{
					equations[i][width / 2 + 2] += arr_of_cells[i_y - 1][0].resistances;
				}
			}
		}
		if (i_y > 0) //upper_rigt
		{
			if (i_y % 2 == 0) // парні рядки
			{
				if (i_x < width / 2) // в отанній клітинці немає правої верньої
				{
					if (i_x < width / 2 - 1) // всі окрім передостанньої бо там великий опір
					{
						equations[i][2] -= arr_of_cells[i_y - 1][i_x * 2 + 2].resistances;
					}
					else
					{
						equations[i][2] -= dielectric;
					}
				}
			}
			else // непарні рядки
			{
				if (i_x < width / 2) // всі окрім останнього контуру де великий опір
				{
					equations[i][1] -= arr_of_cells[i_y - 1][i_x * 2 + 1].resistances;
				}
				else
				{
					equations[i][1] -= dielectric;
				}
			}
		}
		if (i_y < height) // bottom_left 
		{
			if (i_y % 2 == 0) // парні рядки
			{
				if (i_x < width / 2) // всі окрім останнього контуру бо там великий опір
				{
					equations[i][width + 3] -= arr_of_cells[i_y][i_x * 2 + 1].resistances;
				}
				else
				{
					equations[i][width + 3] -= dielectric;
				}
			}
			else // не парні рядки
			{
				if (i_x > 0) // лівий контур не має сусіда
				{
					if (i_x < width / 2) // усі окрім останнього контуру там високий опір
					{
						equations[i][width + 2] -= arr_of_cells[i_y][i_x * 2].resistances;
					}
					else
					{
						equations[i][width + 2] -= dielectric;
					}
				}
				else // немає усіда еле потрібно додати до суми
				{
					equations[i][width / 2 + 2] += arr_of_cells[i_y][0].resistances;
				}
			}
		}
		if (i_y < height) // bottom_right
		{
			if (i_y % 2 == 0) // парні рядки
			{
				if (i_x < width / 2) // всі окрім правої там немає опору
				{
					if (i_x < width / 2 - 1) // всі окрім передостанньої, там великий опір
					{
						equations[i][width + 4] -= arr_of_cells[i_y][i_x * 2 + 2].resistances;
					}
					else
					{
						equations[i][width + 4] -= dielectric;
					}
				}
			}
			else // не парні рядки
			{
				if (i_x < width / 2) // всі окрім останнього, там великий опір
				{
					equations[i][width + 3] -= arr_of_cells[i_y][i_x * 2 + 1].resistances;
				}
				else
				{
					equations[i][width + 3] -= dielectric;
				}
			}
		}
		// self
		{
			equations[i][width / 2 + 2] += -equations[i][0] - equations[i][1] - equations[i][2] - equations[i][width + 2] - equations[i][width + 3] - equations[i][width + 4];
		}
		// free members
		{
			if (i_y % 2 == 0 && i_x == width / 2) currents[i] = voltage / (height / 2 + 1.);
		}
	}

	for (int i = 0; i < num_equations; i++) // direct passage of the solution of the matrix
	{
		currents[i] /= equations[i][width / 2 + 2];					// make 1 in diagonl
		for (int j = width + 4; j > width / 2 + 2; j--)									 // sing > not a >= becous we neve more use this num 
		{
			equations[i][j] /= equations[i][width / 2 + 2];
		}

		for (int j = 1; j <= width / 2 + 2 && i + j < num_equations; j++) // sub line to bottom
		{
			currents[i + j] -= equations[i + j][width / 2 + 2 - j] * currents[i]; // sub of cur
			for (int k = width + 4; k > width / 2 + 2; k--)									// sing > not a >= becous we neve more use this num 
			{
				equations[i + j][k - j] -= equations[i + j][width / 2 + 2 - j] * equations[i][k];
			}
		}

	}

	for (int i = num_equations - 1; i > 0; i--) // inverse passage of the matrix solution
	{

		for (int j = 1; j <= width / 2 + 2 && i - j >= 0; j++) // sub line to top
		{
			currents[i - j] -= currents[i] * equations[i - j][width / 2 + 2 + j];
			//equations[i - j][width + j] = 0;											// this is comented becous we neve more use this num 
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//std::cout << std::setprecision(12) << std::endl;

	//for (int i = 0; i < num_equations; i++)
	//{
	//	//for (int j = width / 2 + 2; j < width / 2 + 3; j++)
	//	//{
	//	//	std::cout << equations[i][j] << '\t';
	//	//}
	//	std::cout << currents[i] ;
	//	std::cout << '\n';
	//}

	//std::cin.get();
	//system("clear");
	//-----------------------------------------------------------------------------------------

	for (int i = 0; i < height; i++) // calculation of currents flowing in nodes
	{
		for (int j = 0; j < width; j++)
		{
			if (j == 0)
			{
				arr_of_cells[i][j].currents = abs(currents[(width / 2 + 1) * ((i / 2) * 2 + 1)]); // ліві дорівнюють потоку в комірці
			}
			else
			{
				if (i % 2 == 0) // парні рядки
				{
					if (j % 2 == 0) // парні стовбчики
					{
						arr_of_cells[i][j].currents = abs(currents[(width / 2 + 1) * i + j / 2 - 1] - currents[(width / 2 + 1) * (i + 1) + j / 2]);
					}
					else // не парні стовбчики
					{
						arr_of_cells[i][j].currents = abs(currents[(width / 2 + 1) * i + j / 2] - currents[(width / 2 + 1) * (i + 1) + j / 2]);
					}
				}
				else // не парні рядки
				{
					if (j % 2 == 0) // парні стовбчики
					{
						arr_of_cells[i][j].currents = abs(currents[(width / 2 + 1) * i + j / 2] - currents[(width / 2 + 1) * (i + 1) + j / 2 - 1]);
					}
					else // не парні стовбчики
					{
						arr_of_cells[i][j].currents = abs(currents[(width / 2 + 1) * i + j / 2] - currents[(width / 2 + 1) * (i + 1) + j / 2]);
					}
				}
			}
		}
	}

	if (is_draw_wind) Draw_wind(arr_of_cells);
}

void Fast_culculation_of_currents::Draw_wind(Cell** arr_of_cells)
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

Fast_culculation_of_currents::~Fast_culculation_of_currents()
{
	delete[] BitmapMemory;

	for (int i = 0; i < num_equations; i++)
		delete[] equations[i];

	delete[] equations;

	delete[] currents;
}
