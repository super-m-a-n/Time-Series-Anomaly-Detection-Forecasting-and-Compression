# Deep Learning Models for Time-Series Anomaly Detection, Forecasting and Compression
The project explores 3 aspects:
* ## Part 1 : Stock market time series forecasting using a recurrent LSTM neural network
  The model is trained using either one seperate time-series or a different subset of the entire set of the input time-series. Dropout layers are used between the lstm layers to avoid overfitting. Stochastic gradient descent was used for optimization. Graphical visualization of the original vs the predicted time-series.
* ## Part 2 : Time Series Anomaly Detection using an LSTM autoencoder
  The recurrent LSTM autoencoder is implemented using encoding/decoding LSTM layers, with intermediate dropout layers to avoid overfitting. Comparison of original and autoencoded time-series, using different thresholds on the mean absolute error as metrics for the anomaly detection. Graphical representation of the anomalies detected, along with the original/autoencoded time-series.

* ## Part 3 : Time Series Compression/Autoencoding using a convolutional neural network
  Dimensinality reduction and information compression on the original time-series using a convolutional neural network, consisting of compression/decompression layers. Experimentation for optimal architecture of the network and the selection of the latent compression vector, which is used for the reduction of the time-series complexity.
## Project's Structure
* **/src/**: source files
* **/src/forecast.py, /src/forecast_utils.py**: Implementation of recurrent LSTM neural network for time-series forecasting
* **/src/detect.py, /src/detect_utils.py**: Implementation of LSTM autoencoder for time-series anomaly detection
* **/src/reduce.py, /src/reduce_utils.py**: Implementation of convolutional neural network for time-series compression
* **/src/content/models**: Pre-Trained models for each of the 3 parts
* **/src/content/data**: Input Data for the training
* **/reports/** : Project's report with experiments / hyperparameter-tuning / results / comments
* **/ipynb_files/**: The above functionality in additional jupyter notebook files for all 3 parts
