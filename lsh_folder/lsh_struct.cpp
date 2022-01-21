//file:lsh_struct.cpp//
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <chrono>
#include <queue>
#include <utility>
#include <vector>
#include <cstdint>
#include "params.hpp"
#include "lsh_struct.hpp"
#include "hash.hpp"
#include "dataset.hpp"
#include "object.hpp"
#include "fred/include/frechet.hpp"

#if 1
Curve* transform_to_Fred_Curve(const Object& obj){

	Points points = Points(1);

	int size = obj.get_dim();
	for (int i = 0 ; i < size ; i++){
		Point point = Point(1);
		point.set(0, obj.get_ith(i));
		points.add(point);
	}
	return new Curve(points);
}
#endif

lsh_struct::lsh_struct(int hash_table_size): curve_vect()//, complexity(-1)
{
	// lsh_hash_struct is an array of pointers to L hash tables
	lsh_hash_struct = new hash_table*[L];

	for (int i = 0; i < L; ++i)
	{
		lsh_hash_struct[i] = new hash_table(hash_table_size);
	}
}

lsh_struct::~lsh_struct()
{
	for (long unsigned int i = 0 ; i  < curve_vect.size() ; i++){
		delete curve_vect[i];
	}
	for (int i = 0; i < L; ++i)
		delete lsh_hash_struct[i];

	delete[] lsh_hash_struct;
}

void lsh_struct::import_data(const Dataset & dataset)
{
	int num_of_Objects = dataset.get_num_of_Objects();

	for (int i = 0; i < num_of_Objects; ++i)					// for each of the point_objects
	{
		const Abstract_Object& abstract_obj = dataset.get_ith_object(i);
		if (algorithm == "Frechet" && metric_func == "continuous"){
			curve_vect.push_back(transform_to_Fred_Curve(dynamic_cast<const Object&>(abstract_obj)));
		}
		for (int j = 0; j < L; ++j)		// for each of the L hashtables
		{
			(this->lsh_hash_struct[j])->insert(abstract_obj); // insert object into hash-table
		}
	}

}

bool lsh_struct::execute(const Dataset & dataset, const Dataset & query_dataset, const std::string & output_file, const int & N, const int & R, double (*metric)(const Abstract_Object &, const Abstract_Object &))
{

	std::ofstream file (output_file, std::ios::out);		// open output file for output operations
	
	if (!file.is_open())			// check if file was opened properly
	    return false;				// error occured

	int num_of_Objects = query_dataset.get_num_of_Objects();
	
	// counters for metrics
	double sum_dist_lsh = 0;
	double sum_dist_true = 0;
	double avg_AF = 0;
	double avg_TF = 0;
	double tApprAvg = 0;
	double tTrueAvg = 0;
	double max_AF = 0;
	int not_found = 0;

	for (int i = 0; i < num_of_Objects; i++)		// run for each of the query Objects
	{
		file << "Query: query Object " << (query_dataset.get_ith_object(i)).get_name() << "\n";
		if (algorithm == "LSH")
			file << "Algorithm: LSH_Vector  \n\n";
		else if (algorithm == "Frechet" && metric_func == "discrete")
			file << "Algorithm: LSH_Frechet_Discrete  \n\n";
		else if (algorithm == "Frechet" && metric_func == "continuous")
			file << "Algorithm: LSH_Frechet_Continuous  \n\n";

		//start timer for lsh
		auto t_lsh_start = std::chrono::high_resolution_clock::now();
		
		// run approximate nearest neighbors and return the neighbors and the distances found
		std::vector <std::pair <double, const Abstract_Object*> > appr_nearest = this->appr_nearest_neighbors(dataset, query_dataset.get_ith_object(i), N, metric);
		
		// end timer for lsh
		auto t_lsh_end = std::chrono::high_resolution_clock::now();

		// start timer for brute force
		auto t_true_start = std::chrono::high_resolution_clock::now();

		// run exact nearest neighbors and return the neighbors and the distances found
		std::vector <std::pair <double, const Abstract_Object*> > exact_nearest = this->exact_nearest_neighbors(dataset, query_dataset.get_ith_object(i), N, metric);
		
		// end timer for brute force
		auto t_true_end = std::chrono::high_resolution_clock::now();

		for (int index = 0; index < N; ++index)	
		{
			double dist_lsh = 0, dist_true = 0, AF = 0;
			bool found = false;

			if ((int) appr_nearest.size() >= index + 1)	// neighbors found may be less than N
			{
				const Abstract_Object * object = std::get<1>(appr_nearest[index]);		// get object-neighbor found
				double dist = std::get<0>(appr_nearest[index]);							// get distance from query object
				sum_dist_lsh += dist;
				dist_lsh = dist;
				found = true;

				file << "Approximate Nearest neighbor-" << index + 1 << " : Object " << object->get_name() << '\n';	//write to file
				file << "distanceApproximate : " << dist << '\n';
			}
			else
				not_found++;

			if ((int) exact_nearest.size() >= index + 1)	// neighbors found may be less than N
			{
				const Abstract_Object * object = std::get<1>(exact_nearest[index]);		// get object-neighbor found
				double dist = std::get<0>(exact_nearest[index]);						// get distance from query object
				sum_dist_true += dist;
				dist_true = dist;

				file << "True Nearest neighbor-" << index + 1 << " : Object " << object->get_name() << '\n';
				file << "distanceTrue : " << dist << "\n\n";			// write to file
			}

			if (found)
			{
				AF = dist_lsh/dist_true;
				if (AF > max_AF)
					max_AF = AF;
				avg_AF += AF;
			}
		}

		// get execution for LSH in milliseconds 
	    std::chrono::duration <double, std::milli> tLSH = t_lsh_end - t_lsh_start;
	    // get execution for brute force in milliseconds
	    std::chrono::duration <double, std::milli> tTrue = t_true_end - t_true_start;
	    // write times of execution in file
	    file << "tLSH : " << tLSH.count() << "ms\n";
	    file << "tTrue : " << tTrue.count() << "ms\n\n";

	    avg_TF += tLSH.count() / tTrue.count();
	    tApprAvg += tLSH.count();
		tTrueAvg += tTrue.count();

	    if (R != 0)  // if given Range for range search is 0 skip range search
	    {
			file << "R-near neighbors: (R = " << R << ")" << '\n';

			// run approximate range search and write results into file
			std::list <std::pair <double, const Abstract_Object*> > R_list = this->range_search(query_dataset.get_ith_object(i), R, metric);
			std::list <std::pair <double, const Abstract_Object*> > ::iterator it = R_list.begin();

			while (it != R_list.end()){
				// object is within range, so ass it to the list
					file << "Object " << (std::get<1>(*it))->get_name() << '\n';
					++it;
			}
			R_list.clear();
		}
		file << "\n\n";
	}

	// print metrics to file
	file << "tApproximateAverage: " << tApprAvg/num_of_Objects << '\n';
	file << "tTrueAverage: " << tTrueAvg/num_of_Objects << '\n';
	file << "MAF: " << max_AF << "\n\n";

	//print metrics to std::out
	std::cout << "\n\nSum dist true / Sum dist lsh = " << sum_dist_true / sum_dist_lsh << std::endl;
	std::cout << "Max AF = " << max_AF << std::endl;
	std::cout << "Average AF = " << avg_AF / (N * num_of_Objects - not_found) << std::endl;
	std::cout << "Average Time Fraction (LSH/True) = " << avg_TF/num_of_Objects << std::endl;
	std::cout << "Not found = " << not_found << std::endl << std::endl;

	return true;
}

