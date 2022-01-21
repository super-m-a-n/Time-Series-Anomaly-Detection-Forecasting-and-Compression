""" file : reduce_utils.py """
""" helper functions for reduce.py main"""

from keras.layers import Input, Dense, Conv1D, MaxPooling1D, UpSampling1D, BatchNormalization, RepeatVector
from keras.models import Model
from keras.models import model_from_json
from keras import regularizers
import requests as req
import json
import pandas as pd
import math as math
import numpy as np
from sklearn.preprocessing import MinMaxScaler
from tqdm import tqdm



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
from keras.models import load_model
from keras.layers import Dense
from keras.layers import Dropout
from keras.layers import *
from keras.callbacks import EarlyStopping

""" sklearn imports """
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error
from sklearn.metrics import mean_absolute_error
from sklearn.model_selection import train_test_split

# size of each timeseries is 3650 points and there are 360 timeseries



        

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

def plot_series(time_points, original_series, encoded_time_points, encoded_series, window, series_name):
    """ plot the original time series vs predicted time series """
    fig, axs = plt.subplots(2)
    axs[0].plot(time_points, original_series, color = 'red', label = 'Original Time Series Values')
    axs[1].plot(encoded_time_points, encoded_series, color = 'blue', label = 'Encoded Time Series Values')
    fig.suptitle("Time Series Prediction (name of series --> %s)" % (series_name))
    #fig.legend()
    fig.show()

def plot_history(history):
    plt.figure(figsize=(15, 5))
    ax = plt.subplot(1, 2, 1)
    plt.plot(history.history["loss"])
    plt.title("Train loss")
    ax = plt.subplot(1, 2, 2)
    plt.plot(history.history["val_loss"])
    plt.title("Test loss")




def split_dataset(input_set, num_of_series, num_of_steps, w, series_to_use):
    """ split each time series in the input set in chanks of size w and use those to train the convolutional autoencoder 
        input_set : 2D array -> columns are different time series, rows are time series values across time
        num_of_series : number of series in input_set
        num_of_steps  : number of steps = size of x_set and y_set
        w : window of sampling 
        is_prediction : Boolean value to see if the split will be used to encode a timeseries. 
                        If the value is False, then the data will be used for the training of the model"""

    # x_set will contain windows of w consecutive values for all time series of input_set
    # It splits the input set to train sets of size w
    x_set = []

    if series_to_use is None:
        i_index_list = range(0,num_of_steps,w) # a prediction is desired so split the timeseries and encode it, don't use it to train the model
    else:
        i_index_list = range(0, num_of_steps-w+1)

    for series_index in range(num_of_series):
        for i in i_index_list:
            # append next window of w consecutive values of time series in x_set
            x_set.append(input_set[i:i+w, series_index])
    # convert to np arrays
    x_set = np.array(x_set)
    #print(x_set.shape)
    # reshape x_set to a tensor of size (#time_series * num_of_steps/window, window, 1)
    x_set = np.reshape(x_set, (x_set.shape[0], x_set.shape[1], 1))

    return x_set


