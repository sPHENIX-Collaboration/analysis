# SiliconSeedAna Framework

**Recent update**: May 30, 2025

This project is designed to simulate and analyze charged particle tracking and calorimeter matching in the sPHENIX environment. The core component, `SiliconSeedAna`, collects information from silicon trackers, EMCal, and HCal, and stores it in a structured ROOT file for further physics analysis.

---

## 📁 Directory Overview

### 1. `PHYTIAmacro/`

- **Purpose**: Contains macros for full simulations based on PYTHIA event generation.
- **Main Macro**: `Fun4All_PHYTIA_Silicon.C`
- **Usage**:
  1. First, compile `SiliconSeedAna` and build the shared library.
  2. Then run this macro to simulate PYTHIA events.
- **Output**: ROOT file containing tracker and calorimeter hit information.

---

### 2. `gunmacro/`

- **Purpose**: Contains macros for single-particle simulations.
- **Main Macro**: `Fun4All_singleParticle_Silicon.C`
- **Usage**:
  1. Compile `SiliconSeedAna` first.
  2. Run the macro to simulate single-particle events with fixed momentum and direction.
- **Use case**: Useful for analyzing magnetic deflection and detector matching at specific kinematics.

---

### 3. `SiliconSeedAna/`

- **Purpose**: Core analyzer that collects and stores track and calorimeter information into ROOT TTrees.
- **Key Files**:
  - `SiliconSeedAna.cc`
  - `SiliconSeedAna.h`
- **Features**:
  - Extracts Track info from silicon detectors tracking and clusters from Calo(EMCal, and HCal)
  - Projects track states to the EMCal radius (e.g., 93.5 cm).
  - Saves detailed information such as:
    - Initial track position and momentum
    - Projected EMCal coordinates
    - Matching-related variables

## 📂 TTree Structure in `SiliconSeedAna`

The following TTrees are created and filled within the `SiliconSeedAna` module. Each serves a specific purpose in tracking and calorimeter studies.

---

### 🌱 `trackTree` – Tracking Information

Stores per-track information from the silicon tracker and projections to EMCal.

**Branches:**
- `evt` — Event number (`int`)
- `track_id` — Track ID (`vector<unsigned int>`)
- `x0`, `y0`, `z0` — Initial position (`vector<float>`)
- `px0`, `py0`, `pz0` — Initial momentum (`vector<float>`)
- `eta0`, `phi0`, `pt0` — Kinematic variables (`vector<float>`)
- `chi2ndf` — Track fit quality (`vector<float>`)
- `charge` — Particle charge (`vector<int>`)
- `nmaps`, `nintt`, `innerintt`, `outerintt` — Tracker hit counts (`vector<int>`)
- `crossing` — Bunch crossing ID (`vector<short int>`)
- `x_proj_emc`, `y_proj_emc`, `z_proj_emc` — EMCal projected position (`vector<float>`)
- `px_proj_emc`, `py_proj_emc`, `pz_proj_emc` — Projected momentum at EMCal (`vector<float>`)
- `eta_proj_emc`, `phi_proj_emc`, `pt_proj_emc` — Projected kinematics at EMCal (`vector<float>`)

---

### 🔥 `caloTree` – Calorimeter Hit Information

Stores calorimeter cluster positions and energy deposits per event.

**Branches:**
- `calo_evt` — Event number (`int`)
- `x`, `y`, `z` — Hit position (`vector<float>`)
- `r`, `phi` — Cylindrical coordinates (`vector<float>`)
- `energy` — Hit energy (`vector<float>`)

---

### 🟪 `SiClusTree` – Silicon Cluster Information

Clusters associated with silicon tracks.

**Branches:**
- `evt` — Event number (`int`)
- `Siclus_trackid` — Associated track ID (`vector<int>`)
- `Siclus_layer` — Cluster layer number (`vector<int>`)
- `Siclus_x`, `Siclus_y`, `Siclus_z` — Cluster positions (`vector<float>`)

---

### 🎯 `truthTree` – Truth Particle Information

Truth-level kinematic info for simulated particles.

**Branches:**
- `truth_pid` — PDG ID (`vector<int>`)
- `truth_px`, `truth_py`, `truth_pz`, `truth_e` — Momentum and energy (`vector<float>`)
- `truth_pt`, `truth_eta`, `truth_phi` — Derived kinematic variables (`vector<float>`)
