//file:hash.hpp//
#ifndef _HASH_HPP_
#define _HASH_HPP_
#include <iostream>
#include <list>
#include <utility>
#include <cstdint>
#include "g_hash.hpp"
#include "object.hpp"
#include "h_grid.hpp"


class hash_table
{
private:
	std::list <std::pair <const Abstract_Object*, uint32_t> > * table;		// hash table implemented as an array of linked lists containing pointers to Abstract-Objects and locality ID for each object
	
	int size;						// number of elements added
	int capacity;					// number of buckets
	g_hash g;						// amplified g hash function for hash table
	h_grid h_delta;					// grid hash function used for time series similarity search (and maps points of dimension dim)


public:
	// constructor
	// dim is the dimension od the grid used. 
	//	For this project a 2-dimension grid suffices since the snapping of an object is compatible even if
	// grid has higher dimension than the object curve's complexity and in this project, the object curves will have complexity of either 1 or 2

	hash_table(int numBuckets, int dim = 2);
	// destructor
	~hash_table();
	// returns number of elements currently in hash table
	int get_size() const;
	// returns number of buckets currently in hash table
	int get_capacity() const;
	// inserts given point-object into hash table
	// complexity is the original complexity of the curves before they passed the filter
	// (necessary for the query curves)
	void insert(const Abstract_Object& p, int complexity = 0);
	// returns bucket index in which given object p hashes in and its locality ID in object_id
	int get_bucket_index(const Abstract_Object& p, uint32_t & object_id);
	// returns bucket-list indicated by bucket index
	const std::list <std::pair <const Abstract_Object*, uint32_t> > & get_ith_bucket(int bucket_index) const;
};

#endif