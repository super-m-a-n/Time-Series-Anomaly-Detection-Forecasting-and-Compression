//file:cluster.cpp//
#include <iostream>
#include <string>
#include "input_check.hpp"
#include "dataset.hpp"
#include "params.hpp"
#include "cluster_info.hpp"
#include <time.h>

// global program parameters
int k, L, d = 0, d1, w, n = 0, probes, M, K, R, N=1;
// variable for algorithm , metric used for frechet
std::string algorithm, metric_func;
double delta, epsilon = 0.01;		// curve lsh (frechet)

int main(int argc, char const *argv[])
{
	srand (time(NULL));

	// variables for program input files
	std::string input_file, config_file, output_file;
	
	// if complete is true, output will be verbose
	bool complete;
	
	// if silhoutte is true, silhouette metric will be calculated for clustering
	bool silhouette;
	
	// method for assignment part of clustering
	std::string assignment_method;	// assignment method is either Classic for classic lloyd's or LSH or Hypercube or LSH_Frechet
	
	// method for centroid-update part of clustering
	std::string update_method; 		// update method is either Mean Frechet or Mean Vector

	// check for input args and initialize them
	if (!check_init_args(argc, argv, input_file, config_file, output_file, update_method, assignment_method, complete, silhouette))
		return EXIT_FAILURE;
	
	// check for config file args and initialize them
	if (!check_init_config_args(config_file, K, L, k, M, d1, probes))
		return EXIT_FAILURE;

	// read input file and initialize arguments n and d
	if (!read_input_file(input_file, n, d))
	{
		std::cerr << "\nGiven input file path/name could not be found (invalid file path)\n\n";
		return EXIT_FAILURE;
	}

	// pointer to metric function to be used
	double (*metric_function)(const Abstract_Object &, const Abstract_Object &);

	if (update_method == "Mean Frechet")
		algorithm = "Frechet";
	else
	{
		// update method is Mean Vector
		if (assignment_method == "LSH" || assignment_method == "Hypercube")
			algorithm = assignment_method;
		else
			algorithm = "";
	}

	if (algorithm == "Frechet")
	{
		metric_func = "discrete";
		metric_function = discrete_frechet;
		// delta << 4 * dim of grid * min {m1, m2} ,  dim of grid = 2, m1=m2=d for our case
		delta = (8 * d) * 0.000205479452054;					// experimental value (testing required)
	}
	else
	{
		metric_func = "";
		metric_function = euclidean;
	}

	std::cout << "\nReading Input Dataset  --> ";
	// create a dataset object that will hold all the input objects-points
	Dataset dataset(n, input_file);
	std::cout << "Completed\n";

	w = 50;

	// create a cluster object that will hold info necessary for clustering
	Cluster_info cluster_info;

	// executes clustering using given update_method, assignment_method and metric, and outputs results into output file
	if (!cluster_info.execute(dataset, output_file, update_method, assignment_method, complete, silhouette, metric_function))
	{
		std::cerr << "\nError occured while opening given output file\n\n";
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}