//file:cluster_info.hpp//
#ifndef _CLUSTER_INFO_HPP_
#define _CLUSTER_INFO_HPP_
#include <iostream>
#include <string>
#include <vector>
#include "object.hpp"
#include "dataset.hpp"

// class Cluster_info holds info for clustering
// i.e. the centroids and the clusters

class Cluster_info
{
private:
	std::vector <Abstract_Object*> centroids;						// a vector of centroids
	std::vector <std::vector <const Abstract_Object*> > clusters; 	// a vector of vectors, i.e a vector of clusters
	

public:
	Cluster_info();
	~Cluster_info();

	// executes clustering using given metric function (pointer to function) and update method and assignment method
	// and outputs results and execution times in output file
	bool execute(const Dataset & dataset, const std::string & output_file, const std::string & update_method, const std::string & assignment_method, bool complete, bool run_silhouette, double (*metric)(const Abstract_Object &, const Abstract_Object &));
	
	// uses the dataset given, to run K-means++ initialization and find K initial centroids
	void K_means_init(const Dataset & dataset, double (*metric)(const Abstract_Object &, const Abstract_Object &));

	// clustering using exact lloyd's as assignment method
	void exact_lloyds(const Dataset & dataset, const std::string & update_method, double (*metric)(const Abstract_Object &, const Abstract_Object &));
	// clustering using lsh range search as assignment method
	void lsh_range_search_clustering(const Dataset & dataset, const std::string & update_method, double (*metric)(const Abstract_Object &, const Abstract_Object &));
	// clustering using hypercube range search as assignment method
	void cube_range_search_clustering(const Dataset & dataset, const std::string & update_method, double (*metric)(const Abstract_Object &, const Abstract_Object &));
	// clustering using frechet range search as assignment method
	void frechet_range_search_clustering(const Dataset & dataset, const std::string & update_method, double (*metric)(const Abstract_Object &, const Abstract_Object &));
	
	// updates each centroid to the mean vector/curve of its cluster, and returns true if centroids changed by little (algorithm has converged)
	bool update(const std::string & update_method, double (*metric)(const Abstract_Object &, const Abstract_Object &));
	
	// finds silhouette for each cluster, and for clustering in total
	std::vector <double> silhouette(double (*metric)(const Abstract_Object &, const Abstract_Object &)) const;
	
	
};

// returns mean vector or mean curve of given cluster of objects
Abstract_Object * mean(const std::string & update_method, const std::vector <const Abstract_Object*> & cluster);
int binary_search(const std::vector <float> & P, float x, int lower_index, int upper_index);
#endif