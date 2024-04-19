# TrackJetQAMaker

A Fun4All module to generate QA plots for track jets and related objects. Reads
in three DSTs (`DST_TRACKS`, `DST_TRKR_HIT`, and `DST_TRKR_CLUSTER`), reconstructs
anti-kt jets from the tracks, and then generates QA histograms.

## General Usage

There are four categories of plots it can produce: tracker hits, tracker clusters,
tracks, and tracks in jets. Each category is then split up into subsystems (MVTX,
INTT, TPC).

The actual routines used to fill each histogram are consolidated into four "sub-modules:"

  1. `HitQAMaker,` which histograms low-level hit information;
  2. `ClustQAMaker,` which histograms cluster information; and
  3. `TrackQAMaker,` which histograms information on tracks (inclusive); and
  4. `JetQAMaker,` which histograms information on jets and the tracks in them.

Each sub-module can be turned on or off by setting the options `.doHitQA`, `.doClustQA`,
`.doTrackQA`, and `.doJetQA` to true/false respectively.

The module is compiled in the usual way:

```
# in src/
cd build
../autogen.sh --prefix=$MYINSTALL
make -j 4
make install
```

## Further Information

Alongside the definitions of the top-level module and its sub-modules, there are a collection
of helper objects used to streamline the code and centralize related information.

  - `TrackJetQAMakerConfig.h:` defines the struct used to collect **all** user-configurable options.
  - `TrackJetQAMakerHelper.h:` consolidates a variety of useful methods and information (e.g.
    checks on whether or not a particular hit/cluster is in a given subsystem).
  - `TrackJetQAMakerHistDef.h:` consoldiates the binning schemes used across all three sub-modules,
    and defines some useful types for automating histogram creation.
