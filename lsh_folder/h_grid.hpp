//file:h_grid.hpp//
#ifndef _H_GRID_HPP_
#define _H_GRID_HPP_
#include <iostream>
#include <vector>
#include "object.hpp"

// class h_grid is used to hold info about an h grid hash function ( maps time series to G_delta 2-dimensional grid or 1-dimensional grid)

class h_grid
{
private:
	// a single precision real t uniformly in [0,delta)^2
	std::vector<double> t;	

public:
	// create a signle precision reat t uniformly in [0,delta)^dim, (Initialised to 2 for the needs of this project)
	h_grid(int dim = 2);
	// overload of () operator, so that each h_grid object can be used as a "function"
	// complexity is the original complexity of the curves before they passed the filter
	// (necessary for query compatibility with query curves
	Abstract_Object * operator()(const Abstract_Object& time_series) const;
};

#endif