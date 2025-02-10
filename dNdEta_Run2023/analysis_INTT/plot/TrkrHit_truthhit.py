#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import *
import numpy
import math
import glob
from plotUtil import *

titlesize = 0.05
labelsize = 0.05
pad1scale = 1.05
pad2scale = 2.6

gROOT.SetBatch(True)

if __name__ == '__main__': 
    hM_PHG4Hit_all_z0 = TH1F("hM_PHG4Hit_all_z0","hM_PHG4Hit_all_z0",500,-25,25)
    hM_TrkrHit_truthhit_z0 = TH1F("hM_TrkrHit_truthhit_z0","hM_TrkrHit_truthhit_z0",500,-25,25)
    
    # os.makedirs('./ClusEtaStripSize/', exist_ok=True)
    
    file = TFile("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/production/Sim_Ntuple_HIJING_ana443_20241102/ntuple_00008.root", "READ")
    # file = TFile("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/macros/testNtuple.root", "READ")
    # file = TFile("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/macros/testNtuple_20241101localtest.root", "READ")
    tree = file.Get("EventTree")
    nentries = tree.GetEntries()
    for ientry in range(nentries):
        tree.GetEntry(ientry)
        for i in range(len(tree.PHG4Hit_z0)):
            hM_PHG4Hit_all_z0.Fill(tree.PHG4Hit_z0[i])
        for i in range(len(tree.TrkrHit_truthHit_z0)):
            hM_TrkrHit_truthhit_z0.Fill(tree.TrkrHit_truthHit_z0[i])
            
    maxbincontent = max(hM_PHG4Hit_all_z0.GetMaximum(), hM_TrkrHit_truthhit_z0.GetMaximum())

    c = TCanvas("c","c",800,600)
    pad1 = TPad( 'pad1', ' ', 0, 0.3, 1, 1)
    pad2 = TPad( 'pad2', ' ', 0, 0, 1, 0.3)
    pad1.SetBottomMargin(0.0)
    pad1.Draw()
    pad2.SetTopMargin(0.0)
    pad2.SetBottomMargin(0.3)
    pad2.Draw()
    pad1.cd()
    hM_PHG4Hit_all_z0.SetLineColor(kBlack)
    hM_PHG4Hit_all_z0.SetLineWidth(2)
    hM_PHG4Hit_all_z0.GetYaxis().SetTitle("Counts")
    hM_PHG4Hit_all_z0.GetYaxis().SetTitleOffset(1.5)
    hM_PHG4Hit_all_z0.GetYaxis().SetRangeUser(0, maxbincontent*1.3)
    hM_PHG4Hit_all_z0.GetYaxis().SetTitleSize(titlesize)
    hM_PHG4Hit_all_z0.GetXaxis().SetTitleSize(titlesize)
    hM_PHG4Hit_all_z0.Draw("hist")
    hM_TrkrHit_truthhit_z0.SetLineColor(kRed+1)
    hM_TrkrHit_truthhit_z0.SetLineWidth(2)
    hM_TrkrHit_truthhit_z0.Draw("hist same")
    print(hM_PHG4Hit_all_z0.Integral(-1,-1), hM_TrkrHit_truthhit_z0.Integral(-1,-1))
    leg = TLegend(0.5, 0.8, 0.9, 0.9)
    leg.AddEntry(hM_PHG4Hit_all_z0, "PHG4Hit (all)", "l")
    leg.AddEntry(hM_TrkrHit_truthhit_z0, "PHG4Hit (TrkrHit matched)", "l")
    leg.SetFillStyle(0)
    leg.SetTextSize(0.05)
    leg.Draw()
    pad2.cd()
    pad2.SetGridy()
    hM_ratio = hM_TrkrHit_truthhit_z0.Clone("hM_ratio")
    hM_ratio.Divide(hM_PHG4Hit_all_z0)
    hM_ratio.SetLineColor(kBlack)
    hM_ratio.SetLineWidth(1)
    hM_ratio.GetXaxis().SetTitle("PHG4Hit z_{0} [cm]")
    hM_ratio.GetYaxis().SetTitle("Ratio")
    hM_ratio.GetYaxis().SetNdivisions(505)
    hM_ratio.GetYaxis().SetTitleSize(titlesize*pad2scale)
    hM_ratio.GetYaxis().SetLabelSize(labelsize*pad2scale)
    hM_ratio.GetYaxis().SetTitleOffset(0.6)
    hM_ratio.GetXaxis().SetTitleSize(titlesize*pad2scale)
    hM_ratio.GetXaxis().SetLabelSize(labelsize*pad2scale)
    hM_ratio.GetXaxis().SetTitleOffset(1.1)
    hM_ratio.GetYaxis().SetRangeUser(0, 1.2)
    hM_ratio.Draw("l")
    pad1.RedrawAxis()
    c.SaveAs("./ClusEtaStripSize/TrkrHit_truthhit_z0.png")
    c.SaveAs("./ClusEtaStripSize/TrkrHit_truthhit_z0.pdf")
    