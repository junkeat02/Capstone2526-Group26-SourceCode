import os

# Get the absolute path of the current script
current_dir = os.path.dirname(os.path.abspath(__file__))

import joblib
MODEL_PATH = os.path.join(current_dir, "lnn.pkl")
SCALER_PATH = os.path.join(current_dir, "scaler.pkl")

'''
This is a linear regression model with sklearn.
The format for the input data of the model will be all in integer, and yes is 1 no is 0.
Sequence: AGE, WEIGHT(kg), HEARTRATE, HEIGHT(m), DIABETIC(bool), HR_IR, GENDER_F(bool), GENDER_M(bool)
'''

def input_data_encode(input_data:dict):
    interface_data = ["age", "weight", "height", "diabetes", "is_female", "is_male"]
    sensor_data = ["heart_rate", "ir"]
    formatted_data = []
    for n, name in enumerate(interface_data):
        formatted_data.append(input_data[name])
        if name == "weight":
            formatted_data.append(input_data[sensor_data[0]])
        if name == "diabetes":
            formatted_data.append(input_data[sensor_data[1]])
    return formatted_data

model = joblib.load(MODEL_PATH)
scaler = joblib.load(SCALER_PATH)