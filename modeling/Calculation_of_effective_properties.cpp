#include "Calculation_of_effective_properties.h"


Calculation_of_effective_properties::Calculation_of_effective_properties(int width, int height): Wind(L"Resistances distribution", 500, 500), width(width), height(height)
{
	BitmapMemory = (void*) new UINT32[width * height];
}

void Calculation_of_effective_properties::Array_initialization(Cell** arr_of_cells, int width, int height, double linear_cell_size)
{
	double volume_cell = pow(linear_cell_size, 3);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//set geometric parameters
			arr_of_cells[i][j].volume = volume_cell;
			arr_of_cells[i][j].linear_size = linear_cell_size;


			// set materials

			//arr_of_cells[i][j].material = (rand() % 100 < 3) ? &Porosity : &Titanium;
			arr_of_cells[i][j].material = &Titanium;
			if (j > width / 3 && j < width * 2 / 3) arr_of_cells[i][j].material = &Copper;
		}
	}
}

void Calculation_of_effective_properties::Calculation_all_of_effective_properties(Cell** arr_of_cells, What_draw what_draw)
{
	Culculation_resistances(arr_of_cells, (what_draw == DR_RESISTANCES));

	Culculation_density(arr_of_cells, what_draw == DR_DENSITY);

	Culculation_heat_capacity(arr_of_cells, what_draw == DR_HEAT_CAPACITY);

	Culculation_thermal_conductivity(arr_of_cells, what_draw == DR_THERMAL_CONDUCTIVITY);
}

void Calculation_of_effective_properties::Culculation_resistances(Cell** arr_of_cells, bool is_draw_wind)
{
	for (int i = 0; i < height; i++) // resistance filling according to the material and cell volume
	{
		for (int j = 0; j < width; j++)
		{

			arr_of_cells[i][j].resistances = arr_of_cells[i][j].material->resistances(0) / arr_of_cells[i][j].linear_size;
		}
	}

	if (is_draw_wind) Draw_wind_resistances(arr_of_cells);
}

void Calculation_of_effective_properties::Draw_wind_resistances(Cell** arr_of_cells)
{
	double min_r = 99999;
	double max_r = 0;

	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			if (arr_of_cells[i][j].resistances > max_r) max_r = arr_of_cells[i][j].resistances;
			if (arr_of_cells[i][j].resistances < min_r) min_r = arr_of_cells[i][j].resistances;
		}
	}

	for (int i = 0; i < height; i++) // filling the pixel array, lower resistances redder pixel, higher resistances bluer pixel
	{
		for (int j = 0; j < width; j++)
		{
			int intens = (arr_of_cells[i][j].resistances - min_r) * 255 / (max_r - min_r);
			SetPixel(BitmapMemory, j, i, width, RGB(intens, 0, 255 - intens));
		}
	}

	Wind.Draw(BitmapMemory, width, height); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Culculation_density(Cell** arr_of_cells, bool is_draw_wind)
{
	for (int i = 0; i < height; i++) // filling according to the material and cell volume
	{
		for (int j = 0; j < width; j++)
		{
			arr_of_cells[i][j].density = arr_of_cells[i][j].material->density(0);
		}
	}

	if (is_draw_wind) Draw_wind_density(arr_of_cells);
}

void Calculation_of_effective_properties::Draw_wind_density(Cell** arr_of_cells)
{
	double min_d = 99999;
	double max_d = 0;

	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			if (arr_of_cells[i][j].density > max_d) max_d = arr_of_cells[i][j].density;
			if (arr_of_cells[i][j].density < min_d) min_d = arr_of_cells[i][j].density;
		}
	}

	for (int i = 0; i < height; i++) 
	{
		for (int j = 0; j < width; j++)
		{
			int intens = (arr_of_cells[i][j].density - min_d) * 255 / (max_d - min_d);
			SetPixel(BitmapMemory, j, i, width, RGB(intens, 0, 255 - intens));
		}
	}

	Wind.Draw(BitmapMemory, width, height); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Culculation_heat_capacity(Cell** arr_of_cells, bool is_draw_wind)
{
	for (int i = 0; i < height; i++) // filling according to the material and cell volume
	{
		for (int j = 0; j < width; j++)
		{
			arr_of_cells[i][j].heat_capacity = arr_of_cells[i][j].material->heat_capacity(0);
		}
	}

	if (is_draw_wind) Draw_wind_heat_capacity(arr_of_cells);
}

