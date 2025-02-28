#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TCanvas, TFile, TLegend, TColor, TLine, gROOT, gPad, gSystem, kBlack, kRed, kBlue, kGreen, kOrange, kMagenta, kCyan, kYellow, kViolet, kAzure, kTeal, kSpring, kPink, kGray, kWhite, kDashed, kSolid, kDashDotted, kDashDotted, kDotted, kTRUE, kFALSE
import numpy
import math
import glob
from plotUtil import *

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

# def Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, evtseltexts, outname):
#     hsimcolor = colorset(len(hsims))

#     hdata.Sumw2()
#     for hsim in hsims:
#         hsim.Sumw2()

#     binwidth = hdata.GetXaxis().GetBinWidth(1)

#     if norm == 'unity':
#         hdata.Scale(1. / hdata.Integral(-1, -1))
#         for hsim in hsims:
#             hsim.Scale(1. / hsim.Integral(-1, -1))
#     elif norm == 'data':
#         for hsim in hsims:
#             hsim.Scale(hdata.Integral(-1, -1) / hsim.Integral(-1, -1))
#     else:
#         if norm != 'none':
#             print('Invalid normalization option: {}'.format(norm))
#             sys.exit(1)
    
#     # Get the maximum bin content 
#     maxbincontent = max(hdata.GetMaximum(), hsim.GetMaximum())

#     c = TCanvas('c', 'c', 800, 700)
#     if logy:
#         c.SetLogy()
#     c.cd()
#     gPad.SetRightMargin(gpadmargin[0])
#     gPad.SetTopMargin(gpadmargin[1])
#     gPad.SetLeftMargin(gpadmargin[2])
#     gPad.SetBottomMargin(gpadmargin[3])
    
#     for i, hsim in enumerate(hsims):
#         if i == 0:
#             if norm == 'unity':
#                 if Ytitle_unit == '':
#                     hsim.GetYaxis().SetTitle(
#                         'Normalized entries / ({:g})'.format(binwidth))
#                 else:
#                     hsim.GetYaxis().SetTitle(
#                         'Normalized entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
#             else:
#                 if Ytitle_unit == '':
#                     hsim.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
#                 else:
#                     hsim.GetYaxis().SetTitle(
#                         'Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))

#             if logy:
#                 hsim.GetYaxis().SetRangeUser(hsim.GetMinimum(0)*0.5, (hsim.GetMaximum()) * ymaxscale)
#             else:
#                 hsim.GetYaxis().SetRangeUser(0., (hsim.GetMaximum()) * ymaxscale)

#             hsim.GetXaxis().SetTitle(XaxisName)
#             hsim.GetXaxis().SetTitleOffset(1.1)
#             hsim.GetYaxis().SetTitleOffset(1.35)
#             hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
#             hsim.SetLineWidth(2)
#             hsim.SetMarkerSize(0)
#             hsim.Draw('histe')
#         else:
#             hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
#             hsim.SetLineWidth(2)
#             hsim.SetMarkerSize(0)
#             hsim.Draw('histe same')

#     hdata.SetMarkerStyle(20)
#     hdata.SetMarkerSize(1)
#     hdata.SetMarkerColor(1)
#     hdata.SetLineColor(1)
#     hdata.SetLineWidth(2)
#     hdata.Draw('same PE1')
#     shift = 0.45 if prelim else 0.75
#     legylow = 0.2 + 0.04 * (len(hsims) - 1)
#     leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
#                   (1-RightMargin)-0.1, (1-TopMargin)-0.03)
#     leg.SetTextSize(0.04)
#     leg.SetFillStyle(0)
#     # prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
#     prelimtext = 'Preliminary' if prelim else 'Internal'
#     leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
#     leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
#     leg.AddEntry(hdata, 'Data', "PE1")
#     for i, lt in enumerate(simlegtex):
#         leg.AddEntry(hsims[i], lt, "le")
#     leg.Draw('same')
    
#     # event selection text
#     legylow_evtselshift = 0.04 * len(evtseltexts)
#     leg2 = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow-legylow_evtselshift,
#                     (1-RightMargin)-0.1, (1-TopMargin)-legylow)
#     leg2.SetTextSize(0.035)
#     leg2.SetFillStyle(0)
#     for i, evtseltext in enumerate(evtseltexts):
#         leg2.AddEntry('', evtseltext, '')
#     leg2.Draw('same')
#     c.RedrawAxis()
#     c.Draw()
#     c.SaveAs(outname+'.pdf')
#     c.SaveAs(outname+'.png')
#     if(c):
#         c.Close()
#         gSystem.ProcessEvents()
#         del c
#         c = 0

