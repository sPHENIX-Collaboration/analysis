#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TFile, TCanvas, TLegend, TColor, gROOT, gSystem, kBlack, kGreen, kRed, kBlue, kOrange, kViolet, kAzure, kMagenta, kCyan, kYellow, kGray, kWhite, gPad, TPad, TLine
import numpy
import math
import glob
from plotUtil import GetHistogram, markerset, colorset2

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.045
LeftMargin = 0.15
RightMargin = 0.05
TopMargin = 0.08
BottomMargin = 0.13
textscale = 2.6

gROOT.SetBatch(True)

def getRelativeDiff(h_2diff, h_nominal):
    # check if the histograms have the same binning
    if h_2diff.GetNbinsX() != h_nominal.GetNbinsX():
        print('Error: histograms have different number of bins')
        return None

    h_reldiff = h_2diff.Clone('h_reldiff')
    h_reldiff.Reset()
    for i in range(1, h_2diff.GetNbinsX() + 1):
        absdiff = abs(h_2diff.GetBinContent(i) - h_nominal.GetBinContent(i))
        nominal = h_nominal.GetBinContent(i)
        reldiff = (absdiff / nominal) if nominal != 0 else 0 
        h_reldiff.SetBinContent(i, reldiff)
    
    return h_reldiff
   

def getMaxRelDiff(l_hm_reldiff):
    hm_maxRelDiff = l_hm_reldiff[0].Clone('hm_maxRelDiff')
    hm_maxRelDiff.Reset()
    for hm_reldiff in l_hm_reldiff:
        for i in range(1, hm_reldiff.GetNbinsX() + 1):
            if hm_reldiff.GetBinContent(i) > hm_maxRelDiff.GetBinContent(i):
                hm_maxRelDiff.SetBinContent(i, hm_reldiff.GetBinContent(i))
    
    return hm_maxRelDiff


def getFinalUncertainty(l_hm):
    # sum in quadrature of the uncertainties
    hM_TotalRelUnc = l_hm[0].Clone('hM_TotalRelUnc')
    hM_TotalRelUnc.Reset()
    for i in range(1, hM_TotalRelUnc.GetNbinsX() + 1):
        totalUnc = 0
        for hm in l_hm:
            totalUnc += hm.GetBinContent(i)**2
            
        totalUnc = math.sqrt(totalUnc)
        hM_TotalRelUnc.SetBinContent(i, totalUnc)
    
    return hM_TotalRelUnc