def execute(series_values, series_names, query_series_values, query_series_names, load_trained_model, series_to_use, window_length, latent_dimension,epochs,max_encoded_lines_to_print, output_dataset_path, output_query_path):
    """ function where main work gets done
        series_values : 2D array -> columns are different time series, rows are time series values across time
        series_names  : 1D array -> rows have the names of the time series
        w : window of sampling
        load_trained_model : if True, a pre trained model will be loaded, otherwise a new model will be trained
        series_to_use   : if not None, training/loading is done per given series, otherwise per entire set of series """

    # train test split 80-20 rule (the first 80% of time series values will be used for the training, rest 20% for testing)
    train_set_size = math.floor((series_values.shape[0])*0.8)

    # save train set after split
    train_set = series_values[:train_set_size]

    # save test set after split
    test_set = series_values[train_set_size:]

    # scale train set manually using min max scaling
    scaled_train_set = (train_set - train_set.min()) / (train_set.max() - train_set.min())

    num_of_series = series_values.shape[1]

    num_of_query_series = query_series_values.shape[1]

    # get x_train
    x_train = split_dataset(scaled_train_set, num_of_series, train_set.shape[0], window_length, series_to_use)

    if load_trained_model is False or (series_to_use is not None and (series_to_use > 10)):
        """
        # configure new convolutional encoder and autoencoder models for series 
        input_window = Input(shape=(window_length,1))
        x = Conv1D(32, 3, activation="relu", padding="same")(input_window) # window_length dim
        #x = BatchNormalization()(x)
        x = MaxPooling1D(2, padding="same")(x) # window_length/2 dim
        x = Conv1D(16,3,activation="relu",padding="same")(x)
        x = Conv1D(1,3,activation='relu',padding='same')(x)
        #x = BatchNormalization()(x)
        x = Flatten()(x)
        encoded = Dense(latent_dimension, activation='relu',name='encoder')(x) # latent_dimension dim

        # Finished autoencoder
        y = Dense((window_length//2), activation='relu')(encoded)
        y = Reshape((window_length//2,1))(y)
        #x = Conv1D(1, 3, activation="relu", padding="same")(x) # 5 dims
        #x = BatchNormalization()(x)
        #encoded = MaxPooling1D(2, padding="same")(x) # 3 dims

        # latent_dimension dimensions in the encoded layer

        y = Conv1D(1, 3, activation="relu", padding="same")(y) # 3 dims
        #y = BatchNormalization()(y)
        y = Conv1D(16, 1, activation='relu')(y) # 5 dims
        y = UpSampling1D(2)(y) # 6 dims
        #y = Conv1D(16,1,activation='relu')(y)
        #y = BatchNormalization()(y)
        #x = UpSampling1D(2)(x) # 10 dims
        decoded = Conv1D(1, 3, activation='sigmoid', padding='same')(y) # 10 dims
        model_autoencoder = Model(input_window, decoded)
        model_autoencoder.summary()
        model_autoencoder.compile(metrics=[tf.metrics.MeanAbsoluteError()],optimizer=tf.optimizers.Adam(), loss='mean_squared_error')
        """
        """
        input_window = Input(shape=(window_length,1))
        x = Conv1D(16, 3, activation="relu", padding="same")(input_window) # 10 dims
        #x = BatchNormalization()(x)
        x = MaxPooling1D(2, padding="same")(x) # 5 dims
        x = Conv1D(1, 3, activation="relu", padding="same")(x) # 5 dims
        #x = BatchNormalization()(x)
        encoded = MaxPooling1D(2, padding="same")(x) # 3 dims

        # 3 dimensions in the encoded layer

        x = Conv1D(1, 3, activation="relu", padding="same")(encoded) # 3 dims
        #x = BatchNormalization()(x)
        x = UpSampling1D(2)(x) # 6 dims
        x = Conv1D(16, 2, activation='relu')(x) # 5 dims
        #x = BatchNormalization()(x)
        x = UpSampling1D(2)(x) # 10 dims
        decoded = Conv1D(1, 3, activation='sigmoid', padding='same')(x) # 10 dims
        model_autoencoder = Model(input_window, decoded)
        model_autoencoder.summary()
        model_autoencoder.compile(optimizer='adam', loss='binary_crossentropy')
        """
        input_window = Input(shape=(window_length,1))
        x = Conv1D(8, 3, activation="relu", padding="same")(input_window) # window_length dim
        #x = BatchNormalization()(x)
        x = MaxPooling1D(2, padding="same")(x) # window_length/2 dim
        x = Conv1D(2,3,activation='relu',padding='same')(x)
        #x = BatchNormalization()(x)
        x = Flatten()(x)
        encoded = Dense(latent_dimension, activation='relu',name='encoder')(x) # latent_dimension dim

        # Finished autoencoder
        y = Dense((window_length//2), activation='relu')(encoded)
        y = Reshape((window_length//2,1))(y)
        #x = Conv1D(1, 3, activation="relu", padding="same")(x) # 5 dims
        #x = BatchNormalization()(x)
        #encoded = MaxPooling1D(2, padding="same")(x) # 3 dims

        # latent_dimension dimensions in the encoded layer

        #y = Conv1D(8, 3, activation="relu", padding="same")(y) # 3 dims
        #y = BatchNormalization()(y)
        y = UpSampling1D(2)(y) # 6 dims
        #y = Conv1D(16,1,activation='relu')(y)
        #y = BatchNormalization()(y)
        #x = UpSampling1D(2)(x) # 10 dims
        decoded = Conv1D(1, 3, activation='sigmoid', padding='same')(y) # 10 dims
        model_autoencoder = Model(input_window, decoded)
        model_autoencoder.summary()
        model_autoencoder.compile(metrics=['accuracy'],optimizer=tf.optimizers.Adam(), loss='binary_crossentropy')


    else:
        if series_to_use is not None:
            # load pre trained convolutional encoder and autoencoder model for series
            model_encoder = load_model('./content/models/reduce/model_' + str(series_to_use) + '.h5')
        else:
            # load pre trained convolutional encoder and autoencoder model for entire set of series
            model_encoder = load_model('./content/models/reduce/big_model.h5')


    # normalize with minmax normalization based on train set
    scaled_test_set = (test_set - test_set.min()) / (test_set.max() - test_set.min())

    # get x_test
    x_test = split_dataset(scaled_test_set, num_of_series, test_set.shape[0], window_length, series_to_use)

    # if load_trained_model option was not given, train new model
    if load_trained_model is False or (series_to_use is not None and (series_to_use > 10)):
        # train model by fitting it to the training set
        history = model_autoencoder.fit(x_train, x_train, epochs =epochs, batch_size = 1024, validation_data=(x_test, x_test), verbose=1)
        model_encoder = Model(input_window, encoded)
        
        # plot the train loss vs test loss learning curve for given training history
        plot_loss(history)

        if series_to_use is not None and (series_to_use <= 10):
            # save pre trained convolutional encoder and autoencoder model for series
            model_encoder.save('./content/models/reduce/model_' + str(series_to_use) + '.h5')
        elif series_to_use is None:
            # save pre trained convolutional encoder and autoencoder model for entire set of series
            model_encoder.save('./content/models/reduce/big_model.h5')

    # time axis of original timeseries
    time_points = [index + 1 for index in range(query_series_values.shape[0])]

    # time axis of encoded timeseries

    encoded_time_points = [((index*window_length)/latent_dimension)+1 for index in range(0,(query_series_values.shape[0]* latent_dimension) // window_length)]

    if series_to_use is None:
        output_dataset_original = open(output_dataset_path + '_original', 'w', encoding='utf-8')
        output_dataset_encoded = open(output_dataset_path + '_encoded', 'w', encoding='utf-8')
        output_query_original = open(output_query_path + '_original', 'w', encoding='utf-8')
        output_query_encoded = open(output_query_path + '_encoded', 'w', encoding='utf-8')

    # for each of the selected time series make the original vs predicted series plot
    for series_index in range(num_of_query_series):
        # get name for current time series
        series_name = query_series_names[series_index, 0]

        # get original series (test set) for current time series
        original_series = query_series_values[:, series_index:series_index+1]

        # get x_test for current time series
    
        #x_test_series = x_test[test_set.shape[0] * series_index : test_set.shape[0] * (series_index+1), :, :]
        
        series_max = original_series.max()

        series_min = original_series.min()
        
        scaled_series = (original_series - series_min) / (series_max - series_min)

        # reduce the size of the time series
        encoded_series = model_encoder.predict(split_dataset(scaled_series, 1, series_values.shape[0], window_length, None))
        

        # get unscaled predictions of time series values (manual inverse transform)
        encoded_series = np.array(encoded_series).reshape(-1,1) * (series_max - series_min) + series_min
        
        
        if series_index < max_encoded_lines_to_print:
          # plot the original time series vs predicted time series
          plot_series(time_points, original_series, encoded_time_points, encoded_series, window_length, series_name)

        if series_to_use is None:
            #  the series will be used for the input_datasets
            # else they will be used for the query_datasets
            if series_index <= int(num_of_query_series*0.95):
                output_dataset_original.write(f'{series_name}\t')
                for i in range(original_series.shape[0]):
                    output_dataset_original.write(f'{original_series[i,0]}\t')
                output_dataset_original.write('\n')

                output_dataset_encoded.write(f'{series_name}\t')
                for i in range(encoded_series.shape[0]):
                    output_dataset_encoded.write(f'{encoded_series[i,0]}\t')
                output_dataset_encoded.write('\n')
            else:
                output_query_original.write(f'{series_name}\t')
                for i in range(original_series.shape[0]):
                    output_query_original.write(f'{original_series[i,0]}\t')
                output_query_original.write('\n')

                output_query_encoded.write(f'{series_name}\t')
                for i in range(encoded_series.shape[0]):
                    output_query_encoded.write(f'{encoded_series[i,0]}\t')
                output_query_encoded.write('\n')
        """
        def write_data_to_out_file(out_filename, df, predictions):
        with open(out_filename, 'w', encoding = 'utf-8') as out:
            for label in df.columns:
                out.write(f'{label}\t')
                for price in predictions[label]:
                    out.write(f'{price}' + '\t')
                out.write('\n')
        """