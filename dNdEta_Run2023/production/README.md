# dNdEta production chain

This directory contains the python script `InttMbdCombineChain.py` and configuration `config_combinechain.py` to run the dNdEta production chain in a push-button way, per requested by the sPHENIX Collaboration.  
Run the the python script `InttMbdCombineChain.py` with the `--help` option to see the usage.
```
Usage: InttMbdCombineChain.py ver [options -h]

Options:
  -h, --help          show this help message and exit
  -i, --inttdst       Run INTT DST production
  -n, --inttntuple    Run INTT ntuple production
  -m, --centmbntuple  Run Centrality&MB ntuple production
  -c, --runcombiner   Run INTT event combiner
```

The configuration `config_combinechain.py` contains the parameters for the INTT DST production, INTT ntuple production, and the INTT-MBD event combiner. To show the parameters, run `InttMbdCombineChain.py` with the `--printparam` option.
```python
username = pwd.getpwuid(os.getuid())[0]
softwarebasedir = '/sphenix/user/{}/software'.format(username)
productiondir = os.path.dirname(os.path.abspath(__file__))
dndetamacrodir = '{}/macros'.format(os.path.abspath(os.path.join(productiondir, os.path.pardir)))
macrodir = '{}/macros'.format(softwarebasedir,username)
macrorepo = 'https://github.com/sPHENIX-Collaboration/macros.git'

runnumber = 20869

inttdstproduction_InttUnpacker_nEvt = -1
inttdstproduction_runTrkrHits = True
inttdstproduction_runTkrkClus = True
inttdstproduction_stripRawHit = True

inttntupleproduction_productionTag = '2023p011'
inttntupleproduction_InttNtupleDir = 'Data_NtupleIntt_Run{}'.format(runnumber)
inttntupleproduction_eventPerJob = 1000
inttntupleproduction_nJob = 551
inttntupleproduction_softwareversion = 'ana.405'
inttntupleproduction_submitcondor = False

centntupleproduction_softwareversion = 'ana.403'
centntupleproduction_productionTag = '2023p011'
centntupleproduction_CentralityNtupleDir = 'Data_NtupleCentrality_Run{}'.format(runnumber)
centntupleproduction_eventPerJob = -1
centntupleproduction_nJob = 1
centntupleproduction_submitcondor = True

inttmbdcombine_combinedNtupleName = 'Data_CombinedNtuple_Run{}.root'.format(runnumber)
```

## INTT DST production
To run the INTT DST production, do
```
python InttMbdCombineChain.py --inttdst
```
The script will do the following steps:
1. Check whether the [sPHENIX macro repository](https://github.com/sPHENIX-Collaboration/macros/tree/master) exists under the `[softwarebasedir]` directory (`[softwarebasedir]` is specified in the configuration `config_combinechain.py`). If yes, it will pull the latest version of the repository. If not, it will clone the repository.  
2. Change the directory to `[softwarebasedir]/macros/InttProduction`, and modify the `runTrkrHits`, `runTkrkClus`, `stripRawHit` flags in `Fun4All_Intt_Combiner.C` according to the flags `[inttdstproduction_runTrkrHits]`, `[inttdstproduction_runTkrkClus]`, and `[inttdstproduction_stripRawHit]` in `config_combinechain.py`.
3. Execute the `intt_makelist.sh` under `[softwarebasedir]/macros/InttProduction` to produce INTT raw data lists of the given run number `[runnumber]`.
4. Run the macro `Fun4All_Intt_Combiner.C` to produce the INTT DST. You could change `inttdstproduction_InttUnpacker_nEvt` to a number other than -1 for testing purpose. If it is set to -1, the macro will process all events.
4. After the INTT DST production is done, the script will clean up the INTT raw data lists and move the INTT DST to the `[productiondir]` directory (`[productiondir]` is specified in `InttMbdCombineChain.py`).

## INTT ntuple production
To run the INTT ntuple production, do
```
python InttMbdCombineChain.py --inttntuple
```
The script will do the following steps:
1. Check whether the INTT DST `[productiondir]/intt-[runnumber].root` exists. If not, it will try to copy the pre-generated DST under `/gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/`. If there is no pre-generated DST, the script will exit.
2. Change the directory to `[dndetamacrodir]/condor/` (`[dndetamacrodir]` is specified in `config_combinechain.py`). 
3. Run the `runCondor.py` to submit condor jobs to produce the INTT ntuples. (See [runCondor.py](https://github.com/sPHENIX-Collaboration/analysis/blob/master/dNdEta_Run2023/macros/condor/runCondor.py) for more details).
4. The INTT ntuples will be stored under `[productiondir]/[inttntupleproduction_InttNtupleDir]`.

## Centrality and minimum-bias ntuple production
To run the centrality and minimum-bias ntuple production, do
```
python InttMbdCombineChain.py --centmbntuple
```
The scripts will do the following steps:
1. Change the directory to `[dndetamacrodir]`. 
2. Execute the sPHENIX pre-built script `CreateDstList.pl` to produce the calorimeter DST lists with the given `[runnumber]`, the sPHENIX software version `[centntupleproduction_softwareversion]`,  and the production tag `[centntupleproduction_productionTag]`.
3. Change the directory to `[dndetamacrodir]/condor/`.
4. Run the `runCondor.py` to submit condor jobs to produce the centrality and minimum-bias ntuple. (Note: You should run a single condor job for the centrality and minimum-bias ntuple production, as the centrality and minimum-bias ntuple production is not parallelizable)
5. The centrality and minimum-bias ntuples will be stored under `[productiondir]/[centntupleproduction_CentralityNtupleDir]`.

## INTT-MBD event combiner
To run the INTT-MBD event combiner, do
```
python InttMbdCombineChain.py --runcombiner
```
The script will do the following steps:
1. Merge the INTT ntuples. The merged INTT ntuples will be stored under `[productiondir]/[inttntupleproduction_InttNtupleDir]`. 
2. Run the INTT-MBD event combiner `intt_mbd_evt_combiner_v1.py`. See [INTT_MBD_evt_combiner repository](https://github.com/sPHENIX-Collaboration/analysis/tree/master/INTT_MBD_evt_combiner) for more details.