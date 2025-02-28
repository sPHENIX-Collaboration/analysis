from ROOT import *
import sys

# note : the function to merge the trees
# note : it won't work if you simply take this function out to other code. As far as I know, the input has to be the clone/copied tree
def merge_trees(outputfile, tree1, tree2) :
    outputfile.cd()

    for branch in tree2.GetListOfBranches() :
        branch.SetTree(tree1)
        tree1.GetListOfBranches().Add(branch)
        tree1.GetListOfLeaves().Add(branch.GetLeaf(branch.GetName()))
        tree2.GetListOfBranches().Remove(branch)
    tree1.Write("", TObject.kOverwrite)

def sync_mbd_intt_DST(outputfname, f_mbd_name, t_mbd_name, f_intt_name, t_intt_name, Nevent):
    
    # note : read tree from mbd
    f_mbd = TFile.Open(f_mbd_name)
    t_mbd = f_mbd.Get(t_mbd_name)
    t_mbd_Nevt = t_mbd.GetEntries() # note : N event inf MBD
    print("-->", t_mbd, " Entry:", t_mbd_Nevt)
    
    # note : read tree from intt
    f_intt = TFile.Open(f_intt_name)
    t_intt = f_intt.Get(t_intt_name)
    t_intt_Nevt = t_intt.GetEntries() # note : N event in INTT
    print("-->", t_intt, " Entry:", t_intt_Nevt)
    
    # note : create output file
    outfile = TFile.Open(outputfname, 'recreate')
    # note : clone the two trees from mbd and intt
    # note : they should be empty now
    t_intt_clone = t_intt.CloneTree(0)
    t_mbd_clone = t_mbd.CloneTree(0)

    # note : check the number of events to be processed
    if (Nevent > 0 and Nevent < t_mbd_Nevt):
        run_event = Nevent
    else:
        print("--> the requested event number is out of range, or not specified, use the full range of events in MBD tree.")
        run_event = t_mbd_Nevt

    # note : draw the correlation between the MBD charge sum and the INTT number of cluster
    # note : decided not to use it as it requires additional branches, I want to make it inclusive
    # note : which means I only require the two clock information from both trees of MBD and INTT
    # h_qmbd_nintt = TH2F("h_qmbd_nintt", "BbcQ vs Intt N", 200, 0, 20000, 200, 0, 4000)
    # h_qmbd_nintt.GetXaxis().SetTitle("NClus")
    # h_qmbd_nintt.GetYaxis().SetTitle("MBD Q")

    # note : TH2F to show the clock difference as a function of event
    # note : if the two trees are synchronized, the clock difference should be constant
    intt_mbd_bco_presync = TH2F("intt_mbd_bco_presync", "INTT - MBD", 100, 0, run_event * 1.5, 100, -10, 100000)
    intt_mbd_bco_presync.GetXaxis().SetTitle("evt")
    intt_mbd_bco_presync.GetYaxis().SetTitle("clock_diff")
    
    # note : TH1F to show the 1D projection of the previous TH2F 
    intt_mbd_bco_presync_1D = TH1F("intt_mbd_bco_presync_1D", "INTT - MBD", 100, -10, 100000)
    intt_mbd_bco_presync_1D.GetXaxis().SetTitle("clock_diff")
    intt_mbd_bco_presync_1D.GetYaxis().SetTitle("entry")
    
    intt_mbd_bco_postsync = TH2F("intt_mbd_bco_postsync", "INTT - MBD", 100, 0, run_event * 1.5, 100, -10, 100000)
    intt_mbd_bco_postsync.GetXaxis().SetTitle("evt")
    intt_mbd_bco_postsync.GetYaxis().SetTitle("clock_diff")
    
    intt_mbd_bco_postsync_1D = TH1F("intt_mbd_bco_postsync_1D", "INTT - MBD", 100, -10, 100000)
    intt_mbd_bco_postsync_1D.GetXaxis().SetTitle("clock_diff")
    intt_mbd_bco_postsync_1D.GetYaxis().SetTitle("entry")

    # note : the variables used in the loop
    prev_mbdclk = 0
    prev_inttbcofull = 0
    import sys

    mbd_evt_offset = 0
    intt_evt_offset = 0

    print("--> start to synchronize the events")
    for entry in range(run_event):
        t_mbd.GetEntry(entry + mbd_evt_offset)
        t_intt.GetEntry(entry + intt_evt_offset)

        mbdclk = t_mbd.femclk
        inttbcofull = t_intt.INTT_BCO
        inttbco16bit = inttbcofull & 0xFFFF
        mbd_prvdiff = (mbdclk - prev_mbdclk) & 0xFFFF
        intt_prvdiff = inttbcofull - prev_inttbcofull

        if (entry % 1000) == 0:
            print(entry, hex(mbdclk), hex(inttbco16bit), "(mbd-intt)", hex((mbdclk - inttbco16bit) & 0xFFFF), "(MBD, femclk-prev)", hex(mbd_prvdiff), "(INTT, bco-prev)", hex(intt_prvdiff))

        intt_mbd_bco_presync.Fill(entry, (mbdclk - inttbco16bit) & 0xFFFF)
        intt_mbd_bco_presync_1D.Fill((mbdclk - inttbco16bit) & 0xFFFF)

        prev_mbdclk = mbdclk
        prev_inttbcofull = inttbcofull
        
        is_sync = False
            
        while entry + 1 + intt_evt_offset < t_intt_Nevt: 
            t_intt.GetEntry(entry + 1 + intt_evt_offset)
            next_inttbco16bit = t_intt.INTT_BCO & 0xFFFF
            
            t_mbd.GetEntry(entry + 1)
            next_mbdclk = t_mbd.femclk
            
            if ((next_mbdclk - next_inttbco16bit) & 0xFFFF) != ((mbdclk - inttbco16bit) & 0xFFFF):
                intt_evt_offset += 1
            else: # break the while loop
                is_sync = True
                break
            
        if is_sync:
            intt_mbd_bco_postsync.Fill(entry, (mbdclk - inttbco16bit) & 0xFFFF)
            intt_mbd_bco_postsync_1D.Fill((mbdclk - inttbco16bit) & 0xFFFF)
            t_intt_clone.Fill() # only fill when synchronized
            t_mbd_clone.Fill()
        

    print("--> INTT Nevent post-sync: ",t_intt_clone.GetEntries())
    print("--> MBD Nevent post-sync: ",t_mbd_clone.GetEntries())

    merge_trees(outfile, t_intt_clone, t_mbd_clone)

    intt_mbd_bco_presync.Write("", TObject.kOverwrite)
    intt_mbd_bco_presync_1D.Write("", TObject.kOverwrite)
    intt_mbd_bco_postsync.Write("", TObject.kOverwrite)
    intt_mbd_bco_postsync_1D.Write("", TObject.kOverwrite)
    f_intt.Close()
    f_mbd.Close()
    outfile.Close()

    print("--> Synchronization done")
    print("--> Merged file:", outputfname, ", Tree name: ", t_intt_name)
    print("--> TH2F plot: intt_mbd_bco_presync, can be checked in the merged file")
    print("--> TH1F plot: intt_mbd_bco_presync_1D, can be checked in the merged file")




