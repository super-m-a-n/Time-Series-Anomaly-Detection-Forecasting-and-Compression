//file:object.cpp//
#include "object.hpp"
#include "params.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cmath>
#include <vector>
#include <utility>
#include <typeinfo>
#include <list>
#include <stdlib.h>     /* srand, rand */


/////////////////////////////// CLASS OBJECT ///////////////////////////////////////////

///////////////////////// CONSTRUCTION / DESTRUCTION ///////////////////////////////////

Object::Object()
{
	float norm_squared = 0.0;
	int dim = 0;
	
	// different Object size depending on algorithm
	if (algorithm == "LSH" || algorithm == "Hypercube")
		dim = d;
	else if (algorithm == "Frechet" && metric_func == "discrete")
		dim = 2*d;
	else if (algorithm == "Frechet" && metric_func == "continuous")
		dim = d;

	for (int i = 0; i < dim; ++i)
	{
		std::random_device rd;
		std::mt19937 generator(rd());
		std::normal_distribution<float> distr(0, 1);		// each coordinate follows normal(0,1) distribution

		data_vector.push_back(distr(generator));

		norm_squared += data_vector[i] * data_vector[i];
	}

	for (int i = 0; i < dim; ++i)
		data_vector[i] = data_vector[i] / sqrt(norm_squared);			// divide each coordinate by norm, to normalize point-object
}

Object::Object(std::vector <float> & input_vector, std::string & object_name) : Abstract_Object(object_name)
{
	for (int i = 0; i < (int) input_vector.size(); ++i)
		data_vector.push_back(input_vector[i]);
}

Object::Object(std::vector <float> & input_vector)
{
	for (int i = 0; i < (int) input_vector.size(); ++i)
		data_vector.push_back(input_vector[i]);
}


Object::~Object() {}

///////////////////////// GETTERS /////////////////////////////////////////////


float Object::get_ith(int i) const
{
	return this->data_vector[i];
}

int Object::get_dim() const
{
	return (int) (this->data_vector).size();
}

///////////////////////// SETTERS /////////////////////////////////////////////

void Object::set(const Abstract_Object & abstract_object)
{
	// downcast abstract object to type Object
	//try {
	const Object& p = dynamic_cast<const Object&>(abstract_object);
	//} catch (const std::bad_cast& error)
	//{
	//	std::cerr << "Object::euclidean_distance : Bad_Cast error --> " << error.what() << std::endl << std::endl;
	//}

	// copy identifier
	this->identifier = p.get_name();

	/*if (this->get_dim() != p.get_dim())		// object dimensions should match
	{
		std::cerr << "Warning : Object::set : dimensions don't match\n\n";
		return;
	}
	*/

	if (this->get_dim() != p.get_dim())
	{
		// resize data vector to dimension of Object p
		(this->data_vector).resize(p.get_dim());
	}

	// copy coordinates
	for (int i = 0; i < this->get_dim(); ++i)
		this->data_vector[i] = p.data_vector[i];		
}


void Object::set_ith(int i, float value)
{
	this->data_vector[i] = value;
}

////////////////////////  PRINTS ///////////////////////////////////////////////

void Object::print() const
{
	std::cout << "Object " << this->identifier << "--> ";
	
	this->print_coordinates();
	
}

void Object::print_coordinates() const{
	std::cout << "(";
	for (int i = 0; i < (this->get_dim() - 1); ++i)
		std::cout << this->data_vector[i] << ",";
	std::cout << this->data_vector[this->get_dim() - 1];
	std::cout << ")";
}

void Object::print(std::ofstream & file) const
{
	file << "Object " << this->identifier << "--> ";
	
	this->print_coordinates(file);
}

void Object::print_coordinates(std::ofstream & file) const{
	file << "(";
	for (int i = 0; i < (this->get_dim() - 1); ++i)
		file << this->data_vector[i] << ",";
	file << this->data_vector[this->get_dim() - 1];
	file << ")";
}

/////////////////////// OBJECT OPERATIONS ///////////////////////////////////////////

float Object::inner_prod(const Object& p) const
{
	float inner_prod = 0.0;

	if (this->get_dim() != p.get_dim())		// object dimensions should match for inner product
	{
		std::cerr << "Warning : Object::inner_prod : dimensions don't match\n\n";
		return inner_prod;		//  returns garbage value
	}

	for (int i = 0; i < this->get_dim(); ++i)
		inner_prod += this->data_vector[i] * p.data_vector[i];

	return inner_prod;
}

