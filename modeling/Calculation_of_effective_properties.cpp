#include "Calculation_of_effective_properties.h"


Calculation_of_effective_properties::Calculation_of_effective_properties(bool draw_window)
{
	if (draw_window)
		Wind = new Wind_wrap(L"Some properties", width_window, height_window);
	else
		Wind = nullptr;

	BitmapMemory = (void *) new UINT32[width_sample * height_sample];
}

void Calculation_of_effective_properties::Array_initialization(Cell** sample)
{
	double volume_cell = pow(linear_cell_size, 3);
	for (int i = 0; i < height_sample; i++)
	{
		for (int j = 0; j < width_sample; j++)
		{
			//set geometric parameters
			sample[i][j].volume = volume_cell;
			sample[i][j].linear_size = linear_cell_size;


			// set materials

			//sample[i][j].material = (rand() % 100 < 3) ? &Porosity : &Titanium;
			sample[i][j].material = &Titanium;
			if (j > width_sample / 3 && j < width_sample * 2 / 3) sample[i][j].material = &Copper;
		}
	}
}

void Calculation_of_effective_properties::Calculation_all_of_effective_properties(Cell** sample, What_draw what_draw)
{
	Culculation_resistances(sample, (what_draw == DR_RESISTANCES));

	Culculation_density(sample, what_draw == DR_DENSITY);

	Culculation_heat_capacity(sample, what_draw == DR_HEAT_CAPACITY);

	Culculation_thermal_conductivity(sample, what_draw == DR_THERMAL_CONDUCTIVITY);
}

void Calculation_of_effective_properties::Culculation_resistances(Cell** sample, bool is_draw_wind)
{
	for (int i = 0; i < height_sample; i++) // resistance filling according to the material and cell volume
	{
		for (int j = 0; j < width_sample; j++)
		{

			sample[i][j].resistances = sample[i][j].material->resistances(0) / sample[i][j].linear_size;
		}
	}

	if (is_draw_wind) Draw_wind_resistances(sample);
}

void Calculation_of_effective_properties::Draw_wind_resistances(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_r = 99999;
	double max_r = 0;

	for (int i = 0; i < height_sample; i++) // minmax search
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (sample[i][j].resistances > max_r) max_r = sample[i][j].resistances;
			if (sample[i][j].resistances < min_r) min_r = sample[i][j].resistances;
		}
	}

	for (int i = 0; i < height_sample; i++) // filling the pixel array, lower resistances redder pixel, higher resistances bluer pixel
	{
		for (int j = 0; j < width_sample; j++)
		{
			int intens = (sample[i][j].resistances - min_r) * 255 / (max_r - min_r);
			SetPixel(BitmapMemory, j, i, width_sample, RGB(intens, 0, 255 - intens));
		}
	}

	Wind->Draw(BitmapMemory, width_sample, height_sample); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Culculation_density(Cell** sample, bool is_draw_wind)
{
	for (int i = 0; i < height_sample; i++) // filling according to the material and cell volume
	{
		for (int j = 0; j < width_sample; j++)
		{
			sample[i][j].density = sample[i][j].material->density(0);
		}
	}

	if (is_draw_wind) Draw_wind_density(sample);
}

void Calculation_of_effective_properties::Draw_wind_density(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_d = 99999;
	double max_d = 0;

	for (int i = 0; i < height_sample; i++) // minmax search
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (sample[i][j].density > max_d) max_d = sample[i][j].density;
			if (sample[i][j].density < min_d) min_d = sample[i][j].density;
		}
	}

	for (int i = 0; i < height_sample; i++) 
	{
		for (int j = 0; j < width_sample; j++)
		{
			int intens = (sample[i][j].density - min_d) * 255 / (max_d - min_d);
			SetPixel(BitmapMemory, j, i, width_sample, RGB(intens, 0, 255 - intens));
		}
	}

	Wind->Draw(BitmapMemory, width_sample, height_sample); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Culculation_heat_capacity(Cell** sample, bool is_draw_wind)
{
	for (int i = 0; i < height_sample; i++) // filling according to the material and cell volume
	{
		for (int j = 0; j < width_sample; j++)
		{
			sample[i][j].heat_capacity = sample[i][j].material->heat_capacity(0);
		}
	}

	if (is_draw_wind) Draw_wind_heat_capacity(sample);
}

