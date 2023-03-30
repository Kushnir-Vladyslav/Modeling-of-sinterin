#pragma once

#define size_x					(20. * 1e-2) // in meters // dimensions of the sample, or pressing together with the matrix
#define size_y					(20. * 1e-2) // in meters

#define width_sample			50 //number of cells in width

#define linear_cell_size		(size_x / width_sample) // the cell has a square shape, so it can be characterized by one parameter

#define height_sample			(size_y / linear_cell_size) //number of cells and height_sample

#define potential_difference	(1. / width_sample) //the potential is applied to the sample, it is assumed that the cross-section of the sample is square (width_sample õ width_sample)

//Window

#define width_window			500
#define height_window			500

//material
#define dielectric				(INT32_MAX / 3.)
#define infinity_heat_resistor	(INT32_MAX / 3.)


// termal
#define Zero_Celsius			273.15
#define initial_temperature		(Zero_Celsius + 0.)

enum Type_heat_exchange
{
	THE_FLOW,		// in volume
	THE_STATIC,		// this and all next, at the border	
	THE_RADIATION,
	THE_CONVECTION,
	THE_MIXED,
	THE_NON,
};

// up
#define UP_Type_heat_exchange	THE_STATIC
#define UP_Static_temperature	(Zero_Celsius + 1000)
// down
#define DOWN_Type_heat_exchange	THE_STATIC
#define DOWN_Static_temperature	initial_temperature
// left
#define LEFT_Type_heat_exchange	THE_NON
#define LEFT_Static_temperature	(Zero_Celsius + 1000)
// right
#define RIGHT_Type_heat_exchange THE_NON
#define RIGHT_Static_temperature initial_temperature