def colorset_alt(i):
    if i == 1:
        return ['#0F4C75']
    elif i == 2:
        return ['#0F4C75', '#810000']
    elif i == 3:
        return ['#0F4C75', '#810000', '#7F167F']
    elif i == 4:
        return ['#0F4C75', '#810000', '#7F167F', '#5E8B7E']
    elif i == 5:
        return ['#0F4C75', '#810000', '#7F167F', '#5E8B7E', '#e99960']
    elif i == 6:
        return ['#0F4C75', '#810000', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB']
    elif i == 7:
        return ['#0F4C75', '#810000', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB', '#ffcc66']
    elif i == 8:
        return ['#0F4C75', '#810000', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']
    else:
        print ("Attempt to use more than 5 colors")
        return ['#0F4C75', '#810000', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']

def Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, evtseltexts, outname):
    hsimcolor = colorset_alt(len(hsims))

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
    minbincontent = min(hdata.GetMinimum(0), hsim.GetMinimum(0))

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
                hsim.GetYaxis().SetRangeUser(minbincontent*0.5, maxbincontent * ymaxscale)
            else:
                hsim.GetYaxis().SetRangeUser(1E-3, (hsim.GetMaximum()) * ymaxscale)

            # hsim.GetXaxis().SetTitle(XaxisName)
            # hsim.GetXaxis().SetTitleOffset(1.1)
            hsim.GetXaxis().SetLabelOffset(999)
            hsim.GetXaxis().SetLabelSize(0)
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
    legylow = 0.2 + 0.09 * (len(hsims))
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
                  (1-RightMargin)-0.15, (1-TopMargin)-0.03)
    leg.SetTextSize(0.05)
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
    legylow_evtselshift = 0.05 * len(evtseltexts)
    leg2 = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow-legylow_evtselshift,
                    (1-RightMargin)-0.15, (1-TopMargin)-legylow)
    leg2.SetTextSize(0.045)
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
        hM_ratio = hdata.Clone('hM_ratio'+str(i))
        hM_ratio.Divide(hsim)
        hM_ratio.SetMarkerStyle(20)
        hM_ratio.SetMarkerSize(0.7)
        hM_ratio.SetMarkerColor(TColor.GetColor(hsimcolor[i]))
        hM_ratio.SetLineColor(TColor.GetColor(hsimcolor[i]))
        hM_ratio.SetLineWidth(2)
        if i == 0:
            # hM_ratio.GetYaxis().SetRangeUser(0.001, 1.999)
            hM_ratio.GetYaxis().SetRangeUser(0.501, 1.499)
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

# Customized version for data-sim comparison with 2 data sets
def Draw_1Dhist_datasimcomp_v2(hdata1, hdata2, hsim, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, legtext, prelim, outname):
    hdata1.Sumw2()
    hdata2.Sumw2()
    hsim.Sumw2()
    binwidth = hdata1.GetXaxis().GetBinWidth(1)
    if norm == 'unity':
        hdata1.Scale(1. / hdata1.Integral(-1, -1))
        hdata2.Scale(1. / hdata2.Integral(-1, -1))
        hsim.Scale(1. / hsim.Integral(-1, -1))
    elif norm == 'data':
        hsim.Scale((hdata1.Integral(-1, -1)+hdata2.Integral(-1, -1)) / hsim.Integral(-1, -1))
    else:
        if norm != 'none':
            print('Invalid normalization option: {}'.format(norm))
            sys.exit(1)

    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(gpadmargin[0])
    gPad.SetTopMargin(gpadmargin[1])
    gPad.SetLeftMargin(gpadmargin[2])
    gPad.SetBottomMargin(gpadmargin[3])
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
    hsim.SetLineColor(1)
    hsim.SetLineWidth(2)
    hsim.Draw('hist')
    hdata1.SetMarkerStyle(20)
    hdata1.SetMarkerSize(1)
    hdata1.SetMarkerColor(TColor.GetColor('#0B60B0'))
    hdata1.SetLineColor(TColor.GetColor('#0B60B0'))
    hdata1.SetLineWidth(2)
    hdata1.Draw('same PE1')
    hdata2.SetMarkerStyle(20)
    hdata2.SetMarkerSize(1)
    hdata2.SetMarkerColor(TColor.GetColor('#D24545'))
    hdata2.SetLineColor(TColor.GetColor('#D24545'))
    hdata2.SetLineWidth(2)
    hdata2.Draw('same PE1')
    shift = 0.45 if prelim else 0.52
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-0.25,
                    (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    # prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    prelimtext = 'Preliminary' if prelim else 'Internal'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry(hdata1, legtext[0], "PE1");
    leg.AddEntry(hdata2, legtext[1], "PE1");
    leg.AddEntry(hsim, legtext[2], "l");
    leg.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

# comparae data and (one of the) simulation
def Draw_2Dhist_datasimcomp(hdata, hsim, logz, norm, rmargin, XaxisName, YaxisName, outname):
    if norm == 'unity':
        hdata.Scale(1. / hdata.Integral(-1, -1, -1, -1))
        hsim.Scale(1. / hsim.Integral(-1, -1, -1, -1))
    elif norm == 'data':
        hsim.Scale(hdata.Integral(-1, -1, -1, -1) / hsim.Integral(-1, -1, -1, -1))
    else:
        if norm != 'none':
            print('Invalid normalization option: {}'.format(norm))
            sys.exit(1)
    
    c = TCanvas('c', 'c', 800, 700)
    if logz:
        c.SetLogz()
    c.cd()
    gPad.SetRightMargin(rmargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    hdata.GetXaxis().SetTitle(XaxisName)
    hdata.GetYaxis().SetTitle(YaxisName)
    hdata.GetXaxis().SetTickSize(TickSize)
    hdata.GetYaxis().SetTickSize(TickSize)
    hdata.GetXaxis().SetTitleSize(AxisTitleSize)
    hdata.GetYaxis().SetTitleSize(AxisTitleSize)
    hdata.GetXaxis().SetLabelSize(AxisLabelSize)
    hdata.GetYaxis().SetLabelSize(AxisLabelSize)
    hdata.GetXaxis().SetTitleOffset(1.1)
    hdata.GetYaxis().SetTitleOffset(1.3)
    hdata.GetZaxis().SetLabelSize(AxisLabelSize)
    hdata.SetLineColor(kBlack)
    hdata.SetLineWidth(1)
    hdata.Draw('box')
    hsim.SetLineColorAlpha(kRed, 0.5)
    hsim.SetLineWidth(1)
    hsim.Draw('box same')
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
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('-d', '--datahistdir', dest='datahistdir', type='string', default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/data_run20869/', help='Histogram file name (data)')
    parser.add_option('-s', '--simhistdir', action='append', dest='simhistdir', type='string', help='Histogram file name (simulation). Example: /sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/ana382_zvtx-20cm_dummyAlignParams/Hists_RecoTracklets_merged.root')
    parser.add_option('-l', '--simlegtext', action='append', dest='simlegtext', type='string', help='Legend text for simulation. Example: HIJING/EPOS/AMPT)')
    parser.add_option('-p', '--plotdir', dest='plotdir', type='string', default='ana382_zvtx-20cm_dummyAlignParams', help='Plot directory')

    (opt, args) = parser.parse_args()

    print('opt: {}'.format(opt))

    datahistdir = opt.datahistdir
    simhistdir = opt.simhistdir
    simlegtext = opt.simlegtext
    plotdir = opt.plotdir

    if os.path.isfile("{}/hists_merged.root".format(datahistdir)):
        os.system("rm {}/hists_merged.root".format(datahistdir))
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(datahistdir, datahistdir))
    else:
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(datahistdir, datahistdir))
    
    for simhistd in simhistdir:
        if os.path.isfile("{}/hists_merged.root".format(simhistd)):
            os.system("rm {}/hists_merged.root".format(simhistd))
            os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(simhistd, simhistd))
        else:
            os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(simhistd, simhistd))

    os.makedirs('./DataSimComp/{}'.format(plotdir), exist_ok=True)

    hM_NClusLayer1_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NClusLayer1')
    hM_NClusLayer2_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NClusLayer2')
    hM_NClusLayer1_zvtxwei_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NClusLayer1_zvtxwei')
    hM_NTklclusLayer1_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NTklclusLayer1')
    hM_NRecotkl_Raw_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NRecotkl_Raw')
    hM_dEta_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco') 
    hM_dEta_reco_altrange_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_altrange')
    hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20')
    hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco')
    hM_dPhi_reco_altrange_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_altrange')
    hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_0to5_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_0to5')
    hM_dPhi_reco_Centrality_5to10_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_5to10')
    hM_dPhi_reco_Centrality_10to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_10to20')
    hM_dPhi_reco_Centrality_20to30_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_20to30')
    hM_dPhi_reco_Centrality_30to40_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_30to40')
    hM_dPhi_reco_Centrality_40to50_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_40to50')
    hM_dPhi_reco_Centrality_50to60_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_50to60')
    hM_dPhi_reco_Centrality_60to70_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_60to70')
    hM_dPhi_reco_Centrality_70to80_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_70to80')
    hM_dPhi_reco_Centrality_80to90_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_80to90')
    hM_dPhi_reco_Centrality_90to100_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_90to100')
    hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75')
    hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20')
    hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20')
    hM_dR_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dR_reco')
    hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco')
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20')
    hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20')
    hM_Phi_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Phi_reco')
    hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20') 
    hM_RecoPVz_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_RecoPVz')
    hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20')
    hM_MBDChargeAsymm_Le0p75_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_MBDChargeAsymm_Le0p75')
    hM_MBDChargeAsymm_Le0p75_VtxZm20to20_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_MBDChargeAsymm_Le0p75_VtxZm20to20')
    hM_clusphi_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphi')
    hM_clusphi_zvtxwei_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphi_zvtxwei')
    hM_cluseta_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_cluseta')
    hM_cluseta_zvtxwei_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_cluseta_zvtxwei')
    hM_clusphisize_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphisize')
    hM_clusadc_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusadc')
    hM_cluseta_clusphisize_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_cluseta_clusphisize')
    hM_clusphi_clusphisize_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphi_clusphisize')

    l_hM_NClusLayer1_sim = []
    l_hM_NClusLayer2_sim = []
    l_hM_NClusLayer1_zvtxwei_sim = []
    l_hM_NTklclusLayer1_sim = []
    l_hM_NRecotkl_Raw_sim = []
    l_hM_dEta_reco_sim = []
    l_hM_dEta_reco_altrange_sim = []
    l_hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_sim = []
    l_hM_dPhi_reco_altrange_sim = []
    l_hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_0to5_sim = []
    l_hM_dPhi_reco_Centrality_5to10_sim = []
    l_hM_dPhi_reco_Centrality_10to20_sim = []
    l_hM_dPhi_reco_Centrality_20to30_sim = []
    l_hM_dPhi_reco_Centrality_30to40_sim = []
    l_hM_dPhi_reco_Centrality_40to50_sim = []
    l_hM_dPhi_reco_Centrality_50to60_sim = []
    l_hM_dPhi_reco_Centrality_60to70_sim = []
    l_hM_dPhi_reco_Centrality_70to80_sim = []
    l_hM_dPhi_reco_Centrality_80to90_sim = []
    l_hM_dPhi_reco_Centrality_90to100_sim = []
    l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_sim = []
    l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_dR_reco_sim = []
    l_hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_sim = []
    l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_Phi_reco_sim = []
    l_hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_RecoPVz_sim = []
    l_hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20_sim = []
    l_hM_MBDChargeAsymm_Le0p75_sim = []
    l_hM_MBDChargeAsymm_Le0p75_VtxZm20to20_sim = []
    l_hM_clusphi_sim = []
    l_hM_clusphi_zvtxwei_sim = []
    l_hM_cluseta_sim = []
    l_hM_cluseta_zvtxwei_sim = []
    l_hM_clusphisize_sim = []
    l_hM_clusadc_sim = []
    l_hM_cluseta_clusphisize_sim = []
    l_hM_clusphi_clusphisize_sim = []
    for i, simhistd in enumerate(simhistdir):
        l_hM_NClusLayer1_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NClusLayer1'))
        l_hM_NClusLayer2_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NClusLayer2'))
        l_hM_NClusLayer1_zvtxwei_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NClusLayer1_zvtxwei'))
        l_hM_NTklclusLayer1_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NTklclusLayer1'))
        l_hM_NRecotkl_Raw_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NRecotkl_Raw'))
        l_hM_dEta_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco'))
        l_hM_dEta_reco_altrange_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_altrange'))
        l_hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco'))
        l_hM_dPhi_reco_altrange_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_altrange'))
        l_hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_0to5_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_0to5'))
        l_hM_dPhi_reco_Centrality_5to10_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_5to10'))
        l_hM_dPhi_reco_Centrality_10to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_10to20'))
        l_hM_dPhi_reco_Centrality_20to30_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_20to30'))
        l_hM_dPhi_reco_Centrality_30to40_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_30to40'))
        l_hM_dPhi_reco_Centrality_40to50_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_40to50'))
        l_hM_dPhi_reco_Centrality_50to60_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_50to60'))
        l_hM_dPhi_reco_Centrality_60to70_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_60to70'))
        l_hM_dPhi_reco_Centrality_70to80_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_70to80'))
        l_hM_dPhi_reco_Centrality_80to90_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_80to90'))
        l_hM_dPhi_reco_Centrality_90to100_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_90to100'))
        l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75'))
        l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_dR_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dR_reco'))
        l_hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco'))
        l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_Phi_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Phi_reco'))
        l_hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_RecoPVz_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_RecoPVz'))
        l_hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20'))
        l_hM_MBDChargeAsymm_Le0p75_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_MBDChargeAsymm_Le0p75'))
        l_hM_MBDChargeAsymm_Le0p75_VtxZm20to20_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_MBDChargeAsymm_Le0p75_VtxZm20to20'))
        l_hM_clusphi_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphi'))
        l_hM_clusphi_zvtxwei_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphi_zvtxwei'))
        l_hM_cluseta_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_cluseta'))
        l_hM_cluseta_zvtxwei_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_cluseta_zvtxwei'))
        l_hM_clusphisize_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphisize'))
        l_hM_clusadc_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusadc'))
        l_hM_cluseta_clusphisize_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_cluseta_clusphisize'))
        l_hM_clusphi_clusphisize_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphi_clusphisize'))
        
    
    str_clussel = 'Cluster ADC>35'
    str_zvtxsel = '|INTT&MBD Z_{vtx}|#leq10cm'
    str_evtsel = 'Trigger bit 10 (MBD N&S#geq 2), Is MinBias'
    str_globsel = ''
    # str_clussel = 'Cluster ADC > 30#timescosh(#eta)'
    padmargin = [0.08,0.06,0.15,0.32]

    # Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, evtseltexts, evtseltextpos, outname)
    hM_NClusLayer1_data.GetXaxis().SetMaxDigits(2)
    for hM_NClusLayer1_sim in l_hM_NClusLayer1_sim:
        hM_NClusLayer1_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NClusLayer1_data, l_hM_NClusLayer1_sim, padmargin, 'data', True, 500, 'Number of clusters (inner)', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/NClusLayer1'.format(plotdir))
    
    hM_NClusLayer2_data.GetXaxis().SetMaxDigits(2)
    for hM_NClusLayer2_sim in l_hM_NClusLayer2_sim:
        hM_NClusLayer2_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NClusLayer2_data, l_hM_NClusLayer2_sim, padmargin, 'data', True, 500, 'Number of clusters (outer)', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/NClusLayer2'.format(plotdir))
    
    hM_NClusLayer1_zvtxwei_data.GetXaxis().SetMaxDigits(2)
    for hM_NClusLayer1_zvtxwei_sim in l_hM_NClusLayer1_zvtxwei_sim:
        hM_NClusLayer1_zvtxwei_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NClusLayer1_zvtxwei_data, l_hM_NClusLayer1_zvtxwei_sim, padmargin, 'data', True, 500, 'Number of clusters (inner)', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/NClusLayer1_zvtxwei'.format(plotdir))
    
    hM_NTklclusLayer1_data.GetXaxis().SetMaxDigits(2)
    for hM_NTklclusLayer1_sim in l_hM_NTklclusLayer1_sim:
        hM_NTklclusLayer1_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NTklclusLayer1_data, l_hM_NTklclusLayer1_sim, padmargin, 'data', True, 500, 'Number of tracklet clusters (inner)', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/NTklClusLayer1'.format(plotdir))
    
    hM_NRecotkl_Raw_data.GetXaxis().SetMaxDigits(2)
    for hM_NRecotkl_Raw_sim in l_hM_NRecotkl_Raw_sim:
        hM_NRecotkl_Raw_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NRecotkl_Raw_data, l_hM_NRecotkl_Raw_sim, padmargin, 'data', True, 500, 'Number of reco-tracklets', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/NRecoTracklets_Raw'.format(plotdir))
    
    Draw_1Dhist_datasimcomp(hM_RecoPVz_data, l_hM_RecoPVz_sim, padmargin, 'data', False, 1.7, 'INTT Z_{vtx} [cm]', 'cm', False, simlegtext, ['{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoPVz'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20_data, l_hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 1.7, 'INTT Z_{vtx} [cm]', 'cm', False, simlegtext, ['{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))+', |Asymm._{MBD}|#leq0.75'], './DataSimComp/{}/RecoPVz_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_MBDChargeAsymm_Le0p75_data, l_hM_MBDChargeAsymm_Le0p75_sim, padmargin, 'data', True, 15, 'MBD charge asymmetry', '', False, simlegtext, ['|Asymm._{MBD}|#leq0.75'], './DataSimComp/{}/MBDChargeAsymm'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_MBDChargeAsymm_Le0p75_VtxZm20to20_data, l_hM_MBDChargeAsymm_Le0p75_VtxZm20to20_sim, padmargin, 'data', True, 15, 'MBD charge asymmetry', '', False, simlegtext, ['{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))+', |Asymm._{MBD}|#leq0.75'], './DataSimComp/{}/MBDChargeAsymm_VtxZm20to20'.format(plotdir))
    
    Draw_1Dhist_datasimcomp(hM_clusphi_data, l_hM_clusphi_sim, padmargin, 'data', False, 1.8, 'Cluster #phi', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/Cluster_Phi'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_clusphi_zvtxwei_data, l_hM_clusphi_zvtxwei_sim, padmargin, 'data', False, 1.8, 'Cluster #phi', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/Cluster_Phi_zvtxwei'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_cluseta_data, l_hM_cluseta_sim, padmargin, 'data', False, 1.8, 'Cluster #eta', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/Cluster_Eta'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_cluseta_zvtxwei_data, l_hM_cluseta_zvtxwei_sim, padmargin, 'data', False, 1.8, 'Cluster #eta', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/Cluster_Eta_zvtxwei'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_clusphisize_data, l_hM_clusphisize_sim, padmargin, 'data', True, 100, 'Cluster #phi size', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/Cluster_PhiSize'.format(plotdir))
    hM_clusadc_data.GetXaxis().SetMaxDigits(3)
    hM_clusadc_data.GetXaxis().SetNdivisions(-10)
    for hM_clusadc_sim in l_hM_clusadc_sim:
        hM_clusadc_sim.GetXaxis().SetMaxDigits(3)
        hM_clusadc_sim.GetXaxis().SetNdivisions(-10)
    Draw_1Dhist_datasimcomp(hM_clusadc_data, l_hM_clusadc_sim, padmargin, 'data', True, 100, 'Cluster ADC', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/Cluster_ADC'.format(plotdir))

    Draw_1Dhist_datasimcomp(hM_dEta_reco_data, l_hM_dEta_reco_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dEta'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_altrange_data, l_hM_dEta_reco_altrange_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dEta_altrange'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 0-70%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 800, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 0-5%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 5-10%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 10-20%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 20-30%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 30-40%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 40-50, |Asymm._{MBD}|#leq0.75', '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dEta_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 50-60%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 60-70%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 70-80%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 80-90%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dEta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#eta', '', False, simlegtext, ['Centrality 90-100%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dEta_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_data, l_hM_dPhi_reco_sim, padmargin, 'data', True, 750, 'Reco-tracklet #Delta#phi', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi'.format(plotdir))
    
    hM_dPhi_reco_altrange_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_0to5_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_5to10_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_10to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_20to30_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_30to40_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_40to50_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_50to60_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_60to70_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_70to80_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_80to90_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_90to100_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data.GetXaxis().SetMaxDigits(2)
    for i in range(len(l_hM_dPhi_reco_altrange_sim)):
        l_hM_dPhi_reco_altrange_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_0to5_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_5to10_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_10to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_20to30_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_30to40_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_40to50_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_50to60_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_60to70_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_70to80_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_80to90_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_90to100_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim[i].GetXaxis().SetMaxDigits(2)
        
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_altrange_data, l_hM_dPhi_reco_altrange_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_altrange'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 0-70%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dPhi_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_0to5_data, l_hM_dPhi_reco_Centrality_0to5_sim, padmargin, 'data', True, 100, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 0-5%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_0to5'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_5to10_data, l_hM_dPhi_reco_Centrality_5to10_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 5-10%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_5to10'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_10to20_data, l_hM_dPhi_reco_Centrality_10to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 10-20%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_10to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_20to30_data, l_hM_dPhi_reco_Centrality_20to30_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 20-30%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_20to30'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_30to40_data, l_hM_dPhi_reco_Centrality_30to40_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 30-40%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_30to40'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_40to50_data, l_hM_dPhi_reco_Centrality_40to50_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 40-50%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_40to50'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_50to60_data, l_hM_dPhi_reco_Centrality_50to60_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 50-60%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_50to60'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_60to70_data, l_hM_dPhi_reco_Centrality_60to70_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 60-70%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_60to70'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_70to80_data, l_hM_dPhi_reco_Centrality_70to80_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 70-80%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_70to80'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_80to90_data, l_hM_dPhi_reco_Centrality_80to90_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 80-90%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_80to90'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_90to100_data, l_hM_dPhi_reco_Centrality_90to100_sim, padmargin, 'data', True, 100, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 90-100%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_90to100'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_sim, padmargin, 'data', True, 100, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 0-5%, |Asymm._{MBD}|#leq0.75','|Asymm._{MBD}|#leq0.75}'], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_0to5_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 5-10%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_5to10_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 10-20%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_10to20_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 20-30%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_20to30_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 30-40%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_30to40_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 40-50%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_40to50_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 50-60%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_50to60_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 60-70%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_60to70_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 70-80%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_70to80_MBDAsymLe0p75'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 80-90%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_80to90_MBDAsymLe0p75'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_data, l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_sim, padmargin, 'data', True, 100, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 90-100%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_90to100_MBDAsymLe0p75'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 100, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 0-5%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 5-10%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 10-20%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 20-30%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 30-40%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 40-50%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 50-60%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 60-70%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 70-80%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 1E3, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 80-90%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dPhi_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 100, 'Reco-tracklet #Delta#phi', '', False, simlegtext, ['Centrality 90-10%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dPhi_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    
    Draw_1Dhist_datasimcomp(hM_dR_reco_data, l_hM_dR_reco_sim, padmargin, 'data', True, 30, 'Reco-tracklet #DeltaR', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_dR'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', True, 30, 'Reco-tracklet #DeltaR', '', False, simlegtext, ['Centrality 0-70%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_dR_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_data, l_hM_Eta_reco_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 0-70%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_Eta_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 0-5%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_Eta_Centrality_0to5_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 5-10%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_5to10_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 10-20%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_10to20_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 20-30%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_20to30_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 30-40%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_30to40_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 40-50%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_40to50_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 50-60%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_50to60_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 60-70%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_60to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 70-80%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_70to80_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 80-90%', str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Eta_Centrality_80to90_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    # Draw_1Dhist_datasimcomp(hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Eta_reco_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2, 'Reco-tracklet #eta', '', False, simlegtext, ['Centrality 90-100%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_Eta_Centrality_90to100_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    
    for hM_Phi_reco_sim in l_hM_Phi_reco_sim:
        hM_Phi_reco_sim.GetYaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_Phi_reco_data, l_hM_Phi_reco_sim, padmargin, 'data', False, 1.8, 'Reco-tracklet #phi', '', False, simlegtext, [str_evtsel, '{}, {}'.format(str_zvtxsel,str_clussel)], './DataSimComp/{}/RecoTracklet_Phi'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_data, l_hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20_sim, padmargin, 'data', False, 2.0, 'Reco-tracklet #phi', '', False, simlegtext, ['Centrality 0-70%', str_evtsel, '{}'.format('{}, {}'.format(str_zvtxsel,str_clussel))], './DataSimComp/{}/RecoTracklet_Phi_Centrality0to70_MBDAsymLe0p75_VtxZm20to20'.format(plotdir))
    
    # Draw_2Dhist_datasimcomp(hdata, hsim, logz, norm, rmargin, XaxisName, YaxisName, outname)
    Draw_2Dhist_datasimcomp(hM_cluseta_clusphisize_data, l_hM_cluseta_clusphisize_sim[0], False, 'data', 0.1, 'Cluster #eta', 'Cluster #phi size', './DataSimComp/{}/ClusEta_ClusPhiSize'.format(plotdir))
    Draw_2Dhist_datasimcomp(hM_clusphi_clusphisize_data, l_hM_clusphi_clusphisize_sim[0], False, 'data', 0.1, 'Cluster #phi', 'Cluster #phi size', './DataSimComp/{}/ClusPhi_ClusPhiSize'.format(plotdir))