double Object::euclidean_distance(const Abstract_Object & abstract_object) const
{
	// downcast abstract object to type Object
	//try {
	const Object& p = dynamic_cast<const Object&>(abstract_object);
	//} catch (const std::bad_cast& error)
	//{
	//	std::cerr << "Object::euclidean_distance : Bad_Cast error --> " << error.what() << std::endl << std::endl;
	//}

	double dist_squared = 0.0;

	if (this->get_dim() != p.get_dim())		// object dimensions should match for euclidean distance
	{
		std::cerr << "Warning : Object::euclidean_distance : dimensions don't match\n\n";
		return dist_squared;		//  returns garbage value
	}

	for (int i = 0; i < this->get_dim(); ++i)
		dist_squared += (double) (this->data_vector[i] - p.data_vector[i]) * (double) (this->data_vector[i] - p.data_vector[i]);

	return sqrt(dist_squared);
}

double Object::discrete_frechet_distance(const Abstract_Object & abstract_object) const
{
	//time_series & P;
	// downcast abstract object to type time_series
	//try {
	const Object & P = dynamic_cast<const Object &>(abstract_object);
	//} catch (const std::bad_cast& error)
	//{
	//	std::cerr << "time_series::discrete_frechet_distance : Bad_Cast error --> " << error.what() << std::endl << std::endl;
	//}

	// a vector of vectors that will serve as the 2D array for dynamic programming
	std::vector <std::vector <double> > OPT(this->get_dim(), std::vector <double> (P.get_dim()));
	// initialize first square at (0,0)
	OPT[0][0] = std::abs(this->get_ith(0) - P.get_ith(0));
	// initialize first column of array
	for (int i = 1; i < this->get_dim(); i++)
		OPT[i][0] = std::max(OPT[i-1][0], (double) std::abs(this->get_ith(i) - P.get_ith(0)));
	// initialize first row of array
	for (int j = 1; j < P.get_dim(); j++)
		OPT[0][j] = std::max(OPT[0][j-1], (double) std::abs(this->get_ith(0) - P.get_ith(j)));

	// initialize rest of array (i > 0 and j > 0)
	for (int i = 1; i < this->get_dim(); i++)
		for (int j = 1; j < P.get_dim(); j++)
			OPT[i][j] = std::max(std::min(OPT[i-1][j], std::min(OPT[i-1][j-1], OPT[i][j-1])), (double) std::abs(this->get_ith(i) - P.get_ith(j)));


	return OPT[this->get_dim() - 1][P.get_dim() - 1];	// value for frechet distance is at top right corner of array
}

const Object * Object::to_Object() const
{
	std::vector <float> copy_vector(this->get_dim());

	for (int i = 0; i < this->get_dim(); ++i)
		copy_vector[i] = data_vector[i];

	return new Object(copy_vector);
}	


std::vector <int> Object::snap(const std::vector<double>& t) const
{
	std::vector <int> snapped_object;

	for (int i = 0; i < this->get_dim(); ++i)
		snapped_object.push_back(floor((this->data_vector[i] - t[0] )/ delta + 1/2));		// snap each coordinate to new integer coordinate of grid
	
	return snapped_object;
}

std::vector <int> Object::remove_dupls(std::vector <int> & snapped_curve) const
{
	std::vector <int> grid_curve;
	
	// for each coordinate of Object, one boolean variable stating if it is duplicate or not
	std::vector <bool> duplicate (this->get_dim(), false);

	// now we mark duplicate coordinates from snapping
	
	int pos = 0;	// index of last non duplicate coordinate

	for (int i = 1; i < this->get_dim(); ++i)	// for each coordinate of object
	{
		// check if coordinate i is the same as last non duplicate coordinate
		if (snapped_curve[i] == snapped_curve[pos])
			duplicate[i] = true;
		else
			pos = i;
	}

	// push back the non duplicate coordinates to grid curve vector
	for (int i = 0; i < this->get_dim(); ++i)
	{
		if (duplicate[i] == false)
		{
			grid_curve.push_back(snapped_curve[i]);
		}
	}

	return grid_curve;
}

