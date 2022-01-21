//file:search_method.hpp//
#ifndef _SEARCH_METHOD_HPP_
#define _SEARCH_METHOD_HPP_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "params.hpp"
#include "hash.hpp"
#include "dataset.hpp"
#include "object.hpp"
#include <set>

// abstract class search_method provides an interface for search.cpp
// all curve similarity search algorithm classes inherit from this class
// and override its pure virtual functions

class search_method
{
public:
	// empty virtual destructor for proper cleaning of derived classes 
  	virtual ~search_method() {}
	
	// imports dataset objects into search struct
	virtual void import_data(const Dataset & dataset) = 0;

	// executes the approximate/exact/ranged nearest neighbors algorithms using given metric function (pointer to function)
	virtual bool execute(const Dataset & dataset, const Dataset & query_dataset, const std::string & output_file, const int & N, const int & R, double (*metric)(const Abstract_Object &, const Abstract_Object &)) = 0;

	// runs approximate nearest neighbors using given metric function and returns an array of nearest neighbors and distances found
	virtual std::vector <std::pair <double, const Abstract_Object*> > appr_nearest_neighbors(const Dataset & dataset, const Abstract_Object & query_object, const int & N, double (*metric)(const Abstract_Object &, const Abstract_Object &)) = 0;

	// runs exact nearest neighbors using given metric function and returns an array of nearest neighbors and distances found
	virtual std::vector <std::pair <double, const Abstract_Object*> > exact_nearest_neighbors(const Dataset & dataset, const Abstract_Object & query_object, const int & N, double (*metric)(const Abstract_Object &, const Abstract_Object &)) = 0;

	// runs approximate range search using given metric function 
	virtual std::list <std::pair <double, const Abstract_Object*> > range_search(const Abstract_Object & query_object, const int & R, double (*metric)(const Abstract_Object &, const Abstract_Object &), const int R2 = 0) = 0;
	
};

#endif