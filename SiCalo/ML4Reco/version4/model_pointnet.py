import torch
import torch.nn as nn

class PointNetRegressor(nn.Module):
    def __init__(self, input_dim=2, emb_dim=64):
        super().__init__()
        self.point_mlp = nn.Sequential(
            nn.Linear(input_dim, 64),
            nn.ReLU(),
            nn.Linear(64, emb_dim),
            nn.ReLU()
        )
        self.regressor = nn.Sequential(
            nn.Linear(emb_dim, 64),
            nn.ReLU(),
            nn.Linear(64, 1)
        )

    def forward(self, x):
        # x shape: [B, N, 2]
        feat = self.point_mlp(x)            # [B, N, emb_dim]
        global_feat = torch.max(feat, dim=1)[0]  # [B, emb_dim]
        out = self.regressor(global_feat)   # [B, 1]
        return out.squeeze(1)               # [B]
