//file:object.hpp//
#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include "params.hpp"

class Object;

/////////////////////////////////////// CLASS ABSTRACT_OBJECT //////////////////////////////////////////////////////////////////////

// abstract class Abstract Object provides an interface for the type of object used by all data structures/program
// all Object-types inherit from this class, and re define its pure virtual methods

class Abstract_Object
{
protected:
	// the object identifier name, as read from input file
	std::string identifier;
	// default class constructor (does nothing)
	Abstract_Object() {}
	// class constructor, just initializes the object identifier name
	Abstract_Object(std::string & object_name) : identifier(object_name) {}

public:
	// name identifier getter
	const std::string & get_name() const { return this->identifier; }

	// print object
	virtual void print() const = 0;

	// print object coordinates to file
	virtual void print_coordinates(std::ofstream & file) const = 0;

	// sets caller object's info to given arg object's info (does a copy basically)
	virtual void set(const Abstract_Object & abstract_object) = 0;

	// virtual destructor, to call destructor of sub-classes
	virtual ~Abstract_Object() {}
	
	// calculates the euclidean distance between caller object and argument object
	virtual double euclidean_distance(const Abstract_Object & abstract_object) const = 0;

	// computes the discrete frechet distance between caller object and argument object
	virtual double discrete_frechet_distance(const Abstract_Object & abstract_object) const = 0;

	// converts caller Abstract Object to an object of type Object
	virtual const Object * to_Object() const = 0;

	// converts caller Abstract Object to grid curve
	virtual Abstract_Object * to_grid_curve(const std::vector<double>& t) const = 0;
	
};

/////////////////////////////////////// CLASS OBJECT ///////////////////////////////////////////////////////////////////////////////

// class Object represents a multidimensional point basically
// class Object holds the data of the input objects-points of the dataset
// i.e. the coordinates in a d-vector and the object name, from input file

class Object : public Abstract_Object
{
private:
	std::vector <float>  data_vector;	// the coordinates of the d-dimensional point-object in an array

public:
	///////////////////////// CONSTRUCTION / DESTRUCTION //////////////////////////////////////////////////////////////////
	
	// default constructor creates a random normalized d-dimensional point-object, each coordinate follows normal(0,1) distribution 
	Object();
	// constructor through another d-dimensional input array
	Object(std::vector <float> & input_vector, std::string & object_name);
	// constructor through another d-dimensional input array with empty object name
	Object(std::vector <float> & input_vector);
	// destructor
	~Object();

	///////////////////////// GETTERS /////////////////////////////////////////////
	
	// object dimensionality getter
	int get_dim() const; 
	// gets ith coordinate of Object
	float get_ith(int i) const;

	///////////////////////// SETTERS /////////////////////////////////////////////
	
	// sets caller object's info to given arg object's info (does a copy basically)
	void set(const Abstract_Object & abstract_object);
	// sets ith coordinate of Object to value
	void set_ith(int i, float value);

	////////////////////////  PRINTS ///////////////////////////////////////////////
	
	// print method for debugging
	void print() const;
	// write object to file
	void print(std::ofstream & file) const;
	//Same as with the above prints but now print only the coordinates of the object
	void print_coordinates() const;
	// same as above, but print to file
	void print_coordinates(std::ofstream & file) const;

	/////////////////////// OBJECT OPERATIONS ///////////////////////////////////////////
	
	// calculates the inner-product of calling object with given object p (both d-dimensional)
	float inner_prod(const Object& p) const;
	// calculates the euclidean distance between caller object and argument object
	double euclidean_distance(const Abstract_Object & abstract_object) const;
	// computes the discrete frechet distance between caller object and argument object
	double discrete_frechet_distance(const Abstract_Object & abstract_object) const;
	// converts caller object to an object of type Object
	const Object * to_Object() const;
	
	//In case of continuous frechet, 
	std::vector <int>* filtering(const Object& obj) const;
	// returns a vector with snapped coordinates to grid integers
	std::vector <int> snap(const std::vector<double>& t) const;
	// returns a vector with snapped coordinates to grid integers after removing duplicates
	std::vector <int> remove_dupls(std::vector <int> & snapped_curve) const;
	// Get extrema
	std::vector <float> get_extrema(std::vector <int> & snapped_curve_no_dupl) const;
	// pads given vector of grid coordinates if necessary
	void pad(std::vector <float> & grid_curve) const;
	// converts caller Abstract Object to grid curve
	Abstract_Object * to_grid_curve(const std::vector<double>& t) const;

};


// class for a 2D time_series object.  We treat a 2D time_series object as a vector of pairs (x,y) values
class time_series : public Abstract_Object
{
private:
	std::vector <std::pair <float, float> > data_vector;	// vector of pairs (x,y) x --> time, y -> curve value
public:
	///////////////////////// CONSTRUCTION / DESTRUCTION ////////////////////////////////

	// constructor through another d-dimensional input array with just y-values
	time_series(std::vector <float> & input_vector, std::string & curve_name);
	// constructor through another d-dimensional input array with empty object name
	time_series(std::vector <float> & input_vector);
	// constructor through another input vector with pairs of x-values and y-values
	time_series(std::vector <std::pair <float, float> > input_vector);
	// copy constructor through another Abstract object (of type time series)
	time_series(const Abstract_Object & abstract_object);
	
	///////////////////////// GETTERS /////////////////////////////////////////////
	
	// returns curve's complexity
	int get_complexity() const;
	// gets ith 2d point of time series
	const std::pair <float, float> & get_ith(int i) const;

	///////////////////////// SETTERS /////////////////////////////////////////////
	
	// sets caller time_series's info to given arg time_series's info (does a copy basically)
	void set(const Abstract_Object & abstract_object);


	////////////////////////  PRINTS ///////////////////////////////////////////////
	
	// print method
	void print() const;
	// print time series points to file
	void print_coordinates(std::ofstream & file) const;

	/////////////////////// TIME_SERIES OPERATIONS ///////////////////////////////////////////

	double euclidean_distance(const Abstract_Object& abstract_object) const;
	// computes the discrete frechet distance between caller object and argument object
	double discrete_frechet_distance(const Abstract_Object & abstract_object) const;
	const Object * to_Object() const;
	
	// returns a vector with snapped points to grid integers
	std::vector <std::pair <int, int> > snap(const std::vector<double>& t) const;
	// returns a vector with snapped points to grid integers after removing duplicates
	std::vector <std::pair <float, float> > remove_dupls(std::vector <std::pair <int, int> > & snapped_curve) const;
	// pads given vector of grid points if necessary
	void pad(std::vector <std::pair <float, float> > & grid_curve) const;
  	Abstract_Object * to_grid_curve(const std::vector<double>& t) const;
	
	std::list<std::pair<int, int> > best_traversal(const time_series* P) const;

	std::vector <std::pair <float, float> > mean_curve_without_filtering(const time_series * P) const;

	// calculates optimal traversal through backtracking from discrete frechet dp array
	// then uses the optimal traversal to return the mean curve of caller and argument time series
	Abstract_Object * mean_curve(const time_series * P) const;

};


// metric wrappers
double euclidean(const Abstract_Object & p, const Abstract_Object & q);
double discrete_frechet(const Abstract_Object & P, const Abstract_Object & Q);
double norm(const std::pair <float, float> & point1, const std::pair <float, float> & point2);

// mean curve wrapper
Abstract_Object * mean_curve(const Abstract_Object * abstract_object1, const Abstract_Object * abstract_object2);

#endif