Here are the instructions for creating the polar efficiency plots for the TPC
--------------------------------------
Go to sphenix/user/<username>/analysis/TPC/DAQ/TPCRawDataTree
Make sure the TPCRawDataTree is able to run by doing the following command
  ./autogen.sh

Then go to sphenix/user/<username>/analysis/TPC/DAQ/macros
Open the Fun4All_TPC_UnpackPRDF.C with a text editor -- I use emacs
  
  Edit the input file to pull the correct run number PRDF files
  Now edit the output file to be some directory you will be working on the Efficiency_ModuleDisplay.C in
  (For emacs)
Save ctrl-x ctrl-s
Exit ctrl-x ctrl-c
---------------------------------------
  
Now source in the PRDFs
  
  source run_UnpackPRDF.sh /sphenix/lustre01/sphnxpro/rawdata/commissioning/tpc/pedestal/*10179*
/sphenix/lustre01/sphnxpro/rawdata/commissioning/tpc/pedestal/TPC_ebdc*_pedestal-00010179-0000.prdf
  
Replace 10179 (the run number) to the one you need
The * will make it run over all the sectors of the TPC. If you want just one sector enter the sector number 
  Example: TPC_ebdc*_pedestal-00010179-0000.prdf  ---> TPC_ebdc00_pedestal-00010179-0000.prdf  
  
Depending on the number of events you run it will take some time (10 events is about 15 minutes on a good day)
  
---------------------------------------
Move to the repository you put these files in and make sure you bring TPC_Channel_QA.C, SectorMap_Display.C, Efficiency_ModuleDisplay.C, /
and Noise_ModuleDisplay.C with you if working in another repository.
Okay now you need to change the run number in input and output in TPC_Channel_QA.C in the following line. Also change the directory.
  /sphenix/user/rosstom/test/testFiles/TPC_ebdc"+sectorNumber+"_"+runNumber+"-0000.prdf_TPCRawDataTree.root
  
This will take a long time probably to generate the root files.
---------------------------------------
Yay, now there's root files we can build from. Now go into SectorMap_Display.C and we'll make some TNtuples from them.
  
  Make sure your input is something like this:
   sphenix/u/jamesj3j3/workfest/sPHENIXProjects/outputfile_TPC_ebdc%s_pedestal-00010305.root
  Again be sure to change the directory and run number appropriately
  
  Make sure to change the outfile name and directory too:
  TFile h_outfile10305("pedestal-10305-outfile.root", "RECREATE");
  
  Save and run. Should be pretty quick.
-----------------------------------------
Okay, now we can do Efficiency_ModuleDisplay.C
  
  Again change the input with proper directory and run number:  
    const TString filename3( Form( "./pedestal-10179-outfile.root") );
  
  Change the titles of the histograms as needed (line 254 and 255)
  Change the minimum value of the live fraction percentage to the needed one (line 349)
  
 Save and run. Will take about 20-30 seconds.
--------------------------------------------
Follow this same procedure for Noise_ModuleDisplay.C
--------------------------------------------
End of process 
