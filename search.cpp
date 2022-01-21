//file:search.cpp//
#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <time.h>
#include <cstdlib>
#include "input_check.hpp"
#include "dataset.hpp"
#include "params.hpp"
#include "search_method.hpp"
#include "lsh_struct.hpp"
#include "hypercube_class.hpp"
#include "assist_functions.hpp"

// main for curve similarity search
// uses 1)vector_lsh, 2)vector_hypercube, 3)curve_lsh(frechet) for time series

// global program parameters
int N = 10, R, d = 0, w, n = 0;		// global
int k, L;							// vector lsh
int d1, probes, M;					// vector hypercube
double delta, epsilon = 0.01;		// curve lsh (frechet)
std::string algorithm, metric_func;	// variable for algorithm , metric used for frechet

int main(int argc, char const *argv[])
{
	// variables for program input files
	std::string input_file, query_file, output_file;
	
	srand(time(NULL));

	// check for input args and initialize them
	if (!check_init_args(argc, argv, input_file, query_file, k, d1, L, M, probes, output_file, algorithm, metric_func, delta))
	{
		std::cerr << "\nWrong command line input. Use : ./search -i <input_file> -q <query_file> -k <int> -L <int> -M <int> -probes <int> ";
		std::cerr << "-o <output_file> -algorithm <LSH / Hypercube / Frechet> -metric <discrete / continuous> -delta <double>\n";
		std::cerr << "Each -x <value> pair is optional.  -metric to be used only for -algorithm Frechet\n\n";
		return EXIT_FAILURE;
	}
	
	//ask for input path, if not given through command line
	if (input_file.empty())
	{
		std::cout << "\nPlease give an input file path ->  ";
		std::getline(std::cin, input_file);
		std::cout << std::endl;
	}
	
	// read input file and initialize arguments n and d
	if (!read_input_file(input_file, n, d))
	{
		std::cerr << "\nGiven input file path/name could not be found (invalid file path)\n\n";
		return EXIT_FAILURE;
	}

	if (algorithm.empty())
	{
		while (algorithm != "LSH" && algorithm != "Hypercube" && algorithm != "Frechet")
		{
			std::cout << "\nPlease give an algorithm for curve similarity search (LSH / Hypercube / Frechet) ->  ";
			std::getline(std::cin, algorithm);
			std::cout << std::endl;
		}
	}

	if (metric_func.empty() && algorithm == "Frechet")
	{
		while (metric_func != "discrete" && metric_func != "continuous")
		{
			std::cout << "\nPlease give a metric for frechet curve similarity search (discrete / continuous) ->  ";
			std::getline(std::cin, metric_func);
			std::cout << std::endl;
		}
	}

	std::cout << "\nReading Input Dataset   --> ";
	// create a dataset object that will hold all the input objects-points
	Dataset dataset(n, input_file); 
	std::cout << "Completed\n";

	// pointer to abstract class search_method object
	search_method * method;
	
	// pointer to metric function to be used
	double (*metric_function)(const Abstract_Object &, const Abstract_Object &);
	int numBuckets;

	// depending on algorithm used, method pointer will point to necessary structure
	if (algorithm == "LSH")
	{
		w = 20;									// experimental value (testing required)
		numBuckets = floor(n/16);				// experimental value (testing required)
		method = new lsh_struct(numBuckets);	// vector-curve lsh to be used, so create entire structure for lsh algorithm
		metric_function = euclidean;			// metric used is euclidean distance
	}
	else if (algorithm == "Hypercube")
	{
		w = 50;							// experimental value (testing required)
		method = new hypercube;			// vector-curve hypercube to be used, so create entire structure for hypercube algorithm
		metric_function = euclidean;	// metric used is euclidean distance	
	}
	else if (algorithm == "Frechet")
	{
		w = 40;									// experimental value (testing required)
		numBuckets = floor(n/16);				// experimental value (testing required)
		method = new lsh_struct(numBuckets);	// frechet-curve lsh to be used, so create entire structure for lsh algorithm

		if (!delta)		// if no value was given for delta through command line
		{
			if (metric_func == "discrete")
			{
				// delta << 4 * dim of grid * min {m1, m2} ,  dim of grid = 2, m1=m2=d for our case
				delta = (8 * d) * 0.000205479452054;					// experimental value (testing required)
			}
			else // metric continuous
			{
				// delta << 4 * dim of grid * min {m1, m2} ,  dim of grid = 1, m1=m2=d for our case
				delta = (4 * d) * 0.000205479452054;					// experimental value (testing required)	
			}
		}

		//If the metric function is the continuous frechet, the distance will not be between Abstract Objects but
		// between fred Curves using the continuous frechet distance of the fred library.
		// However, for the approximate neighbors, the discrete frechet will be used (but in modified curves unlike the discrete frechet case)
		metric_function = discrete_frechet;
	}
	else	// default
	{
		w = 30;									// experimental value (testing required)
		numBuckets = floor(n/16);				// experimental value (testing required)
		method = new lsh_struct(numBuckets);	// vector-curve lsh to be used, so create entire structure for lsh algorithm
		metric_function = euclidean;			// metric used is euclidean distance
	}

	std::cout << "Importing Input Dataset --> ";
	// import dataset
	method->import_data(dataset);
	std::cout << "Completed\n";

	bool exit_val = false;

	while (exit_val == false)
	{
		if (query_file.empty())
		{
			std::cout << "\nPlease give a query file path ->  ";
			std::getline(std::cin, query_file);
			std::cout << std::endl;
		}

		int nq = 0, dq = 0;
		// read query file and initialize arguments nq and dq
		if (!read_input_file(query_file, nq, dq))
		{
			std::cerr << "\nGiven query file path/name could not be found (invalid file path)\n\n";
			return EXIT_FAILURE;
		}

		std::cout << "Creating Query Dataset  --> ";
		// create a dataset object that will hold all the query objects-points
		Dataset query_dataset(nq, query_file); 
		std::cout << "Completed\n";

		if (output_file.empty())
		{
			std::cout << "\nPlease give an output file path ->  ";
			std::getline(std::cin, output_file);
			std::cout << std::endl;
		}

		std::cout << "Executing ...\n";
		// execute kNN, range search nearest neighbors algorithms using given metric function
		if (!method->execute(dataset, query_dataset, output_file, N, 0, metric_function))
		{
			std::cerr << "\nError occured while opening given output file\n\n";
			return EXIT_FAILURE;
		}
		
		std::string answer;
		while (answer != "t" && answer != "r")	// user must type "t" to terminate or "r" to run again
		{
			std::cout << "\nTerminate (t) or run again for different query file (r) ? (t/r) : ";
			std::getline(std::cin, answer);
		}

		if (answer == "t")
			exit_val = true;
		else if (answer == "r")
		{
			query_file = "";
			output_file = "";
		}
	}
	
	delete method;
	return EXIT_SUCCESS;
}