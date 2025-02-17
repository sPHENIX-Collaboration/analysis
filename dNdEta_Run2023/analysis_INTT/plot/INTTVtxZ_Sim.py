#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TCanvas, TFile, TGraphAsymmErrors, TLegend, TColor, gROOT, gStyle, gPad, gSystem, kTRUE, kFALSE, kBird, kThermometer, RDataFrame, TF1
import numpy as np
import math
import glob
from plotUtil import Draw_2Dhist

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13


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
    f1 = TF1('f1', 'gaus', hist.GetXaxis().GetXmin(), hist.GetXaxis().GetXmax())
    f1.SetParameter(1,hist.GetMean())
    f1.SetParLimits(1,-1,1)
    f1.SetParameter(2,hist.GetRMS())
    f1.SetParLimits(2,0,100)
    f1.SetLineColorAlpha(TColor.GetColor('#F54748'), 0.9)
    hist.Fit(f1, 'B')
    f1.Draw('same')
    leg = TLegend((1-RightMargin)-0.45, (1-TopMargin)-0.13,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.040)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    leg2 = TLegend(0.54, 0.65, 0.88, 0.78)
    leg2.SetTextSize(0.033)
    leg2.SetFillStyle(0)
    leg2.AddEntry(f1,'Gaussian fit','l')
    leg2.AddEntry('', '#mu={0:.2e}#pm{1:.2e}'.format(f1.GetParameter(1), f1.GetParError(1)), '')
    leg2.AddEntry('', '#sigma={0:.2e}#pm{1:.2e}'.format(f1.GetParameter(2), f1.GetParError(2)), '')
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


def Draw_HistGraph(h, xaxistitle, yaxistitle, yaxisrange, xaxisbinlabel, outname):
    c = TCanvas('c1', 'c1', 800, 700)
    gPad.SetRightMargin(0.08)
    c.cd()
    h.GetXaxis().SetTitle(xaxistitle)
    h.GetYaxis().SetTitle(yaxistitle)
    h.SetLineWidth(2)
    h.SetMarkerSize(1.5)
    h.GetXaxis().SetTitleOffset(1.2)
    h.GetXaxis().SetNdivisions(10, 0, 0, kTRUE)
    h.GetYaxis().SetTitleOffset(1.5)
    h.GetYaxis().SetRangeUser(yaxisrange[0], yaxisrange[1])
    # Set bin labels
    for i in range(len(xaxisbinlabel)):
        h.GetXaxis().SetBinLabel(i+1, xaxisbinlabel[i])

    h.Draw('PEX0')
    
    leg = TLegend(1 - RightMargin - 0.45, 1-TopMargin-0.15, 1 - RightMargin - 0.08, 1-TopMargin-0.035)
    leg.SetTextSize(0.040)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

