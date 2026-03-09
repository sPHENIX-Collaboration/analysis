
import numpy as np
import torch
from torch.utils.data import Dataset
import uproot
from numpy.linalg import norm

def point_line_distance(point, line_point1, line_point2):
    """
    计算 point 到 line_point1-line_point2 构成的直线的距离
    """
    point = np.array(point)
    line_point1 = np.array(line_point1)
    line_point2 = np.array(line_point2)
    line_vec = line_point2 - line_point1
    return norm(np.cross(point - line_point1, line_vec)) / norm(line_vec)

class TrackPointNetDataset(Dataset):
    def __init__(self, list_file, tree_name="tree", max_hits=6, pt_min=0.0, pt_max=10.0):
        self.samples = []
        self.targets = []
        self.max_hits = max_hits

        fail_012 = 0
        fail_34 = 0
        fail_56 = 0
        fail_calo = 0
        fail_truth = 0

        branches_to_load = [
            "trk_system", "trk_layer", "trk_X", "trk_Y", "trk_Z",
            "caloClus_system", "caloClus_X", "caloClus_Y", "caloClus_Z", "caloClus_edep", 
            "caloClus_innr_X", "caloClus_innr_Y", "caloClus_innr_Z", "caloClus_innr_edep",
            "PrimaryG4P_Pt"
        ]

        with open(list_file, "r") as f:
            root_files = [line.strip() for line in f if line.strip()]

        for root_file in root_files:
            try:
                file = uproot.open(root_file)
                tree = file[tree_name]
                data = tree.arrays(branches_to_load, library="np")
                n_entries = len(data["trk_system"])

                for i in range(n_entries):
                    trk_layer = data["trk_layer"][i]
                    trk_x = data["trk_X"][i]
                    trk_y = data["trk_Y"][i]
                    trk_z = data["trk_Z"][i]
                    trk_hits = list(zip(trk_layer, trk_x, trk_y, trk_z))
                    # trk_hits = [
                    #     (layer0, x0, y0, z0),
                    #     (layer1, x1, y1, z1),
                    #     ...
                    # ]

                    clu_34 = [p for p in trk_hits if p[0] in (3, 4)]
                    clu_56 = [p for p in trk_hits if p[0] in (5, 6)]
                    if len(clu_34) != 1 or len(clu_56) != 1:
                        fail_34 += 1
                        continue
                    # 选出 layer 为 3 或 4 的 hit，存在 clu_34 列表中。
                    # clu_34 = [(3, 12.5, -7.3, 85.0)]  ----- (layer=3, x=12.5, y=-7.3, z=85.0)

                    p34 = np.array(clu_34[0][1:3])
                    p56 = np.array(clu_56[0][1:3])
                    # 取出 clu_34 列表的第一个元素，即 (3, 12.5, -7.3, 85.0)，
                    # [1:3] -- 切片语法，表示取第 1 到第 2 个元素（第 3 个不取）：only x 和 y 坐标

                    track_points = []  # 最终要保留的 hits
                    success = True 
                    for layer_id in [0, 1, 2]:
                        layer_hits = [p for p in trk_hits if p[0] == layer_id]
                        if len(layer_hits) == 0:
                            success = False
                            break
                        dists = [point_line_distance(p[1:3], p34, p56) for p in layer_hits]
                        min_idx = np.argmin(dists)
                        # track_points.append(layer_hits[min_idx][1:3])  # 取 xy

                    if not success:
                        fail_012 += 1
                        continue

                    track_points.append(p34)
                    track_points.append(p56)
                    

                    # # padding if needed
                    # pad_len = max_hits - len(track_points)
                    # if pad_len > 0:
                    #     track_points = np.vstack([track_points, np.zeros((pad_len, 2))])
                    # elif pad_len < 0:
                    #     track_points = track_points[:max_hits]

                    # 选取 EMCal Only 1 cluster, 加入 calo cluster 的 (x, y)
                    calo_system = data["caloClus_system"][i]
                    if len(calo_system) == 0 or not (calo_system[0] == 0 and np.sum(calo_system == 0) == 1):
                        fail_calo += 1
                        continue

                    calo_x = data["caloClus_X"][i][0]
                    calo_y = data["caloClus_Y"][i][0]
                    calo_point = np.array([calo_x, calo_y])
                    track_points.append(calo_point)

                    track_points = np.array(track_points)  # [N_hits, 2] (最多6个)

                    pt_truth = data["PrimaryG4P_Pt"][i]
                    if len(pt_truth) != 1:
                        fail_truth += 1
                        continue

                    pt_value = pt_truth[0]
                    if not (pt_min <= pt_value < pt_max):
                        continue

                    self.samples.append(track_points)
                    self.targets.append(pt_value)

            except Exception as e:
                print(f"Error reading {root_file}: {e}")
                continue

        print(f"✅ Total usable entries: {len(self.samples)}")
        print(f"[Stats] Events failed 012: {fail_012}")
        print(f"[Stats] Events failed 34/56: {fail_34}")
        print(f"[Stats] Events failed calo: {fail_calo}")
        print(f"[Stats] Events failed truth: {fail_truth}")

        self.samples = np.array(self.samples)
        self.targets = np.array(self.targets)

    def __len__(self):
        return len(self.samples)

    def __getitem__(self, idx):
        x = torch.tensor(self.samples[idx], dtype=torch.float32)  # [max_hits, 2]
        y = torch.tensor(self.targets[idx], dtype=torch.float32)  # scalar
        return x, y
