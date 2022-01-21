""" file : forecast.py """

""" general imports """
import argparse     # for command line argument parsing
import sys
import string
import os

""" imports for dataframes, numpy """
import pandas as pd
import numpy as np
import math

""" import functions from forecast_utils.py """
from reduce_utils import execute

import random
import tensorflow as tf
def reproducibleResults(seed):
    os.environ['PYTHONHASHSEED'] = str(seed)
    random.seed(seed)
    tf.random.set_seed(seed)
    np.random.seed(seed)

reproducibleResults(6969)

# main functionality

# parse command line input
parser = argparse.ArgumentParser()
parser.add_argument('-d', help="Specify input dataset file path", type=str, required=True)
parser.add_argument('-q', help="Specify query dataset file path", type=str, required=True)
parser.add_argument('-od', help="Specify output file path of input dataset", type=str, required=True)
parser.add_argument('-oq', help="Specify output file path of query dataset", type=str, required=True)
parser.add_argument('-load', help="Specify use of pre-trained models", dest='load', action='store_true', required=False)
parser.add_argument('-train', help="Specify training of models", dest='load', action='store_false', required=False)

# Optional arguments
#parser.add_argument('-n', help="Specify index of time series to reduce", action='store_const',const=0,required=False)
#parser.add_argument('-w', help="Specify the size of the window length",  action='store_const',const=10,required=False)
#parser.add_argument('-dim', help="Specify the latent dimension",  action='store_const',const=3,required=False)



parser.set_defaults(load=True)
args = parser.parse_args()
# save arguments
dataset_path = args.d
query_path = args.q
num_of_series = 0 #args.n
output_dataset_path = args.od
output_query_path = args.oq
window_length = 10 #args.w
latent_dimension = 3 #args.dim
epochs = 30
# load pre trained model or train new model
load_trained_model = args.load
# Load and peek the input dataset csv file
dataset = pd.read_csv(dataset_path, sep='\t', lineterminator='\n', header=None)
# keep names of time series in seperate dataframe and convert it to an array
series_names = pd.DataFrame(dataset, columns=[dataset.columns[0]]).values

# specify columns to keep (all but the first one)
cols = [False]
rest_cols= [True for x in range(dataset.shape[1]-1)]
cols.extend(rest_cols)

# Load and peek the input query dataset csv file
query_dataset = pd.read_csv(query_path, sep='\t', lineterminator='\n', header=None)

# keep names of time series in seperate dataframe and convert it to an array
query_series_names = pd.DataFrame(query_dataset, columns=[dataset.columns[0]]).values

# specify columns to keep (all but the first one)
query_cols = [False]
query_rest_cols= [True for x in range(query_dataset.shape[1]-1)]
query_cols.extend(query_rest_cols)



# If args.n is smaller than 1, then the model will take into account all timeseries in the dataset
if num_of_series < 1:
    num = len(cols)
# otherwise it will only process the first `args.n` time_series
else:
    num = num_of_series

# convert input dataset for first n time series to array of series values
series_values = dataset.iloc[:num, cols].values

query_series_values = query_dataset.iloc[:num, query_cols].values

# get transpose to have columns as different time series and rows as time series values at specific time points
series_values = series_values.T

query_series_values = query_series_values.T

# execute training, plots
execute(series_values, series_names, query_series_values, query_series_names, load_trained_model, None, window_length, latent_dimension, epochs, 5, output_dataset_path, output_query_path)

"""
# train one model for/over each of the series
for series_index in range(min(3,num)):
    # keep series name
    series_name = series_names[series_index, 0]

    # convert input dataset for specific time series to array of series values
    series_values = dataset.iloc[[series_index], cols].values

    # reshape to column array.  Rows are time series values at specific time points
    series_values = series_values.reshape(-1,1)

    # execute training, plots
    execute(series_values, series_names[series_index:], series_values, series_names[series_index:], load_trained_model, series_index, window_length, latent_dimension, epochs, 4, None, None)

"""
# train one model over all series

