//file:params.hpp//
#ifndef _PARAMS_HPP_
#define _PARAMS_HPP_

#include <string>

// just the declaration of some useful global program parameters
extern int k;	// For the LSH it is: the number of hash functions h that compose the amplified hash function g
extern int d1;  // For the hypercube: It is the dimension of the hypercube
extern int L;	// the number of different hash-tables and amplified hash functions g
extern int d;	// the dimension of the objects-points of the dataset
extern int w;   // the window parameter for the h hash functions
extern int M;
extern int probes;
extern int K;	// the number of clusters-centroids

extern double epsilon;  // the epsilon constant used for filtering 
extern double delta;	// the number used to define the grids for snapping the curves
extern std::string algorithm;	// algorithm used
extern std::string metric_func;      // metric used


#endif