# provide a list of histograms to draw and compare. The first histogram in the list is the baseline
def drawcomp(l_hm, l_hmleg, legheader, plotname):
    lmarker = markerset(len(l_hm))
    lcolor = colorset2(len(l_hm) - 1)
    ms = 1.2
    alpha = 0.8
    ymin = 1E9
    ymax = -1
    
    # take the ratio to the baseline; the baseline is always the first in the list
    l_ratio = []
    for i in range(0, len(l_hm)):
        h_ratio = l_hm[i].Clone('h_ratio_variation{}'.format(i))
        h_ratio.Divide(l_hm[0])
        l_ratio.append(h_ratio)
        
    for hm in l_hm:
        if hm.GetMinimum(0) < ymin:
            ymin = hm.GetMinimum(0)
        if hm.GetMaximum() > ymax:
            ymax = hm.GetMaximum()

    # create a dummy histogram for axis settings
    hdummy = TH1F('hdummy', 'hdummy', l_hm[0].GetNbinsX(), l_hm[0].GetXaxis().GetXmin(), l_hm[0].GetXaxis().GetXmax())
    
    c = TCanvas('c', 'c', 800, 700)
    pad1 = TPad( 'pad1', ' ', 0, 0.3, 1, 1)
    pad2 = TPad( 'pad2', ' ', 0, 0, 1, 0.3)
    pad1.SetTopMargin(0.06*(len(l_hm)+2))
    pad1.SetBottomMargin(0.0)
    pad1.Draw()
    pad2.SetTopMargin(0.0)
    pad2.SetBottomMargin(0.4)
    pad2.Draw()
    pad1.cd()
    # set the axis titles and labels for the histograms
    # hdummy.GetXaxis().SetTitle('Pseudorapidity #eta')
    hdummy.GetYaxis().SetTitle('dN_{ch}/d#eta')
    hdummy.GetYaxis().SetTitleOffset(1.6)
    hdummy.GetYaxis().SetNdivisions(505)
    hdummy.GetYaxis().SetRangeUser(ymin*0.975, ymax*1.025)
    hdummy.Draw()
    l_hm[0].SetMarkerStyle(20)
    l_hm[0].SetMarkerColor(kBlack)
    l_hm[0].SetMarkerSize(ms)
    l_hm[0].SetLineColor(kBlack)
    l_hm[0].SetLineWidth(0)
    l_hm[0].Draw('pex0 same')
    for i in range(1, len(l_hm)):
        l_hm[i].SetMarkerStyle(lmarker[i])
        l_hm[i].SetMarkerColorAlpha(TColor.GetColor(lcolor[i - 1]), alpha)
        l_hm[i].SetMarkerSize(ms if lmarker[i] != 33 else ms * 1.4)
        l_hm[i].SetLineColorAlpha(TColor.GetColor(lcolor[i - 1]), alpha)
        l_hm[i].SetLineWidth(0)
        l_hm[i].Draw('pex0 same')
    leg = TLegend(gPad.GetLeftMargin(), 1 - gPad.GetTopMargin() + 0.02, gPad.GetLeftMargin() + 0.2, 0.98)
    leg.SetHeader(legheader)
    leg.SetTextSize(0.045)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    for i in range(len(l_hm)):
        leg.AddEntry(l_hm[i], '{}'.format(l_hmleg[i]), 'p')
    leg.Draw()
    c.RedrawAxis()
    c.Update()
    # cd to the pad2 and draw the ratio
    pad2.cd()
    for i in range(1, len(l_ratio)):
        if i == 1:
            l_ratio[i].GetYaxis().SetTitle('Ratio')
            l_ratio[i].GetYaxis().SetTitleOffset(0.6)
            l_ratio[i].GetYaxis().SetRangeUser(0.9, 1.1)
            l_ratio[i].GetYaxis().SetNdivisions(502)
            l_ratio[i].GetYaxis().SetTitleSize(AxisTitleSize * textscale)
            l_ratio[i].GetYaxis().SetLabelSize(AxisLabelSize * textscale)
            l_ratio[i].GetXaxis().SetTitle('Pseudorapidity #eta')
            l_ratio[i].GetXaxis().SetTitleSize(AxisTitleSize * textscale)
            l_ratio[i].GetXaxis().SetLabelSize(AxisLabelSize * textscale)
            l_ratio[i].GetXaxis().SetTickLength(0.03*textscale)
            l_ratio[i].SetMarkerStyle(lmarker[i])
            l_ratio[i].SetMarkerColor(TColor.GetColor(lcolor[i - 1]))
            l_ratio[i].SetMarkerSize(ms if lmarker[i] != 33 else ms * 1.4)
            l_ratio[i].SetLineColor(TColor.GetColor(lcolor[i - 1]))
            l_ratio[i].SetLineWidth(0)
            l_ratio[i].Draw('pex0')
        else:
            l_ratio[i].SetMarkerStyle(lmarker[i])
            l_ratio[i].SetMarkerColor(TColor.GetColor(lcolor[i - 1]))
            l_ratio[i].SetMarkerSize(ms if lmarker[i] != 33 else ms * 1.4)
            l_ratio[i].SetLineColor(TColor.GetColor(lcolor[i - 1]))
            l_ratio[i].SetLineWidth(0)
            l_ratio[i].Draw('pex0 same')
    # draw a horizontal line at 1
    hline = TLine(l_ratio[1].GetXaxis().GetXmin(), 1, l_ratio[1].GetXaxis().GetXmax(), 1)
    hline.SetLineColor(kBlack)
    hline.SetLineWidth(1)
    hline.SetLineStyle(2)
    hline.Draw('same')
    c.Update()
    c.SaveAs(plotname+'.pdf')
    c.SaveAs(plotname+'.png')
    if (c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('--desc', dest='desc', type='string', default='Centrality0to70_Zvtxm30p0tom10p0_noasel', help='String for input file and output plot description (cuts, binnings, etc...)')
    
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))
    
    desc = opt.desc
    
    # deltaR selection - baseline 0.5, variation  0.4, 0.6
    # cluster ADC cut - baseline 35, variation 50 and 0
    # cluster phi-size cut - baseline 40, variation none
    # random noise cluster
    # Statistical uncertainties in the geometric correction and alpha corrections as propagated to the final dNdEta
    
    str_centrality = desc.split('_')[0]
    centrality_low = str_centrality.split('to')[0].replace('Centrality', '')
    centrality_high = str_centrality.split('to')[1]
    str_zvtx = desc.split('_')[1]
    zvtx_low = str_zvtx.split('to')[0].replace('Zvtx', '').replace('m', '-').replace('p', '.') if 'm' in str_zvtx else str_zvtx.split('to')[0].replace('Zvtx', '').replace('p', '.')
    zvtx_high = str_zvtx.split('to')[1].replace('m', '-').replace('p', '.') if 'm' in str_zvtx else str_zvtx.split('to')[1].replace('p', '.')
    str_asel = desc.split('_')[2]
    print (str_centrality, str_zvtx, str_asel)
    print ('centrality_low: {}, centrality_high: {}, zvtx_low: {}, zvtx_high: {}, str_asel: {}'.format(centrality_low, centrality_high, zvtx_low, zvtx_high, str_asel))
    alt_zvtx1 = 'Zvtxm10p0to0p0'
    alt_zvtx2 = 'Zvtx0p0to10p0'
    
    os.makedirs('./systematics/{}'.format(desc), exist_ok=True)
    
    h1WEfinal_nominal = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    # tracklet delta-R cut
    h1WEfinal_dRcutSet1 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_dRcutSet2 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    # cluster ADC cut
    h1WEfinal_clusAdcCutSet1 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_clusAdcCutSet2 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    # cluster phi-size cut
    h1WEfinal_clusphisizeSet1 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    # strangeness enhanced by 40% in HIJING
    h1WEfinal_strangeness = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_enhancedstrangeness_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    # event generator 
    h1WEfinal_EPOS = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_EPOS_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_AMPT = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_AMPT_segment1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    # different segments 
    h1WEfinal_segment2 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment2/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_segment3 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment3/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_segment4 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment4/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_segment5 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment5/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_segment6 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_segment6/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    
    # get the uncertainty from the nominal 
    hM_statunc_nominal = h1WEfinal_nominal.Clone('hM_statunc_nominal')
    hM_statunc_nominal.Reset()
    for i in range(1, h1WEfinal_nominal.GetNbinsX() + 1):
        statunc = h1WEfinal_nominal.GetBinError(i) / h1WEfinal_nominal.GetBinContent(i) if h1WEfinal_nominal.GetBinContent(i) != 0 else 0
        hM_statunc_nominal.SetBinContent(i, statunc)
    
    # calculate the relative difference (absolute value of difference divided by the nominal value)
    # tracklet delta-R cut
    hM_reldiff_h1WEfinal_dRcutSet1 = getRelativeDiff(h1WEfinal_dRcutSet1, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_dRcutSet2 = getRelativeDiff(h1WEfinal_dRcutSet2, h1WEfinal_nominal)
    hM_maxreldiff_dRcut = getMaxRelDiff([hM_reldiff_h1WEfinal_dRcutSet1, hM_reldiff_h1WEfinal_dRcutSet2])
    # hM_maxreldiff_dRcut = getMaxRelDiff([hM_reldiff_h1WEfinal_dRcutSet1, hM_reldiff_h1WEfinal_dRcutSet2, hM_reldiff_h1WEfinal_dRcutSet3, hM_reldiff_h1WEfinal_dRcutSet4])
    # cluster ADC cut
    hM_reldiff_h1WEfinal_clusAdcCutSet1 = getRelativeDiff(h1WEfinal_clusAdcCutSet1, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_clusAdcCutSet2 = getRelativeDiff(h1WEfinal_clusAdcCutSet2, h1WEfinal_nominal)
    # hM_maxreldiff_clusAdcCut = getMaxRelDiff([hM_reldiff_h1WEfinal_clusAdcCutSet1])
    hM_maxreldiff_clusAdcCut = getMaxRelDiff([hM_reldiff_h1WEfinal_clusAdcCutSet1, hM_reldiff_h1WEfinal_clusAdcCutSet2])
    # cluster phi-size cut
    hM_reldiff_h1WEfinal_clusPhiSizeCutSet1 = getRelativeDiff(h1WEfinal_clusphisizeSet1, h1WEfinal_nominal)
    hM_maxreldiff_clusPhiSizeCut = getMaxRelDiff([hM_reldiff_h1WEfinal_clusPhiSizeCutSet1])
    # strangeness enhanced by 40%
    hM_reldiff_h1WEfinal_strangeness = getRelativeDiff(h1WEfinal_strangeness, h1WEfinal_nominal)
    hM_maxreldiff_strangeness = getMaxRelDiff([hM_reldiff_h1WEfinal_strangeness])
    # event generator
    hM_reldiff_h1WEfinal_EPOS = getRelativeDiff(h1WEfinal_EPOS, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_AMPT = getRelativeDiff(h1WEfinal_AMPT, h1WEfinal_nominal)
    hM_maxreldiff_eventgen = getMaxRelDiff([hM_reldiff_h1WEfinal_EPOS, hM_reldiff_h1WEfinal_AMPT])
    # different segments 
    hM_reldiff_h1WEfinal_segment2 = getRelativeDiff(h1WEfinal_segment2, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_segment3 = getRelativeDiff(h1WEfinal_segment3, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_segment4 = getRelativeDiff(h1WEfinal_segment4, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_segment5 = getRelativeDiff(h1WEfinal_segment5, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_segment6 = getRelativeDiff(h1WEfinal_segment6, h1WEfinal_nominal)
    hM_maxreldiff_segment = getMaxRelDiff([hM_reldiff_h1WEfinal_segment2, hM_reldiff_h1WEfinal_segment3, hM_reldiff_h1WEfinal_segment4, hM_reldiff_h1WEfinal_segment5, hM_reldiff_h1WEfinal_segment6])
    
    hM_TotalRelUnc = getFinalUncertainty([hM_statunc_nominal,  
                                          hM_maxreldiff_dRcut, 
                                          hM_maxreldiff_clusAdcCut,
                                          hM_maxreldiff_clusPhiSizeCut, 
                                          hM_maxreldiff_strangeness,
                                          hM_maxreldiff_eventgen,
                                          hM_maxreldiff_segment
                                        ])
    
    # get the absolute uncertainty 
    hM_final = h1WEfinal_nominal.Clone('hM_final')
    hM_TotalUnc = hM_TotalRelUnc.Clone('hM_TotalUnc')
    for i in range(1, hM_TotalUnc.GetNbinsX() + 1):
        hM_TotalUnc.SetBinContent(i, hM_TotalRelUnc.GetBinContent(i) * h1WEfinal_nominal.GetBinContent(i))
        hM_final.SetBinError(i, hM_TotalRelUnc.GetBinContent(i) * h1WEfinal_nominal.GetBinContent(i))
        
    # only include bins with |eta|<=1.1
    for i in range(1, hM_TotalRelUnc.GetNbinsX() + 1):
        if abs(hM_TotalRelUnc.GetXaxis().GetBinCenter(i)) > 1.25:
            # print ('i: {}, eta: {}'.format(i, hM_TotalRelUnc.GetXaxis().GetBinCenter(i)))
            hM_statunc_nominal.SetBinContent(i, 0)
            hM_maxreldiff_dRcut.SetBinContent(i, 0)
            hM_maxreldiff_clusAdcCut.SetBinContent(i, 0)
            hM_maxreldiff_clusPhiSizeCut.SetBinContent(i, 0)
            hM_maxreldiff_strangeness.SetBinContent(i, 0)
            hM_maxreldiff_eventgen.SetBinContent(i, 0)
            hM_maxreldiff_segment.SetBinContent(i, 0)
            hM_TotalRelUnc.SetBinContent(i, 0)
            hM_TotalUnc.SetBinContent(i, 0)
            hM_final.SetBinContent(i, 0)
            
        
    # Root file with the histograms (with the final uncertainty)
    fout = TFile('./systematics/{}/finalhists_systematics_{}.root'.format(desc, desc), 'recreate')
    fout.cd()
    hM_statunc_nominal.Write()
    hM_maxreldiff_dRcut.SetName('hM_maxreldiff_dRcut')
    hM_maxreldiff_dRcut.Write()
    hM_maxreldiff_clusAdcCut.SetName('hM_maxreldiff_clusAdcCut')
    hM_maxreldiff_clusAdcCut.Write()
    hM_maxreldiff_clusPhiSizeCut.SetName('hM_maxreldiff_clusPhiSizeCut')
    hM_maxreldiff_clusPhiSizeCut.Write()
    hM_maxreldiff_strangeness.SetName('hM_maxreldiff_strangeness')
    hM_maxreldiff_strangeness.Write()
    hM_maxreldiff_eventgen.SetName('hM_maxreldiff_eventgen')
    hM_maxreldiff_eventgen.Write()
    hM_maxreldiff_segment.SetName('hM_maxreldiff_segment')
    hM_maxreldiff_segment.Write()
    hM_TotalRelUnc.Write()
    hM_TotalUnc.Write()
    hM_final.Write()
    fout.Close()
    
    # plotting 
    # split the string by '_' and replace some characters
    descstr = desc.split('_')
    centstr = descstr[0]
    zvtxstr = descstr[1]
    aselstr = descstr[2]
    centstr = centstr.replace('Centrality', 'Centrality [')
    centstr = centstr.replace('to', '-')
    centstr = centstr + ']%'
    zvtxstr = zvtxstr.replace('m', '-')
    zvtxstr = zvtxstr.replace('p', '.')
    zvtxstr = zvtxstr.replace('to', ', ')
    zvtxstr = zvtxstr.replace('Zvtx', 'Z_{vtx} [')
    zvtxstr = zvtxstr + '] cm'
    aselstr = '' if aselstr == 'noasel' else ', (with additional selection)'
    legstr = centstr + ', ' + zvtxstr + aselstr
    
    # drawcomp(l_hm, l_hmleg, legheader, plotname):
    drawcomp([h1WEfinal_nominal, h1WEfinal_dRcutSet1, h1WEfinal_dRcutSet2], 
             ['Nominal (#DeltaR<0.5)', '#DeltaR<0.4', '#DeltaR<0.6'], 
             '#DeltaR cut variation, ' + centstr, './systematics/{}/h1WEfinal_dRcut_variation_{}'.format(desc, desc))
    drawcomp([h1WEfinal_nominal, h1WEfinal_clusAdcCutSet1, h1WEfinal_clusAdcCutSet2], 
             ['Nominal (Cluster ADC>35)', 'w.o cluster ADC cut', 'Cluster ADC>50'], 
             'Cluster ADC cut variation, ' + centstr, './systematics/{}/h1WEfinal_clusAdcCut_variation_{}'.format(desc, desc))
    drawcomp([h1WEfinal_nominal, h1WEfinal_clusphisizeSet1],
             ['Nominal (Cluster #phi-size cut#leq40)', 'w.o cluster #phi-size cut'], 
             'Cluster #phi-size cut variation, ' + centstr, './systematics/{}/h1WEfinal_clusPhiSizeCut_variation_{}'.format(desc, desc))
    drawcomp([h1WEfinal_nominal, h1WEfinal_strangeness],
             ['Nominal', 'Enhanced strangeness by 40%'],
             'Enhanced strangeness, ' + centstr, './systematics/{}/h1WEfinal_strangeness_variation_{}'.format(desc, desc))
    drawcomp([h1WEfinal_nominal, h1WEfinal_EPOS, h1WEfinal_AMPT],
             ['Nominal (HIJING)', 'EPOS', 'AMPT'],
             'Event generator variation, ' + centstr, './systematics/{}/h1WEfinal_eventgen_variation_{}'.format(desc, desc))
    drawcomp([h1WEfinal_nominal, h1WEfinal_segment2, h1WEfinal_segment3, h1WEfinal_segment4, h1WEfinal_segment5, h1WEfinal_segment6], 
             ['Nominal', 'Segment 2', 'Segment 3', 'Segment 4', 'Segment 5', 'Segment 6'],
             'Run segment variation, ' + centstr, './systematics/{}/h1WEfinal_segment_variation_{}'.format(desc, desc))
    
    # scale the histograms by 100 to get the percentage
    hM_TotalRelUnc.Scale(100)
    hM_statunc_nominal.Scale(100)
    hM_maxreldiff_dRcut.Scale(100)
    hM_maxreldiff_clusAdcCut.Scale(100)
    hM_maxreldiff_clusPhiSizeCut.Scale(100)
    hM_maxreldiff_strangeness.Scale(100)
    hM_maxreldiff_eventgen.Scale(100)
    hM_maxreldiff_segment.Scale(100)
    
    # plot the relative difference
    markersty = 20
    c = TCanvas('c', 'c', 800, 700)
    c.cd()
    hM_TotalRelUnc.GetXaxis().SetTitle('#eta')
    hM_TotalRelUnc.GetYaxis().SetTitle('Relative uncertainty [%]')
    hM_TotalRelUnc.GetYaxis().SetTitleOffset(1.6)
    hM_TotalRelUnc.SetLineColor(kBlack)
    hM_TotalRelUnc.SetLineWidth(2)
    hM_TotalRelUnc.GetYaxis().SetRangeUser(0, hM_TotalRelUnc.GetMaximum() * 2.0)
    hM_TotalRelUnc.Draw('hist')
    hM_statunc_nominal.SetMarkerStyle(20)
    hM_statunc_nominal.SetMarkerColor(TColor.GetColor('#0F4C75'))
    hM_statunc_nominal.SetLineColor(TColor.GetColor('#0F4C75'))
    hM_statunc_nominal.SetLineWidth(3)
    hM_statunc_nominal.Draw('P same')
    hM_maxreldiff_dRcut.SetMarkerStyle(21)
    hM_maxreldiff_dRcut.SetMarkerColor(TColor.GetColor('#99ddff'))
    hM_maxreldiff_dRcut.SetLineColor(TColor.GetColor('#99ddff'))
    hM_maxreldiff_dRcut.SetLineWidth(3)
    hM_maxreldiff_dRcut.Draw('P same')
    hM_maxreldiff_clusAdcCut.SetMarkerStyle(33)
    hM_maxreldiff_clusAdcCut.SetMarkerSize(1.6)
    hM_maxreldiff_clusAdcCut.SetMarkerColor(TColor.GetColor('#44bb99'))
    hM_maxreldiff_clusAdcCut.SetLineColor(TColor.GetColor('#44bb99'))
    hM_maxreldiff_clusAdcCut.SetLineWidth(3)
    hM_maxreldiff_clusAdcCut.Draw('P same')
    hM_maxreldiff_clusPhiSizeCut.SetMarkerStyle(markersty)
    hM_maxreldiff_clusPhiSizeCut.SetMarkerColor(TColor.GetColor('#aadd00'))
    hM_maxreldiff_clusPhiSizeCut.SetLineColor(TColor.GetColor('#aadd00'))
    hM_maxreldiff_clusPhiSizeCut.SetLineWidth(2)
    hM_maxreldiff_clusPhiSizeCut.Draw('P same')
    hM_maxreldiff_strangeness.SetMarkerStyle(34)
    hM_maxreldiff_strangeness.SetMarkerColor(TColor.GetColor('#e99960'))
    hM_maxreldiff_strangeness.SetLineColor(TColor.GetColor('#e99960'))
    hM_maxreldiff_strangeness.SetLineWidth(3)
    hM_maxreldiff_strangeness.Draw('P same')
    hM_maxreldiff_eventgen.SetMarkerStyle(22)
    hM_maxreldiff_eventgen.SetMarkerColor(TColor.GetColor('#DE7C7D'))
    hM_maxreldiff_eventgen.SetLineColor(TColor.GetColor('#DE7C7D'))
    hM_maxreldiff_eventgen.SetLineWidth(3)
    hM_maxreldiff_eventgen.Draw('P same')
    hM_maxreldiff_segment.SetMarkerStyle(47)
    hM_maxreldiff_segment.SetMarkerColor(TColor.GetColor('#AA60C8'))
    hM_maxreldiff_segment.SetLineColor(TColor.GetColor('#AA60C8'))
    hM_maxreldiff_segment.SetLineWidth(3)
    hM_maxreldiff_segment.Draw('P same')
    leg = TLegend(0.2, 0.6, 0.45, 0.91)
    leg.SetHeader(legstr)
    leg.SetTextSize(0.035)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.AddEntry(hM_TotalRelUnc, 'Total uncertainty', 'l')
    leg.AddEntry(hM_statunc_nominal, 'Statistical uncertainty in corrections', 'p')
    leg.AddEntry(hM_maxreldiff_dRcut, 'Tracklet #DeltaR cut variation', 'p')
    leg.AddEntry(hM_maxreldiff_clusAdcCut, 'Cluster ADC cut variation', 'p')
    leg.AddEntry(hM_maxreldiff_clusPhiSizeCut, 'Cluster #phi-size cut variation', 'p')
    leg.AddEntry(hM_maxreldiff_strangeness, 'Strangeness variation', 'p')
    leg.AddEntry(hM_maxreldiff_eventgen, 'Event generator variation', 'p')
    leg.AddEntry(hM_maxreldiff_segment, 'Run segment variation', 'p')
    leg.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs('./systematics/{}/hM_TotalRelUnc_{}.pdf'.format(desc, desc))
    c.SaveAs('./systematics/{}/hM_TotalRelUnc_{}.png'.format(desc, desc))
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0