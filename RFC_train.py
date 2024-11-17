import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
import micromlgen  # To convert the trained model for microcontroller deployment

# Generate or load your IMU dataset
# Assume columns: ['ax', 'ay', 'az', 'gx', 'gy', 'gz', 'label']
# ax, ay, az = Accelerometer values
# gx, gy, gz = Gyroscope values
# label = Stand detection (1 for standing, 0 for not standing)
data = pd.read_csv('imu_data.csv')  # Replace with your dataset path

# find min and max values of each column
min_max = data.describe().loc[['min', 'max']]
print(min_max)

# Preprocessing: Normalize accelerometer and gyroscope data between -1 and 1
scaler = MinMaxScaler(feature_range=(-1, 1))
features = ['ax', 'ay', 'az', 'gx', 'gy', 'gz']
data[features] = scaler.fit_transform(data[features])

# Split dataset into training and testing sets
X = data[features].values
y = data['stand'].values
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.1, random_state=42)

# Train Random Forest Classifier
rfc = RandomForestClassifier(n_estimators=30, random_state=42)
rfc.fit(X_train, y_train)

# Evaluate the model
y_pred = rfc.predict(X_test)
print(classification_report(y_test, y_pred))

# Export the trained model to Arduino-compatible code
model_code = micromlgen.port(rfc)
with open("rfc_model.h", "w") as f:
    f.write(model_code)