if __name__ == '__main__' :
    import sys
    args = sys.argv[1:]

    Nevent = -200

    if len(args) < 5 or len(args) > 6:
        print("--> No input or wrong arguments!")
        print("--> Usage: python3 python3_merge_test1.py <outfile_full_directory> <intt_file_full_directory> <intt_tree_name> <mbd_file_in_full_directory> <mbd_tree_name> <N event (optional)>")
        print("--> The file tree name will be intt tree name")
        sys.exit(1)

    outputfname = args[0]
    print("--> Outputfname:", outputfname)    
    
    f_intt = args[1]
    t_intt = args[2]
    print("--> Direcotry of INTT file: ", f_intt)
    print("--> INTT tree name: ", t_intt)

    f_mbd = args[3]
    t_mbd = args[4]
    print("--> Direcotry of MBD file: ", f_mbd)
    print("--> MBD tree name: ", t_mbd)

    if len(args) == 6:
        Nevent = int(args[5])
        
    gBenchmark.Start('InttMbdEvtCombiner')
    
    sync_mbd_intt_DST(outputfname, f_mbd, t_mbd, f_intt, t_intt, Nevent)
    
    gBenchmark.Show('InttMbdEvtCombiner')


# Nevent = 10000
# f_mbd = "/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/centrality_run20869.root"
# t_mbd = "EventTree"
# f_intt = "/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/intt_run20869.root"
# t_intt = "EventTree"
# outputfname = "python3_merge_test1.root"

# sync_mbd_intt_DST(outputfname, f_mbd, t_mbd, f_intt, t_intt)
    