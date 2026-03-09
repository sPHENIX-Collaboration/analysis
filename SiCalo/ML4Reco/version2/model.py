import torch
import torch.nn as nn

class TrackCaloRegressor(nn.Module):
    def __init__(self, input_dim=7, hidden_dim=256):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, hidden_dim),
            nn.ReLU(),
            # nn.Dropout(0.2),
            nn.Linear(hidden_dim, hidden_dim),
            nn.ReLU(),
            # nn.Dropout(0.2),
            nn.Linear(hidden_dim, hidden_dim),
            nn.ReLU(),
            # nn.Dropout(0.2),
            nn.Linear(hidden_dim, 1)  # 回归输出
        )

    def forward(self, x):
        return self.net(x)
