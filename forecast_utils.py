""" file : forecast_utils.py """
""" helper functions for forecast.py main"""

""" general imports """
import sys
import string

""" imports for plots, dataframes, numpy """
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import math

""" keras neural network imports """
import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers import Dropout
from keras.layers import *
from keras.callbacks import EarlyStopping

""" sklearn imports """
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error
from sklearn.metrics import mean_absolute_error
from sklearn.model_selection import train_test_split


def config_model(input_size):
    """ configure and return RNN-LSTM model to be trained """
    model = Sequential()
    # Adding the first input LSTM layer
    model.add(LSTM(units = 50, return_sequences = True, input_shape = (input_size, 1)))
    # Dropout layer to avoid overfitting
    model.add(Dropout(0.5))
    # Adding a second LSTM layer
    model.add(LSTM(units = 50))
    # Dropout layer to avoid overfitting
    model.add(Dropout(0.5))
    # Adding a third LSTM layer
    #model.add(LSTM(units = 50))
    # Dropout layer to avoid overfitting
    #model.add(Dropout(0.5))
    # Adding a fourth LSTM layer
    #model.add(LSTM(units = 50))
    # Dropout layer to avoid overfitting
    #model.add(Dropout(0.5))
    # Adding the output layer
    model.add(Dense(units = 1))
    # Compiling the RNN by determinng optimizer and loss function
    model.compile(optimizer = 'sgd', loss = 'mean_squared_error')
    return model


def plot_loss(history):
    """ plot the train loss vs test loss learning curve for given training history """
    plt.figure(figsize=(16, 6), dpi=120)
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model train vs test loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper right')
    plt.show()


def plot_series(time_points, original_series, predicted_series, window, series_name):
    """ plot the original time series vs predicted time series """
    plt.figure(figsize=(16, 6), dpi=120)
    plt.plot(time_points, original_series, color = 'red', label = 'Original Time Series Values')
    plt.plot(time_points, predicted_series, color = 'blue', label = 'Predicted Time Series Values')
    plt.xticks(np.arange(time_points[0], time_points[-1], window))
    plt.title("Time Series Prediction (name of series --> %s)" % (series_name))
    plt.xlabel('Time')
    plt.ylabel('Time Series Value')
    plt.legend()
    plt.show()

def create_dataset(input_set, num_of_series, num_of_steps, w):
    """ creates the x_set, y_set (x_train / y_train or x_test / y_test ) out of the input_set
        input_set : 2D array -> columns are different time series, rows are time series values across time
        num_of_series : number of series in input_set
        num_of_steps  : number of steps = size of x_set and y_set
        w : window of sampling """

    # x_set will contain windows of w consecutive values for all time series of input_set
    x_set = []
    # y_set will contain the next value of corresponding time series for each of the windows of x_set
    y_set = []

    for series_index in range(num_of_series):
        for i in range(w, num_of_steps + w):
            # append next window of w consecutive values of time series in x_set
            x_set.append(input_set[i-w:i, series_index])
            # append next value of time series outside current window in y_set
            y_set.append(input_set[i, series_index])

    # convert to np arrays
    x_set, y_set = np.array(x_set), np.array(y_set)
    # reshape x_set to a tensor of size (#time_series * num_of_steps, window, 1)
    x_set = np.reshape(x_set, (x_set.shape[0], x_set.shape[1], 1))

    return x_set, y_set


def execute(series_values, series_names, w):
    """ function where main work gets done
        series_values : 2D array -> columns are different time series, rows are time series values across time
        series_names  : 1D array -> rows have the names of the time series
        w : window of sampling """

    # train test split 80-20 rule (the first 80% of time series values will be used for the training, rest 20% for testing)
    train_set_size = math.floor((series_values.shape[0])*0.8)

    # save train set after split
    train_set = series_values[:train_set_size]

    # save test set after split
    test_set = series_values[train_set_size:]

    # scale train set manually using min max scaling
    scaled_train_set = (train_set - train_set.min()) / (train_set.max() - train_set.min())

    num_of_series = series_values.shape[1]

    # get x_train, y_train
    x_train, y_train = create_dataset(scaled_train_set, num_of_series, train_set.shape[0] - w, w)

    # configure RNN-LSTM model
    model = config_model(x_train.shape[1])

    # getting all the values of all selected time series starting from the last window before the test set
    inputs = series_values[series_values.shape[0] - test_set.shape[0] - w:]
    # normalize with minmax normalization based on train set
    inputs = (inputs - train_set.min()) / (train_set.max() - train_set.min())

    # get x_test, y_test
    x_test, y_test = create_dataset(inputs, num_of_series, test_set.shape[0], w)

    # train model by fitting it to the training set
    history = model.fit(x_train, y_train, epochs = 80, batch_size = 64, validation_data=(x_test, y_test), verbose=0)
    # plot the train loss vs test loss learning curve for given training history
    plot_loss(history)

    # time axis
    time_points = [index + 1 for index in range(train_set.shape[0], series_values.shape[0])]

    # for each of the selected time series make the original vs predicted series plot
    for series_index in range(num_of_series):
        # get name for current time series
        series_name = series_names[series_index, 0]

        # get original series (test set) for current time series
        original_series = test_set[:, series_index:series_index+1]

        # get x_test for current time series
        x_test_series = x_test[test_set.shape[0] * series_index : test_set.shape[0] * (series_index+1), :, :]

        # get predictions of future time series values (time series values on test set) (for current time series)
        predicted_series = model.predict(x_test_series)

        # get unscaled predictions of time series values (manual inverse transform)
        predicted_series = predicted_series * (train_set.max() - train_set.min()) + train_set.min()

        # plot the original time series vs predicted time series
        plot_series(time_points, original_series, predicted_series, w, series_name)