std::vector <float> Object::get_extrema(std::vector <int> & snapped_curve_no_dupl) const{
		int size = snapped_curve_no_dupl.size();
		int i = 0, j = 1;
		std::vector<float> filtered_data;
		while(j < size - 1){
			float a = snapped_curve_no_dupl[i];
			float b = snapped_curve_no_dupl[j];
			float c = snapped_curve_no_dupl[j+1];
			float min = (a < c) ? a : c;
			float max = (a < c) ? c : a;

			if (! ( min <= b && b <= max) ){
				filtered_data.push_back(a);
				i = j;
			}
			j += 1;
		}

		// Special handling in case the vector has dimension 1
		if (j == size -1){
			filtered_data.push_back(snapped_curve_no_dupl[j]);
		}
		else {
			filtered_data.push_back(snapped_curve_no_dupl[0]);
		}

		return filtered_data;

}

void Object::pad(std::vector <float> & grid_curve) const
{
	// finally check if dimension of snapped grid object diminished, and apply padding if necessary
	// with special big padding number
	unsigned long int M = 1000000;  /// further testing required

	int size = 0;

	//If the algorithm is continuous frechet, then size = this-.get_dim()
	// is not reliable because all curves are filtered and thus will have different
	// complexity. d is a global variable which tells which is the original complexity of the curves
	if (algorithm == "Frechet" && metric_func == "continuous"){
		size = d;
	}
	else{
		size = this->get_dim();
	}

	for (int i = (int) grid_curve.size(); i < size; ++i)
		grid_curve.push_back(M);
}

Abstract_Object * Object::to_grid_curve(const std::vector<double>& t) const
{
	// first of all the function snaps the caller Object-time series to grid, without multiplying by delta or shifting by t
	// we do this so that we can remove duplicate points from snapping by comparing integers (otherwise we would compare floats , oof)
	
	// vector holds snapped points-coordinates
	std::vector <int> snapped_object = this->snap(t);

	// vector of grid curve coordinates after removing duplicates from snapping
	std::vector <int> grid_curve_temp = this->remove_dupls(snapped_object);

	std::vector <float> grid_curve = this->get_extrema(grid_curve_temp);


	// multiply by delta and shift by t to get final grid curve
	for (int i = 0; i < (int) grid_curve.size(); ++i)
		grid_curve[i] = grid_curve[i] * delta + t[0];
	// do padding necessary
	this->pad(grid_curve);

	// return the new Object-grid_curve-time_series
	return new Object(grid_curve);
}



/////////////////////////////// CLASS TIME_SERIES ///////////////////////////////////////////

///////////////////////// CONSTRUCTION / DESTRUCTION ///////////////////////////////////

time_series::time_series(std::vector <float> & input_vector, std::string & curve_name) : Abstract_Object(curve_name)
{
	float x_val = 1;

	for (int i = 0; i < (int) input_vector.size(); ++i)
	{
		data_vector.push_back(std::make_pair(x_val, input_vector[i]));
		x_val++;
	}
}

time_series::time_series(std::vector <float> & input_vector)
{
	float x_val = 1;

	for (int i = 0; i < (int) input_vector.size(); ++i)
	{
		data_vector.push_back(std::make_pair(x_val, input_vector[i]));
		x_val++;
	}
}

time_series::time_series(std::vector <std::pair <float, float> > input_vector)
{
	for (int i = 0; i < (int) input_vector.size(); ++i)
	{
		data_vector.push_back(input_vector[i]);
	}
}

time_series::time_series(const Abstract_Object & abstract_object)
{
	// downcast abstract object to type time_series
	//try {
	const time_series & P = dynamic_cast<const time_series &>(abstract_object);
	//} catch (const std::bad_cast& error)
	//{
	//	std::cerr << "time_series::discrete_frechet_distance : Bad_Cast error --> " << error.what() << std::endl << std::endl;
	//}

	for (int i = 0; i < P.get_complexity(); ++i)
	{
		data_vector.push_back(P.data_vector[i]);
	}
}

///////////////////////// GETTERS /////////////////////////////////////////////

int time_series::get_complexity() const
{
	return (int) (this->data_vector).size();
}

const std::pair <float, float> & time_series::get_ith(int i) const
{
	return this->data_vector[i];
}

///////////////////////// SETTERS /////////////////////////////////////////////
	
