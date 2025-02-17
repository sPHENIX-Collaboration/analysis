To set up, cd to the run directory and do 
bash setup.sh [your build directory] [your install directory]

This will set everything up, build, and install the module.

To get luminosities:

first create a list of run numbers you wish to use. This can be accomplished in various ways, but probably the best one is to use the repo at https://github.com/sPHENIX-Collaboration/analysis/blob/master/runListGeneration/Run2024pp/ to get a list.

Then do

CreateDstList.pl --build [build to use] --cdb [cdb tag] --list [your run list]

To get lists of DSTs.

Next do

bash run_runs.sh

To get a bunch of ttrees to a tg directory that setup.sh created. You won't need to look at these.

Then do

bash run_alladd.sh

To get more condensed ttrees for analysis.

At this point, you can either run

root "check.C([run number])" to dump out the info for a given run

or

root get_all_lumi.sh [your runlist]

to get the total luminosity for all triggers for all the runs in your list.