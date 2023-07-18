
Here are the instructions for creating the polar efficiency plots for the TPC

*** If you encounter any problems please reach out to Jennifer (jennifer.l.james@vanderbilt.edu) or Luke (luke.legnosky@stonybrook.edu)

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

To change event number go into run_UnpackPRDF.sh located in sphenix/user/<username>/analysis/TPC/DAQ/macros and update the variable nEvents
---------------------------------------

Now source in the PRDFs, you need all of the two lines below (pedestal):

  source run_UnpackPRDF.sh /sphenix/lustre01/sphnxpro/rawdata/commissioning/tpc/pedestal/*10179*
  /sphenix/lustre01/sphnxpro/rawdata/commissioning/tpc/pedestal/TPC_ebdc*_pedestal-00010179-0000.prdf

OR -- you need all of the two lines below (beam):

  source run_UnpackPRDF.sh /sphenix/lustre01/sphnxpro/commissioning/tpc/beam/*11000*
  /sphenix/lustre01/sphnxpro/commissioning/tpc/beam/TPC_ebdc*_beam-00011000-0000.prdf

Replace the run number to the one you need
The * will make it run over all the sectors of the TPC. If you want just one sector enter the sector number
  Example: TPC_ebdc*_pedestal-00010179-0000.prdf  ---> TPC_ebdc00_pedestal-00010179-0000.prdf

Depending on the number of events you run it will take some time (100 events is about 15 minutes on a good day)

---------------------------------------

Move to the repository you put these files (If you'd like, you can move TPC_Channel_QA.C, SectorMap_Display.C, Efficiency_ModuleDisplay.C,
and Noise_ModuleDisplay.C with you to avoid calling the whole path).

Okay now you need to change the run number in TPC_Channel_QA.C in the following line,
string runNumber = "pedestal-00010179";

 Also change the directory,
  /sphenix/user/rosstom/test/testFiles/TPC_ebdc"+sectorNumber+"_"+runNumber+"-0000.prdf_TPCRawDataTree.root
  *** Make sure to keep TPC_ebdc at the end of the path ***

Example: /sphenix/user/rosstom/test/testFiles/TPC_ebdc"+sectorNumber+"_"+runNumber+"-0000.prdf_TPCRawDataTree.root \

         ---> /sphenix/<username>/<Working Directory>/TPC_ebdc"+sectorNumber+"_"+runNumber+"-0000.prdf_TPCRawDataTree.root

Run TPC_Channel_QA.C with,

    root TPC_Channel_QA.C 

This will take a long time probably to generate the root files.
---------------------------------------
Yay, now there's root files we can build from. Now go into SectorMap_Display.C and we'll make some TNtuples from them.
  
  Change the variable runNumber to whichever run you're looking at and make sure to change the path in filename2 to the correct directory:

   Example: sphenix/u/jamesj3j3/workfest/sPHENIXProjects/outputfile_TPC_ebdc%s_pedestal-00010305.root \

            ---> sphenix/user/<username>/<Working Directory>/outputfile_TPC_ebdc%s_pedestal-00010305.root

  Make sure to change the outfile name to be the correct run number:
  TFile h_outfile10305("pedestal-10179-outfile.root", "RECREATE");

  Save and run. Should be pretty quick.
-----------------------------------------
Okay, now we can do Efficiency_ModuleDisplay.C

  Again change the input with proper run number:
    const TString filename3( Form( "./pedestal-10179-outfile.root") );

  Change the titles of the histograms as needed (dummy_his1 and dummy_his2)
  Change the minimum value of the live fraction percentage to the needed one (ErrCSide and ErrASide)

 Save and run. Will take about 20-30 seconds.
--------------------------------------------
Follow this same procedure for Noise_ModuleDisplay.C
--------------------------------------------
End of process
