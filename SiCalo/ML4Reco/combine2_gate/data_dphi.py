import numpy as np
import torch
from torch.utils.data import Dataset
# from sklearn.preprocessing import StandardScaler
import uproot
from collections import Counter
from numpy.linalg import norm
from sklearn.preprocessing import StandardScaler

import ROOT

def angle_between(v1, v2):
    cos_theta = np.dot(v1, v2) / (norm(v1) * norm(v2))
    cos_theta = np.clip(cos_theta, -1.0, 1.0)  # 防止数值超出 arccos 定义域
    return np.arccos(cos_theta)

def point_line_distance(point, line_point1, line_point2):
    """
    计算 point 到 line_point1-line_point2 构成的直线的距离
    """
    point = np.array(point)
    line_point1 = np.array(line_point1)
    line_point2 = np.array(line_point2)
    line_vec = line_point2 - line_point1
    return norm(np.cross(point - line_point1, line_vec)) / norm(line_vec)

class TrackCaloDataset(Dataset):
    def __init__(self, list_file, tree_name="tree", scaler=None, pt_min=0.0, pt_max=10.0):
        """
        自定义 Dataset，传入 list 文件路径即可，自动提取特征。
        如果传入 scaler，则使用给定 scaler 标准化；否则自己创建一个并 fit。
        """
        # 提取数据
        data_X, data_Y = self.extract_features_from_rootlist(list_file, tree_name)

        # 筛选 pt 区间
        data_X = np.array(data_X)
        data_Y = np.array(data_Y)
        mask = (data_Y >= pt_min) & (data_Y < pt_max)
        data_X = data_X[mask]
        data_Y = data_Y[mask]

        # # 标准化
        # if scaler is None:
        #     scaler = StandardScaler()
        #     data_X = scaler.fit_transform(data_X)
        # else:
        #     data_X = scaler.transform(data_X)

        self.X = torch.tensor(data_X, dtype=torch.float32)
        self.Y = torch.tensor(data_Y, dtype=torch.float32).view(-1, 1)
        self.valid_indices = []
        # self.scaler = scaler  # 保存 scaler 以便导出或后续使用

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.Y[idx]

    @staticmethod
    def extract_features_from_rootlist(list_file, tree_name="tree"):
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

        X_data = []
        Y_data = []

        with open(list_file, "r") as f:
            root_files = [line.strip() for line in f if line.strip()]

        # position of EMCal correct
        file_C = ROOT.TFile.Open(
            "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/"
            "ParticleGen/output/calo_positron_dphi_ptbin_woC.root", "READ"
        )
        g1_dphi_C = file_C.Get("grPeakVsX") 

        for root_file in root_files:
            try:
                file = uproot.open(root_file)
                tree = file[tree_name]
                data = tree.arrays(branches_to_load, library="np")

                n_entries = len(data["trk_system"])  # 每个 entry 是一个 event

                for i in range(n_entries):
                    trk_layer = data["trk_layer"][i]
                    trk_x = data["trk_X"][i]
                    trk_y = data["trk_Y"][i]
                    trk_z = data["trk_Z"][i]
                    
                    trk_hits = list(zip(trk_layer, trk_x, trk_y))

                    # 找到3/4层和5/6层的击中
                    clu_34 = [p for p in trk_hits if p[0] in (3, 4)]
                    clu_56 = [p for p in trk_hits if p[0] in (5, 6)]
                    if len(clu_34) != 1 or len(clu_56) != 1:
                        continue  # 要求唯一匹配的34、56
                    
                    p34 = np.array(clu_34[0][1:3])
                    p56 = np.array(clu_56[0][1:3])

                    phi34 = np.arctan2(p34[1], p34[0])  # y, x
                    phi56 = np.arctan2(p56[1], p56[0])


                    # 对于每层0/1/2，找到距离 p34-p56 线最近的 cluster
                    track_point_layers = []
                    success = True
                    for layer_id in [0, 1, 2]:
                        layer_hits = [p for p in trk_hits if p[0] == layer_id]
                        if len(layer_hits) == 0:
                            success = False
                            break
                        dists = [point_line_distance(p[1:], p34, p56) for p in layer_hits]
                        min_idx = np.argmin(dists)
                        track_point_layers.append(layer_hits[min_idx][1:])  # 只取 xyz

                    if not success:
                        continue
                    
                    trk_feat = np.concatenate([
                        # np.array(track_point_layers).flatten(),  # 0/1/2 层的 9 维
                        p34,                                     # INTT 3/4 层的 3 维
                        p56                                      # INTT 5/6 层的 3 维
                    ])

                    # calo geom center cluster
                    calo_system = data["caloClus_system"][i]
                    calo_x = data["caloClus_X"][i]
                    calo_y = data["caloClus_Y"][i]
                    calo_z = data["caloClus_Z"][i]
                    calo_e = data["caloClus_edep"][i]

                    if len(calo_system) == 0:
                        fail_calo += 1
                        continue

                    if not (calo_system[0] == 0 and np.sum(calo_system == 0) == 1):
                        fail_calo += 1
                        continue
                    
                    calo_feat = np.array([calo_x[0], calo_y[0], calo_z[0], calo_e[0]])

                    # calo innr center cluster
                    calo_innr_x = data["caloClus_innr_X"][i]
                    calo_innr_y = data["caloClus_innr_Y"][i]
                    calo_innr_z = data["caloClus_innr_Z"][i]
                    calo_innr_e = data["caloClus_innr_edep"][i]
                    if len(calo_innr_x) != 1:
                        fail_calo += 1
                        continue
                    
                    # calo_innr_feat = np.array([
                    #     calo_innr_x[0], calo_innr_y[0]
                    # ])
                    phi_calo = np.arctan2(calo_innr_y[0], calo_innr_x[0]) 

                    # feat setting
                    pcalo = np.array([calo_x[0], calo_y[0]]) 
                    # pcalo = np.array([calo_innr_x[0], calo_innr_y[0]]) 

                    # theta_correct = g1_dphi_C.Eval(calo_innr_e)
                    # rotation_matrix = np.array([[np.cos(theta_correct), -np.sin(theta_correct)],
                    #                             [np.sin(theta_correct),  np.cos(theta_correct)]])
                    
                    # pcalo = rotation_matrix.dot(pcalo)

                    vec1 = p56 - p34
                    vec2 = pcalo - p56
                    cos_theta = np.dot(vec1, vec2) / (norm(vec1) * norm(vec2))
                    cos_theta = np.clip(cos_theta, -1.0, 1.0)
                    angle = np.arccos(cos_theta)

                    phi1 = np.arctan2(vec1[1], vec1[0])
                    phi2 = np.arctan2(vec2[1], vec2[0])
                    dphi = phi2 - phi1
                    dphi = (dphi + np.pi) % (2 * np.pi) - np.pi
                    
                    if dphi <= 0.01:
                        continue  # 直接 skip 当前 event

                    # proxy_trans = np.log(dphi + 1e-5)
                    proxy_trans = 1/dphi
                    
                    # proxy_eta = -np.log(np.tan(angle / 2))

                    # feat = np.array([angle])  # 弧度值，范围 [0, π]
                    # feat = np.array([proxy_trans])
                    feat = np.array([proxy_trans, 0])  # 2D 特征

                    # feat = np.concatenate([trk_feat, calo_feat, calo_innr_feat])
                    # feat = np.concatenate([trk_feat, calo_innr_feat])

                    X_data.append(feat)

                    # Y: PrimaryG4P_Pt - truth pt 
                    Truth_Pt = data["PrimaryG4P_Pt"][i]
                    if len(Truth_Pt) != 1:
                        continue
                    Y_data.append(Truth_Pt[0])

            except Exception as e:
                print(f"Error reading {root_file}: {e}")
                continue

        print(f"Total usable entries: {len(X_data)}")
        print(f"[Stats] Events failed cond_012: {fail_012}")
        print(f"[Stats] Events failed cond_3or4: {fail_34}")
        print(f"[Stats] Events failed cond_5or6: {fail_56}")

        return np.array(X_data), np.array(Y_data)
