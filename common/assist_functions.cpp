// file : assist_functions.cpp
#include <iostream>
#include "assist_functions.hpp"
#include "params.hpp"

//computes floor(log_2(n)) + 1
unsigned int get_lg(unsigned int n){
    int r = 0;

    while ( n > 0){
        n = n >> 1;
        r += 1;
    }

    return r;
}

std::vector<float> filter_input_curve(std::vector<float> input_data){
	if (algorithm == "Frechet" && metric_func == "continuous"){
        
		int size = input_data.size();
        int i = 0, j = 1;
		std::vector<float> filtered_data;
		while(j < size - 1){
			float a = std::abs(input_data[j] - input_data[i]);
			float b = std::abs(input_data[j+1] - input_data[j]);
			float max = (a<b)?b:a;
			if (max >= epsilon){
				filtered_data.push_back(input_data[i]);
				i = j;
			}
			j += 1;
		}

		// Special handling in case the vector has dimension 1
		if (j == size -1){
            filtered_data.push_back(input_data[i]);
			filtered_data.push_back(input_data[j]);
		}
		else {
			filtered_data.push_back(input_data[0]);
		}
		return filtered_data;

	}
	else {
		return input_data;
	}
	
}

//returns largest power of 2 smaller than given number
unsigned int largest_power_of_2_smaller_than(unsigned int n)
{
	// returns the number with the leftmost set bit of number set, and the rest all unset
	n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n ^ (n >> 1);
}