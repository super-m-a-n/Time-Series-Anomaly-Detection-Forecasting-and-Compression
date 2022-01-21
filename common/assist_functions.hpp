// file : assist_functions.hpp
//Functions used for assistance with minor functionalities of the program
#pragma once
#include <vector>

//computes floor(log_2(n)) + 1
unsigned int get_lg(unsigned int n);

//Used for the filtering of the input curve (Only used for continuous frechet)
std::vector<float> filter_input_curve(std::vector<float> input_data);

//returns largest power of 2 smaller than given number
unsigned int largest_power_of_2_smaller_than(unsigned int n);

