To set up, cd to the run directory and do 
bash setup.sh [user's build directory] [user's install directory]

This will set everything up, build, and install the module. Note that the scripts are currently set up to use the ana.458 build, which is old. The user should probably update this, but it should be kept consistent across scripts.

The User can skip steps marked optional if the user wants to - the default setup reaches into jocl's tg directory to find the ttrees. If the user doesn't want to skip them (i.e., the user wants to generate their own ttrees for whatever reason), the user should edit the file addcommand.sh by removing all instances of the text jocl in the file (there should only be one) and replace with $UN

To get luminosities:

1. create a list of run numbers the user wishes to use. This can be accomplished in various ways, but probably the best one is to use the repo at https://github.com/sPHENIX-Collaboration/analysis/blob/master/runListGeneration/Run2024pp/ to get a list. This step is non-optional.

Then do

2. (optional) CreateDstList.pl --build [build to use] --cdb [cdb tag] --list [user's run list]

To get lists of DSTs.

3. (optional) do

bash run_runs.sh

To get a bunch of ttrees to a tg directory that setup.sh created. The user won't need to look at these.

4. do

bash run_alladd.sh

To get more condensed ttrees for analysis.

At this point, the user can either run

root "check.C([run number])" to dump out the info for a given run

or

root get_all_lumi.sh [user's runlist]

to get the total luminosity for all triggers for all the runs in the user's list. Note that this reports luminosity ONLY for good/analyzable segments. Segments which are corrupt or otherwise unable to be analyzed are not used.