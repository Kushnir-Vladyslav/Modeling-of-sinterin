#pragma once

#define size_x					(20. * 1e-2) // in meters // dimensions of the sample, or pressing together with the matrix
#define size_y					(20. * 1e-2) // in meters

#define width_sample			100 //number of cells in width

#define linear_cell_size		(size_x / width_sample) // the cell has a square shape, so it can be characterized by one parameter

#define height_sample			(size_y / linear_cell_size) //number of cells and height_sample

#define potential_difference	(1. / width_sample) //the potential is applied to the sample, it is assumed that the cross-section of the sample is square (width_sample õ width_sample)

//Window

#define width_window			500
#define height_window			500

//material
#define dielectric				(INT32_MAX / 3.)

#define initial_temperature		273.





