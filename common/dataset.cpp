//file:dataset.cpp//
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "dataset.hpp"
#include "assist_functions.hpp"
#include "object.hpp"
#include "params.hpp"



Dataset::Dataset(int num_of_Points, std::string & input_file) : num_of_Objects(num_of_Points)
{
	// create dataset array
	dataset = new Abstract_Object*[num_of_Points];

	// we use C code to read file because its faster than C++ and given input files are massive
	char * line = NULL;
	char * file_name = &input_file[0];
    size_t length = 0;

    FILE *file_ptr;
	file_ptr = fopen(file_name, "r");  /*open input file , in read mode*/
	
	if (file_ptr == NULL)
	{
		// file not found ( this should NEVER occur however, since we have already checked that input_file path name is valid, from lsh.cpp)
	    std::cerr << "\nGiven input file path/name could not be found (invalid file path)\n\n";
		exit(EXIT_FAILURE);
	}

	else    /*following block of code reads from the file and inserts the data in the dataset array*/
	{
		int point_index = -1;

		while(getline(&line, &length, file_ptr) != -1)		// while loop will run for exaclty num_of_Objects times
		{
			point_index++;
		    line[strlen(line)-1] = '\0';		// remove newline character from line read from file
		    if (line[strlen(line)-1] == '\r')	// remove potential \r character from line read from file
		    	line[strlen(line)-1] = '\0';

		    char *str = strtok(line, "\t");
		    int j = 0;

		    // local variables that will serve as arguments for Abstract Object subclass constructor
		    std::string object_name;
		    std::vector <float> input_data(d);
		    
		    while(str != NULL)
		    {
		    	switch (j)
		        {
		         	case 0:  object_name = str; break;				// save name identifier of point as string
		         	default: input_data[j-1] = atof(str);			// save coordinates of point
		         	         	
		        }
		        
		        j++;
		        str = strtok(NULL, "\t");
		    }

		    // different Abstract_Object depending on algorithm
		    if (algorithm == "LSH" || algorithm == "Hypercube")
		    {
		    	dataset[point_index] = new Object(input_data, object_name);		 // create Abstract Object of type Object
		    }
		    else if (algorithm == "Frechet" && metric_func == "discrete")
		    {
		    	dataset[point_index] = new time_series(input_data, object_name); // create Abstract Object of type time_series
		    }
		    else if (algorithm == "Frechet" && metric_func == "continuous")
		    {	

				std::vector<float> filtered_curve = filter_input_curve(input_data); // Filter the curve

		    	dataset[point_index] = new Object(filtered_curve, object_name);		 // create Abstract Object of type Object (flattened time_series)
		    }
		    else
		    {
		    	dataset[point_index] = new Object(input_data, object_name);		 // default : create Abstract Object of type Object
		    }


		}

		free(line);
	}

	fclose(file_ptr);
}

Dataset::~Dataset()
{
	for (int i = 0; i < num_of_Objects; ++i)
	{
		delete dataset[i];
	}

	delete[] dataset;
}

void Dataset::print() const
{
	for (int i = 0; i < num_of_Objects; ++i)
	{
		this->dataset[i]->print();
		std::cout << std::endl << std::endl;
	}
}

int Dataset::get_num_of_Objects() const
{
	return this->num_of_Objects;
}

const Abstract_Object& Dataset::get_ith_object(int i) const
{
	return *(this->dataset[i]);
}