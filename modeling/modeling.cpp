#include <iostream>

//------------------------
#include "Cell.h"
//-------------------------

#include "Calculation_of_currents_Gauss.h"
#include "Calculation_of_currents_SOR.h"
#include "Calculation_of_effective_properties.h"
#include "Calculation_of_heat_distribution.h"

#include "data.h"


//++++++++++++++++++++++++++++++++++++++
#include "Timer.h"

#include "Print_line.h"
//--------------------------------------



int main()
{
	Timer t;

	srand(time(NULL));

	Cell** all_cell = new Cell * [height_sample]; // create arr of cells
	for (int i = 0; i < height_sample; i++)
	{
		all_cell[i] = new Cell[width_sample];
	}

	Calculation_of_effective_properties Resistances (false);
	
	Resistances.Array_initialization(all_cell);

	Resistances.Calculation_all_of_effective_properties(all_cell);

	Resistances.Test_filling_resistances(all_cell, true);

	//Calculation_of_currents_SOR Current (true);
	//Timer tt;
	//while (true)
	//{
	//	Current.Culculation(all_cell, true);
	//	std::cout << "time:\t" << tt.Dt() << std::endl;

	//	for (int i = 0; i < height_sample; i++) // generation of cell resistances (with neck)
	//	{
	//		for (int j = 0; j < width_sample; j++)
	//		{
	//			all_cell[i][j].resistances += all_cell[i][j].resistances * (rand() % 10 / 1000.) * ((rand() % 2 == 0) ? -1 : 1);
	//		}
	//	}
	//}

	//std::cin.get();
	//for (int i = 0; i < height_sample; i++) // generation of cell resistances (with neck)
	//{
	//	for (int j = 0; j < width_sample; j++)
	//	{
	//		all_cell[i][j].resistances += all_cell[i][j].resistances * (1. + rand() % 10 / 10.) * (rand() % 100 == 0) ? -1 : 1;
	//	}
	//}

	//Current.Culculation(all_cell, U, true);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		double min_c = all_cell[0][0].currents;
		double max_c = all_cell[0][0].currents;

		double sum_c = 0;

		for (int i = 0; i < height_sample; i++) // minmax search
		{
			for (int j = 0; j < width_sample; j++)
			{
				if (all_cell[i][j].currents > max_c) max_c = all_cell[i][j].currents;
				if (all_cell[i][j].currents < min_c) min_c = all_cell[i][j].currents;
				sum_c += all_cell[i][j].currents;
			}
		}

		Print_line("Min cur.: " + std::to_string(min_c) + " Max cur.: " + std::to_string(max_c) + " Median cur.: " + std::to_string(sum_c / (width_sample * height_sample)), 1);

	}
	//--------------------------------------------------------
	bool t5 = true;
	Timer time_of;
	int iter = 0;
	Calculation_of_heat_distribution heat_distribution (true);
	double max_del_t = 0;
	double time_step = 0.0001;
	double time = 0;
	while (true)
	{
		time += time_step;
		heat_distribution.Culculation(all_cell, time_step);

		Print_line(std::to_string(int(time / 3600)) + ':' + std::to_string(int(time) % 3600 / 60) + ':' + std::to_string(int(time) % 3600 % 60), 0);

		{
			double min_t = all_cell[0][0].new_temperature;
			double max_t = -all_cell[0][0].new_temperature;

			double sum_t = 0;

			max_del_t = 0;

			for (int i = 0; i < height_sample; i++) // minmax search
			{
				for (int j = 0; j < width_sample; j++)
				{
					if (all_cell[i][j].new_temperature > max_t) max_t = all_cell[i][j].new_temperature;
					if (all_cell[i][j].new_temperature < min_t) min_t = all_cell[i][j].new_temperature;
					sum_t += all_cell[i][j].new_temperature;
					if (i > 0 && i < height_sample - 1 && j > 0 && j < width_sample - 1)
					if (abs(all_cell[i][j].new_temperature - all_cell[i][j].old_temperature) > max_del_t) max_del_t = abs(all_cell[i][j].new_temperature - all_cell[i][j].old_temperature);
					if (max_del_t == 0) max_del_t = 0.1;
				}
			}

			double median = sum_t / (width_sample * height_sample);

			Print_line("Min temp.: " + std::to_string(min_t - 273) + " Max temp.: " + std::to_string(max_t - 273) + " Median temp.: " + std::to_string(median - 273), 2);

			Print_line("Time step: " + std::to_string(time_step) + ";\t\tMax temperature per step: " + std::to_string(max_del_t), 5);

			time_step = 0.0001 / max_del_t;

		}

		for (int i = 0; i < height_sample; i++)
		{
			for (int j = 0; j < width_sample; j++)
			{
				all_cell[i][j].swap();
			}
		}

		if (time_of.elapsed() > 1.)
		{
			Print_line("Iter per second:\t\t" + std::to_string(iter / time_of.Dt()), 7);
			iter = 0;
		}
		else
			iter++;

		//if (time > 60. * 2.5 && t5)
		//{
		//	std::cin.get();
		//	t5 = false;
		//}

		//Sleep(1000);
		//std::cin.get();
	}
	std::cout << "end";
	std::cin.get();
	
	for (int i = 0; i < height_sample; i++)
	{
		delete[] all_cell[i];
	}
	delete[] all_cell;

}