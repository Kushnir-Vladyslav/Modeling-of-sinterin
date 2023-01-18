#pragma once

using function = double (*)(double);

enum State
{
	ST_SOLID,
	ST_LIQUID,
	ST_GAS
};

class Material
{
public:
	Material(function resistances, function density, function heat_capacity, function thermal_conductivity, State state) :
		resistances(resistances), density(density), heat_capacity(heat_capacity), thermal_conductivity(thermal_conductivity), state(state) {}

	function resistances; // electrical resistivity of the material, ohm / m (ohm * m^2 / m) (R*S/l)

	function density; // density of material, g/m3 (m/v)
	function heat_capacity; // Specific heat capacity of the material, J/kg·K (Q/(m*dT))
	function thermal_conductivity; // thermal conductivity of the material, W/(m·K)

	State state;
};

static Material Porosity([](double temperature) {return double (INT32_MAX / 3); },
				[](double temperature) {return 1.; },
				[](double temperature) {return  double(INT32_MAX / 3); },
				[](double temperature) {return 0.; },
				ST_GAS);

static Material Titanium([](double temperature) {return 0.66995 * 1e-6 * 1e-2; },
				[](double temperature) {return 4.54 * 1e3; },
				[](double temperature) {return 540.; },
				[](double temperature) {return 21.9; },
				ST_SOLID);

static Material Copper([](double temperature) {return 0.01724 * 1e-6; },
				[](double temperature) {return 8890.; },
				[](double temperature) {return 385.; },
				[](double temperature) {return 390.; },
				ST_SOLID);
