from __future__ import annotations

import torch
import torch.nn as nn


class MLP(nn.Module):
    def __init__(self, dims, dropout=0.0):
        super().__init__()

        layers = []
        for i in range(len(dims) - 1):
            layers.append(nn.Linear(dims[i], dims[i + 1]))

            if i != len(dims) - 2:
                layers.append(nn.ReLU())
                if dropout > 0:
                    layers.append(nn.Dropout(dropout))

        self.net = nn.Sequential(*layers)

    def forward(self, x):
        return self.net(x)


class ECPMPointNet(nn.Module):
    def __init__(
        self,
        global_dim: int = 6,
        tower_dim: int = 4,
        tower_hidden_dim: int = 64,
        event_hidden_dim: int = 128,
        output_dim: int = 1,
        pooling: str = "sum",
        dropout: float = 0.0,
    ):
        super().__init__()

        assert pooling in ["sum", "mean", "max"]
        self.pooling = pooling

        self.tower_mlp = MLP(
            [
                tower_dim,
                tower_hidden_dim,
                tower_hidden_dim,
                tower_hidden_dim,
            ],
            dropout=dropout,
        )

        self.event_mlp = MLP(
            [
                tower_hidden_dim + global_dim,
                event_hidden_dim,
                event_hidden_dim,
                output_dim,
            ],
            dropout=dropout,
        )

    def forward(self, global_x, towers, tower_mask):
        """
        global_x:
            [B, global_dim]

        towers:
            [B, Ntower_max, tower_dim]

        tower_mask:
            [B, Ntower_max]
            True  = real tower
            False = padding tower
        """

        B, N, F = towers.shape

        tower_emb = self.tower_mlp(towers)  # [B, N, hidden]

        mask = tower_mask.unsqueeze(-1)     # [B, N, 1]
        tower_emb = tower_emb * mask

        if self.pooling == "sum":
            shower_emb = tower_emb.sum(dim=1)

        elif self.pooling == "mean":
            denom = mask.sum(dim=1).clamp_min(1)
            shower_emb = tower_emb.sum(dim=1) / denom

        elif self.pooling == "max":
            tower_emb = tower_emb.masked_fill(~mask, -1.0e9)
            shower_emb = tower_emb.max(dim=1).values

        event_x = torch.cat([global_x, shower_emb], dim=1)

        out = self.event_mlp(event_x)

        return out


def select_target(target, target_mode: str = "dphi"):
    """
    target from data module:
        target[:, 0] = target_dphi
        target[:, 1] = target_dR
    """

    if target_mode == "dphi":
        return target[:, 0:1]

    if target_mode == "dR":
        return target[:, 1:2]

    if target_mode == "both":
        return target

    raise ValueError(f"Unknown target_mode: {target_mode}")


def build_ecpm_model(target_mode: str = "dphi"):
    if target_mode in ["dphi", "dR"]:
        output_dim = 1
    elif target_mode == "both":
        output_dim = 2
    else:
        raise ValueError(f"Unknown target_mode: {target_mode}")

    model = ECPMPointNet(
        global_dim=7,
        tower_dim=4,
        tower_hidden_dim=64,
        event_hidden_dim=128,
        output_dim=output_dim,
        pooling="sum",
        dropout=0.0,
    )

    return model


if __name__ == "__main__":
    batch_size = 8
    max_ntower = 5

    global_x = torch.randn(batch_size, 6)
    towers = torch.randn(batch_size, max_ntower, 4)

    tower_mask = torch.tensor(
        [
            [1, 1, 1, 0, 0],
            [1, 1, 0, 0, 0],
            [1, 1, 1, 1, 1],
            [1, 0, 0, 0, 0],
            [1, 1, 1, 1, 0],
            [1, 1, 0, 0, 0],
            [1, 1, 1, 0, 0],
            [1, 1, 1, 1, 0],
        ],
        dtype=torch.bool,
    )

    model = build_ecpm_model(target_mode="dphi")

    pred = model(global_x, towers, tower_mask)

    print(model)
    print("pred shape:", pred.shape)