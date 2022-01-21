#!/bin/bash

#make all

#echo $'\n'"LSH for original timeseries"$'\n'

#./search -i ./out_input_data_original -q ./out_query_data_original -o output_search_lsh_data_original -algorithm LSH -delta 0.01

#echo $'\n'"LSH for encoded timeseries"$'\n'

#./search -i ./out_input_data_encoded -q ./out_query_data_encoded -o output_search_lsh_data_encoded -algorithm LSH -delta 0.01





#echo $'\n'"Hypercube for original timeseries"$'\n'

#./search -i ./out_input_data_original -q ./out_query_data_original -o output_search_cube_data_original -algorithm Hypercube -delta 0.01

#echo $'\n'"Hypercube for encoded timeseries"$'\n'

#./search -i ./out_input_data_encoded -q ./out_query_data_encoded -o output_search_cube_data_encoded -algorithm Hypercube -delta 0.01




#echo $'\n'"Frechet discrete for original timeseries"$'\n'

#./search -i ./out_input_data_original -q ./out_query_data_original -o output_frechet_discrete_data_original -algorithm Frechet -metric discrete -delta 0.01

#echo $'\n'"Frechet discrete for encoded timeseries"$'\n'

#./search -i ./out_input_data_encoded -q ./out_query_data_encoded -o output_frechet_discrete_data_encoded -algorithm Frechet -metric discrete -delta 0.01





#echo $'\n'"Frechet continuous for original timeseries"$'\n'

#./search -i ./out_input_data_original -q ./out_query_data_original -o output_frechet_discrete_data_original -algorithm Frechet -metric continuous -delta 1

#echo $'\n'"Frechet continuous for encoded timeseries"$'\n'

#./search -i ./out_input_data_encoded -q ./out_query_data_encoded -o output_frechet_discrete_data_encoded -algorithm Frechet -metric continuous -delta 1





echo $'\n'"Cluster for input dataset_original classic method"$'\n'

./cluster -i ./out_input_data_original -c ./content/data/cluster.config -o output_classic_cluster_input_original -update Mean Frechet -assignment Classic -complete 

echo $'\n'"Cluster for input dataset_encoded classic method"$'\n'

./cluster -i ./out_input_data_encoded -c ./content/data/cluster.config -o output_classic_cluster_input_encoded -update Mean Frechet -assignment Classic -complete





echo $'\n'"Cluster for input dataset_original LSH method"$'\n'

./cluster -i ./out_input_data_original -c ./content/content/data/cluster.config -o output_lsh_cluster_input_original -update Mean Frechet -assignment LSH -complete

echo $'\n'"Cluster for input dataset_encoded LSH method"$'\n'

./cluster -i ./out_input_data_encoded -c ./content/data/cluster.config -o output_lsh_cluster_input_encoded -update Mean Frechet -assignment LSH -complete



echo $'\n'"Cluster for input dataset_original Hyperube method"$'\n'

./cluster -i ./out_input_data_original -c ./content/data/cluster.config -o output_hypercube_cluster_input_original -update Mean Frechet -assignment LSH_Frechet -complete

echo $'\n'"Cluster for input dataset_encoded Hyperube method"$'\n'

./cluster -i ./out_input_data_encoded -c ./content/data/cluster.config -o output_hypercube_cluster_input_encoded -update Mean Frechet -assignment LSH_Frechet -complete 
