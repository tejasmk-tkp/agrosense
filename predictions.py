import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.tree import DecisionTreeRegressor

''' NPK Prediction '''
features = pd.read_csv('data.csv')

N_label = np.array(features['N'])
P_label = np.array(features['P'])
K_label = np.array(features['K'])
features = features.drop(['N', 'P', 'K', 'ph', 'rainfall', 'label'], axis=1)
features = np.array(features)

train_features_N, test_features_N, train_labels_N, test_labels_N = train_test_split(features, N_label, test_size=0.1, random_state=0)
train_features_P, test_features_P, train_labels_P, test_labels_P = train_test_split(features, P_label, test_size=0.1, random_state=0)
train_features_K, test_features_K, train_labels_K, test_labels_K = train_test_split(features, K_label, test_size=0.1, random_state=0)

N_model = RandomForestRegressor(max_depth=4, n_estimators=100, random_state=42).fit(train_features_N, train_labels_N.reshape(-1, 1))
P_model = RandomForestRegressor(max_depth=4, n_estimators=100, random_state=42).fit(train_features_P, train_labels_P.reshape(-1, 1))
K_model = RandomForestRegressor(max_depth=4, n_estimators=100, random_state=42).fit(train_features_K, train_labels_K.reshape(-1, 1))

def NPK_prediction(temperature, humidity):
    N = N_model.predict([[temperature, humidity]])
    P = P_model.predict([[temperature, humidity]])
    K = K_model.predict([[temperature, humidity]])
    #print(N, P, K)
    return N, P, K
''' Weather Prediction''' 
features_w = pd.read_csv('chennai 2023-01-01 to 2024-02-27.csv')
cc_label = np.array(features_w['cloudcover'])
#print(np.shape(cc_label))
solar_label = np.array(features_w['solarradiation'])
#print(np.shape(solar_label))

features_w = features_w.drop(['name', 'datetime', 'preciptype', 'snow', 'snowdepth', 'sunrise', 'sunset', 'conditions', 'description', 'icon', 'stations', 'cloudcover', 'solarenergy', 'tempmax', 'tempmin', 'feelslikemax', 'feelslikemin', 'feelslike', 'dew', 'precip', 'precipprob', 'precipcover', 'preciptype', 'windgust', 'windspeed', 'winddir', 'visibility', 'solarradiation', 'uvindex', 'severerisk', 'moonphase', 'sealevelpressure'], axis = 1)
features_w = np.array(features_w)
#print(np.shape(features_w))

train_features_cc, test_features_cc, train_labels_cc, test_labels_cc = train_test_split(features_w, cc_label, test_size=0.1, random_state=0)
train_features_solar, test_features_solar, train_labels_solar, test_labels_solar = train_test_split(features_w, solar_label, test_size=0.1, random_state=0)

cc_model = DecisionTreeRegressor(max_depth=4).fit(train_features_cc, train_labels_cc)
solar_model = DecisionTreeRegressor(max_depth=4).fit(train_features_solar, train_labels_solar)

def weather_prediction(temperature, humidity):
    cloudcover = cc_model.predict([[temperature, humidity]])
    solarradiation = solar_model.predict([[temperature, humidity]])
    return cloudcover, solarradiation