void time_series::set(const Abstract_Object & abstract_object)
{
	// downcast abstract object to type time_series
	//try {
	const time_series & P = dynamic_cast<const time_series &>(abstract_object);
	//} catch (const std::bad_cast& error)
	//{
	//	std::cerr << "time_series::discrete_frechet_distance : Bad_Cast error --> " << error.what() << std::endl << std::endl;
	//}

	// copy identifier
	this->identifier = P.get_name();

	/*if (this->get_complexity() != P.get_complexity())		// time series complexities should match
	{
		std::cerr << "Warning : time_series::set : complexities don't match\n\n";
		return;
	}*/

	if (this->get_complexity() != P.get_complexity())
	{
		// resize data vector to complexity of time series P
		(this->data_vector).resize(P.get_complexity());
	}

	// copy points
	for (int i = 0; i < this->get_complexity(); ++i)
	{
		(this->data_vector[i]).first = (P.data_vector[i]).first;
		(this->data_vector[i]).second = (P.data_vector[i]).second;
	}

}


////////////////////////  PRINTS ///////////////////////////////////////////////
void time_series::print() const
{
	std::cout << "Time Series " << this->identifier << "-->   (";
	for (int i = 0; i < this->get_complexity() - 1; ++i)
		std::cout << "(" << std::get<0>(this->data_vector[i]) << ", " << std::get<1>(this->data_vector[i]) << ") , ";

	std::cout << "(" << std::get<0>(this->data_vector[this->get_complexity()-1]) << ", " << std::get<1>(this->data_vector[this->get_complexity()-1]) << ") ) \n\n";
}

void time_series::print_coordinates(std::ofstream & file) const
{
	file << "( ";
	for (int i = 0; i < this->get_complexity() - 1; ++i)
		file << "(" << std::get<0>(this->data_vector[i]) << ", " << std::get<1>(this->data_vector[i]) << ") , ";

	file << "(" << std::get<0>(this->data_vector[this->get_complexity()-1]) << ", " << std::get<1>(this->data_vector[this->get_complexity()-1]) << ")";
	file << " )";

}

/////////////////////// OBJECT OPERATIONS ///////////////////////////////////////////

double time_series::euclidean_distance(const Abstract_Object& abstract_object) const
{
	// for time_series euclidean_distance is not supported
	std::cerr << "Warning : time_series::euclidean_distance : Caller object is of type time_series (undefined behavior)\n\n";
	// return garbage value
	return -1.0;
}

double time_series::discrete_frechet_distance(const Abstract_Object & abstract_object) const
{
	
	// downcast abstract object to type time_series
	//try {
	const time_series & P = dynamic_cast<const time_series &>(abstract_object);
	//} catch (const std::bad_cast& error)
	//{
	//	std::cerr << "time_series::discrete_frechet_distance : Bad_Cast error --> " << error.what() << std::endl << std::endl;
	//}

	// a vector of vectors that will serve as the 2D array for dynamic programming
	std::vector <std::vector <double> > OPT(this->get_complexity(), std::vector <double> (P.get_complexity()));
	// initialize first square at (0,0)
	OPT[0][0] = norm(this->get_ith(0), P.get_ith(0));
	// initialize first column of array
	for (int i = 1; i < this->get_complexity(); i++)
		OPT[i][0] = std::max(OPT[i-1][0], norm(this->get_ith(i), P.get_ith(0)));
	// initialize first row of array
	for (int j = 1; j < P.get_complexity(); j++)
		OPT[0][j] = std::max(OPT[0][j-1], norm(this->get_ith(0), P.get_ith(j)));

	// initialize rest of array (i > 0 and j > 0)
	for (int i = 1; i < this->get_complexity(); i++)
		for (int j = 1; j < P.get_complexity(); j++)
			OPT[i][j] = std::max(std::min(OPT[i-1][j], std::min(OPT[i-1][j-1], OPT[i][j-1])), norm(this->get_ith(i), P.get_ith(j)));


	return OPT[this->get_complexity() - 1][P.get_complexity() - 1];	// value for frechet distance is at top right corner of array
}

const Object * time_series::to_Object() const
{
	std::vector <float> flattened_time_series;

	for (int i = 0; i < this->get_complexity(); ++i)
	{
		flattened_time_series.push_back(std::get<0>(this->data_vector[i]));
		flattened_time_series.push_back(std::get<1>(this->data_vector[i]));
	}

	return new Object(flattened_time_series);
}

