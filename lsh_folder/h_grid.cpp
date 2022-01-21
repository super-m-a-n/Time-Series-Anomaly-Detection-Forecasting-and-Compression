//file:h_grid.cpp//
#include "h_grid.hpp"
#include "params.hpp"
#include "object.hpp"
#include <iostream>
#include <random>
#include <cmath>
#include <vector>

// create a signle precision reat t uniformly in [0,delta)^dim,
h_grid::h_grid(int dim){
	const float lower_bound = 0.0;
	const float upper_bound = (float) delta;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> distr(lower_bound, upper_bound);
	for (int i = 0 ; i  < dim ; i++){
		t.push_back(distr(generator));
	}
}

Abstract_Object * h_grid::operator()(const Abstract_Object& time_series) const
{
	return time_series.to_grid_curve(this->t);
}
	
