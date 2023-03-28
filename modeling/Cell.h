#pragma once
#include <algorithm> 
#include "Materials.h"

//+++++++++++++++++++++++++++++++
#include <random>
//-------------------------------

class Cell
{
public:
	// electrical properties
	double resistances = 0.; // electrical resistance of the cell
	double currents = 0.; // electric current flowing through the cell

	// thermal/thermodynamic properties
	double density = 0.; // effective density of material in the cell g/m3
	double heat_capacity = 0.; // effective heat capacity of the material in the cell
	double thermal_conductivity = 0.; // effective thermal conductivity of the material in the cell

	// amount of void
	double porosity = 0; // pore content in the cell % (0 - 100), sum of all gases

	// block of variables that are enumerated at each step and depend on previous values
	double new_temperature = 273.; // the cell temperature is calculated in the last step
	double old_temperature = 273.; // cell temperature, which will be calculated in the next step

	// geometric parameters, are constant values and do not change over time
	double volume = 0.;
	double linear_size = 0.; // the cell has a square shape, so it can be characterized by one parameter

	Material* material; // cell material, in the future it will be an array of several materials with an indication of their contents and average grain size, or its distribution

	void swap() // after the end of the cycle, the values for which the values of the previous step are needed for calculation are swapped
	{
		std::swap(new_temperature, old_temperature);
	}

};

