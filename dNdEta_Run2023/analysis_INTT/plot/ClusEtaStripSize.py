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

gROOT.SetBatch(True)

if __name__ == '__main__': 
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('-d', '--datahistdir', dest='datahistdir', type='string', default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/data_run20869/', help='Histogram file name (data)')
    parser.add_option('-s', '--simhistdir', dest='simhistdir', type='string', help='Histogram file name (simulation). Example: /sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/ana382_zvtx-20cm_dummyAlignParams/Hists_RecoTracklets_merged.root')
    parser.add_option('-l', '--simlegtext', action='append', dest='simlegtext', type='string', help='Legend text for simulation. Example: HIJING/EPOS/AMPT)')

    (opt, args) = parser.parse_args()

    print('opt: {}'.format(opt))

    datahistdir = opt.datahistdir
    simhistdir = opt.simhistdir
    simlegtext = opt.simlegtext
    
    if os.path.isfile("{}/hists_merged.root".format(datahistdir)):
        os.system("rm {}/hists_merged.root".format(datahistdir))
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(datahistdir, datahistdir))
    else:
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(datahistdir, datahistdir))
    
    if os.path.isfile("{}/hists_merged.root".format(simhistdir)):
        os.system("rm {}/hists_merged.root".format(simhistdir))
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(simhistdir, simhistdir))
    else:
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(simhistdir, simhistdir))

    os.makedirs('./ClusEtaStripSize/', exist_ok=True)
    
    hM_ClusEtaPV_1p6cmstrip_all_data = GetHistogram("{}/hists_merged.root".format(datahistdir), "hM_ClusEtaPV_1p6cmstrip_all")
    hM_ClusEtaPV_2cmstrip_all_data = GetHistogram("{}/hists_merged.root".format(datahistdir), "hM_ClusEtaPV_2cmstrip_all")
    hM_ClusEtaPV_1p6cmstrip_all_sim = GetHistogram("{}/hists_merged.root".format(simhistdir), "hM_ClusEtaPV_1p6cmstrip_all")
    hM_ClusEtaPV_2cmstrip_all_sim = GetHistogram("{}/hists_merged.root".format(simhistdir), "hM_ClusEtaPV_2cmstrip_all")
    # 2D 
    hM_ClusEtaPV_PVz_all_LadderZID0_data = GetHistogram("{}/hists_merged.root".format(datahistdir), "hM_ClusEtaPV_PVz_all_LadderZID0")
    hM_ClusEtaPV_PVz_all_LadderZID1_data = GetHistogram("{}/hists_merged.root".format(datahistdir), "hM_ClusEtaPV_PVz_all_LadderZID1")
    hM_ClusEtaPV_PVz_all_LadderZID2_data = GetHistogram("{}/hists_merged.root".format(datahistdir), "hM_ClusEtaPV_PVz_all_LadderZID2")
    hM_ClusEtaPV_PVz_all_LadderZID3_data = GetHistogram("{}/hists_merged.root".format(datahistdir), "hM_ClusEtaPV_PVz_all_LadderZID3")
    hM_ClusEtaPV_PVz_all_LadderZID0_sim = GetHistogram("{}/hists_merged.root".format(simhistdir), "hM_ClusEtaPV_PVz_all_LadderZID0")
    hM_ClusEtaPV_PVz_all_LadderZID1_sim = GetHistogram("{}/hists_merged.root".format(simhistdir), "hM_ClusEtaPV_PVz_all_LadderZID1")
    hM_ClusEtaPV_PVz_all_LadderZID2_sim = GetHistogram("{}/hists_merged.root".format(simhistdir), "hM_ClusEtaPV_PVz_all_LadderZID2")
    hM_ClusEtaPV_PVz_all_LadderZID3_sim = GetHistogram("{}/hists_merged.root".format(simhistdir), "hM_ClusEtaPV_PVz_all_LadderZID3")
    
    hM_ClusEtaPV_all_data = hM_ClusEtaPV_1p6cmstrip_all_data.Clone("hM_ClusEtaPV_all_data")
    hM_ClusEtaPV_all_data.Add(hM_ClusEtaPV_2cmstrip_all_data)
    hM_ClusEtaPV_all_sim = hM_ClusEtaPV_1p6cmstrip_all_sim.Clone("hM_ClusEtaPV_all_sim")
    hM_ClusEtaPV_all_sim.Add(hM_ClusEtaPV_2cmstrip_all_sim)
    normfactor = hM_ClusEtaPV_all_data.Integral(-1,-1)/hM_ClusEtaPV_all_sim.Integral(-1,-1)
    # normalize the simulation histograms to data
    hM_ClusEtaPV_all_sim.Scale(normfactor)
    hM_ClusEtaPV_1p6cmstrip_all_sim.Scale(normfactor)
    hM_ClusEtaPV_2cmstrip_all_sim.Scale(normfactor)
    maxbincontent = max(hM_ClusEtaPV_all_data.GetMaximum(), hM_ClusEtaPV_all_sim.GetMaximum())
    minbincontent = min(hM_ClusEtaPV_all_data.GetMinimum(0), hM_ClusEtaPV_all_sim.GetMinimum(0))
    
    c = TCanvas("c", "c", 800, 600)
    c.SetLeftMargin(0.15)
    c.SetRightMargin(0.08)
    c.SetTopMargin(0.08)
    c.SetBottomMargin(0.15)
    hM_ClusEtaPV_all_sim.GetXaxis().SetTitle("Cluster #eta (w.r.t. PV)")
    hM_ClusEtaPV_all_sim.GetYaxis().SetTitle("Normalized counts")
    hM_ClusEtaPV_all_sim.GetYaxis().SetRangeUser(0, maxbincontent*1.9)
    hM_ClusEtaPV_all_sim.SetLineColor(kRed+1)
    hM_ClusEtaPV_all_sim.SetLineWidth(2)
    hM_ClusEtaPV_all_sim.Draw("hist")
    hM_ClusEtaPV_1p6cmstrip_all_sim.SetLineColor(kRed-7)
    hM_ClusEtaPV_1p6cmstrip_all_sim.SetFillColorAlpha(kRed-7, 0.3)
    hM_ClusEtaPV_1p6cmstrip_all_sim.SetLineWidth(2)
    # hM_ClusEtaPV_1p6cmstrip_all_sim.SetLineStyle(kDashed)
    hM_ClusEtaPV_1p6cmstrip_all_sim.Draw("hist SAME")
    hM_ClusEtaPV_2cmstrip_all_sim.SetLineColor(kRed-6)
    hM_ClusEtaPV_2cmstrip_all_sim.SetFillColorAlpha(kRed-6, 0.5)
    hM_ClusEtaPV_2cmstrip_all_sim.SetLineWidth(2)
    # hM_ClusEtaPV_2cmstrip_all_sim.SetLineStyle(kDotted)
    hM_ClusEtaPV_2cmstrip_all_sim.Draw("hist SAME")
    hM_ClusEtaPV_all_data.SetLineColor(kBlack)
    hM_ClusEtaPV_all_data.SetMarkerColor(kBlack)
    hM_ClusEtaPV_all_data.SetMarkerStyle(20)
    hM_ClusEtaPV_all_data.SetMarkerSize(0.8)
    hM_ClusEtaPV_all_data.SetLineWidth(2)
    hM_ClusEtaPV_all_data.Draw("PE same")
    hM_ClusEtaPV_1p6cmstrip_all_data.SetLineColor(kGray+2)
    hM_ClusEtaPV_1p6cmstrip_all_data.SetMarkerColor(kGray+2)
    hM_ClusEtaPV_1p6cmstrip_all_data.SetMarkerStyle(21)
    hM_ClusEtaPV_1p6cmstrip_all_data.SetMarkerSize(0.8)
    hM_ClusEtaPV_1p6cmstrip_all_data.SetLineWidth(2)
    hM_ClusEtaPV_1p6cmstrip_all_data.Draw("PE same")
    hM_ClusEtaPV_2cmstrip_all_data.SetLineColor(kGray+2)
    hM_ClusEtaPV_2cmstrip_all_data.SetMarkerColor(kGray+2)
    hM_ClusEtaPV_2cmstrip_all_data.SetMarkerStyle(25)
    hM_ClusEtaPV_2cmstrip_all_data.SetMarkerSize(0.8)
    hM_ClusEtaPV_2cmstrip_all_data.SetLineWidth(2)
    hM_ClusEtaPV_2cmstrip_all_data.Draw("PE SAME")
    c.RedrawAxis()
    leg = TLegend(0.2, 0.66, 0.5, 0.88)
    leg.AddEntry(hM_ClusEtaPV_all_data, "Data (1.6 cm + 2 cm strip)", "ep")
    leg.AddEntry(hM_ClusEtaPV_1p6cmstrip_all_data, "Data (1.6 cm strip)", "ep")
    leg.AddEntry(hM_ClusEtaPV_2cmstrip_all_data, "Data (2 cm strip)", "ep")
    leg.AddEntry(hM_ClusEtaPV_all_sim, "Simulation (1.6 cm + 2 cm strip)", "lf")
    leg.AddEntry(hM_ClusEtaPV_1p6cmstrip_all_sim, "Simulation (1.6 cm strip)", "lf")
    leg.AddEntry(hM_ClusEtaPV_2cmstrip_all_sim, "Simulation (2 cm strip)", "lf")
    leg.SetTextSize(0.04)
    leg.Draw()
    str_clussel = ''
    str_zvtxsel = '|INTT Z_{vtx}|#leq10[cm]'
    str_evtsel = 'Trigger bit 13 (MBD N&S#geq 2, |MBD Z_{vtx}|<30[cm])'
    evtseltexts = [str_zvtxsel,str_evtsel]
    legylow_evtselshift = 0.05 * len(evtseltexts)
    leg2 = TLegend(0.2, 0.66-legylow_evtselshift, 0.5, 0.66)
    leg2.SetTextSize(0.035)
    leg2.SetFillStyle(0)
    for i, evtseltext in enumerate(evtseltexts):
        leg2.AddEntry('', evtseltext, '')
    leg2.Draw('same')
    c.RedrawAxis()
    c.SaveAs("ClusEtaStripSize/ClusEtaPV_LadderZId_all.pdf")
    c.SaveAs("ClusEtaStripSize/ClusEtaPV_LadderZId_all.png")   
    
    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname)
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID0_data, True, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID0_all_data")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID1_data, True, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID1_all_data")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID2_data, True, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID2_all_data")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID3_data, True, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID3_all_data")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID0_sim, False, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID0_all_sim")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID1_sim, False, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID1_all_sim")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID2_sim, False, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID2_all_sim")
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_LadderZID3_sim, False, False, True, 0.15, "Cluster #eta (w.r.t. PV)", "INTT Z_{vtx} [cm]", "Normalized counts", "colz", "ClusEtaStripSize/ClusEtaPV_PVz_LadderZID3_all_sim")