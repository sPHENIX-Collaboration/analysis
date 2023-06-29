# STrackCutStudy

This class reads in the output of the SVtxEvaluator class and studies the impact of various cuts on track populations.  In other words, it takes in the track evaluator tuples and generates a large number of histograms and plots which show how various quantities change after carving out different populations of tracks.  Additional scripts and macros which may be useful can be found in the `macros` and `scripts` directories.

## Usage

After copying the source files (in `src/`), compile the package in the usual manner:

```
./autogen.sh --prefix=$MYINSTALL
make -j 4
make install
```

The class can be run locally via:

```
root -b -q DoFastTrackCutStudy.C
```

The above macro only generates a portion of the possible output. This class is *very* memory-hungry, and can quickly devour all available RAM.  If you would like to run with the full output on SDCC, it's recommended to run it on condor. After editing `DoTrackCutStudy.C` and `DoTrackCutStudyOnCondor.job` for your purposes, do:

```
./SubmitTrackCutStudy.sh
```
