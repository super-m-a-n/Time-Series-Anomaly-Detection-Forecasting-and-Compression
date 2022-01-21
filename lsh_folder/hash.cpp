//file:hash.cpp//
#include "hash.hpp"
#include "object.hpp"
#include "g_hash.hpp"
#include "h_grid.hpp"
#include <iostream>
#include <list>
#include <utility>
#include <cstdint>

hash_table::hash_table(int numBuckets, int dim) : size(0), capacity(numBuckets), h_delta(dim)
{
	// calls default constructor for g hash function of hash table
	// allocates memory for the table of lists
	table = new std::list <std::pair <const Abstract_Object*, uint32_t> >[numBuckets];
}

hash_table::~hash_table()
{
	delete[] table;
}

int hash_table::get_size() const
{
	return this->size;
}

int hash_table::get_capacity() const
{
	return this->capacity;
}
	
void hash_table::insert(const Abstract_Object& p, int complexity)
{
	uint32_t object_id = 0;

	// different insert hash function depending on algorithm
	if (algorithm == "LSH")
	{
		int index = g(p, this->capacity, object_id);				 // g hash function is used to get index in hash-table and locality object_id of given object
		this->table[index].push_back(std::make_pair(&p, object_id)); // pointer to given Abstract-Object and its locality ID are inserted at index
	}
	else if (algorithm == "Frechet" /*&& metric_func == "discrete"*/)
	{
		// we first apply the grid function h_delta
		const Abstract_Object * gridCurve = h_delta(p);
		int index = g(*gridCurve, this->capacity, object_id);		  // g hash function is used to get index in hash-table and locality object_id of given object
		this->table[index].push_back(std::make_pair(&p, object_id));  // pointer to given Abstract-Object and its locality ID are inserted at index
		delete gridCurve;
	}
	#if 0
	else if (algorithm == "Frechet" /*&& metric_func == "continuous"*/){
		// we first apply the grid function h_delta
		const Abstract_Object * gridCurve = h_delta(p);
		int index = g(*gridCurve, this->capacity, object_id);		  // g hash function is used to get index in hash-table and locality object_id of given object
		this->table[index].push_back(std::make_pair(&p, object_id));  // pointer to given Abstract-Object and its locality ID are inserted at index
		delete gridCurve;
	}
	#endif
}

int hash_table::get_bucket_index(const Abstract_Object& p, uint32_t & object_id)
{
	int index = 0;

	// different hash function depending on algorithm
	if (algorithm == "LSH")
	{
		index = g(p, this->capacity, object_id);
	}
	else if (algorithm == "Frechet")
	{
		// we first apply the grid function h_delta to snap object to grid
		const Abstract_Object * gridCurve = h_delta(p);
		index = g(*gridCurve, this->capacity, object_id);
		delete gridCurve;
	}

	return index;
}

const std::list <std::pair <const Abstract_Object*, uint32_t> > & hash_table::get_ith_bucket(int bucket_index) const
{
	return this->table[bucket_index];
}