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

gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

NpercentileDiv = 10
interval = 100/NpercentileDiv

def Draw_1Dhist_wPercentile(hist, l_percentile, norm1, logx, logy, ymaxscale, XaxisName, Ytitle_unit, outname):
    hist.Sumw2()
    binwidth = hist.GetXaxis().GetBinWidth(1)
    binwidth2 = hist.GetXaxis().GetBinWidth(2)
    printbinwidth = True
    if binwidth != binwidth2:
        printbinwidth = False
    
    c = TCanvas('c', 'c', 800, 700)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1))
    if logy:
        c.SetLogy()
    if logx:
        c.SetLogx()
    c.cd()
    gPad.SetRightMargin(RightMargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    if printbinwidth:
        if norm1:
            if Ytitle_unit == '':
                hist.GetYaxis().SetTitle('Normalized entries / ({:g})'.format(binwidth))
            else:
                hist.GetYaxis().SetTitle('Normalized entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
        else:
            if Ytitle_unit == '':
                hist.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
            else:
                hist.GetYaxis().SetTitle('Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
    else:
        if norm1:
            if Ytitle_unit == '':
                hist.GetYaxis().SetTitle('Normalized entries')
            else:
                hist.GetYaxis().SetTitle('Normalized entries {unit})'.format(unit=Ytitle_unit))
        else:
            if Ytitle_unit == '':
                hist.GetYaxis().SetTitle('Entries')
            else:
                hist.GetYaxis().SetTitle('Entries {unit}'.format(unit=Ytitle_unit))

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
    leg = TLegend(1 - RightMargin - 0.45, 1-TopMargin-0.15, 1 - RightMargin - 0.08, 1-TopMargin-0.035)
    leg.SetTextSize(0.040)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    linestorage = []
    textstorage = []
    for i,p in enumerate(l_percentile):
        if (logx == False and hist.GetXaxis().GetXmax() <= 1000 and i > 3):
            continue
        pline = TLine(p, 0, p, (hist.GetMaximum()/ymaxscale)*0.8)
        pline.SetLineWidth(1)
        pline.SetLineStyle(kDashed)
        pline.SetLineColor(2)
        if len(linestorage)==0:
            pline.Draw()
        else:
            pline.Draw('same')
        gPad.Update()
        linestorage.append(pline)

        if (logx == False and hist.GetXaxis().GetXmax() > 1000 and i > 3) or (logx == False and hist.GetXaxis().GetXmax() <= 1000 and i <= 3) or logx == True:
            ptext = TText(p,(hist.GetMaximum()/ymaxscale)*0.3,'{:d}-{:d}%'.format(int(interval*((len(l_percentile)+1)-(i+2))),int(interval*((len(l_percentile)+1)-(i+1)))))
            ptext.SetTextAlign(13)
            ptext.SetTextSize(0.02)
            ptext.SetTextColor(2)
            ptext.SetTextAngle(90)
            if len(textstorage)==0:
                ptext.Draw()
            else:
                ptext.Draw('same')
            gPad.Update()
            textstorage.append(ptext)

    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

def Draw_2Dhist_wPercentile(hist, l_percentile, logz, norm1, rmargin, XaxisName, YaxisName, drawopt, outname):
    c = TCanvas('c', 'c', 800, 700)
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
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetLabelSize(AxisLabelSize)
    hist.SetContour(1000)
    hist.Draw(drawopt)

    linestorage = []
    for i,p in enumerate(l_percentile):
        pline = TLine(p, 0, p, hist.GetYaxis().GetXmax())
        pline.SetLineWidth(1)
        pline.SetLineStyle(kDashed)
        pline.SetLineColor(2)
        if len(linestorage)==0:
            pline.Draw()
        else:
            pline.Draw('same')
        gPad.Update()
        linestorage.append(pline)
        gPad.Update()

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
    parser = OptionParser(usage="usage: %prog ver [options -n]")
    parser.add_option("-f", "--inputfile", dest="inputfile", type="string", default='/sphenix/user/hjheng/TrackletAna/minitree/INTT/VtxEvtMap_ana382_zvtx-20cm_dummyAlignParams/INTTVtxZ.root', help="Input ntuple file name")
    parser.add_option("-d", "--plotdir", dest="plotdir", type="string", default='./centProxy/ana382_zvtx-20cm_dummyAlignParams', help="Plot directory")

    (opt, args) = parser.parse_args()

    inputfile = opt.inputfile
    plotdir = opt.plotdir

    os.makedirs(plotdir, exist_ok=True)

    df = ROOT.RDataFrame('minitree', inputfile)
    np_NClusL1 = df.AsNumpy(columns=['NClusLayer1'])
    NClusL1_percentile = []
    Binedge_NClusL1_percentile = [0]
    NClusL1_percentile_cut = [0]
    for i in range(NpercentileDiv-1):
        print('percentile={}-{}%, Nhits={}'.format(i*interval, (i+1)*interval, np.percentile(np_NClusL1['NClusLayer1'], (i+1)*interval)))
        NClusL1_percentile.append(np.percentile(np_NClusL1['NClusLayer1'], (i+1)*interval))
        Binedge_NClusL1_percentile.append(np.percentile(np_NClusL1['NClusLayer1'], (i+1)*interval))
        if i % 2 == 1:
            NClusL1_percentile_cut.append(np.percentile(np_NClusL1['NClusLayer1'], (i+1)*interval))
    NClusL1_percentile_cut.append(5000)
    Binedge_NClusL1_percentile.append(5000)
    
    with open('{}/Centrality_bin.txt'.format(plotdir), 'w') as f:
        for i in Binedge_NClusL1_percentile:
            print('{:3g}'.format(i), file=f)

    hM_NClusLayer1 = TH1F('hM_NClusLayer1', 'hM_NClusLayer1', 200, 0, 4000)
    hM_NClusLayer1_MBDquantity = TH2F('hM_NClusLayer1_MBDquantity', 'hM_NClusLayer1_MBDquantity', 200, 0, 4000, 200, 0, 25)
    f = TFile(inputfile, 'r')
    tree = f.Get('minitree')
    for idx in range(tree.GetEntries()):
        tree.GetEntry(idx)
        hM_NClusLayer1.Fill(tree.NClusLayer1)
        hM_NClusLayer1_MBDquantity.Fill(tree.NClusLayer1, tree.Centrality_mbdquantity)


    Draw_1Dhist_wPercentile(hM_NClusLayer1, NClusL1_percentile, False, False, True, 5, 'Number of clusters (Layer 3+4)', '', '{}/NClusLayer1_wPercentile'.format(plotdir))
    Draw_2Dhist_wPercentile(hM_NClusLayer1_MBDquantity, NClusL1_percentile, True, False, 0.13, 'Number of clusters (Layer 3+4)', 'MBD charge sum (N+S)', 'colz', '{}/NClusLayer1_MBDquantity'.format(plotdir))
