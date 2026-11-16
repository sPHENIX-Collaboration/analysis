# EventDisplayMaker

You can load event displays and GDML files for viewing here: [sPHENIX Event Display Online Page](https://www.sphenix.bnl.gov/edisplay/)

Two Fun4All `SubsysReco` modules that write per-event JSON snapshots of
reconstructed tracks — and optionally calorimeter towers — for loading into
a 3D web-based event-display viewer. A full sPHENIX detector geometry
(`gdml/sphenix.gdml`) file is included with an offset of the MVTX so the clusters line up.

There are two event display makers for historical reasons

- **`displayEvents`** — shows everything in a collision. Typically it's crossing zero so that the calorimeters can be shown as well. This was made to showcase full collisions events for the end of RHIC operations.

- **`event_display_maker`** — shows a decay reconstucted by KFParticle, and was the original code that `displayEvents` is derived from. It reads the
  output of a prior `KFParticle_sPHENIX` reconstruction, picks out events
  where a named "mother" particle (e.g. `K_S0`) falls in a mass window,
  and color-codes the daughter tracks by species so the candidate stands
  out against the rest of the event.

## Requirements

Both modules need the same core set of nodes on the tree, and treat every
one of them as a hard requirement — `load_nodes()` returns `ABORTRUN` if
any is missing:

- `SvtxVertexMap`, `SvtxTrackMap`
- `TRKR_CLUSTER` (used to look up cluster global positions for hit points)
- `ActsGeometry` (used to convert cluster keys to global positions)
- `Gl1Packet` — tried first as `GL1RAWHIT`, then `GL1Packet` to get the BCO (users can also request a specific set of BCOs to avoid processing events they don't want or grab one from their analysis they think will look good)
- `TriggerRunInfo`

`event_display_maker` additionally requires a `KFParticle_Container` node
named `<setKFParticleContainerName()>_KFParticle_Container` (default
container name `"reconstructedParticles"`, so the default node name is
`reconstructedParticles_KFParticle_Container`) — this is the output of
whatever `KFParticle_sPHENIX` reconstruction you ran upstream.

Calorimeter nodes are optional and only looked up when `addCaloInfo(true)`
is set. For each of EMCal/IHCal/OHCal, the calibrated tower container is
tried first (`TOWERINFO_CALIB_CEMC`, etc.), falling back to the raw
`TOWERS_CEMC`-style name; the matching `TOWERGEOM_*` container is also
required. A missing calorimeter just gets skipped for that run (`hasEMCal`
etc. get set to `false`) rather than aborting the event.

## Building

Note the built library name doesn't match the folder name: `configure.ac`
names the package `hf_trigger_data_to_json` for different legacy reasons, so the library you link/load
is `libhf_trigger_data_to_json.la` (`R__LOAD_LIBRARY(libhf_trigger_data_to_json.so)`
in a macro), containing both `displayEvents` and `event_display_maker`.

## How it works

Both modules follow the same per-event shape; `event_display_maker` adds a
selection/highlighting layer on top.

1. **Node retrieval**, as above — aborts the run if any required node is
   missing.
2. **Loop over every reconstructed vertex** in `SvtxVertexMap` for the
   event. Each vertex gets its own JSON file, so a multi-vertex event
   produces multiple output files. The loop (and the module) stops
   producing further output once `setMaxEvtDisplays()` displays have been
   written, even mid-event.
3. **(`event_display_maker` only) Select the decay.** Walk the
   `KFParticle_Container`, find the entry matching `setMotherName()`
   (default `"K_S0"`), and check its reconstructed mass against
   `setMassRange()`. Entries matching `setIntermediateNames()` (e.g.
   `Lambda0`) are kept separately for drawing. Any KFParticle whose
   species is one of `e-, mu-, pi+, K+, proton` **and** whose track shares
   the vertex's beam crossing becomes a "trigger" daughter. If fewer than
   `setNumberDaughters()` trigger daughters were found, the vertex is
   skipped.
4. **Collect every track at the vertex** (`displayEvents`: this is the
   whole track set; `event_display_maker`: this is the *background* set,
   with the trigger daughters excluded by track ID so they aren't drawn
   twice).
5. **Decode triggers** via `TriggerAnalyzer::decodeTriggers()` — this is
   called every event but isn't visibly used to gate anything else in the
   code as it stands; worth knowing if you're relying on it for trigger
   selection.
6. **Compute the BCO** (`Gl1Packet` BCO + track beam-crossing offset) and,
   if `useTheseBCOs()` was set, skip this vertex unless its BCO is in that
   list.
7. **Build the JSON.** Event metadata (run number, date, primary vertex
   position, a caption that differs depending on `plotsApproved()`), then
   for each track: its cluster/track-state hit positions (silicon hits
   come from the track's silicon seed, since track states don't reliably
   carry silicon clusters; TPC hit positions come from the track state
   directly, other detectors' hit positions are recomputed via
   `ActsGeometry::getGlobalPosition()`), and its momentum, position, decay
   length, charge, and draw color. `event_display_maker` additionally
   draws the mother and any intermediates as their own track-like entries,
   colored distinctly from the daughters and from the background.
8. **(optional) Add calorimeter hits** if `addCaloInfo(true)`, one entry
   per tower above an energy threshold, with projective-geometry metadata
   (radius range, angular bin size, color, display scale) for each
   calorimeter.
9. **Write the file** to `setEventDisplayPath()` as
   `EvtDisplay_<run>_<bco>.json` (`displayEvents`) or
   `EvtDisplay_<motherName>_<run>_<bco>.json` (`event_display_maker`), and
   increment the display counter.

## Quick start

No example macro ships in this repo yet — these snippets are illustrative,
built directly from the constructors/setters in the header files, not
copied from an existing file.

```cpp
// General-purpose: show every track at every vertex
displayEvents *disp = new displayEvents("EventDisplay");
disp->setEventDisplayPath("./displays");
disp->setMaxEvtDisplays(20);
disp->addCaloInfo(true);
se->registerSubsystem(disp);
```

```cpp
// KFParticle-driven: highlight K-short candidates
event_display_maker *disp = new event_display_maker("KshortDisplay");
disp->setKFParticleContainerName("reconstructedParticles");  // -> reconstructedParticles_KFParticle_Container
disp->setMotherName("K_S0");
disp->setMassRange(0.46, 0.53);
disp->setMaxEvtDisplays(20);
disp->addCaloInfo(true);
se->registerSubsystem(disp);
```

## Configuration reference

A number of setters have a default *argument* that differs from the
member's default if the setter is never called at all — worth knowing
since "I didn't call the setter" and "I called it with no arguments" give
you different behavior (maybe because I was lazy or tired). Those cases are called out explicitly below.

### `displayEvents`

| Setter | Meaning | If never called | If called with no args |
|---|---|---|---|
| `setEventDisplayPath(path)` | Output directory for JSON files | `"./"` | — |
| `setMaxEvtDisplays(max)` | Stop after this many displays | `100` | `10` |
| `setDecayTag(tag)` | Free-text tag shown in the plot caption | `"200 GeV p+p"` | — |
| `useTheseBCOs(bcoList)` | Only write displays for these BCOs | all BCOs | — |
| `plotsApproved(approved)` | Switch the caption between an internal/debug stamp (with BCO) and an approved-looking one | `false` | `true` |
| `addCaloInfo(add)` | Include EMCal/IHCal/OHCal tower hits | `false` | `true` |
| `dontShowTrackClusters(avoid)` | Suppress track hit points from the output (only in `displayEvents`) | `false` | `true` |

### `event_display_maker`

| Setter | Meaning | If never called | If called with no args |
|---|---|---|---|
| `setKFParticleContainerName(name)` | Base name; node read is `<name>_KFParticle_Container` | `"reconstructedParticles"` | — (required arg) |
| `setNumberDaughters(n)` | Minimum trigger daughters required at a vertex | `2` | `2` |
| `setEventDisplayPath(path)` | Output directory | `"./"` | — |
| `setMassRange(min, max)` | Mother mass window [GeV] | `0.4, 0.6` | `0.48, 0.51` |
| `setMaxEvtDisplays(max)` | Stop after this many displays | `100` | `10` |
| `setMotherName(name)` | Species name to match in the KFParticle container | `"K_S0"` | `"K_S0"` |
| `setIntermediateNames(names)` | Intermediate-resonance species to also draw | *(empty)* | `{"Lambda0"}` |
| `setDecayTag(tag)` | Free-text tag shown in the plot caption | `"200 GeV p+p"` | — |
| `useTheseBCOs(bcoList)` | Only write displays for these BCOs | all BCOs | — |
| `plotsApproved(approved)` | Switch the caption between internal/debug and approved-looking | `false` | `true` |
| `addCaloInfo(add)` | Include EMCal/IHCal/OHCal tower hits | `false` | `true` |

Calorimeter energy thresholds differ between the two classes and aren't
currently exposed as setters: `displayEvents` uses a per-calorimeter
threshold (EMCal 0.08, IHCal 0.01, OHCal 0.04 GeV), while
`event_display_maker` uses one flat threshold (0.005 GeV) for all three.
The projective tower geometry constants (radius range, angular bin size,
display color, scale) are the same fixed sPHENIX detector values in both
classes.

## Tips and known limitations

- All of `SvtxVertexMap`, `SvtxTrackMap`, `TRKR_CLUSTER`, `ActsGeometry`,
  `Gl1Packet`, and `TriggerRunInfo` are hard per-event requirements for
  both modules — missing any of them aborts the run, not just the event.
- `event_display_maker`'s daughter-count check
  (`kfp_daughters.size() < m_number_of_daughters`) is a coarse
  completeness filter, not a guarantee that the *right* daughters were
  found — the code's own comment next to it calls this out as "risky."
- One JSON file is written per *vertex*, not per event, so an event with
  several reconstructed vertices produces several files with the same run
  number but different BCOs.
- `dontShowTrackClusters()` only exists on `displayEvents` — there's no
  equivalent toggle on `event_display_maker`.