def Draw_1DEff(EffErr, logx, XaxisName, axesrange, outname):
    c = TCanvas('c', 'c', 800, 700)
    if logx:
        c.SetLogx()
    c.cd()
    gPad.SetRightMargin(RightMargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    EffErr.GetXaxis().SetTitle(XaxisName)
    EffErr.GetYaxis().SetTitle('Efficiency')
    EffErr.GetXaxis().SetRangeUser(axesrange[0], axesrange[1])
    # EffErr.GetXaxis().SetMoreLogLabels()
    EffErr.GetYaxis().SetRangeUser(axesrange[2], axesrange[3])
    EffErr.SetMarkerColor(1)
    EffErr.SetMarkerSize(1.5)
    EffErr.SetMarkerStyle(20)
    EffErr.GetXaxis().SetTitleOffset(1.2)
    # EffErr.GetYaxis().SetTitleOffset(ytitleoffset)
    # EffErr.GetXaxis().SetNdivisions(10, 0, 0, kTRUE)
    EffErr.Draw('AP')
    leg = TLegend(LeftMargin+0.03, 1-TopMargin-0.15, LeftMargin+0.1, 1-TopMargin-0.035)
    leg.SetTextSize(0.040)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0


def Draw_1DEffComp(leff, lcolor, logx, XaxisName, NdivisionArg, legtext, axesrange, outname):
    c = TCanvas('c', 'c', 800, 700)
    if logx:
        c.SetLogx()
    c.cd()
    gPad.SetRightMargin(RightMargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    for i,eff in enumerate(leff):
        if i == 0:
            leff[i].GetXaxis().SetTitle(XaxisName)
            leff[i].GetXaxis().SetTitleOffset(1.2)
            leff[i].GetYaxis().SetTitle('Efficiency')
            leff[i].GetXaxis().SetRangeUser(axesrange[0], axesrange[1])
            leff[i].GetYaxis().SetRangeUser(axesrange[2], axesrange[3])
            leff[i].SetLineColor(TColor.GetColor(lcolor[i]))
            leff[i].SetMarkerColor(TColor.GetColor(lcolor[i]))
            leff[i].SetMarkerSize(1.3)
            leff[i].SetMarkerStyle(20)
            if len(NdivisionArg) != 0:
                leff[i].GetXaxis().SetNdivisions(NdivisionArg[0], NdivisionArg[1], NdivisionArg[2], NdivisionArg[3])
            leff[i].Draw('AP')
        else:
            leff[i].SetLineColor(TColor.GetColor(lcolor[i]))
            leff[i].SetMarkerColor(TColor.GetColor(lcolor[i]))
            leff[i].SetMarkerSize(1.3)
            leff[i].SetMarkerStyle(20)
            leff[i].Draw('Psame')

    leg = TLegend(LeftMargin, (1-TopMargin)-0.15,
                  LeftMargin+0.25, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    leg1 = TLegend((1-gPad.GetRightMargin())-0.47, gPad.GetBottomMargin()+0.04,
                   (1-gPad.GetRightMargin())-0.05, gPad.GetBottomMargin()+0.06*len(legtext))
    leg1.SetTextSize(0.035)
    leg1.SetFillStyle(0)
    for i, eff in enumerate(leff):
        leg1.AddEntry(eff, legtext[i], "pe")
    leg1.Draw()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

def Draw_2Dhist_eff(hist, logx, logy, logz, norm1, rmargin, XaxisName, YaxisName, drawopt, outname):
    # gStyle.SetPalette(kThermometer)
    gStyle.SetPaintTextFormat('1.3g')
    c = TCanvas('c', 'c', 800, 700)
    if logx:
        c.SetLogx()
    if logy:
        c.SetLogy()
    if logz:
        c.SetLogz()
    c.cd()
    gPad.SetRightMargin(rmargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1, -1, -1))
    hist.GetXaxis().SetTitle(XaxisName)
    hist.GetYaxis().SetTitle(YaxisName)
    hist.GetZaxis().SetTitle('Efficiency')
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetTitleOffset(1.2)
    hist.GetZaxis().SetLabelSize(AxisLabelSize)
    hist.GetZaxis().SetRangeUser(0, 1)
    hist.SetContour(1000)
    hist.SetMarkerSize(0.7)
    hist.Draw(drawopt)
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
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("-i", "--infiledir", dest="infiledir", default='/sphenix/user/hjheng/TrackletAna/minitree/INTT/VtxEvtMap_ana382_zvtx-20cm_dummyAlignParams', help="Input file directory")
    parser.add_option("-o", "--outdirprefix", dest="outdirprefix", default='ana382_zvtx-20cm_dummyAlignParams', help="Output file directory")
    
    gStyle.SetPalette(kBird)

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    infiledir = opt.infiledir
    outdirprefix = opt.outdirprefix

    plotpath = './RecoPV_ana/'
    os.makedirs('{}/{}'.format(plotpath,outdirprefix), exist_ok=True)

    # Input file
    fname = '{}/minitree_merged.root'.format(infiledir)
    # os.system('hadd -f -j 20 {}/minitree_merged.root {}/minitree_00*.root'.format(infiledir, infiledir))
        

    # of clusters in inner layer, percentile
    df = RDataFrame('minitree', fname).Filter('is_min_bias')
    nparr_NClusInner = df.AsNumpy(columns=['NClusLayer1'])
    NclusInner_percentile = []
    Binedge_NclusInner_percentile = [1]
    NclusInner_percentile_cut = [0]
    NpercentileDiv = 20
    for i in range(NpercentileDiv-1):
        # print('percentile={}%, Nhits={}'.format((i+1)*5, np.percentile(np_NhitsL1['NClusLayer1'], (i+1)*5)))
        NclusInner_percentile.append(np.percentile(nparr_NClusInner['NClusLayer1'], (i+1)*5))
        Binedge_NclusInner_percentile.append(np.percentile(nparr_NClusInner['NClusLayer1'], (i+1)*5))
        if i % 2 == 1:
            NclusInner_percentile_cut.append(np.percentile(nparr_NClusInner['NClusLayer1'], (i+1)*5))
    NclusInner_percentile_cut.append(10000)
    Binedge_NclusInner_percentile.append(10000)

    # Prepare histograms
    l_hM_DiffVtxZ = []
    for i in range(10):
        if i < 7: 
            l_hM_DiffVtxZ.append(TH1F('hM_DiffVtxZ_Cent{}'.format(5*(2*i+1)), 'hM_DiffVtxZ_Cent{}'.format(5*(2*i+1)), 100, -2, 2))
        elif i >= 7:
            l_hM_DiffVtxZ.append(TH1F('hM_DiffVtxZ_Cent{}'.format(5*(2*i+1)), 'hM_DiffVtxZ_Cent{}'.format(5*(2*i+1)), 100, -5, 5))
        else:
            print('ERROR: i={}'.format(i))
            sys.exit()

    hM_NClusInner_NtruthVtx1 = TH1F('hM_NClusInner_NtruthVtx1', 'hM_NClusInner_NtruthVtx1', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile))
    hM_NClusInner_NtruthVtx1_absdiffle120cm = TH1F('hM_NClusInner_NtruthVtx1_absdiffle120cm', 'hM_NClusInner_NtruthVtx1_absdiffle120cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile)) # very loose cut
    hM_NClusInner_NtruthVtx1_absdiffle5cm = TH1F('hM_NClusInner_NtruthVtx1_absdiffle5cm', 'hM_NClusInner_NtruthVtx1_absdiffle5cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile))
    hM_NClusInner_NtruthVtx1_absdiffle2cm = TH1F('hM_NClusInner_NtruthVtx1_absdiffle2cm', 'hM_NClusInner_NtruthVtx1_absdiffle2cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile))
    hM_NClusInner_NtruthVtx1_absdiffle1cm = TH1F('hM_NClusInner_NtruthVtx1_absdiffle1cm', 'hM_NClusInner_NtruthVtx1_absdiffle1cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile))
    hM_Centrality_NtruthVtx1 = TH1F('hM_Centrality_NtruthVtx1', 'hM_Centrality_NtruthVtx1', 10, 0, 100)
    hM_Centrality_NtruthVtx1_absdiffle120cm = TH1F('hM_Centrality_NtruthVtx1_absdiffle120cm', 'hM_Centrality_NtruthVtx1_absdiffle120cm', 10, 0, 100) # very loose cut
    hM_Centrality_NtruthVtx1_absdiffle5cm = TH1F('hM_Centrality_NtruthVtx1_absdiffle5cm', 'hM_Centrality_NtruthVtx1_absdiffle5cm', 10, 0, 100)
    hM_Centrality_NtruthVtx1_absdiffle2cm = TH1F('hM_Centrality_NtruthVtx1_absdiffle2cm', 'hM_Centrality_NtruthVtx1_absdiffle2cm', 10, 0, 100)
    hM_Centrality_NtruthVtx1_absdiffle1cm = TH1F('hM_Centrality_NtruthVtx1_absdiffle1cm', 'hM_Centrality_NtruthVtx1_absdiffle1cm', 10, 0, 100)
    hM_NClusInner_TruthPVz_NtruthVtx1 = TH2F('hM_NClusInner_TruthPVz_NtruthVtx1', 'hM_NClusInner_TruthPVz_NtruthVtx1', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile), 15, -30, 30)
    hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle120cm = TH2F('hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle120cm', 'hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle120cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile), 15, -30, 30) # very loose cut
    hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle5cm = TH2F('hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle5cm', 'hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle5cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile), 15, -30, 30)
    hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle2cm = TH2F('hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle2cm', 'hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle2cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile), 15, -30, 30)
    hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle1cm = TH2F('hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle1cm', 'hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle1cm', NpercentileDiv, np.asarray(Binedge_NclusInner_percentile), 15, -30, 30)
    hM_Centrality_TruthPVz_NtruthVtx1 = TH2F('hM_Centrality_TruthPVz_NtruthVtx1', 'hM_Centrality_TruthPVz_NtruthVtx1', 10, 0, 100, 15, -30, 30)
    hM_Centrality_TruthPVz_NtruthVtx1_absdiffle120cm = TH2F('hM_Centrality_TruthPVz_NtruthVtx1_absdiffle120cm', 'hM_Centrality_TruthPVz_NtruthVtx1_absdiffle120cm', 10, 0, 100, 15, -30, 30) # very loose cut
    hM_Centrality_TruthPVz_NtruthVtx1_absdiffle5cm = TH2F('hM_Centrality_TruthPVz_NtruthVtx1_absdiffle5cm', 'hM_Centrality_TruthPVz_NtruthVtx1_absdiffle5cm', 10, 0, 100, 15, -30, 30)
    hM_Centrality_TruthPVz_NtruthVtx1_absdiffle2cm = TH2F('hM_Centrality_TruthPVz_NtruthVtx1_absdiffle2cm', 'hM_Centrality_TruthPVz_NtruthVtx1_absdiffle2cm', 10, 0, 100, 15, -30, 30)
    hM_Centrality_TruthPVz_NtruthVtx1_absdiffle1cm = TH2F('hM_Centrality_TruthPVz_NtruthVtx1_absdiffle1cm', 'hM_Centrality_TruthPVz_NtruthVtx1_absdiffle1cm', 10, 0, 100, 15, -30, 30)

    # Loop events and fill histograms
    f = TFile(fname, 'r')
    tree = f.Get('minitree')
    for idx in range(tree.GetEntries()):
        tree.GetEntry(idx)
        # skip MBD_centrality = nan
        if math.isnan(tree.MBD_centrality):
            continue
        
        if tree.is_min_bias == False:
            continue
        
        if tree.NTruthVtx == 1:
            idx = int(((tree.MBD_centrality / 5) - 1) / 2)
            l_hM_DiffVtxZ[idx].Fill(tree.PV_z - tree.TruthPV_z)
            hM_NClusInner_NtruthVtx1.Fill(tree.NClusLayer1)
            if abs(tree.PV_z - tree.TruthPV_z) <= 120:
                hM_NClusInner_NtruthVtx1_absdiffle120cm.Fill(tree.NClusLayer1)
            if abs(tree.PV_z - tree.TruthPV_z) <= 5:
                hM_NClusInner_NtruthVtx1_absdiffle5cm.Fill(tree.NClusLayer1)
            if abs(tree.PV_z - tree.TruthPV_z) <= 2:
                hM_NClusInner_NtruthVtx1_absdiffle2cm.Fill(tree.NClusLayer1)
            if abs(tree.PV_z - tree.TruthPV_z) <= 1:
                hM_NClusInner_NtruthVtx1_absdiffle1cm.Fill(tree.NClusLayer1)

            # centbin = int(((tree.MBD_centrality / 5) - 1) / 2)
            hM_Centrality_NtruthVtx1.Fill(tree.MBD_centrality)
            if abs(tree.PV_z - tree.TruthPV_z) <= 120:
                hM_Centrality_NtruthVtx1_absdiffle120cm.Fill(tree.MBD_centrality)
            if abs(tree.PV_z - tree.TruthPV_z) <= 5:
                hM_Centrality_NtruthVtx1_absdiffle5cm.Fill(tree.MBD_centrality)
            if abs(tree.PV_z - tree.TruthPV_z) <= 2:
                hM_Centrality_NtruthVtx1_absdiffle2cm.Fill(tree.MBD_centrality)
            if abs(tree.PV_z - tree.TruthPV_z) <= 1:
                hM_Centrality_NtruthVtx1_absdiffle1cm.Fill(tree.MBD_centrality)

            hM_NClusInner_TruthPVz_NtruthVtx1.Fill(tree.NClusLayer1, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 120:
                hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle120cm.Fill(tree.NClusLayer1, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 5:
                hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle5cm.Fill(tree.NClusLayer1, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 2:
                hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle2cm.Fill(tree.NClusLayer1, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 1:
                hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle1cm.Fill(tree.NClusLayer1, tree.TruthPV_z)

            hM_Centrality_TruthPVz_NtruthVtx1.Fill(tree.MBD_centrality, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 120:
                hM_Centrality_TruthPVz_NtruthVtx1_absdiffle120cm.Fill(tree.MBD_centrality, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 5:
                hM_Centrality_TruthPVz_NtruthVtx1_absdiffle5cm.Fill(tree.MBD_centrality, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 2:
                hM_Centrality_TruthPVz_NtruthVtx1_absdiffle2cm.Fill(tree.MBD_centrality, tree.TruthPV_z)
            if abs(tree.PV_z - tree.TruthPV_z) <= 1:
                hM_Centrality_TruthPVz_NtruthVtx1_absdiffle1cm.Fill(tree.MBD_centrality, tree.TruthPV_z)

    
    l_res_vtxZ = []
    l_reserr_vtxZ = []
    h_resolution_cent = TH1F('h_resolution_cent', 'h_resolution_cent', 10, 0, 10)
    h_bias_cent = TH1F('h_bias_cent', 'h_bias_cent', 10, 0, 10)
    for i, hist in enumerate(l_hM_DiffVtxZ):
        mean, meanerr, sigma, sigmaerr = Draw_1Dhist_fitGaussian(hist, False, True, 50, '#Deltaz(vtx_{Truth}, vtx_{Reco}) [cm]', 'cm', '{}/{}/DiffVtxZ_Cent{}'.format(plotpath,outdirprefix,5*(2*i+1)))
        l_res_vtxZ.append(sigma)
        l_reserr_vtxZ.append(sigmaerr)
        h_bias_cent.SetBinContent(i+1, mean)
        h_bias_cent.SetBinError(i+1, meanerr)
        h_resolution_cent.SetBinContent(i+1, sigma)
        h_resolution_cent.SetBinError(i+1, sigmaerr)

    Draw_HistGraph(h_bias_cent, 'Centrality', '#mu_{#Deltaz}^{Gaussian} [cm]', [-0.1,0.1], ['{}-{}%'.format(10*i, 10*(i+1)) for i in range(len(l_reserr_vtxZ))], '{}/{}/VtxZBias_Cent'.format(plotpath,outdirprefix))
    Draw_HistGraph(h_resolution_cent, 'Centrality', '#sigma_{#Deltaz}^{Gaussian} [cm]', [0, h_resolution_cent.GetMaximum()*1.5], ['{}-{}%'.format(10*i, 10*(i+1)) for i in range(len(l_reserr_vtxZ))], '{}/{}/VtxZReolution_Cent'.format(plotpath,outdirprefix))

    # Efficiency
    err_RecoPVEff_NClusInner_absdiffle60cm = TGraphAsymmErrors()
    err_RecoPVEff_NClusInner_absdiffle60cm.Divide(hM_NClusInner_NtruthVtx1_absdiffle120cm, hM_NClusInner_NtruthVtx1)
    err_RecoPVEff_NClusInner_absdiffle5cm = TGraphAsymmErrors()
    err_RecoPVEff_NClusInner_absdiffle5cm.Divide(hM_NClusInner_NtruthVtx1_absdiffle5cm, hM_NClusInner_NtruthVtx1)
    err_RecoPVEff_NClusInner_absdiffle2cm = TGraphAsymmErrors()
    err_RecoPVEff_NClusInner_absdiffle2cm.Divide(hM_NClusInner_NtruthVtx1_absdiffle2cm, hM_NClusInner_NtruthVtx1)
    err_RecoPVEff_NClusInner_absdiffle1cm = TGraphAsymmErrors()
    err_RecoPVEff_NClusInner_absdiffle1cm.Divide(hM_NClusInner_NtruthVtx1_absdiffle1cm, hM_NClusInner_NtruthVtx1)
    list_eff_NClusInner = [err_RecoPVEff_NClusInner_absdiffle60cm, err_RecoPVEff_NClusInner_absdiffle5cm, err_RecoPVEff_NClusInner_absdiffle2cm, err_RecoPVEff_NClusInner_absdiffle1cm]
    list_color = ['#03001C', '#9B0000', '#035397', '#347928']
    list_leg = ['|#Deltaz(vtx_{Reco},vtx_{Truth})|#leq120cm', '|#Deltaz(vtx_{Reco},vtx_{Truth})|#leq5cm', '|#Deltaz(vtx_{Reco},vtx_{Truth})|#leq2cm', '|#Deltaz(vtx_{Reco},vtx_{Truth})|#leq1cm']
    Draw_1DEffComp(list_eff_NClusInner, list_color, True, 'Number of clusters (inner)', [], list_leg, [0,10100,0,1.4], '{}/{}/RecoPVEff_NClusInner_EffComp'.format(plotpath,outdirprefix))

    err_RecoPVEff_Centrality_absdiffle120cm = TGraphAsymmErrors()
    err_RecoPVEff_Centrality_absdiffle120cm.Divide(hM_Centrality_NtruthVtx1_absdiffle120cm, hM_Centrality_NtruthVtx1)
    # err_RecoPVEff_Centrality_absdiffle120cm.GetXaxis().SetMaxDigits(2)
    err_RecoPVEff_Centrality_absdiffle5cm = TGraphAsymmErrors()
    err_RecoPVEff_Centrality_absdiffle5cm.Divide(hM_Centrality_NtruthVtx1_absdiffle5cm, hM_Centrality_NtruthVtx1)
    # err_RecoPVEff_Centrality_absdiffle5cm.GetXaxis().SetMaxDigits(2)
    err_RecoPVEff_Centrality_absdiffle2cm = TGraphAsymmErrors()
    err_RecoPVEff_Centrality_absdiffle2cm.Divide(hM_Centrality_NtruthVtx1_absdiffle2cm, hM_Centrality_NtruthVtx1)
    err_RecoPVEff_Centrality_absdiffle1cm = TGraphAsymmErrors()
    err_RecoPVEff_Centrality_absdiffle1cm.Divide(hM_Centrality_NtruthVtx1_absdiffle1cm, hM_Centrality_NtruthVtx1)
    list_eff_Centrality = [err_RecoPVEff_Centrality_absdiffle120cm, err_RecoPVEff_Centrality_absdiffle5cm, err_RecoPVEff_Centrality_absdiffle2cm, err_RecoPVEff_Centrality_absdiffle1cm]
    Draw_1DEffComp(list_eff_Centrality, list_color, False, 'Centrality', [10,0,0,kTRUE], list_leg, [0,100,0,1.4], '{}/{}/RecoPVEff_CentralityBin_EffComp'.format(plotpath,outdirprefix))

    # 2D efficiency
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle120cm = hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle120cm.Clone()
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle120cm.Divide(hM_NClusInner_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle5cm = hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle5cm.Clone()
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle5cm.Divide(hM_NClusInner_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle2cm = hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle2cm.Clone()
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle2cm.Divide(hM_NClusInner_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle1cm = hM_NClusInner_TruthPVz_NtruthVtx1_absdiffle1cm.Clone()
    eff_RecoPVEff_NClusInner_TruthPVz_absdiffle1cm.Divide(hM_NClusInner_TruthPVz_NtruthVtx1)
    # Draw_2Dhist_eff(hist, logx, logy, logz, norm1, rmargin, XaxisName, YaxisName, drawopt, outname)
    Draw_2Dhist_eff(eff_RecoPVEff_NClusInner_TruthPVz_absdiffle120cm, True, False, False, False, 0.18, 'Number of clusters (inner)', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_NClusInner_TruthPVz_absdiffle120cm'.format(plotpath,outdirprefix))
    Draw_2Dhist_eff(eff_RecoPVEff_NClusInner_TruthPVz_absdiffle5cm, True, False, False, False, 0.18, 'Number of clusters (inner)', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_NClusInner_TruthPVz_absdiffle5cm'.format(plotpath,outdirprefix))
    Draw_2Dhist_eff(eff_RecoPVEff_NClusInner_TruthPVz_absdiffle2cm, True, False, False, False, 0.18, 'Number of clusters (inner)', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_NClusInner_TruthPVz_absdiffle2cm'.format(plotpath,outdirprefix))
    Draw_2Dhist_eff(eff_RecoPVEff_NClusInner_TruthPVz_absdiffle1cm, True, False, False, False, 0.18, 'Number of clusters (inner)', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_NClusInner_TruthPVz_absdiffle1cm'.format(plotpath,outdirprefix))

    eff_RecoPVEff_Centrality_TruthPVz_absdiffle120cm = hM_Centrality_TruthPVz_NtruthVtx1_absdiffle120cm.Clone()
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle120cm.Divide(hM_Centrality_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle120cm.GetXaxis().SetNdivisions(10, 0, 0, kTRUE)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle5cm = hM_Centrality_TruthPVz_NtruthVtx1_absdiffle5cm.Clone()
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle5cm.Divide(hM_Centrality_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle5cm.GetXaxis().SetNdivisions(10, 0, 0, kTRUE)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle2cm = hM_Centrality_TruthPVz_NtruthVtx1_absdiffle2cm.Clone()
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle2cm.Divide(hM_Centrality_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle2cm.GetXaxis().SetNdivisions(10, 0, 0, kTRUE)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle1cm = hM_Centrality_TruthPVz_NtruthVtx1_absdiffle1cm.Clone()
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle1cm.Divide(hM_Centrality_TruthPVz_NtruthVtx1)
    eff_RecoPVEff_Centrality_TruthPVz_absdiffle1cm.GetXaxis().SetNdivisions(10, 0, 0, kTRUE)
    Draw_2Dhist_eff(eff_RecoPVEff_Centrality_TruthPVz_absdiffle120cm, False, False, False, False, 0.18, 'Centrality', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_Centrality_TruthPVz_absdiffle120cm'.format(plotpath,outdirprefix))
    Draw_2Dhist_eff(eff_RecoPVEff_Centrality_TruthPVz_absdiffle5cm, False, False, False, False, 0.18, 'Centrality', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_Centrality_TruthPVz_absdiffle5cm'.format(plotpath,outdirprefix))
    Draw_2Dhist_eff(eff_RecoPVEff_Centrality_TruthPVz_absdiffle2cm, False, False, False, False, 0.18, 'Centrality', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_Centrality_TruthPVz_absdiffle2cm'.format(plotpath,outdirprefix))
    Draw_2Dhist_eff(eff_RecoPVEff_Centrality_TruthPVz_absdiffle1cm, False, False, False, False, 0.18, 'Centrality', 'Truth vtx_{z} [cm]', 'colz text45', '{}/{}/RecoPVEff_Centrality_TruthPVz_absdiffle1cm'.format(plotpath,outdirprefix))
    