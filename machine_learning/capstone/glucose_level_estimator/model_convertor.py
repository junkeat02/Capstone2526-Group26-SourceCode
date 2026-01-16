import joblib
import json

pipeline = joblib.load("capstone\glucose_level_estimator\glucose_model.pkl")

scaler = pipeline.named_steps["scaler"]
model = pipeline.named_steps["model"]

export = {
    "mean": scaler.mean_.tolist(),
    "scale": scaler.scale_.tolist(),
    "weights": model.coef_.tolist(),
    "bias": model.intercept_.item()
}

with open("linear_model.json", "w") as f:
    json.dump(export, f, indent=2)