std::vector <std::pair <int, int> > time_series::snap(const std::vector<double>& t) const
{
	std::vector <std::pair <int, int> > snapped_time_series;

	for (int i = 0; i < this->get_complexity(); ++i)
	{
		// snap each point to new integer point coordinate of grid
		int x_value = floor((std::get<0>(this->data_vector[i]) - t[0]) / delta + 1/2);
		int y_value = floor((std::get<1>(this->data_vector[i]) - t[1]) / delta + 1/2);
		snapped_time_series.push_back(std::make_pair(x_value, y_value));
	}

	return snapped_time_series;
}

std::vector <std::pair <float, float> > time_series::remove_dupls(std::vector <std::pair <int, int> > & snapped_curve) const
{
	std::vector <std::pair <float, float> > grid_curve;
	
	// for each point of time_series, one boolean variable stating if it is duplicate or not
	std::vector <bool> duplicate (this->get_complexity(), false);

	// now we mark duplicate points from snapping
	
	int pos = 0;	// index of last non duplicate point

	for (int i = 1; i < this->get_complexity(); ++i)	// for each point of time_series
	{
		// check if point i is the same as last non duplicate point
		if (std::get<0>(snapped_curve[i]) == std::get<0>(snapped_curve[pos]) && 
			std::get<1>(snapped_curve[i]) == std::get<1>(snapped_curve[pos]))
			duplicate[i] = true;
		else
			pos = i;
	}

	// push back the non duplicate points to grid curve vector
	for (int i = 0; i < this->get_complexity(); ++i)
	{
		if (duplicate[i] == false)
		{
			grid_curve.push_back(snapped_curve[i]);
		}
	}

	return grid_curve;
}

void time_series::pad(std::vector <std::pair <float, float> > & grid_curve) const
{
	// finally check if complexity of snapped grid time_series diminished, and apply padding if necessary
	// with special big padding number
	unsigned long int M = 1000000;  /// further testing required
	for (int i = (int) grid_curve.size(); i < this->get_complexity(); ++i)
		grid_curve.push_back(std::make_pair(M, M));
}


Abstract_Object * time_series::to_grid_curve(const std::vector<double> & t) const
{
	// first of all the function snaps the given time series to grid, without multiplying by delta or shifting by t
	// we do this so that we can remove duplicate points from snapping by comparing integers (otherwise we would compare floats , oof)
	
	// vector holds snapped points-coordinates
	std::vector <std::pair <int, int> > snapped_time_series = this->snap(t);

	// vector of grid curve coordinates after removing duplicates from snapping
	std::vector <std::pair <float, float> > grid_curve = this->remove_dupls(snapped_time_series);

	// multiply by delta and shift by t to get final grid curve
	for (int i = 0; i < (int) grid_curve.size(); ++i)
	{
		grid_curve[i].first = grid_curve[i].first * delta + t[0];
		grid_curve[i].second = grid_curve[i].second * delta + t[1];
	}
	
	// do padding necessary
	this->pad(grid_curve);

	// return the new grid_curve-time_series
	return new time_series(grid_curve);
}

//A general way to compute the argmin of 3 numbers
template<typename T>
int argmin(T x, T y, T z){
	T min = x;
	int pos = 0;
	if (y < min) { min = y; pos = 1;}
	if (z < min) { min = z; pos = 2;}
	return pos;
}

