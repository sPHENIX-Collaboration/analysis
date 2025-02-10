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

def Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, evtseltexts, outname):
    hsimcolor = colorset(len(hsims))

    hdata.Sumw2()
    for hsim in hsims:
        hsim.Sumw2()

    binwidth = hdata.GetXaxis().GetBinWidth(1)

    if norm == 'unity':
        hdata.Scale(1. / hdata.Integral(-1, -1))
        for hsim in hsims:
            hsim.Scale(1. / hsim.Integral(-1, -1))
    elif norm == 'data':
        for hsim in hsims:
            hsim.Scale(hdata.Integral(-1, -1) / hsim.Integral(-1, -1))
    else:
        if norm != 'none':
            print('Invalid normalization option: {}'.format(norm))
            sys.exit(1)
    
    # Get the maximum bin content 
    maxbincontent = max(hdata.GetMaximum(), hsim.GetMaximum())

    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(gpadmargin[0])
    gPad.SetTopMargin(gpadmargin[1])
    gPad.SetLeftMargin(gpadmargin[2])
    gPad.SetBottomMargin(gpadmargin[3])
    
    for i, hsim in enumerate(hsims):
        if i == 0:
            if norm == 'unity':
                if Ytitle_unit == '':
                    hsim.GetYaxis().SetTitle(
                        'Normalized entries / ({:g})'.format(binwidth))
                else:
                    hsim.GetYaxis().SetTitle(
                        'Normalized entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
            else:
                if Ytitle_unit == '':
                    hsim.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
                else:
                    hsim.GetYaxis().SetTitle(
                        'Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))

            if logy:
                hsim.GetYaxis().SetRangeUser(hsim.GetMinimum(0)*0.5, (hsim.GetMaximum()) * ymaxscale)
            else:
                hsim.GetYaxis().SetRangeUser(0., (hsim.GetMaximum()) * ymaxscale)

            hsim.GetXaxis().SetTitle(XaxisName)
            hsim.GetXaxis().SetTitleOffset(1.1)
            hsim.GetYaxis().SetTitleOffset(1.35)
            hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe')
        else:
            hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe same')

    hdata.SetMarkerStyle(20)
    hdata.SetMarkerSize(1)
    hdata.SetMarkerColor(1)
    hdata.SetLineColor(1)
    hdata.SetLineWidth(2)
    hdata.Draw('same PE1')
    shift = 0.45 if prelim else 0.75
    legylow = 0.2 + 0.04 * (len(hsims) - 1)
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    # prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    prelimtext = 'Preliminary' if prelim else 'Internal'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry(hdata, 'Data', "PE1")
    for i, lt in enumerate(simlegtex):
        leg.AddEntry(hsims[i], lt, "le")
    leg.Draw('same')
    
    # event selection text
    legylow_evtselshift = 0.04 * len(evtseltexts)
    leg2 = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow-legylow_evtselshift,
                    (1-RightMargin)-0.1, (1-TopMargin)-legylow)
    leg2.SetTextSize(0.035)
    leg2.SetFillStyle(0)
    for i, evtseltext in enumerate(evtseltexts):
        leg2.AddEntry('', evtseltext, '')
    leg2.Draw('same')
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0


if __name__ == '__main__': 
    plotpath = './ClusEtaAdcPhisize/'
    os.makedirs(plotpath, exist_ok=True)
    
    gStyle.SetPalette(kRainBow)

    datahist = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/Cluster/hists_merged.root'
    simhist_20240528 = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana419_20240528/Cluster/hists_merged.root'
    simhist_20240910 = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana419_20240910/Cluster/hists_merged.root'
    
    hM_ClusEtaPV_all_ClusADCg35_data = GetHistogram(datahist, 'hM_ClusEtaPV_all_ClusADCg35')
    hM_ClusEtaPV_all_ClusADCg35_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_all_ClusADCg35') # with MVTX support
    hM_ClusEtaPV_all_ClusADCg35_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_all_ClusADCg35') # without MVTX support
    hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize_data = GetHistogram(datahist, 'hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize')
    hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize') # with MVTX support
    hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize') # without MVTX support
    
    # take ratio of hM_ClusEtaPV_all_ClusADCg35_sim_20240528 and hM_ClusEtaPV_all_ClusADCg35_sim_20240910
    # first clone two histograms 
    hM_ClusEtaPV_all_ClusADCg35_sim_20240528_clone = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_all_ClusADCg35') # with MVTX support
    hM_ClusEtaPV_all_ClusADCg35_sim_20240910_clone = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_all_ClusADCg35') # without MVTX support
    hM_ClusEtaPV_all_ClusADCg35_sim_20240528_clone.Scale(1./hM_ClusEtaPV_all_ClusADCg35_sim_20240528_clone.Integral(-1, -1))
    hM_ClusEtaPV_all_ClusADCg35_sim_20240910_clone.Scale(1./hM_ClusEtaPV_all_ClusADCg35_sim_20240910_clone.Integral(-1, -1))
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport = hM_ClusEtaPV_all_ClusADCg35_sim_20240910_clone.Clone('hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport')
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.Divide(hM_ClusEtaPV_all_ClusADCg35_sim_20240528_clone)
    c = TCanvas('c', 'c', 800, 700)
    c.cd()
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.GetXaxis().SetTitle('Cluster #eta')
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.GetYaxis().SetTitle('w.o MVTX support / w. MVTX support')
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.GetYaxis().SetTitleOffset(1.3)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.GetYaxis().SetRangeUser(0.8, 1.2)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.SetMarkerStyle(20)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.SetMarkerSize(0.7)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.SetMarkerColor(1)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.SetLineColor(1)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.SetLineWidth(2)
    hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.Draw('PE1')
    line = TLine(hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.GetXaxis().GetXmin(), 1, hM_ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.GetXaxis().GetXmax(), 1)
    line.SetLineColor(kRed)
    line.SetLineStyle(2)
    line.Draw()
    c.SaveAs(plotpath+'ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.pdf')
    c.SaveAs(plotpath+'ratio_ClusEtaPV_all_ClusADCg35_MVTXsupport.png')
    
    
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data = hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize_data.Clone('hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data')
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528 = hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240528.Clone('hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528')
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910 = hM_ClusEtaPV_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240910.Clone('hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910')
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data.Divide(hM_ClusEtaPV_all_ClusADCg35_data)
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528.Divide(hM_ClusEtaPV_all_ClusADCg35_sim_20240528)
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910.Divide(hM_ClusEtaPV_all_ClusADCg35_sim_20240910)
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data.GetYaxis().SetRangeUser(0, hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data.GetBinContent(hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data.GetMaximumBin())*1.3)
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528.GetYaxis().SetRangeUser(0, hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528.GetBinContent(hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528.GetMaximumBin())*1.3)
    hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910.GetYaxis().SetRangeUser(0, hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910.GetBinContent(hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910.GetMaximumBin())*1.3)
    
    Draw_1Dhist_datasimcomp(hM_ClusEtaPV_all_ClusADCg35_data, [hM_ClusEtaPV_all_ClusADCg35_sim_20240528, hM_ClusEtaPV_all_ClusADCg35_sim_20240910], [0.05, 0.05, 0.15, 0.15], 'unity', False, 1.8, 'Cluster #eta', '', False, ['HIJING (w. MVTX support)', 'HIJING (w.o MVTX support)'], ['Cluster ADC > 35'], plotpath+'ClusEtaPV_all_ClusADCg35')
    Draw_1Dhist_datasimcomp(hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_data, [hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240528, hM_ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize_sim_20240910], [0.05, 0.05, 0.15, 0.15], 'none', False, 1.5, 'Cluster #eta', '', False, ['HIJING (w. MVTX support)', 'HIJING (w.o MVTX support)'], ['Cluster ADC > 35'], plotpath+'ClusEtaPV_all_ClusADCg35_avgclusadcoverphisize')
    
    hM_ClusEtaPV_ClusADC_all_data = GetHistogram(datahist, 'hM_ClusEtaPV_ClusADC_all_zoomin')
    hM_ClusEtaPV_ClusADC_all_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_ClusADC_all_zoomin')
    hM_ClusEtaPV_ClusADC_all_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_ClusADC_all_zoomin')

    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname)
    Draw_2Dhist(hM_ClusEtaPV_ClusADC_all_data, True, False, True, 0.15, 'Cluster #eta', 'Cluster ADC', 'Normalized counts', 'colz', plotpath+'ClusEtaPV_ClusADC_all_data')
    Draw_2Dhist(hM_ClusEtaPV_ClusADC_all_sim_20240528, False, False, True, 0.15, 'Cluster #eta', 'Cluster ADC', 'Normalized counts', 'colz', plotpath+'ClusEtaPV_ClusADC_all_sim_20240528')
    Draw_2Dhist(hM_ClusEtaPV_ClusADC_all_sim_20240910, False, False, True, 0.15, 'Cluster #eta', 'Cluster ADC', 'Normalized counts', 'colz', plotpath+'ClusEtaPV_ClusADC_all_sim_20240910')

    hM_ClusEtaPV_ClusADC_all_data.Scale(1./hM_ClusEtaPV_ClusADC_all_data.GetEntries())
    hM_ClusEtaPV_ClusADC_all_sim_20240528.Scale(1./hM_ClusEtaPV_ClusADC_all_sim_20240528.GetEntries())
    hM_ClusEtaPV_ClusADC_all_sim_20240910.Scale(1./hM_ClusEtaPV_ClusADC_all_sim_20240910.GetEntries())
    hM_ClusEtaPV_ClusADC_all_ratio = hM_ClusEtaPV_ClusADC_all_data.Clone('hM_ClusEtaPV_ClusADC_all_ratio')
    hM_ClusEtaPV_ClusADC_all_ratio.Divide(hM_ClusEtaPV_ClusADC_all_sim_20240528)
    hM_ClusEtaPV_ClusADC_all_ratio.GetZaxis().SetRangeUser(0, 2)
    Draw_2Dhist(hM_ClusEtaPV_ClusADC_all_ratio, True, False, False, 0.15, 'Cluster #eta', 'Cluster ADC', 'Data/Simulation (w. MVTX support)', 'colz', plotpath+'ClusEtaPV_ClusADC_all_datatosim')
    
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data = GetHistogram(datahist, 'hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35')
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35')
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35')
    # project to y axis
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy = hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data.ProjectionY('hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy')
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528_projy = hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528.ProjectionY('hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528_projy')
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910_projy = hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910.ProjectionY('hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910_projy')
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy.Scale(1./hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy.GetEntries())
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528_projy.Scale(1./hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528_projy.GetEntries())
    hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910_projy.Scale(1./hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910_projy.GetEntries())
    Draw_1Dhist_datasimcomp(hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy, [hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240528_projy, hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_sim_20240910_projy], [0.05, 0.05, 0.15, 0.15], 'none', False, 1.3, 'Cluster ADC/Cluster #phi size', '', False, ['HIJING (w. MVTX support)', 'HIJING (w.o MVTX support)'], ['Cluster ADC > 35'], plotpath+'ClusEtaPV_ClusADCoverClusPhiSize_projy_all_ClusADCg35')
    # for hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy, print out the bin range and bin content of each bin
    for i in range(1, hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy.GetNbinsX()+1):
        print('bin {}: [{}, {}], content: {:.5f}'.format(i, hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy.GetXaxis().GetBinLowEdge(i), hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy.GetXaxis().GetBinUpEdge(i), hM_ClusEtaPV_ClusADCoverClusPhiSize_all_ClusADCg35_data_projy.GetBinContent(i)))
    
    hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut_data = GetHistogram(datahist, 'hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut')
    hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut')
    hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut')
    Draw_1Dhist_datasimcomp(hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut_data, [hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut_sim_20240528, hM_ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut_sim_20240910], [0.05, 0.05, 0.15, 0.15], 'unity', False, 1.8, 'Cluster #eta', '', False, ['HIJING (w. MVTX support)', 'HIJING (w.o MVTX support)'], ['Cluster ADC > 35','Cluster ADC/#phi size != ([79,82] & [111,114])'], plotpath+'ClusEtaPV_all_ClusADCg35_ClusADCoverPhisizeCut')
    
    datahist_tkl = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/hists_merged.root'
    simhist_20240528_tkl = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana419_20240528/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/hists_merged.root'
    simhist_20240910_tkl = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana419_20240910/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/hists_merged.root'
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_data = GetHistogram(datahist_tkl, 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240528 = GetHistogram(simhist_20240528_tkl, 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240910 = GetHistogram(simhist_20240910_tkl, 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_data, [hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240528, hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240910], [0.05, 0.05, 0.15, 0.15], 'unity', False, 1.8, 'Tracklet #eta', '', False, ['HIJING (w. MVTX support)', 'HIJING (w.o MVTX support)'], ['Centrality 0-70%', 'MBD asymmetry < 0.75', '-30#leqZ_{vtx}#leq-10[cm]'], plotpath+'Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240528_clone = GetHistogram(simhist_20240528_tkl, 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240910_clone = GetHistogram(simhist_20240910_tkl, 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240528_clone.Scale(1./hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240528_clone.Integral(-1, -1))
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240910_clone.Scale(1./hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240910_clone.Integral(-1, -1))
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport = hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240910_clone.Clone('hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport')
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.Divide(hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim_20240528_clone)
    c = TCanvas('c', 'c', 800, 700)
    c.cd()
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.GetXaxis().SetTitle('Tracklet #eta')
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.GetYaxis().SetTitle('w.o MVTX support / w. MVTX support')
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.GetYaxis().SetTitleOffset(1.3)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.GetYaxis().SetRangeUser(0.8, 1.2)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.SetMarkerStyle(20)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.SetMarkerSize(0.7)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.SetMarkerColor(1)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.SetLineColor(1)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.SetLineWidth(2)
    hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.Draw('PE1')
    line = TLine(hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.GetXaxis().GetXmin(), 1, hM_ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.GetXaxis().GetXmax(), 1)
    line.SetLineColor(kRed)
    line.SetLineStyle(2)
    line.Draw()
    c.SaveAs(plotpath+'ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.pdf')
    c.SaveAs(plotpath+'ratio_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_MVTXsupport.png')
    
    # TH2F *hM_ClusEtaPV_PVz_all_ClusADCg35 = new TH2F("hM_ClusEtaPV_PVz_all_ClusADCg35", "hM_ClusEtaPV_PVz_all_ClusADCg35", 160, -4, 4, 160, -40, 0);
    # TH2F *hM_ClusEtaPV_PVz_layer1_ClusADCg35 = new TH2F("hM_ClusEtaPV_PVz_layer1_ClusADCg35", "hM_ClusEtaPV_PVz_layer1_ClusADCg35", 160, -4, 4, 160, -40, 0);
    # TH2F *hM_ClusEtaPV_PVz_layer2_ClusADCg35 = new TH2F("hM_ClusEtaPV_PVz_layer2_ClusADCg35", "hM_ClusEtaPV_PVz_layer2_ClusADCg35", 160, -4, 4, 160, -40, 0);
    # TH2F *hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize = new TH2F("hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize", "hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize", 160, -4, 4, 160, -40, 0);
    # TH2F *hM_ClusEtaPV_PVz_layer1_ClusADCg35_weiClusADCoverPhiSize = new TH2F("hM_ClusEtaPV_PVz_layer1_ClusADCg35_weiClusADCoverPhiSize", "hM_ClusEtaPV_PVz_layer1_ClusADCg35_weiClusADCoverPhiSize", 160, -4, 4, 160, -40, 0);
    # TH2F *hM_ClusEtaPV_PVz_layer2_ClusADCg35_weiClusADCoverPhiSize = new TH2F("hM_ClusEtaPV_PVz_layer2_ClusADCg35_weiClusADCoverPhiSize", "hM_ClusEtaPV_PVz_layer2_ClusADCg35_weiClusADCoverPhiSize", 160, -4, 4, 160, -40, 0);
    hM_ClusEtaPV_PVz_all_ClusADCg35_data = GetHistogram(datahist, 'hM_ClusEtaPV_PVz_all_ClusADCg35')
    hM_ClusEtaPV_PVz_all_ClusADCg35_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_PVz_all_ClusADCg35')
    # hM_ClusEtaPV_PVz_all_ClusADCg35_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_PVz_all_ClusADCg35')
    hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_data = GetHistogram(datahist, 'hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize')
    hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240528 = GetHistogram(simhist_20240528, 'hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize')
    # hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240910 = GetHistogram(simhist_20240910, 'hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize')
    hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_data.Divide(hM_ClusEtaPV_PVz_all_ClusADCg35_data)
    hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240528.Divide(hM_ClusEtaPV_PVz_all_ClusADCg35_sim_20240528)
    # hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240910.Divide(hM_ClusEtaPV_PVz_all_ClusADCg35_sim_20240910)
    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname)
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_data, True, False, False, 0.15, 'Cluster #eta', 'PV_{z} [cm]', 'Cluster ADC/#phi size', 'colz', plotpath+'ClusEtaPV_PVz_all_ClusADCg35_avgClusADCoverPhiSize')
    Draw_2Dhist(hM_ClusEtaPV_PVz_all_ClusADCg35_weiClusADCoverPhiSize_sim_20240528, False, False, False, 0.15, 'Cluster #eta', 'PV_{z} [cm]', 'Cluster ADC/#phi size', 'colz', plotpath+'ClusEtaPV_PVz_all_ClusADCg35_avgClusADCoverPhiSize_sim_20240528')
    # Draw_2Dhist(hM_ClusEtaPV_PVz_all_ClusADCg35_sim_20240910, False, False, True, 0.15, 'Cluster #eta', 'PV_z [cm]', 'Cluster ADC/#phi size', 'colz', plotpath+'ClusEtaPV_PVz_all_ClusADCg35_sim_20240910')