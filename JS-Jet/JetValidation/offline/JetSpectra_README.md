This is a guide to creating jet spectra using pythia+HIJING via the EMCal and HCal JetValidation.h/cc 
For this analysis to work you’ll need your own fork of the ‘analysis’ repository that is within the sPHENIX Collaboration Github
cd into the correct repo:
`/sphenix/user/<username>/analysis/JS-Jet/JetValidation`

You should see three repositories, all of importance for the analysis
Once you have this you will need to load the proper libraries using the following set of commands highlighted:

```
cd src
mkdir build
cd build
source /opt/sphenix/core/bin/sphenix_setup.sh
export MYINSTALL=~/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
```

Go back to src
```
cd ..             
autogen.sh --prefix=$MYINSTALL
make -j 4
make install
cd ../macro/
```

Get some files to run on using CreateFileList.pl, for example to get 1000 events of pythia dijets embeded in minimum bias HIJING:
CreateFileList.pl -n 1000 -type 11 -embed DST_CALO_CLUSTER DST_TRUTH_JET DST_GLOBAL
Test run using Fun4All. The Fun4All macro takes in input file lists for the truth jet and calo cluster DSTs and an output file name as inputs. For example, you can run:
```
root -b -q 'Fun4All_JetVal.C("dst_truth_jet.list", "dst_calo_cluster.list", "dst_global.list", "output.root")'
.q
```
This will quit root if it does not close
You should now have a output.root file that you can make the Jet Spectra from. In the offline directory, there is an example macro that draws a few figures that you can base a separate macro on that you’ll make for Jet Spectra.
If you’re unsure what is in a root file, I like to open a TBrowser, find the root file, open it’s TTree’s and find the branches that we’d like to make figures of. This way I know what they are named in the root file too

```
root output.root
`TBrowser b
```
A TBrowser should open in a separate window: sometimes it will take a bit to load. We see that output.root has a TTree called ‘T’ and many branches containing the TH1D histograms we’d like. Specifically we want branch ‘pt’ 



See next page
I can double check that this is what we want by looking at a very basic histogram in TBrowser of pt: In root do the following command:
```
T->Draw(“pt”,””,””) or
T->Draw(“pt”,””,”colz”
```

Alright this looks like the shape we’d expect, with a high number of jets at low pt and a low number towards higher pt. Now we need to make a simple macro

Quit root using .q and the TBrowser will close. Move to the offline directory
```
cd ../offline
```
From here on, you should try to create your own base macro. You will need some normalization and labels so please see my code when the time comes. But I really suggest you try creating it yourself first as it is a good learning experience

See this link [my GitHub](https://github.com/jamesjl3/analysis/blob/master/JS-Jet/JetValidation/offline/JetSpectra.C).
See this link [for this README file with images](https://docs.google.com/document/d/1QWnMbti5S1oRhRMRGH1ADHYjiT_jWWklqyUcjFASofc/edit?usp=sharing)

I probably have made adjustments so make your own edits to your copy as needed

