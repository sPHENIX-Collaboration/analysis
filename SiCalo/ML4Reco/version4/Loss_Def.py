import torch
import torch.nn.functional as F

def monotonic_loss(y_pred):
    """
    计算 y_pred 单调递减违反程度
    如果 y_{i+1} > y_i，算 penalty
    """
    dy = y_pred[1:] - y_pred[:-1]c
    penalty = F.relu(dy).sum()  # relu(dy) > 0 部分是违反单调递减的
    return penalty
