#include <iostream>

//------------------------
#include "Cell.h"
//-------------------------

#include "Culculation_of_currents.h"
#include "Calculation_of_effective_properties.h"
#include "Calculation_of_heat_distribution.h"

#include "Fast_culculation_of_currents.h"

//++++++++++++++++++++++++++++++++++++++
#include "Timer.h"

#include "Print_line.h"
//--------------------------------------

const double size_x = 20*1e-2; // in meters // dimensions of the sample, or pressing together with the matrix
const double size_y = 20*1e-2; // in meters


const int num_x = 100;

const double linear_cell_size = size_x / num_x; // the cell has a square shape, so it can be characterized by one parameter

const int num_y = size_y / linear_cell_size;


double U = 1. / num_x;



int main()
{
	Timer t;

	srand(time(NULL));

	Cell** all_cell = new Cell * [num_y]; // create arr of cells
	for (int i = 0; i < num_y; i++)
	{
		all_cell[i] = new Cell[num_x];
	}



	Calculation_of_effective_properties Resistances (num_x, num_y);
	
	Resistances.Array_initialization(all_cell, num_x, num_y, linear_cell_size);

	Resistances.Calculation_all_of_effective_properties(all_cell);

	//Resistances.Test_filling_resistances(all_cell);

	
	Culculation_of_currents Current(num_x, num_y);



	 
	//Fast_culculation_of_currents Current(num_x, num_y);

	//Current.Culculation(all_cell, U, false);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		double min_c = all_cell[0][0].currents;
		double max_c = all_cell[0][0].currents;

		double sum_c = 0;

		for (int i = 0; i < num_y; i++) // minmax search
		{
			for (int j = 0; j < num_x; j++)
			{
				if (all_cell[i][j].currents > max_c) max_c = all_cell[i][j].currents;
				if (all_cell[i][j].currents < min_c) min_c = all_cell[i][j].currents;
				sum_c += all_cell[i][j].currents;
			}
		}

		Print_line("Min cur.: " + std::to_string(min_c) + " Max cur.: " + std::to_string(max_c) + " Median cur.: " + std::to_string(sum_c / (num_x * num_y)), 1);

	}
	//--------------------------------------------------------

	Calculation_of_heat_distribution heat_distribution(num_x, num_y);
	double time = 0;
	while (true)
	{
		time += 0.01;
		heat_distribution.Culculation(all_cell, 0.01);

		Print_line(std::to_string(int(time / 3600)) + ':' + std::to_string(int(time) % 3600 / 60) + ':' + std::to_string(int(time) % 3600 % 60), 0);

		for (int i = 0; i < num_y; i++)
		{
			for (int j = 0; j < num_x; j++)
			{
				all_cell[i][j].swap();
			}
		}


		{
			double min_t = all_cell[0][0].new_temperature;
			double max_t = -all_cell[0][0].new_temperature;

			double sum_t = 0;

			for (int i = 0; i < num_y; i++) // minmax search
			{
				for (int j = 0; j < num_x; j++)
				{
					if (all_cell[i][j].new_temperature > max_t) max_t = all_cell[i][j].new_temperature;
					if (all_cell[i][j].new_temperature < min_t) min_t = all_cell[i][j].new_temperature;
					sum_t += all_cell[i][j].new_temperature;
				}
			}

			Print_line("Min temp.: " + std::to_string(min_t) + " Max temp.: " + std::to_string(max_t) + " Median temp.: " + std::to_string(sum_t / (num_x * num_y)), 2);

		}

		if (time > 3.)
		{
			std::cin.get();
		}


		//Sleep(1000);
		//std::cin.get();
	}
	std::cout << "end";
	std::cin.get();
	
	for (int i = 0; i < num_y; i++)
	{
		delete[] all_cell[i];
	}
	delete[] all_cell;

}