std::list<std::pair<int, int> > time_series::best_traversal(const time_series* P) const{
	// a vector of vectors that will serve as the 2D array for dynamic programming
	std::vector <std::vector <double> > OPT(this->get_complexity(), std::vector <double> (P->get_complexity()));
	// initialize first square at (0,0)
	OPT[0][0] = norm(this->get_ith(0), P->get_ith(0));
	// initialize first column of array
	for (int i = 1; i < this->get_complexity(); i++)
		OPT[i][0] = std::max(OPT[i-1][0], norm(this->get_ith(i), P->get_ith(0)));
	// initialize first row of array
	for (int j = 1; j < P->get_complexity(); j++)
		OPT[0][j] = std::max(OPT[0][j-1], norm(this->get_ith(0), P->get_ith(j)));

	// initialize rest of array (i > 0 and j > 0)
	for (int i = 1; i < this->get_complexity(); i++)
		for (int j = 1; j < P->get_complexity(); j++)
			OPT[i][j] = std::max(std::min(OPT[i-1][j], std::min(OPT[i-1][j-1], OPT[i][j-1])), norm(this->get_ith(i), P->get_ith(j)));


	std::list <std::pair <int, int> > best_traversal;
	
	// backtrack on OPT array to compute the best traversal
	int i = this->get_complexity() - 1;
	int j = P->get_complexity() - 1;

	// last point of each curve is definitely part (endpoint actually) of best traversal
	best_traversal.push_front(std::make_pair(i, j));

	// while we have not reached starting point for either curves
	while(i != 0 && j != 0)
	{
		int min_index = argmin(OPT[i-1][j], OPT[i][j-1], OPT[i-1][j-1]);
		if (min_index == 0)
			best_traversal.push_front(std::make_pair(--i, j));
		else if (min_index == 1)
			best_traversal.push_front(std::make_pair(i, --j));
		else
			best_traversal.push_front(std::make_pair(--i, --j));
	}

	// complete traversal by adding pair of points until we reach starting point for both curves
	if (i == 0 && j != 0)	// if we have reached starting point for first curve but not for second curve
	{
		while(j != 0)
		{
			// complete traversal by adding points of second curve
			best_traversal.push_front(std::make_pair(i, --j));
		}
	}
	else if (j == 0 && i != 0)	// else if we have reached starting point for second curve but not for first curve
	{
		while(i != 0)
		{
			// complete traversal by adding points of first curve
			best_traversal.push_front(std::make_pair(--i, j));
		}

	}

	return best_traversal;
}

std::vector <std::pair <float, float> > time_series::mean_curve_without_filtering(const time_series * P) const
{
	
	std::list <std::pair <int, int> > best_traversal = this->best_traversal(P);
	// compute the mean curve from best traversal
	std::vector <std::pair <float, float> > mean_curve;

	for (auto const& index_pair : best_traversal)
	{
		const std::pair <float, float> & caller_curve_point = this->get_ith(std::get<0>(index_pair));
		const std::pair <float, float> & argument_curve_point = P->get_ith(std::get<1>(index_pair));

		float x_value = (std::get<0>(caller_curve_point) + std::get<0>(argument_curve_point)) / 2; 
		float y_value = (std::get<1>(caller_curve_point) + std::get<1>(argument_curve_point)) / 2;
		mean_curve.push_back(std::make_pair(x_value, y_value));
	}
	return mean_curve;
}


Abstract_Object * time_series::mean_curve(const time_series * P) const
{
	std::vector <std::pair <float, float> > mean_curve = this->mean_curve_without_filtering(P);

	// filter mean_curve points, to decrease its complexity
	int extra_points = mean_curve.size() - this->get_complexity();
	// for each excessive point of mean curve
	for (int i = 0; i < extra_points; ++i)
	{
		// pick a random non-last point index of mean-curve
		int index = rand() % (mean_curve.size()-1);
		// find mean point of point at index, index+1
		float x_value = (std::get<0>(mean_curve[index]) + std::get<0>(mean_curve[index+1])) / 2;
		float y_value = (std::get<1>(mean_curve[index]) + std::get<1>(mean_curve[index+1])) / 2;
		// replace the 2 consecutive points with the mean point, reducing mean curves complexity by one
		mean_curve.erase(mean_curve.begin()+index+1);
		mean_curve[index].first = x_value;
		mean_curve[index].second = y_value;
	}


	return new time_series(mean_curve);
}

// metric wrappers

double discrete_frechet(const Abstract_Object & P, const Abstract_Object & Q)
{
	return P.discrete_frechet_distance(Q);
}

double euclidean(const Abstract_Object & p, const Abstract_Object & q)
{
	return p.euclidean_distance(q);
}

double norm(const std::pair <float, float> & point1, const std::pair <float, float> & point2)
{
	double x1 = (double) std::get<0>(point1);
	double x2 = (double) std::get<0>(point2);
	double y1 = (double) std::get<1>(point1);
	double y2 = (double) std::get<1>(point2);
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// mean curve wrapper
Abstract_Object * mean_curve(const Abstract_Object * abstract_object1, const Abstract_Object * abstract_object2)
{
	if (abstract_object2 == nullptr)
		return new time_series(*abstract_object1);

	// downcast abstract objects to type time_series
	const time_series * P = dynamic_cast<const time_series *>(abstract_object1);
	const time_series * Q = dynamic_cast<const time_series *>(abstract_object2);

	return P->mean_curve(Q);
}