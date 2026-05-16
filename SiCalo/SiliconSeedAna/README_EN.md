# 📘 SiliconCaloMatching README (English)

## Overview
SiliconCaloMatching is a module in the sPHENIX framework that matches silicon tracks (SvtxTrack) with EMCal clusters (RawCluster) and reconstructs transverse momentum (pT).

---

## Input
- SvtxTrack (track)
- RawCluster (EMCal cluster: x, y, z)

---

## Matching

Δφ = φ_track − φ_cluster  
Δz = z_track − z_cluster

Distance:

r² = (Δφ × R)² + (Δz)²

---

## Track Direction

```cpp
phi_intt = atan2(oCpos.y()-iCpos.y(), oCpos.x()-iCpos.x());
phi_calo = atan2(emc_y - oCpos.y(), emc_x - oCpos.x());
```

---

## Momentum Reconstruction

```cpp
float dphi = phi_calo - phi_intt;
float pt_calo = 0.21 * pow(fabs(dphi), -0.986);
```

Formula:

pT = 0.21 × |Δφ|^-0.986

---

## Interpretation
Momentum is reconstructed from track bending in magnetic field.

---

## Summary
Both matching and momentum reconstruction are based on Δφ.