std::vector <std::pair <double, const Abstract_Object*> > lsh_struct::appr_nearest_neighbors(const Dataset & dataset, const Abstract_Object & query_object, const int & N, double (*metric)(const Abstract_Object &, const Abstract_Object &))
{
	// run approximate kNN

	// initialize an empty set of strings-object names, that will serve as a visited set, so that we check each object at most once
	std::set <std::string> visited_set;

	// initialize a max heap priority queue, that will store the distance of Object from query object and a pointer to the Object itself
	std::priority_queue <std::pair <double, const Abstract_Object*> > max_heap;

	for (int i = 0; i < L; ++i)
	{
		uint32_t query_object_id = 0;

		// get bucket index in i-th hash table, for given query object , and query object locality ID
		int bucket = (this->lsh_hash_struct[i])->get_bucket_index(query_object, query_object_id);

		// iterate the bucket of hash table that the bucket index indicates
		for (auto const& object_info : (this->lsh_hash_struct[i])->get_ith_bucket(bucket))
		{
			const Abstract_Object * object = std::get<0>(object_info);			// get object
			uint32_t object_id = std::get<1>(object_info);						// get object's locality ID

			// if given object has not been visited yet and has same locality ID with query object
			if (visited_set.count(object->get_name()) == 0 && object_id == query_object_id)
			{
				// add object's name (unique identifier) into visited set
				visited_set.insert(object->get_name());
				// calculate object's distance from query object
				double dist = (*metric)(query_object, *object);

				if ((int) max_heap.size() < N)	// if we haven't found N neighbors yet
					max_heap.push(std::make_pair(dist, object));	// simply push the new object-neighbor found
				else
				{
					// N neighbors have already been found, so compare with largest distance
					double largest_dist = std::get<0>(max_heap.top());

					if (dist < largest_dist)	// if new object is closer
					{
						max_heap.pop();									// pop the Object with the largest distance
						max_heap.push(std::make_pair(dist, object));	// and insert the new object
					}
				}
			}
		}
	}

	// initialize a vector with how many approximate nearest neighbors were found
	std::vector <std::pair <double, const Abstract_Object*> > nearest(max_heap.size());

	for (int i = nearest.size() - 1; i >= 0; --i)	// for each nearest neighbor found
	{
		nearest[i] = max_heap.top();	// save nearest neighbor
		max_heap.pop();
	}
	
	return nearest;
}

