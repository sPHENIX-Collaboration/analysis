#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TCanvas, TFile, TTree, TChain, TLegend, TLatex, TF1, gROOT, gPad, gSystem, kHAlignRight, kVAlignBottom
import numpy as np
import math
import glob
from plotUtil import *

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

def drawcomparison_data(hists, normhist, logy, histcolor, legentry, legposition, xtitle, ytitle, yscale, outname):
    
    hmax = -999
    for i, h in enumerate(hists):
        h.Scale(normhist.Integral(-1,-1)/h.Integral(-1,-1))
        if h.GetMaximum() > hmax:
            hmax = h.GetMaximum()
    
    c = TCanvas('c', 'c', 800, 700)
    c.cd()
    if logy: 
        c.SetLogy()
    # c.SetLeftMargin(LeftMargin)
    # c.SetRightMargin(RightMargin)
    c.SetTopMargin(0.18)
    # c.SetBottomMargin(BottomMargin)
    
    # leg = TLegend(legposition[0], legposition[1], legposition[2], legposition[3])
    leg = TLegend(gPad.GetLeftMargin(), 1-gPad.GetTopMargin()+0.01, 1-gPad.GetRightMargin(), 0.98)
    leg.SetNColumns(2)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextSize(0.03)
    
    for i, h in enumerate(hists):
        h.SetLineColor(TColor.GetColor(histcolor[i]))
        h.SetMarkerColor(TColor.GetColor(histcolor[i]))
        h.SetMarkerStyle(20)
        h.SetMarkerSize(0.6)
        h.SetLineWidth(2)
        
        if i == 0:
            h.GetXaxis().SetTitle(xtitle)
            h.GetYaxis().SetTitle(ytitle)
            h.GetXaxis().SetTitleOffset(1.2)
            h.GetYaxis().SetTitleOffset(1.6)
            if logy:
                h.GetYaxis().SetRangeUser(0.5, hmax * yscale)
            else:
                h.GetYaxis().SetRangeUser(0., hmax * yscale)
                
            h.Draw('pe1')
        else:
            h.Draw('same pe1')
            
        leg.AddEntry(h, legentry[i], 'pe1')
            
        
    leg.Draw()
    legsphnx = TLegend((1-gPad.GetRightMargin())-0.43, (1-gPad.GetTopMargin())-0.15, (1-gPad.GetRightMargin())-0.11, (1-gPad.GetTopMargin())-0.05)
    legsphnx.SetTextSize(0.04)
    legsphnx.SetFillStyle(0)
    legsphnx.AddEntry("", "#it{#bf{sPHENIX}} Internal", "")
    legsphnx.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
    legsphnx.Draw()
    c.SaveAs('{}.pdf'.format(outname))
    c.SaveAs('{}.png'.format(outname))
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0
    
    
def Draw_1Dhist_fitGaussian(hist, norm1, logy, ymaxscale, XaxisName, Ytitle_unit, outname):
    hist.Sumw2()
    binwidth = hist.GetXaxis().GetBinWidth(1)
    c = TCanvas('c', 'c', 800, 700)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1))
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(RightMargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    if norm1:
        if Ytitle_unit == '':
            hist.GetYaxis().SetTitle(
                'Normalized entries / ({:g})'.format(binwidth))
        else:
            hist.GetYaxis().SetTitle(
                'Normalized entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
    else:
        if Ytitle_unit == '':
            hist.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
        else:
            hist.GetYaxis().SetTitle(
                'Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))

    # hist.GetXaxis().SetRangeUser(hist.GetBinLowEdge(1)-binwidth, hist.GetBinLowEdge(hist.GetNbinsX())+2*binwidth)
    if logy:
        hist.GetYaxis().SetRangeUser(hist.GetMinimum(0)*0.5, (hist.GetMaximum()) * ymaxscale)
    else:
        hist.GetYaxis().SetRangeUser(0., (hist.GetMaximum()) * ymaxscale)
    hist.GetXaxis().SetTitle(XaxisName)
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.35)
    hist.SetLineColor(1)
    hist.SetLineWidth(2)
    hist.Draw('hist')
    histxmin = hist.GetBinLowEdge(hist.FindFirstBinAbove(0))
    histxmax = hist.GetBinLowEdge(hist.FindLastBinAbove(0))+binwidth 
    f1 = TF1('f1', 'gaus', histxmin, histxmax)
    f1.SetParameter(1,hist.GetMean())
    f1.SetParLimits(1,-50,0)
    f1.SetParameter(2,hist.GetRMS())
    f1.SetParLimits(2,0,100)
    f1.SetLineColorAlpha(TColor.GetColor('#F54748'), 0.9)
    hist.Fit(f1, 'B')
    f1.Draw('same')
    leg = TLegend((1-RightMargin)-0.45, (1-TopMargin)-0.13,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.040)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Internal', '')
    leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    leg2 = TLegend(0.52, 0.65, 0.88, 0.78)
    leg2.SetTextSize(0.033)
    leg2.SetFillStyle(0)
    leg2.AddEntry(f1,'Gaussian fit','l')
    leg2.AddEntry('', '#mu={0:.3e}#pm{1:.3e}'.format(f1.GetParameter(1), f1.GetParError(1)), '')
    leg2.AddEntry('', '#sigma={0:.3e}#pm{1:.3e}'.format(f1.GetParameter(2), f1.GetParError(2)), '')
    leg2.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0
    return f1.GetParameter(1), f1.GetParError(1), f1.GetParameter(2), f1.GetParError(2)

def Draw_2Dhist_wtext(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, addtext, drawopt, outname):
    c = TCanvas('c', 'c', 800, 700)
    if logz:
        c.SetLogz()
    c.cd()
    if ZaxisName == '':
        gPad.SetRightMargin(rmargin)
    else:
        gPad.SetRightMargin(rmargin+0.03)

    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1, -1, -1))
    hist.GetXaxis().SetTitle(XaxisName)
    hist.GetYaxis().SetTitle(YaxisName)
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    if ZaxisName != '':
        hist.GetZaxis().SetTitle(ZaxisName)
        hist.GetZaxis().SetTitleSize(AxisTitleSize)
        hist.GetZaxis().SetTitleOffset(1.2*(rmargin/0.14))
        
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetLabelSize(AxisLabelSize)
    hist.SetContour(1000)
    hist.Draw(drawopt)

    # rightshift = 0.09 if IsData else 0.1
    rightshift = 0
    leg = TLegend((1-RightMargin)-0.2, (1-TopMargin)+0.01, 1-gPad.GetRightMargin(), (1-TopMargin)+0.04)
    leg.SetTextAlign(kHAlignRight+kVAlignBottom)
    leg.SetTextSize(0.045)
    leg.SetFillStyle(0)
    if IsData:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Internal", "")
        # leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
    else:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Simulation", "")
        # leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
    leg.Draw()
    c.RedrawAxis()
    # add text
    if addtext != '':
        tex = TLatex()
        tex.SetNDC()
        tex.SetTextSize(0.04)
        tex.DrawLatex(0.2, 0.85, addtext)
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0
        
        
if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("-i", "--infiledir", dest="infiledir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/RecoVtx', help="Input file directory")
    parser.add_option("-o", "--outdirprefix", dest="outdirprefix", default='Data_Run20869_HotDead_BCO_ADC_Survey', help="Output file directory")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 
    
    infiledir = opt.infiledir
    outdirprefix = opt.outdirprefix
    plotpath = './RecoPV_ana/'
    os.makedirs('{}/{}'.format(plotpath,outdirprefix), exist_ok=True)
    
    if os.path.isfile("{}/hists_merged.root".format(infiledir)):
        os.system("rm {}/hists_merged.root".format(infiledir))
        os.system("hadd -j 20 -f {}/hists_merged.root {}/hists_*.root".format(infiledir, infiledir))
    else:
        os.system("hadd -j 20 -f {}/hists_merged.root {}/hists_*.root".format(infiledir, infiledir))
    
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10 = GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10')
    hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive = GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive')
    hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10 = GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10')
    hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1 = GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1')
    
    centrality_cut = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
    l_hM_INTTVtxZ_Centrality = [GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_Centrality_{:d}to{:d}'.format(centrality_cut[i], centrality_cut[i+1])) for i in range(len(centrality_cut)-1)]
    l_hM_INTTVtxZ_Centrality_MBDAsymLe1 = [GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_Centrality_{:d}to{:d}_MBDAsymLe1'.format(centrality_cut[i], centrality_cut[i+1])) for i in range(len(centrality_cut)-1)]
    l_hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10 = [GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_Centrality_{:d}to{:d}_MBDAsymLe1_VtxZm10to10'.format(centrality_cut[i], centrality_cut[i+1])) for i in range(len(centrality_cut)-1)]
    l_hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10_forfit = [GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_Centrality_{:d}to{:d}_MBDAsymLe1_VtxZm10to10'.format(centrality_cut[i], centrality_cut[i+1])) for i in range(len(centrality_cut)-1)]
    l_hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe1_VtxZm10to10 = [GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_MBDAsymm_Centrality_{:d}to{:d}_MBDAsymLe1_VtxZm10to10'.format(centrality_cut[i], centrality_cut[i+1])) for i in range(len(centrality_cut)-1)]
    
    colors = ['#001219','#013a63', '#005f73', '#0a9396', '#94d2bd', '#ee9b00', '#ca6702', '#bb3e03', '#9b2226', '#7b193a']
    # drawcomparison_data(hists, normhist, logy, histcolor, legentry, legposition, xtitle, ytitle, yscale, outname)
    drawcomparison_data(l_hM_INTTVtxZ_Centrality, l_hM_INTTVtxZ_Centrality[0], True, colors, ['Centrality 0-10%', 'Centrality 10-20%', 'Centrality 20-30%', 'Centrality 30-40%', 'Centrality 40-50%', 'Centrality 50-60%', 'Centrality 60-70%', 'Centrality 70-80%', 'Centrality 80-90%', 'Centrality 90-100%'], [0.17, 0.6, 0.42, 0.9], 'INTT vtx_{Z} [cm]', 'Normalized entries', 10, '{}/{}/InttVtxz_MBDCentrality_Comp'.format(plotpath, outdirprefix))
    drawcomparison_data(l_hM_INTTVtxZ_Centrality_MBDAsymLe1, l_hM_INTTVtxZ_Centrality_MBDAsymLe1[0], True, colors, ['Centrality 0-10%', 'Centrality 10-20%', 'Centrality 20-30%', 'Centrality 30-40%', 'Centrality 40-50%', 'Centrality 50-60%', 'Centrality 60-70%', 'Centrality 70-80%', 'Centrality 80-90%', 'Centrality 90-100%'], [0.17, 0.6, 0.42, 0.9], 'INTT vtx_{Z} [cm]', 'Normalized entries', 10, '{}/{}/InttVtxz_MBDCentrality_MBDAsymLe1_Comp'.format(plotpath, outdirprefix)) 
    drawcomparison_data(l_hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10, l_hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10[0], False, colors, ['Centrality 0-10%', 'Centrality 10-20%', 'Centrality 20-30%', 'Centrality 30-40%', 'Centrality 40-50%', 'Centrality 50-60%', 'Centrality 60-70%', 'Centrality 70-80%', 'Centrality 80-90%', 'Centrality 90-100%'], [0.17, 0.6, 0.42, 0.9], 'INTT vtx_{Z} [cm]', 'Normalized entries', 1.2, '{}/{}/InttVtxz_MBDCentrality_MBDAsymLe1_VtxZm10to10_Comp'.format(plotpath, outdirprefix))
    
    l_hM_INTTVtxZ_MBDVtxZ_Centrality = [GetHistogram('{}/hists_merged.root'.format(infiledir), 'hM_INTTVtxZ_MBDVtxZ_Centrality_{:d}to{:d}'.format(centrality_cut[i], centrality_cut[i+1])) for i in range(len(centrality_cut)-1)]
    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname):
    # Draw_2Dhist_wtext(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, addtext, drawopt, outname):
    Draw_2Dhist_wtext(hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1, True, False, False, 0.18, 'INTT vtx_{Z} [cm]', 'MBD vtx_{Z} [cm]', 'Entries', "Centrality: 0-70%", 'colz', '{}/{}/InttVtxz_MbdVtxz_MBDCentrality0to70_MBDAsymLe1'.format(plotpath, outdirprefix))
    Draw_2Dhist_wtext(hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive, True, False, False, 0.15, 'INTT vtx_{Z} [cm]', 'MBD charge asymmetry', 'Entries', "Centrality: 0-70%", 'colz', '{}/{}/InttVtxz_MbdAsymm_MBDCentrality0to70'.format(plotpath, outdirprefix))
    Draw_2Dhist_wtext(hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10, True, False, False, 0.15, 'INTT vtx_{Z} [cm]', 'MBD charge asymmetry', 'Entries', "Centrality: 0-70%", 'colz', '{}/{}/InttVtxz_MbdAsymm_MBDCentrality0to70_MBDAsymLe1_VtxZm10to10'.format(plotpath, outdirprefix))
    
    for i in range(len(centrality_cut)-1):
        # Draw_1Dhist_fitGaussian(hist, norm1, logy, ymaxscale, XaxisName, Ytitle_unit, outname)
        Draw_1Dhist_fitGaussian(l_hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10_forfit[i], False, False, 1.3, 'INTT vtx_{Z} [cm]', 'cm', '{}/{}/InttVtxz_MBDCentrality_MBDAsymLe1_VtxZm10to10_{:d}to{:d}_GaussianFit'.format(plotpath, outdirprefix, centrality_cut[i], centrality_cut[i+1]))
        Draw_2Dhist_wtext(l_hM_INTTVtxZ_MBDVtxZ_Centrality[i], True, False, False, 0.15, 'INTT vtx_{Z} [cm]', 'MBD vtx_{Z} [cm]', 'Entries', "Centrality: {}-{}%".format(centrality_cut[i], centrality_cut[i+1]), 'colz', '{}/{}/InttVtxz_MbdVtxz_MBDCentrality{:d}to{:d}'.format(plotpath, outdirprefix, centrality_cut[i], centrality_cut[i+1]))
    
    Draw_1Dhist_fitGaussian(hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10, False, False, 1.3, 'INTT vtx_{Z} [cm]', 'cm', '{}/{}/InttVtxz_MBDCentrality0to70_MBDAsymLe1_VtxZm10to10_GaussianFit'.format(plotpath, outdirprefix))