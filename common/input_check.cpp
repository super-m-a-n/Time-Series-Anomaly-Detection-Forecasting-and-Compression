//file:input_check.cpp//
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "input_check.hpp"
#include "params.hpp"
#include "assist_functions.hpp"

bool check_init_args(int argc, const char ** argv, std::string & input_file, std::string & query_file, int & k, int & L, std::string & output_file, int & N, int & R)
{
	// set default values for the parameters, in case no specific value was given through terminal (all cmd parameters are optional)
	input_file = "";
	query_file = "";
	output_file = "";
	k = 4;
	L = 5;
	N = 1;
	R = 10000;

	// cmd input should have an odd number of args (an even number due to ("-x", value) pairs , plus the argv[0])
	if (argc % 2 == 0)
		return false;
	int i = 2;
	// cmd input should have "-x" at odd positions (x is in {i,q,o,k,L,N,R}) and actual parameter values at even positions
	while (i < argc)
	{
		if (!strcmp(argv[i-1], "-i"))
		{
			input_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-q"))
		{
			query_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-o"))
		{
			output_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-k"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;

			k = atoi(argv[i]);

		}
		else if (!strcmp(argv[i-1], "-L"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			L = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-N"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			N = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-R"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			R = atoi(argv[i]);
		}
		else
			return false;

		i+=2;
	}

	return true;
}


//Check arguments for hypercube
bool check_init_args(int argc, const char ** argv, std::string & input_file, std::string & query_file, int & d1, int & M, int & probes, std::string & output_file, int & N, int & R){
	// set default values for the parameters, in case no specific value was given through terminal (all cmd parameters are optional)
	input_file = "";
	query_file = "";
	output_file = "";
	d1 = 14;
	M = 10;
	probes = 2;
	N = 1;
	R = 10000;

	// cmd input should have an odd number of args (an even number due to ("-x", value) pairs , plus the argv[0])
	if (argc % 2 == 0)
		return false;
	int i = 2;
	// cmd input should have "-x" at odd positions (x is in {i,q,o,k,L,N,R}) and actual parameter values at even positions
	while (i < argc)
	{
		if (!strcmp(argv[i-1], "-i"))
		{
			input_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-q"))
		{
			query_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-o"))
		{
			output_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-k"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;

			d1 = atoi(argv[i]);

		}
		else if (!strcmp(argv[i-1], "-probes"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			probes = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-M"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			M = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-N"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			N = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-R"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			R = atoi(argv[i]);
		}
		else
			return false;

		i+=2;
	}
	return true;
}

bool check_init_args(int argc, const char ** argv, std::string & input_file, std::string & config_file, std::string & output_file, bool & complete, std::string & method)
{
	// all cmd parameters here are mandatory (for clustering)
	if (argc != 10 && argc != 9)
	{
		std::cerr << "\nWrong command line input. Use : ./cluster -i <input_file> -c <config_file> -o <output_file> -complete <optional> -m <method>\n";
		std::cerr << "-complete is optional, <method> is Classic or LSH or Hypercube\n\n";
		return false;
	}

	if (strcmp(argv[1], "-i") != 0 || strcmp(argv[3], "-c") != 0 || strcmp(argv[5], "-o") != 0)
	{
		std::cerr << "\nError: one or more wrong input parameters" << std::endl << "Use : -i -c -o -complete <optional> -m\n\n";
		return false;
	}

	//initialize files
	input_file = argv[2];
	config_file = argv[4];
	output_file = argv[6];

	// check if -complete is given
	if (!strcmp(argv[7], "-complete"))
	{
		complete = true;

		if (!strcmp(argv[8], "-m"))		// check for -m
		{
			// check if <method> is either Classic or LSH or Hypercube
			if (strcmp(argv[9], "Classic") != 0 && strcmp(argv[9], "LSH") != 0 && strcmp(argv[9], "Hypercube") != 0)
			{
				std::cerr << "Error: invalid input parameter <method>" << std::endl << "Use : <method> --> Classic or LSH or Hypercube\n\n";
				return false;
			}
			
			method = argv[9];

		}
		else
		{
			std::cerr << "\nError: one or more wrong input parameters" << std::endl << "Use : -i -c -o -complete <optional> -m\n\n";
			return false;
		}
	}
	else if (!strcmp(argv[7], "-m"))
	{
		complete = false;

		// check if <method> is either Classic or LSH or Hypercube
		if (strcmp(argv[8], "Classic") != 0 && strcmp(argv[8], "LSH") != 0 && strcmp(argv[8], "Hypercube") != 0)
		{
			std::cerr << "Error: invalid input parameter <method>" << std::endl << "Use : <method> --> Classic or LSH or Hypercube\n\n";
			return false;
		}
		
		method = argv[8];

	}
	else
	{
		std::cout << "Error input 3" << std::endl;
		std::cerr << "\nError: one or more wrong input parameters" << std::endl << "Use : -i -c -o -complete <optional> -m\n\n";
		return false;
	}

	
	return true;
}

bool check_init_config_args(std::string & config_file, int & K, int & L, int & k, int & M, int & d1, int & probes)
{
	return read_config_file(config_file, K, L, k, M, d1, probes);
}

bool read_config_file(std::string & config_file, int & K, int & L, int & k, int & M, int & d1, int & probes)
{
	char * line = NULL;
	char * file_name = &config_file[0];
    size_t length = 0;

    FILE *file_ptr;
	file_ptr = fopen(file_name, "r");  /*open config file , in read mode*/
	
	if (file_ptr == NULL)
	{
		std::cerr << "\nGiven config file path/name could not be found (invalid file path)\n\n";
	    return false;				// file not found
	}

	else    /*following block of code reads from the file and initializes the parameters */
	{
		// set default values for the optional parameters, in case no specific value was given in config file
		L = 3;
		k = 4;
		M = 10;
		d1 = 3;
		probes = 2;

		int line_number = 0;

	  	while(getline(&line, &length, file_ptr) != -1)
	    {
	    	line_number++;
	    	if (line[strlen(line)-1] == '\n')
	    		line[strlen(line)-1] = '\0';		// remove newline character from line read from file
	    	if (line[strlen(line)-1] == '\r')	// remove potential \r character from line read from file
	    		line[strlen(line)-1] = '\0';

	    	char * str = strtok(line, " ");
	    	std::string desc;	// description of line
	    	int value = 0;
		    int i = 0;

		    while(str != NULL)
		    {
		    	if (line_number == 1 && i == 0 && strcmp(str, "number_of_clusters:") != 0)
		    	{
		    		std::cerr << "Error: invalid config file" << std::endl << "First line of file should be : number_of_clusters: <int>\n\n";
		    		free(line);
		    		fclose(file_ptr);
		    		return false;	 
		    	}
		    	
		      	if (i == 0)
		      		desc = str;			// save description
		      	else if (i == 1)
		      	{
		      		if (!is_integer(str) || !atoi(str))	// each line's second argument should be a valid integer
		      		{
		      			std::cerr << "Error: invalid config file" << std::endl << "Each line of file should be of the form : description: <int>\n\n";
		      			free(line);
		    			fclose(file_ptr);
						return false;
		      		}
					else
						value = atoi(str);	// save value
		      	}

		      	i++;
		        str = strtok(NULL, " ");
		    }

		    if (i != 2)
		    {
		    	std::cerr << "Error: invalid config file" << std::endl << "Each line of file should be of the form : description: <int>\n\n";
		    	free(line);
		    	fclose(file_ptr);
		    	return false;
		    }

		    // check which description was read, and assign value to corresponding parameter
		    if (desc == "number_of_clusters:")
		    	K = value;
		    else if (desc == "number_of_vector_hash_tables:")
		    	L = value;
		    else if (desc == "number_of_vector_hash_functions:")
		    	k = value;
		    else if (desc == "max_number_M_hypercube:")
		    	M = value;
		    else if (desc == "number_of_hypercube_dimensions:")
		    	d1 = value;
		    else if (desc == "number_of_probes:")
		    	probes = value;
		    else
		    {
		    	std::cerr << "Error: invalid config file" << std::endl << "description" << desc << "not valid\n\n";
		    	free(line);
		    	fclose(file_ptr);
		    	return false;
		    }
	    }

	   	free(line);
	}

	// close file
	fclose(file_ptr);


	//std::cout << "After configuration " << std::endl;

	return true;
}

bool is_integer(const char * string)
{
	uint k = strlen(string);
	for (uint i = 0; i < k ; i++)
	{
		if (string[i] < '0' || string[i] > '9')
			return false;
	}

	return true;
}

//./cube –i <input file> –q <query file> –k <int> -M <int> -probes <int> -ο
// <output file> -Ν <number of nearest> -R <radius>

bool read_input_file(std::string & input_file, int & n, int & d)
{
	// we use C code to read file because its faster than C++ and given input files are massive
	char * line = NULL;
	char * file_name = &input_file[0];
    size_t length = 0;

    FILE *file_ptr;
	file_ptr = fopen(file_name, "r");  /*open input file , in read mode*/
	
	if (file_ptr == NULL)
	    return false;				// file not found

	else    /*following block of code reads from the file and finds the number of points n and the dimension of the points d */
	{
	  	while(getline(&line, &length, file_ptr) != -1)
	    {
	    	n++;			// one point-object is in each line of file
	    	if (n == 1)		// find dimension d of all point-objects just once (at first point)
	    	{
	    		line[strlen(line)-1] = '\0';		// remove newline character from line read from file
	    		if (line[strlen(line)-1] == '\r')	// remove potential \r character from line read from file
	    			line[strlen(line)-1] = '\0';

	    		char *str = strtok(line, "\t");
		      	int i = 0;
		      	while(str != NULL)
		      	{
		      		i++;
		         	str = strtok(NULL, "\t");
		      	}

		      	d = i - 1;
	    	}
	     
	     }

	    free(line);
	}

	// rewind to the start of the file
	rewind(file_ptr);
	// close file
	fclose(file_ptr);

	return true;
}

// check and set arguments for curve search (incluces check_init_args for lsh and hypercube)
bool check_init_args(int argc, const char ** argv, std::string & input_file, std::string & query_file, int & k, int & d1, int & L, int & M, int & probes, 
	std::string & output_file, std::string & algorithm, std::string & metric, double & delta)
{
	// set default values for the parameters, in case no specific value was given through terminal (all cmd parameters are optional)
	input_file = "";
	query_file = "";
	output_file = "";
	algorithm = "";
	metric = "";
	k = 4;
	L = 5;
	d1 = 14;
	M = 10;
	probes = 2;
	delta = 0;

	// cmd input should have an odd number of args (an even number due to ("-x", value) pairs , plus the argv[0])
	if (argc % 2 == 0)
		return false;
	int i = 2;
	// cmd input should have "-x" at odd positions (x is in {i, q, o, k, L, M, probes, algorithm, metric, delta}) and actual parameter values at even positions
	while (i < argc)
	{
		if (!strcmp(argv[i-1], "-i"))
		{
			input_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-q"))
		{
			query_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-o"))
		{
			output_file = argv[i];
		}
		else if (!strcmp(argv[i-1], "-algorithm"))
		{
			if (strcmp(argv[i], "LSH") && strcmp(argv[i], "Hypercube") && strcmp(argv[i], "Frechet"))
				return false;		// algorithm is either LSH or Hypercube or Frechet

			algorithm = argv[i];
		}
		else if (!strcmp(argv[i-1], "-metric"))
		{
			if (strcmp(argv[i], "discrete") && strcmp(argv[i], "continuous"))
				return false;		// metric is either discrete or continuous
			
			metric = argv[i];
		}
		else if (!strcmp(argv[i-1], "-k"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;

			// both are set, only one will be used depending on algorithm
			k = atoi(argv[i]);	
			d1 = atoi(argv[i]);

		}
		else if (!strcmp(argv[i-1], "-L"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			L = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-M"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			M = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-probes"))
		{
			if (!is_integer(argv[i]) || !atoi(argv[i]))
				return false;
			probes = atoi(argv[i]);
		}
		else if (!strcmp(argv[i-1], "-delta"))
		{
			if (!atof(argv[i])) // error in string to double conversion
				return false;
			delta = atof(argv[i]);
		}
		
		else
			return false;

		i+=2;
	}

	if (!algorithm.empty() && algorithm != "Frechet" && !metric.empty())	// metric only to be used with algorithm Frechet
		return false;

	return true;
}

// checks for correct input args from terminal and initializes program parameters if so (for clustering)
bool check_init_args(int argc, const char ** argv, std::string & input_file, std::string & config_file, std::string & output_file, std::string & update_method, std::string & assignment_method, bool& complete, bool& silhouette)
{
	// all cmd parameters here are mandatory (for clustering), except for -complete and -silhouette
	if (argc != 12 && argc != 13 && argc != 14)
	{
		std::cerr << "\nWrong command line input. Use : ./cluster -i <input_file> -c <config_file> -o <output_file> -update <update_method> -assignment <assignment_method> -complete <optional> -silhouette <optional>\n";
		std::cerr << "-complete is optional, -silhouette is optional, <update_method> is Mean Frechet or Mean Vector, <assignment_method> is Classic or LSH or Hypercube or LSH_Frechet\n\n";
		return false;
	}

	if (strcmp(argv[1], "-i") != 0 || strcmp(argv[3], "-c") != 0 || strcmp(argv[5], "-o") != 0 || strcmp(argv[7], "-update") != 0 || strcmp(argv[10], "-assignment") != 0 )
	{
		std::cerr << "\nError: one or more wrong input parameters" << std::endl << "Use : -i -c -o -update -assignment -complete <optional> -silhouette <optional>\n\n";
		return false;
	}

	// initialize files
	input_file = argv[2];
	config_file = argv[4];
	output_file = argv[6];

	// check if <update_method> is either Mean Frechet or Mean Vector
	if (strcmp(argv[8], "Mean") != 0 || (strcmp(argv[9], "Vector") != 0 && strcmp(argv[9], "Frechet") != 0))
	{
		std::cerr << "\nError: invalid input parameter <update_method>" << std::endl << "Use : <update_method> --> Mean Frechet or Mean Vector\n\n";
		return false;
	}

	// initialize update_method
	update_method = argv[8];
	std::string str = argv[9];
	update_method += " ";
	update_method += str;

	// check if <assignment_method> is either Classic or LSH or Hypercube or LSH_Frechet
	if (strcmp(argv[11], "Classic") != 0 && strcmp(argv[11], "LSH") != 0 && strcmp(argv[11], "Hypercube") != 0 && strcmp(argv[11], "LSH_Frechet") != 0)
	{
		std::cerr << "\nError: invalid input parameter <assignment_method>" << std::endl << "Use : <assignment_method> --> Classic or LSH or Hypercube or LSH_Frechet\n\n";
		return false;
	}

	// initialize assignment_method
	assignment_method = argv[11];

	// check if pair of update_method and assignment_method is valid
	if (update_method == "Mean Vector"  &&  assignment_method == "LSH_Frechet")
	{
		std::cerr << "\nMean Vector <update_method> is not compatible with LSH_Frechet <assignment_method>\n";
		std::cerr << "Use : <assignment_method> --> Classic or LSH or Hypercube  , when <update_method> = Mean Vector\n\n";
		return false;
	}

	if (update_method == "Mean Frechet"  &&  (assignment_method == "LSH" || assignment_method == "Hypercube"))
	{
		std::cerr << "Mean Frechet <update_method> is not compatible with " << assignment_method << " <assignment_method>\n";
		std::cerr << "Use : <assignment_method> --> Classic or LSH_Frechet  , when <update_method> = Mean Frechet\n\n";
		return false;
	}

	// check if -complete or -silhouette were given
	if (argc == 12)
	{
		complete = false;
		silhouette = false;
	}
	else if (argc == 13)
	{
		if (!strcmp(argv[12], "-complete"))
		{
			complete = true;
			silhouette = false;
		}
		else if (!strcmp(argv[12], "-silhouette"))
		{
			complete = false;
			silhouette = true;
		}
		else
		{
			std::cerr << "\nError: Expected -complete or -silhouette" << std::endl << "Use : -i -c -o -update -assignment -complete <optional> -silhouette <optional>\n\n";
			return false;
		}
	}
	else if (argc == 14)
	{
		if (strcmp(argv[12], "-complete") != 0 || strcmp(argv[13], "-silhouette") != 0)
		{
			std::cerr << "\nError: Expected -complete and then -silhouette" << std::endl << "Use : -i -c -o -update -assignment -complete <optional> -silhouette <optional>\n\n";
			return false;
		}

		complete = true;
		silhouette = true;
	}
	else
		return false;

	return true;
}

