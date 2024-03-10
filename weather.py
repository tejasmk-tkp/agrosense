''' 
partly-cloudy-day: 0
rain: 1
clear-day: 2
wind: 3
'''

import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.tree import DecisionTreeRegressor

features_2 = pd.read_csv('icon.csv')
labels_2 = np.array(features_2['icon'])
features_2 = features_2.drop(['name', 'datetime', 'preciptype', 'snow', 'snowdepth', 'sunrise', 'sunset', 'conditions', 'description', 'icon', 'stations', 'cloudcover', 'solarenergy', 'tempmax', 'tempmin', 'feelslikemax', 'feelslikemin', 'feelslike', 'dew', 'precip', 'precipprob', 'precipcover', 'preciptype', 'windgust', 'windspeed', 'winddir', 'visibility', 'solarradiation', 'uvindex', 'severerisk', 'moonphase', 'sealevelpressure'], axis = 1)
feature_list_2 = list(features_2.columns)
features_2 = np.array(features_2)
train_features_2, test_features_2, train_labels_2, test_labels_2 = train_test_split(features_2, labels_2, test_size = 0.1, random_state = 80)
rf_2 = DecisionTreeRegressor(random_state = 80)
rf_2.fit(train_features_2, train_labels_2)

predictions = rf_2.predict(test_features_2)
errors = abs(predictions - test_labels_2)
print('Mean Absolute Error:', round(np.mean(errors), 2), 'degrees.')

pre = rf_2.predict([[79.2, 7.00]])
print(pre)

#print(len(errors), len(test_labels))

mape = 0
for i in range(len(errors)):
    mape += abs(predictions[i]-test_labels_2[i])
    mape /= len(errors)

accuracy = 100 - mape
print('Accuracy:', round(accuracy, 4), '%.')

def weather_predict(temperature, humidity):
    pre = rf_2.predict([[temperature, humidity]])
    return pre

#print(weather_predict(79.2, 7.00))
'''import matplotlib.pyplot as plt
try:
    x = predictions
    y = test_labels_2
    plt.plot(x, label = 'Predicted Values')
    plt.plot(y, label = 'Actual Values')
    plt.xlabel('Days')
    plt.ylabel('Cloud Cover')
    plt.legend()
    plt.show()
except KeyboardInterrupt:
    print('Closed by user!')
    plt.close()'''
