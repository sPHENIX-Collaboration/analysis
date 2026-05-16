# 📘 SiliconCaloMatching README (Complete Version / English)

---

## ■ Overview

`SiliconCaloMatching` is a module in the sPHENIX analysis framework that integrates two detector subsystems:

- Silicon tracker reconstructed tracks (`SvtxTrack`)
- Electromagnetic calorimeter clusters (`RawCluster`)

The module not only performs matching between these objects but also:

👉 **reconstructs the transverse momentum (pT) of particles using detector geometry and bending information.**

---

## ■ Input Data

### ● SvtxTrack (Track)

Represents reconstructed charged particle trajectories.

#### Main parameters
- Transverse momentum: `pT`
- Azimuthal angle: `φ`
- Position: `(x, y, z)`
- Charge: `charge`

---

### ● RawCluster (EMCal Cluster)

Represents energy deposits in the electromagnetic calorimeter.

```cpp
(x, y, z)
```

👉 Stored in Cartesian coordinates (NOT η–φ representation)

---

## ■ Overall Algorithm

```
SvtxTrack
    ↓
Projection to EMCal surface
    ↓
Cluster search
    ↓
Compute Δφ, Δz
    ↓
Select closest cluster
    ↓
pT reconstruction
    ↓
SiliconCaloTrack output
```

---

# ■ Track Representation (Important)

This algorithm does NOT use full track fitting.

👉 Instead, the track direction is approximated using **two silicon clusters**.

---

## ● Cluster Retrieval

```cpp
TrackSeed* si_seed = track->get_silicon_seed();
```

---

## ● Inner and Outer Clusters

```cpp
ckey_outer
ckey_inner
```

```cpp
Acts::Vector3 oCpos;
Acts::Vector3 iCpos;
```

---

## ● Track Direction (φ_intt)

```cpp
phi_intt = atan2(
    oCpos.y() - iCpos.y(),
    oCpos.x() - iCpos.x()
);
```

👉 Direction of the track inside the silicon detector

---

## ● Direction toward EMCal (φ_calo)

```cpp
phi_calo = atan2(
    emc_y - oCpos.y(),
    emc_x - oCpos.x()
);
```

👉 Direction from outer silicon cluster to EMCal cluster

---

# ■ Definition of Δφ (Core Quantity)

```
Δφ = φ_calo − φ_intt
```

👉 Represents the **bending angle** of the particle in the magnetic field

---

# ■ Matching

## ● Matching Variables

```
Δφ = φ_track − φ_cluster
Δz = z_track − z_cluster
```

---

## ● Distance Metric

```
r² = (Δφ × R)² + (Δz)²
```

- R ≈ 93.5 cm (EMCal radius)

---

## ● Interpretation

- φ difference is converted into arc length
- z difference is directly used

👉 Matching is performed in **real spatial coordinates**

---

# 🚀 ■ Momentum Reconstruction (Core Part)

## ■ Implementation (Exact Code)

```cpp
float phi_intt = atan2(oCpos.y()-iCpos.y(), oCpos.x()-iCpos.x());
float phi_calo = atan2(emc_y - oCpos.y(), emc_x - oCpos.x());

float dphi = phi_calo - phi_intt;

float pt_calo = 0.21 * pow(fabs(dphi), -0.986);
```

---

## ■ Formula

pT = 0.21 × |Δφ|^-0.986

---

## ■ Physical Interpretation

In a magnetic field:

Δφ ∝ 1 / pT

Therefore:

pT ∝ 1 / Δφ

---

## ■ Why exponent ≠ -1

Expected ideal relation:

pT ∝ 1 / Δφ

Actual implementation:

pT ∝ |Δφ|^-0.986

Reasons:

- Non-uniform magnetic field
- Detector geometry effects
- Finite resolution of measurements

👉 Parameters are obtained through calibration using simulation or real data

---

## ■ General Form

pT = A × |Δφ|^α

| Parameter | Value |
|----------|------|
| A | 0.21 |
| α | -0.986 |

---

# ■ Core Idea of Algorithm

👉 **Reconstruct momentum with minimal information**

```
2 silicon points + 1 EMCal point → pT
```

---

# ■ Features

✅ Computationally lightweight  
✅ Works without full tracking (e.g., no TPC)  
✅ Calibrated with experimental data  

---

# ■ Limitations

- Reduced accuracy at high pT (small Δφ)  
- Sensitive to detector alignment  
- Possible mismatching in high occupancy events  

---

# ■ Summary

- φ is computed using atan2
- Matching uses Δφ and Δz
- Clusters are handled in (x, y, z)
- pT is reconstructed as:

pT = 0.21 × |Δφ|^-0.986

---

# ■ One-line Description

👉 Reconstruct transverse momentum from track bending angle (Δφ) using an empirical formula.

---

# ✅ Usage Notes

This documentation:

- Matches the actual implementation
- Includes physics interpretation
- Suitable for both beginners and developers

👉 Can be directly used as a GitHub documentation file
