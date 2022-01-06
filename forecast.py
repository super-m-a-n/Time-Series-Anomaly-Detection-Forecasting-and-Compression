""" file : forecast.py """

""" general imports """
import argparse     # for command line argument parsing
import sys
import string

""" imports for dataframes, numpy """
import pandas as pd
import numpy as np
import math

""" import functions from forecast_utils.py """
from forecast_utils import execute

# main functionality

# parse command line input
parser = argparse.ArgumentParser()
parser.add_argument('-d', help="Specify input dataset file path", type=str, required=True)
parser.add_argument('-n', help="Specify index of time series to forecast", type=int, required=True)
args = parser.parse_args()

if args.n < 1:
    sys.exit("Invalid time series index. Index should be positive integer\n")

# save arguments
dataset_path = args.d
num_of_series = args.n

# Load and peek the input dataset csv file
dataset = pd.read_csv(dataset_path, sep='\t', lineterminator='\n', header=None)

# keep names of time series in seperate dataframe and convert it to an array
series_names = pd.DataFrame(dataset, columns=[dataset.columns[0]]).values

# specify columns to keep (all but the first one)
cols = [False]
rest_cols= [True for x in range(dataset.shape[1]-1)]
cols.extend(rest_cols)

# set window
w = 100

# train one model for/over each of the series
for series_index in range(num_of_series):
    # keep series name
    series_name = series_names[series_index, 0]

    # convert input dataset for specific time series to array of series values
    series_values = dataset.iloc[[series_index], cols].values

    # reshape to column array.  Rows are time series values at specific time points
    series_values = series_values.reshape(-1,1)

    # execute training, plots
    execute(series_values, series_names[series_index:], w)


# train one model over all series

# convert input dataset for first n time series to array of series values
series_values = dataset.iloc[:num_of_series, cols].values

# get transpose to have columns as different time series and rows as time series values at specific time points
series_values = series_values.T

# execute training, plots
execute(series_values, series_names, w)
