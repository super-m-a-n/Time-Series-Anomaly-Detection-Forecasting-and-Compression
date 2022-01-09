""" file : detect.py """

""" general imports """
import argparse     # for command line argument parsing
import sys
import string

""" imports for dataframes, numpy """
import pandas as pd
import numpy as np
import math

""" import functions from detect_utils.py """
from detect_utils import execute

# main functionality

# parse command line input
parser = argparse.ArgumentParser()
parser.add_argument('-d', help="Specify input dataset file path", type=str, required=True)
parser.add_argument('-n', help="Specify index of time series to forecast", type=int, required=True)
parser.add_argument('-mae', help="Specify mean absolute error threshold", type=float, required=True)
parser.add_argument('-load', help="Specify use of pre-trained models", dest='load', action='store_true', required=False)
parser.add_argument('-train', help="Specify training of models", dest='load', action='store_false', required=False)
parser.set_defaults(load=True)
args = parser.parse_args()

if args.n < 1:
    sys.exit("Invalid time series index. Index should be positive integer\n")

# save arguments
dataset_path = args.d
num_of_series = args.n
threshold = args.mae
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

# set window
w = 30

# train one model over all series of dataset

# convert entire input dataset for all time series to array of series values
series_values = dataset.iloc[:, cols].values

# get transpose to have columns as different time series and rows as time series values at specific time points
series_values = series_values.T

# execute training or load pre trained model, make plots for first num_of_series series
execute(series_values, series_names, w, num_of_series, threshold, load_trained_model)
