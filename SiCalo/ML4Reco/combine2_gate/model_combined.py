### model_combined.py
import torch
import torch.nn as nn
import torch.nn.functional as F

class FusionRegressor(nn.Module):
    def __init__(self, input_dim=2, hidden_dim=256):
        super().__init__()
        self.gate = nn.Sequential(
            nn.Linear(input_dim, hidden_dim),
            nn.ReLU(),
            nn.Linear(hidden_dim, hidden_dim),
            nn.ReLU(),
            nn.Linear(hidden_dim, hidden_dim),
            nn.ReLU(),
            nn.Linear(hidden_dim, 2)
        )

    def forward(self, x):
        logits  = self.gate(x)
        weights = F.softmax(logits, dim=1)
        return weights[:,0:1] * x[:,0:1] + weights[:,1:2] * x[:,1:2]