void Calculation_of_effective_properties::Draw_wind_heat_capacity(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_hp = 99999;
	double max_hp = 0;

	for (int i = 0; i < height_sample; i++) // minmax search
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (sample[i][j].heat_capacity > max_hp) max_hp = sample[i][j].heat_capacity;
			if (sample[i][j].heat_capacity < min_hp) min_hp = sample[i][j].heat_capacity;
		}
	}

	for (int i = 0; i < height_sample; i++)
	{
		for (int j = 0; j < width_sample; j++)
		{
			int intens = (sample[i][j].heat_capacity - min_hp) * 255 / (max_hp - min_hp);
			SetPixel(BitmapMemory, j, i, width_sample, RGB(intens, 0, 255 - intens));
		}
	}

	Wind->Draw(BitmapMemory, width_sample, height_sample); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Culculation_thermal_conductivity(Cell** sample, bool is_draw_wind)
{
	for (int i = 0; i < height_sample; i++) // filling according to the material and cell volume
	{
		for (int j = 0; j < width_sample; j++)
		{
			sample[i][j].thermal_conductivity = sample[i][j].material->thermal_conductivity(0);
		}
	}

	if (is_draw_wind) Draw_wind_thermal_conductivity(sample);
}

void Calculation_of_effective_properties::Draw_wind_thermal_conductivity(Cell** sample)
{
	if (Wind == nullptr) return;

	double min_tc = 99999;
	double max_tc = 0;

	for (int i = 0; i < height_sample; i++) // minmax search
	{
		for (int j = 0; j < width_sample; j++)
		{
			if (sample[i][j].thermal_conductivity > max_tc) max_tc = sample[i][j].thermal_conductivity;
			if (sample[i][j].thermal_conductivity < min_tc) min_tc = sample[i][j].thermal_conductivity;
		}
	}

	for (int i = 0; i < height_sample; i++)
	{
		for (int j = 0; j < width_sample; j++)
		{
			int intens = (sample[i][j].thermal_conductivity - min_tc) * 255 / (max_tc - min_tc);
			SetPixel(BitmapMemory, j, i, width_sample, RGB(intens, 0, 255 - intens));
		}
	}

	Wind->Draw(BitmapMemory, width_sample, height_sample); // sending an array of pixels for rendering
}

void Calculation_of_effective_properties::Test_filling_resistances(Cell** sample, bool is_draw_wind)
{

	for (int i = 0; i < height_sample; i++) // resistance filling according to the material and cell volume
	{
		for (int j = 0; j < width_sample; j++)
		{

			sample[i][j].resistances = sample[i][j].material->resistances(0) / sample[i][j].linear_size;
		}
	}

	//for (int i = 0; i < height_sample; i++) // random generation of cell resistances
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		double r = rand() % 100;
	//		sample[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (Little big resistance)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		double r = Titanium.resistances(0);
	//		sample[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (Little big resistance)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		//double r = (rand() % 100 > 50) ? Porosity.resistances(0) : Titanium.resistances(0);
	//		double r = (rand() % 100 > 50) ? 1 : 100;
	//		sample[i][j].resistances = r;
	//	}
	//}

	for (int i = 0; i < height_sample; i++) // generation of cell resistances (with neck)
	{
		for (int j = 0; j < width_sample; j++)
		{
			double r = 1;
			if (i > height_sample / 2 - height_sample / 10 && i < height_sample / 2 + height_sample / 10 &&
				(j < width_sample / 2 - width_sample / 10 || j > width_sample / 2 + width_sample / 10)) r = dielectric;
			sample[i][j].resistances = r;
		}
	}

	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (non-conductive bar)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		double r = 1;
	//		if (i > height_sample / 2 - height_sample / 10 && i < height_sample / 2 + height_sample / 10) r = dielectric;
	//		sample[i][j].resistances = r;
	//	}
	//}


	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (cut circle in the center)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		double r = 1;
	//		if (abs(pow(height_sample / 2 - j, 2) + pow(height_sample / 2 - i, 2) - pow(width_sample / 3, 2)) <= width_sample / 2 && i > height_sample / 4) r = dielectric;
	//		sample[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (circle in the center)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		double r = 1;
	//		if (abs(pow(height_sample / 2 - j, 2) + pow(height_sample / 2 - i, 2) - pow(width_sample / 3, 2)) <= width_sample / 2) r = dielectric;
	//		sample[i][j].resistances = r;
	//	}
	//}

	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (chessboard)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		double r = ((i * (height_sample - 1) + j) % 2) ? 1 : 100;
	//		sample[i][j].resistances = r;
	//	}
	//}

	if (is_draw_wind) Draw_wind_resistances(sample);
}

Calculation_of_effective_properties::~Calculation_of_effective_properties()
{
	delete		Wind;
	delete[]	BitmapMemory;
}
