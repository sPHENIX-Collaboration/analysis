# 📘 SiliconCaloMatching README（日本語版）

## 概要
SiliconCaloMatching は sPHENIX 解析フレームワークにおいて、
シリコントラック（SvtxTrack）と EMCal クラスタ（RawCluster）を対応付け、
さらに横運動量 pT を再構成するモジュールである。

---

## 入力
- SvtxTrack（トラック）
- RawCluster（EMCalクラスタ: x, y, z）

---

## マッチング

Δφ = φ_track − φ_cluster  
Δz = z_track − z_cluster

距離：

r² = (Δφ × R)² + (Δz)²

---

## トラック情報

シリコンクラスタ（内層・外層）を用いてトラック方向を定義：

```cpp
phi_intt = atan2(oCpos.y()-iCpos.y(), oCpos.x()-iCpos.x());
```

EMCal方向：

```cpp
phi_calo = atan2(emc_y - oCpos.y(), emc_x - oCpos.x());
```

---

## 運動量再構成

```cpp
float dphi = phi_calo - phi_intt;
float pt_calo = 0.21 * pow(fabs(dphi), -0.986);
```

数式：

pT = 0.21 × |Δφ|^-0.986

---

## 意味
磁場中の曲がり Δφ から pT を推定する。

---

■出力（SiliconCaloTrack）
- track情報（pT, φ, z, charge）
- cluster情報（x, y, z, energy）
- dphi, dz
- pt_calo

---
## まとめ
シリコン2点＋EMCal1点から曲がりを測り、pTを再構成する。 その結果をSiliconCaloTrackに保存し、SvtxTrackのpx, pyを更新する
