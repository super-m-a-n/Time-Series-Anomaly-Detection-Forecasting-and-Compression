//file:lsh_struct.hpp//
#ifndef _LSH_STRUCT_HPP_
#define _LSH_STRUCT_HPP_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "params.hpp"
#include "hash.hpp"
#include "dataset.hpp"
#include "object.hpp"
#include "search_method.hpp"
#include <set>

// using the fred library to store Curves
// But only used in continuous frechet and when an input curve is filtered then make a copy of it but of Class Curve
#include "fred/include/curve.hpp"



// class lsh_struct contains all the data structures used by lsh
// namely L hash-tables
class lsh_struct : public search_method
{
private:
	hash_table ** lsh_hash_struct;	// an array of pointers to L hashtables
	

	std::vector<Curve*> curve_vect; // Used to store the input data as curves of the fred library for
									// for compatibility with the continuous frechet distance of the fred library
									// This vector is always empty if the distance to be calculated in the exeutable is not the continuous frechet
									
//int complexity;						// In case of continuous frechet, after the filtering, all curves will have
										// almost certainly smaller complexity and not necessary the same
										// That means that if a query curve, after the filtering,snapping and min-max,
										// If it ends up with a higher complexity than all filtered curves' complexities
										// Then it is impossible to be inserted into the vector LSH
										// Thus keep the original input curve complexity
public:
	// constructor, creates the L hashtables, each with number of buckets = hash_table_size
	lsh_struct(int hash_table_size);
	~lsh_struct();
	// imports dataset objects into lsh struct (into the L hash tables)
	void import_data(const Dataset & dataset);

	// executes the approximate/exact/ranged nearest neighbors algorithms using given metric function (pointer to function)
	// and outputs results and execution times in output file
	bool execute(const Dataset & dataset, const Dataset & query_dataset, const std::string & output_file, const int & N, const int & R, double (*metric)(const Abstract_Object &, const Abstract_Object &));

	// runs approximate nearest neighbors using given metric function and returns an array of nearest neighbors and distances found
	std::vector <std::pair <double, const Abstract_Object*> > appr_nearest_neighbors(const Dataset & dataset, const Abstract_Object & query_object, const int & N, double (*metric)(const Abstract_Object &, const Abstract_Object &));

	// runs exact nearest neighbors using given metric function and returns an array of nearest neighbors and distances found
	std::vector <std::pair <double, const Abstract_Object*> > exact_nearest_neighbors(const Dataset & dataset, const Abstract_Object & query_object, const int & N, double (*metric)(const Abstract_Object &, const Abstract_Object &));

	// runs approximate range search using given metric function 
	//There is another parameter R2. Given R and R2, this function actually only stores in the set points which belong to the ring [R2, R) with center the query_object
	// Here by ring [r, R) with center C is the set of points with r <= dist < R where dist is the distance of the point from the center
	// By default R2 is 0 so the ring [0, R) is acually the ball with radius R 
	std::list <std::pair <double, const Abstract_Object*> > range_search(const Abstract_Object & query_object, const int & R, double (*metric)(const Abstract_Object &, const Abstract_Object &), const int R2 = 0);
	

	std::list <std::pair <double, const Abstract_Object*> > range_search_with_set(const Abstract_Object & query_object, const int & R, std::set<std::string> & already_visited, double (*metric)(const Abstract_Object &, const Abstract_Object &));
};

#endif
