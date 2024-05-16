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

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)


if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("--datafiledir", dest="datafiledir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/RecoVtx', help="Data file directory")
    parser.add_option("--simfiledir", dest="simfiledir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana413_20240509/RecoVtx', help="Simulation file directory")
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
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.SetName("hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim = GetHistogram("{}/hists_merged.root".format(simfiledir), "hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.SetName("hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim")
    
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Scale(1.0/hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Integral(-1,-1))
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.Scale(1.0/hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.Integral(-1,-1))
    VtxZ_reweight = hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Clone("VtxZ_reweight")
    VtxZ_reweight.Divide(hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim)
    
    c = TCanvas("c", "c", 800, 700)
    c.cd()
    VtxZ_reweight.GetXaxis().SetTitle("INTT Z_{vtx} [cm]")
    VtxZ_reweight.GetYaxis().SetTitle("Data/Simulation")
    VtxZ_reweight.GetYaxis().SetTitleOffset(1.3)
    VtxZ_reweight.GetXaxis().SetRangeUser(-30, -10)
    VtxZ_reweight.GetYaxis().SetRangeUser(0, VtxZ_reweight.GetMaximum()*1.2)
    # VtxZ_reweight.SetMarkerStyle(20)
    # VtxZ_reweight.SetMarkerSize(0.8)
    VtxZ_reweight.SetMarkerColor(kBlack)
    VtxZ_reweight.SetLineColor(kBlack)
    gStyle.SetPaintTextFormat("1.4g");
    VtxZ_reweight.Draw("PE1text")
    c.SaveAs("./{}/VtxZ_reweight.png".format(plotdir))
    c.SaveAs("./{}/VtxZ_reweight.pdf".format(plotdir))
    
    c.Clear()
    c.cd()
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.GetXaxis().SetTitle("INTT Z_{vtx} [cm]")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.GetYaxis().SetTitle("Normalized counts")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.GetYaxis().SetTitleOffset(1.5)
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.GetYaxis().SetRangeUser(0, hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.GetMaximum()*1.2 if hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.GetMaximum() > hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.GetMaximum() else hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.GetMaximum() * 1.2)
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.SetMarkerStyle(20)
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.SetMarkerSize(0.8)
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.SetMarkerColor(kBlack)
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.SetLineColor(kBlack)
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Draw("PE1")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.SetLineColor(TColor.GetColor("#9A031E"))
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.Draw("hist same")
    leg = TLegend(0.67, 0.8, 0.87, 0.9)
    leg.AddEntry(hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data, "Data", "lep")
    leg.AddEntry(hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim, "Simulation", "l")
    leg.SetFillStyle(0)
    leg.Draw("same")
    c.SaveAs("./{}/VtxZ4Reweight_data_sim.png".format(plotdir))
    c.SaveAs("./{}/VtxZ4Reweight_data_sim.pdf".format(plotdir))
    
    f = TFile("./{}/VtxZ_reweight.root".format(plotdir), "recreate")
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_data.Write()
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse_sim.Write()
    VtxZ_reweight.Write()
    f.Close()
    