std::vector <std::pair <double, const Abstract_Object*> > lsh_struct::exact_nearest_neighbors(const Dataset & dataset, const Abstract_Object & query_object, const int & N, double (*metric)(const Abstract_Object &, const Abstract_Object &))
{
	// run brute force exact kNN
	int num_of_Objects = dataset.get_num_of_Objects();
	Curve* fred_curve = nullptr;
	if (algorithm == "Frechet" && metric_func == "continuous"){
		fred_curve = transform_to_Fred_Curve(dynamic_cast<const Object&> (query_object));
	}
	// initialize a max heap priority queue, that will store the distance of Object from query object and a pointer to the Object itself
	std::priority_queue <std::pair <double, const Abstract_Object*> > max_heap;

	// check each of the dataset objects by brute force
	for (int i = 0; i < num_of_Objects; ++i)
	{
		double dist = 0;
		if (algorithm == "Frechet" && metric_func == "continuous"){
			if (fred_curve->complexity() >= 2 && curve_vect[i]->complexity() >= 2){
				dist = (Frechet::Continuous::distance(*(curve_vect[i]), *fred_curve)).value;
			}
			else{
				std::cerr << " A fred curve has complexity 1" << std::endl; 
				dist = (*metric)(query_object, dataset.get_ith_object(i));
			}
		}
		else {
			// find its distance from query object
			dist = (*metric)(query_object, dataset.get_ith_object(i));
		}
		if ((int) max_heap.size() < N)	// if we haven't found N neighbors yet
			max_heap.push(std::make_pair(dist, & dataset.get_ith_object(i)));
		else
		{
			// N neighbors have already been found, so compare with largest distance
			double largest_dist = std::get<0>(max_heap.top());

			if (dist < largest_dist)	// if new object is closer
			{
				max_heap.pop();									// pop the Object with the largest distance
				max_heap.push(std::make_pair(dist, & dataset.get_ith_object(i)));	// and insert the new object
			}
		}
	}

	// initialize a vector with how many exact nearest neighbors were found
	std::vector <std::pair <double, const Abstract_Object*> > nearest(max_heap.size());

	for (int i = nearest.size() - 1; i >= 0; --i)	// for each nearest neighbor found
	{
		nearest[i] = max_heap.top();	// save nearest neighbor
		max_heap.pop();
	}
	if (algorithm == "Frechet" && metric_func == "continuous"){
		delete fred_curve;
	}
	return nearest;
}


std::list <std::pair <double, const Abstract_Object*> > lsh_struct::range_search(const Abstract_Object & query_object, const int & R, double (*metric)(const Abstract_Object &, const Abstract_Object &), const int R2)
{

	std::set<std::string> visited_set;

	//Save all object-points who are within radius R of the query_object
	std::list<std::pair <double, const Abstract_Object*> > R_list;

	for (int i = 0; i < L; ++i)
	{
		uint32_t query_object_id = 0;

		// get bucket index in i-th hash table, for given query object
		int bucket = (this->lsh_hash_struct[i])->get_bucket_index(query_object, query_object_id);

		// iterate the bucket of hash table that the bucket index indicates
		for (auto const& object_info : (this->lsh_hash_struct[i])->get_ith_bucket(bucket))
		{
			const Abstract_Object * object = std::get<0>(object_info);			// get object

			// if current object has not been visited yet
			if (visited_set.count(object->get_name()) == 0)
			{
				// add object's name (unique identifier) into visited set
				visited_set.insert(object->get_name());

				double dist = (*metric)(query_object, *object);
				//if it is also within range
				if (R2 <= dist && dist < R )
				{
					R_list.push_back(std::make_pair(dist, object));
				}
			}
		}
	}

	return R_list;
}



std::list <std::pair <double, const Abstract_Object*> > lsh_struct::range_search_with_set(const Abstract_Object & query_object, const int & R, std::set<std::string> & already_visited , double (*metric)(const Abstract_Object &, const Abstract_Object &))
{

	std::set<std::string> visited_set;

	//Save all object-points who are within radius R of the query_object
	std::list<std::pair <double, const Abstract_Object*> > R_list;

	for (int i = 0; i < L; ++i)
	{
		uint32_t query_object_id = 0;

		// get bucket index in i-th hash table, for given query object
		int bucket = (this->lsh_hash_struct[i])->get_bucket_index(query_object, query_object_id);

		// iterate the bucket of hash table that the bucket index indicates
		for (auto const& object_info : (this->lsh_hash_struct[i])->get_ith_bucket(bucket))
		{
			const Abstract_Object * object = std::get<0>(object_info);			// get object
			
			if (already_visited.count(object->get_name()) != 0){
				continue;
			}
			else{
				already_visited.insert(object->get_name());
			}

			// if current object has not been visited yet
			if (visited_set.count(object->get_name()) == 0)
			{
				// add object's name (unique identifier) into visited set
				visited_set.insert(object->get_name());

				double dist = (*metric)(query_object, *object);
				//if it is also within range
				if (dist < R )
				{
					R_list.push_back(std::make_pair(dist, object));
				}
			}
		}
	}

	return R_list;
}