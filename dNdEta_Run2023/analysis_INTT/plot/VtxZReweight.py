#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import *
import numpy as np
import math
import glob
from plotUtil import GetHistogram

LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)


if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("--datafiledir", dest="datafiledir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/RecoVtx', help="Data file directory")
    parser.add_option("--simfiledir", dest="simfiledir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_new_20240424/RecoVtx', help="Simulation file directory")
    parser.add_option('--plotdir', dest='plotdir', type='string', default='RecoPV_ana', help='Plot directory')

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 
    
    datafiledir = opt.datafiledir
    simfiledir = opt.simfiledir
    plotdir = opt.plotdir
    os.makedirs('./{}'.format(plotdir), exist_ok=True)
    
    if os.path.isfile("{}/hists_merged.root".format(datafiledir)):
        os.system("rm {}/hists_merged.root".format(datafiledir))
        os.system("hadd {}/hists_merged.root {}/hists_00*.root".format(datafiledir, datafiledir))
    else:
        os.system("hadd {}/hists_merged.root {}/hists_00*.root".format(datafiledir, datafiledir))
        
    if os.path.isfile("{}/hists_merged.root".format(simfiledir)):
        os.system("rm {}/hists_merged.root".format(simfiledir))
        os.system("hadd {}/hists_merged.root {}/hists_00*.root".format(simfiledir, simfiledir))
    else:
        os.system("hadd {}/hists_merged.root {}/hists_00*.root".format(simfiledir, simfiledir))
        
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data = GetHistogram("{}/hists_merged.root".format(datafiledir), "hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim = GetHistogram("{}/hists_merged.root".format(simfiledir), "hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse")
    
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Scale(1.0/hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Integral(-1,-1))
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.Scale(1.0/hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.Integral(-1,-1))
    VtxZ_reweight = hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Clone("VtxZ_reweight")
    VtxZ_reweight.Divide(hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim)
    
    c = TCanvas("c", "c", 800, 700)
    c.cd()
    VtxZ_reweight.GetXaxis().SetTitle("Vertex Z [cm]")
    VtxZ_reweight.GetYaxis().SetTitle("Data/Simulation")
    VtxZ_reweight.GetYaxis().SetTitleOffset(1.3)
    # VtxZ_reweight.GetXaxis().SetRangeUser(-30, 30)
    VtxZ_reweight.GetYaxis().SetRangeUser(0, VtxZ_reweight.GetMaximum()*1.2)
    # VtxZ_reweight.SetMarkerStyle(20)
    # VtxZ_reweight.SetMarkerSize(0.8)
    VtxZ_reweight.SetMarkerColor(kBlack)
    VtxZ_reweight.SetLineColor(kBlack)
    VtxZ_reweight.Draw("PE1")
    c.SaveAs("./{}/VtxZ_reweight.png".format(plotdir))
    c.SaveAs("./{}/VtxZ_reweight.pdf".format(plotdir))
    
    f = TFile("./{}/VtxZ_reweight.root".format(plotdir), "recreate")
    VtxZ_reweight.Write()
    f.Close()
    