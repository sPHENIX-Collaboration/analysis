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

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

def Draw_1Dhist_datasimcomp(hdatas, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, evtseltexts, outname):
    if len(hdatas) != len(hsims):
        print('Number of data histograms and simulation histograms do not match')
        sys.exit(1)
        
    hsimcolor = colorset(len(hsims))

    for hdata in hdatas:
        hdata.Sumw2()
    for hsim in hsims:
        hsim.Sumw2()

    binwidth = hdatas[0].GetXaxis().GetBinWidth(1)

    if norm == 'unity':
        for hdata in hdatas:
            hdata.Scale(1. / hdata.Integral(-1, -1))
        for hsim in hsims:
            hsim.Scale(1. / hsim.Integral(-1, -1))
    elif norm == 'data':
        for i, hsim in enumerate(hsims):
            hsim.Scale(hdatas[i].Integral(-1, -1) / hsim.Integral(-1, -1))
    else:
        if norm != 'none':
            print('Invalid normalization option: {}'.format(norm))
            sys.exit(1)
    
    # Get the maximum bin content 
    maxbincontent = max([hdata.GetMaximum() for hdata in hdatas] + [hsim.GetMaximum() for hsim in hsims])

    c = TCanvas('c', 'c', 800, 700)
    pad1 = TPad( 'pad1', ' ', 0, 0.3, 1, 1)
    pad2 = TPad( 'pad2', ' ', 0, 0, 1, 0.3)
    pad1.SetRightMargin(gpadmargin[0])
    pad1.SetTopMargin(gpadmargin[1])
    pad1.SetLeftMargin(gpadmargin[2])
    pad1.SetBottomMargin(0.0)
    pad1.Draw()
    pad2.SetRightMargin(gpadmargin[0])
    pad2.SetTopMargin(0.0)
    pad2.SetLeftMargin(gpadmargin[2])
    pad2.SetBottomMargin(gpadmargin[3])
    pad2.Draw() # Draw the TPad on the TCanvas before plotting something on TPad
    # cd to the pad1
    pad1.cd()
    if logy:
        pad1.SetLogy()
    
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
                hsim.GetYaxis().SetRangeUser(1E-3, (hsim.GetMaximum()) * ymaxscale)

            hsim.GetXaxis().SetTitle(XaxisName)
            hsim.GetXaxis().SetTitleOffset(1.1)
            hsim.GetYaxis().SetTitleOffset(1.35)
            hsim.SetLineColorAlpha(TColor.GetColor(hsimcolor[i]), 0.95)
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe')
        else:
            hsim.SetLineColorAlpha(TColor.GetColor(hsimcolor[i]), 0.95)
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe same')

    for i, hdata in enumerate(hdatas):
        hdata.SetMarkerStyle(20)
        hdata.SetMarkerSize(0.7)
        hdata.SetMarkerColorAlpha(TColor.GetColor(hsimcolor[i]), 0.95)
        hdata.SetLineColorAlpha(TColor.GetColor(hsimcolor[i]), 0.95)
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
    for i, lt in enumerate(simlegtex):
        leg.AddEntry(hdatas[i], lt, "lpe")
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
    
    c.Update()
    # cd to the pad2
    textscale = 2.7
    pad2.cd()
    pad2.SetLogy(0)
    pad2.SetGridy(0)
    # take the ratio = data/simulation
    l_hM_ratio = []
    for i, hsim in enumerate(hsims):
        hM_ratio = hdatas[i].Clone('hM_ratio'+str(i))
        hM_ratio.Divide(hsim)
        hM_ratio.SetMarkerStyle(20)
        hM_ratio.SetMarkerSize(0.7)
        hM_ratio.SetMarkerColorAlpha(TColor.GetColor(hsimcolor[i]), 0.95)
        hM_ratio.SetLineColorAlpha(TColor.GetColor(hsimcolor[i]), 0.95)
        hM_ratio.SetLineWidth(2)
        if i == 0:
            hM_ratio.GetYaxis().SetRangeUser(0.8+1E-3, 1.2-1E-3)
            hM_ratio.GetYaxis().SetNdivisions(505)
            hM_ratio.GetYaxis().SetTitle('Data/Sim.')
            hM_ratio.GetYaxis().SetTitleOffset(0.5)
            hM_ratio.GetYaxis().SetTickSize(TickSize)
            hM_ratio.GetYaxis().SetTitleSize(AxisTitleSize*textscale)
            hM_ratio.GetYaxis().SetLabelSize(AxisLabelSize*textscale)
            hM_ratio.GetXaxis().SetTickSize(TickSize*textscale)
            hM_ratio.GetXaxis().SetTitleSize(AxisTitleSize*textscale)
            hM_ratio.GetXaxis().SetLabelSize(AxisLabelSize*textscale)
            hM_ratio.GetXaxis().SetTitleOffset(1.1)
            hM_ratio.GetXaxis().SetTitle(XaxisName)
            hM_ratio.Draw('PE1X0')
        else:
            hM_ratio.Draw('PE1X0 same')
            
        l_hM_ratio.append(hM_ratio)
    
    # Draw a line at 1
    line = TLine(hM_ratio.GetXaxis().GetXmin(), 1, hM_ratio.GetXaxis().GetXmax(), 1)
    line.SetLineColor(kBlack)
    line.SetLineStyle(2)
    line.Draw()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0
        
        

if __name__ == '__main__':
    plotpath = './ClusADCCutComp/'
    os.makedirs(plotpath, exist_ok=True)

    histbasepath = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists'
    datahistspath = ['Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet{}'.format(i) for i in range(3)]
    simhistspath = ['Sim_Ntuple_HIJING_new_20240424/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet{}'.format(i) for i in range(3)]
    
    for i, datahistpath in enumerate(datahistspath):
        if os.path.isfile("{}/{}/hists_merged.root".format(histbasepath, datahistpath)):
            os.system("rm {}/{}/hists_merged.root".format(histbasepath, datahistpath))
            os.system("hadd -f -j 20 {}/{}/hists_merged.root {}/{}/hists_*.root".format(histbasepath, datahistpath, histbasepath, datahistpath))
        else:
            os.system("hadd -f -j 20 {}/{}/hists_merged.root {}/{}/hists_*.root".format(histbasepath, datahistpath, histbasepath, datahistpath))
        
    for i, simhistpath in enumerate(simhistspath):
        if os.path.isfile("{}/{}/hists_merged.root".format(histbasepath, simhistpath)):
            os.system("rm {}/{}/hists_merged.root".format(histbasepath, simhistpath))
            os.system("hadd -f -j 20 {}/{}/hists_merged.root {}/{}/hists_*.root".format(histbasepath, simhistpath, histbasepath, simhistpath))
        else:
            os.system("hadd -f -j 20 {}/{}/hists_merged.root {}/{}/hists_*.root".format(histbasepath, simhistpath, histbasepath, simhistpath))
            
    l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_data = [GetHistogram(histbasepath+'/'+datahistpath+'/hists_merged.root', 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10') for datahistpath in datahistspath]
    l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim = [GetHistogram(histbasepath+'/'+simhistpath+'/hists_merged.root', 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10') for simhistpath in simhistspath]
    
    Draw_1Dhist_datasimcomp(l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_data, l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_sim, [0.05,0.06,0.15,0.32], 'data', False, 2.0, 'Reco-tracklet #eta', '', False, ['Cluster ADC > 35', 'Cluster ADC > 50', 'Cluster ADC > 65'], ['Centrality 0-70%, |Asymm._{MBD}|#leq0.75, -30#leqZ_{vtx}#leq-10[cm]'], plotpath+'Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10_clusAdcCutComp')