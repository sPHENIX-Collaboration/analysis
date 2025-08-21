import torch
import numpy as np
import matplotlib.pyplot as plt

def visualize_model(model, device, x_min=0.001, x_max=0.5, num_points=500):
    model.eval()

    x_test = np.linspace(x_min, x_max, num_points)
    # proxy_trans_test = np.log(x_test + 1e-5)  # 做 feature 变换
    proxy_trans_test = 1 / x_test 
    # proxy_trans_test = x_test

    x_tensor = torch.tensor(proxy_trans_test, dtype=torch.float32).unsqueeze(1).to(device)

    with torch.no_grad():
        y_pred = model(x_tensor).cpu().numpy().flatten()

    plt.figure(figsize=(8,6))
    plt.plot(x_test, y_pred, color='red', linewidth=2)
    plt.xlabel("x")
    plt.ylabel("Model Output f(x)")
    plt.title("Visualization of Model Output f(x)")
    plt.grid(True)
    plt.savefig("outputFile/model_func.png")
    # plt.show()
