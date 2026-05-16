# 📘 SiliconCaloMatching README（完全版・日本語）

## ■概要

`SiliconCaloMatching` は、sPHENIX解析フレームワークにおいて  
以下の2種類の検出器情報を統合するモジュールである：

- シリコントラッカーで再構成された飛跡（SvtxTrack）  
- 電磁カロリメータによるエネルギークラスタ（RawCluster）  

本モジュールはこれらを対応付けるだけでなく、  
👉 **トラックの横運動量（pT）を再構成するアルゴリズム**も実装している。

---

## ■入力データ

### ● SvtxTrack（トラック）

粒子の飛跡情報を持つオブジェクト。

#### 主なパラメータ
- 横運動量：`pT`
- 方位角：`φ`
- 空間座標：`(x, y, z)`
- 電荷：`charge`

---

### ● RawCluster（EMCalクラスタ）

電磁カロリメータにおけるエネルギー沈着を表す。

```cpp
(x, y, z)
```

👉 ηやφではなく、**デカルト座標で取得される**

---

## ■アルゴリズム全体

```
SvtxTrack
    ↓
EMCal面へ外挿
    ↓
クラスタ探索
    ↓
Δφ, Δz 計算
    ↓
距離最小クラスタ選択
    ↓
pT再構成
    ↓
SiliconCaloTrack 出力
```

---

# ■トラック情報の扱い（重要）

本アルゴリズムでは、通常のトラックフィットではなく：

👉 **シリコンクラスタ2点から方向を決定する**

---

## ● クラスタ取得

```cpp
TrackSeed* si_seed = track->get_silicon_seed();
```

---

## ● 外層・内層クラスタ

```cpp
ckey_outer
ckey_inner
```

```cpp
Acts::Vector3 oCpos;
Acts::Vector3 iCpos;
```

---

## ● トラック方向（φ_intt）

```cpp
phi_intt = atan2(
    oCpos.y() - iCpos.y(),
    oCpos.x() - iCpos.x()
);
```

👉 シリコン内での粒子の進行方向

---

## ● EMCal方向（φ_calo）

```cpp
phi_calo = atan2(
    emc_y - oCpos.y(),
    emc_x - oCpos.x()
);
```

👉 シリコン外層クラスタから見た EMCal 方向

---

# ■Δφの定義（コア）

```
Δφ = φ_calo − φ_intt
```

👉 粒子の曲がり角（磁場の影響）

---

# ■マッチング

## ● マッチング変数

```
Δφ = φ_track − φ_cluster
Δz = z_track − z_cluster
```

---

## ● 距離評価

```
r² = (Δφ × R)² + (Δz)²
```

- R ≈ 93.5 cm（EMCal半径）

---

## ● 意味

- φ差 → 円弧距離に変換
- z差 → そのまま使用

👉 **実空間距離で最も近いクラスタを選択**

---

# 🚀 ■運動量再構成（最重要）

## ■実装コード（完全一致）

```cpp
float phi_intt = atan2(oCpos.y()-iCpos.y(), oCpos.x()-iCpos.x());
float phi_calo = atan2(emc_y - oCpos.y(), emc_x - oCpos.x());

float dphi = phi_calo - phi_intt;

float pt_calo = 0.21 * pow(fabs(dphi), -0.986);
```

---

## ■数式

\[
p_T = 0.21 \cdot |\Delta \phi|^{-0.986}
\]

---

## ■物理的意味

磁場中では粒子は曲がるため：

\[
\Delta \phi \propto rac{1}{p_T}
\]

したがって：

\[
p_T \propto rac{1}{\Delta \phi}
\]

---

## ■なぜ指数が -1 ではないのか

理想的な関係：

```
pT ∝ 1 / Δφ
```

実装では：

```
pT ∝ |Δφ|^-0.986
```

理由：

- 磁場の非一様性
- 検出器幾何
- 分解能効果

👉 実データ／シミュレーションでフィットされた結果

---

## ■一般形

\[
p_T = A \cdot |\Delta \phi|^lpha
\]

| パラメータ | 値 |
|------|------|
| A | 0.21 |
| α | -0.986 |

---

# ■このアルゴリズムの本質

👉 **最小情報による運動量再構成**

```
シリコン（2点） + EMCal（1点） → pT
```

---

# ■特徴

✅ 計算が軽量  
✅ TPCなしでも動作  
✅ 実験データでキャリブレーション済み  

---

# ■制限

- 高pTでは精度低下（Δφが小さい）  
- アライメントに敏感  
- 高多重度環境で誤マッチ  

---

# ■まとめ

- φは atan2 で計算  
- マッチングは Δφ・Δz  
- クラスタは (x, y, z) で扱う  
- pTは以下で再構成：

\[
p_T = 0.21 \cdot |\Delta \phi|^{-0.986}
\]

---

# ■一言でいうと

👉 トラックの曲がり角（Δφ）から経験式でpTを再構成するアルゴリズム

---


## ■出力（SiliconCaloTrack）

- トラック情報（pT, φ, z, charge）
- クラスタ情報（x, y, z, energy）
- dphi, dz
- pt_calo

- 加えて、　SvtxTrackのpx, pyも更新する
- 