void Calculation_of_effective_properties::Draw_wind_heat_capacity(Cell** arr_of_cells)
{
	double min_hp = 99999;
	double max_hp = 0;

	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			if (arr_of_cells[i][j].heat_capacity > max_hp) max_hp = arr_of_cells[i][j].heat_capacity;
			if (arr_of_cells[i][j].heat_capacity < min_hp) min_hp = arr_of_cells[i][j].heat_capacity;
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int intens = (arr_of_cells[i][j].heat_capacity - min_hp) * 255 / (max_hp - min_hp);
			SetPixel(BitmapMemory, j, i, width, RGB(intens, 0, 255 - intens));
		}
	}

	Wind.Draw(BitmapMemory, width, height); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Culculation_thermal_conductivity(Cell** arr_of_cells, bool is_draw_wind)
{
	for (int i = 0; i < height; i++) // filling according to the material and cell volume
	{
		for (int j = 0; j < width; j++)
		{
			arr_of_cells[i][j].thermal_conductivity = arr_of_cells[i][j].material->thermal_conductivity(0);
		}
	}

	if (is_draw_wind) Draw_wind_thermal_conductivity(arr_of_cells);
}

void Calculation_of_effective_properties::Draw_wind_thermal_conductivity(Cell** arr_of_cells)
{
	double min_tc = 99999;
	double max_tc = 0;

	for (int i = 0; i < height; i++) // minmax search
	{
		for (int j = 0; j < width; j++)
		{
			if (arr_of_cells[i][j].thermal_conductivity > max_tc) max_tc = arr_of_cells[i][j].thermal_conductivity;
			if (arr_of_cells[i][j].thermal_conductivity < min_tc) min_tc = arr_of_cells[i][j].thermal_conductivity;
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int intens = (arr_of_cells[i][j].thermal_conductivity - min_tc) * 255 / (max_tc - min_tc);
			SetPixel(BitmapMemory, j, i, width, RGB(intens, 0, 255 - intens));
		}
	}

	Wind.Draw(BitmapMemory, width, height); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Test_filling_resistances(Cell** arr_of_cells, bool is_draw_wind)
{

	for (int i = 0; i < height; i++) // resistance filling according to the material and cell volume
	{
		for (int j = 0; j < width; j++)
		{

			arr_of_cells[i][j].resistances = arr_of_cells[i][j].material->resistances(0) / arr_of_cells[i][j].linear_size;
		}
	}

	//for (int i = 0; i < height; i++) // random generation of cell resistances
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		double r = rand() % 100;
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height; i++) // generation of cell resistances (Little big resistance)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		double r = Titanium.resistances(0);
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height; i++) // generation of cell resistances (Little big resistance)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		//double r = (rand() % 100 > 50) ? Porosity.resistances(0) : Titanium.resistances(0);
	//		double r = (rand() % 100 > 50) ? 1 : 2;
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height; i++) // generation of cell resistances (with neck)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		double r = 1;
	//		if (i > height / 2 - height / 10 && i < height / 2 + height / 10 &&
	//			(j < width / 2 - width / 10 || j > width / 2 + width / 10)) r = dielectric;
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height; i++) // generation of cell resistances (non-conductive bar)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		double r = 1;
	//		if (i > height / 2 - height / 10 && i < height / 2 + height / 10) r = dielectric;
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height; i++) // generation of cell resistances (circle in the center)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		double r = 1;
	//		if (abs(pow(height / 2 - j, 2) + pow(height / 2 - i, 2) - pow(width / 3, 2)) <= width / 2 && i > height / 4) r = dielectric;
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height; i++) // generation of cell resistances (chessboard)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		double r = ((i * (height - 1) + j) % 2) ? 1 : 100;
	//		arr_of_cells[i][j].resistances = r;
	//	}
	//}

	if (is_draw_wind) Draw_wind_resistances(arr_of_cells);
}

Calculation_of_effective_properties::~Calculation_of_effective_properties()
{
	delete[] BitmapMemory;
}
