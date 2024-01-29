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

gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

def Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, outname):
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
    shift = 0.45 if prelim else 0.54
    legylow = 0.2 + 0.04 * (len(hsims) - 1)
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry(hdata, 'Data', "PE1");
    for i, lt in enumerate(simlegtex):
        leg.AddEntry(hsims[i], lt, "le");
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
    prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
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
    parser.add_option('-d', '--datahistdir', dest='datahistdir', type='string', default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/data_run20869/Hists_RecoTracklets_merged.root', help='Histogram file name (data)')
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
        os.system("hadd {}/hists_merged.root {}/hists_*.root".format(datahistdir, datahistdir))
    else:
        os.system("hadd {}/hists_merged.root {}/hists_*.root".format(datahistdir, datahistdir))
    
    for simhistd in simhistdir:
        if os.path.isfile("{}/hists_merged.root".format(simhistd)):
            os.system("rm {}/hists_merged.root".format(simhistd))
            os.system("hadd {}/hists_merged.root {}/hists_*.root".format(simhistd, simhistd))
        else:
            os.system("hadd {}/hists_merged.root {}/hists_*.root".format(simhistd, simhistd))

    os.makedirs('./DataSimComp/{}'.format(plotdir), exist_ok=True)

    hM_NClusLayer1_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NClusLayer1')
    hM_NPrototkl_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NPrototkl')
    hM_NRecotkl_Raw_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_NRecotkl_Raw')
    hM_dEta_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco') 
    hM_dEta_reco_altrange_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_altrange')
    hM_dEta_reco_altrange_Nclusle4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_altrange_Nclusle4000')
    hM_dEta_reco_altrange_Nclusgt4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dEta_reco_altrange_Nclusgt4000')
    hM_dPhi_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco')
    hM_dPhi_reco_altrange_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_altrange')
    hM_dPhi_reco_altrange_Nclusle4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_altrange_Nclusle4000')
    hM_dPhi_reco_altrange_Nclusgt4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dPhi_reco_altrange_Nclusgt4000')
    hM_dR_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dR_reco')
    hM_dR_reco_Nclusle4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dR_reco_Nclusle4000')
    hM_dR_reco_Nclusgt4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_dR_reco_Nclusgt4000')
    hM_Eta_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco')
    hM_Eta_reco_Nclusle4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Nclusle4000')
    hM_Eta_reco_Nclusgt4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Eta_reco_Nclusgt4000')
    hM_Phi_reco_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Phi_reco')
    hM_Phi_reco_Nclusle4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Phi_reco_Nclusle4000')
    hM_Phi_reco_Nclusgt4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_Phi_reco_Nclusgt4000')
    hM_RecoPVz_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_RecoPVz')
    hM_RecoPVz_Nclusle4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_RecoPVz_Nclusle4000')
    hM_RecoPVz_Nclusgt4000_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_RecoPVz_Nclusgt4000')
    hM_clusphi_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphi')
    hM_cluseta_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_cluseta')
    hM_clusphisize_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphisize')
    hM_cluseta_clusphisize_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_cluseta_clusphisize')
    hM_clusphi_clusphisize_data = GetHistogram("{}/hists_merged.root".format(datahistdir), 'hM_clusphi_clusphisize')

    l_hM_NClusLayer1_sim = []
    l_hM_NPrototkl_sim = []
    l_hM_NRecotkl_Raw_sim = []
    l_hM_dEta_reco_sim = []
    l_hM_dEta_reco_altrange_sim = []
    l_hM_dPhi_reco_sim = []
    l_hM_dPhi_reco_altrange_sim = []
    l_hM_dR_reco_sim = []
    l_hM_Eta_reco_sim = []
    l_hM_Phi_reco_sim = []
    l_hM_RecoPVz_sim = []
    l_hM_clusphi_sim = []
    l_hM_cluseta_sim = []
    l_hM_clusphisize_sim = []
    l_hM_cluseta_clusphisize_sim = []
    l_hM_clusphi_clusphisize_sim = []
    for i, simhistd in enumerate(simhistdir):
        l_hM_NClusLayer1_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NClusLayer1'))
        l_hM_NPrototkl_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NPrototkl'))
        l_hM_NRecotkl_Raw_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_NRecotkl_Raw'))
        l_hM_dEta_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco'))
        l_hM_dEta_reco_altrange_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dEta_reco_altrange'))
        l_hM_dPhi_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco'))
        l_hM_dPhi_reco_altrange_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dPhi_reco_altrange'))
        l_hM_dR_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_dR_reco'))
        l_hM_Eta_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Eta_reco'))
        l_hM_Phi_reco_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_Phi_reco'))
        l_hM_RecoPVz_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_RecoPVz'))
        l_hM_clusphi_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphi'))
        l_hM_cluseta_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_cluseta'))
        l_hM_clusphisize_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphisize'))
        l_hM_cluseta_clusphisize_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_cluseta_clusphisize'))
        l_hM_clusphi_clusphisize_sim.append(GetHistogram("{}/hists_merged.root".format(simhistd), 'hM_clusphi_clusphisize'))

    # Draw_1Dhist_datasimcomp(hdata, hsim, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, outname)
    hM_NClusLayer1_data.GetXaxis().SetMaxDigits(2)
    for hM_NClusLayer1_sim in l_hM_NClusLayer1_sim:
        hM_NClusLayer1_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NClusLayer1_data, l_hM_NClusLayer1_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Number of clusters (inner)', '', False, simlegtext, './DataSimComp/{}/NClusLayer1'.format(plotdir))
    hM_NPrototkl_data.GetXaxis().SetMaxDigits(2)
    for hM_NPrototkl_sim in l_hM_NPrototkl_sim:
        hM_NPrototkl_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NPrototkl_data, l_hM_NPrototkl_sim, [0.1,0.08,0.15,0.13], 'data', True, 3, 'Number of proto-tracklets', '', False, simlegtext, './DataSimComp/{}/NProtoTracklets'.format(plotdir))
    hM_NRecotkl_Raw_data.GetXaxis().SetMaxDigits(2)
    for hM_NRecotkl_Raw_sim in l_hM_NRecotkl_Raw_sim:
        hM_NRecotkl_Raw_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NRecotkl_Raw_data, l_hM_NRecotkl_Raw_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Number of reco-tracklets', '', False, simlegtext, './DataSimComp/{}/NRecoTracklets_Raw'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_RecoPVz_data, l_hM_RecoPVz_sim, [0.1,0.08,0.15,0.13], 'data', True, 250, 'Reco PV z [cm]', 'cm', False, simlegtext, './DataSimComp/{}/RecoPVz'.format(plotdir))
    # Draw_1Dhist_datasimcomp_v2(hM_RecoPVz_Nclusle4000_data, hM_RecoPVz_Nclusgt4000_data, l_hM_RecoPVz_sim, [0.1,0.08,0.15,0.13], 'unity', True, 250, 'Reco PV z [cm]', '', ['# of clusters (inner) < 4000', '# of clusters (inner) > 4000', 'Simulation'], False, './DataSimComp/{}/RecoPVz_Nclusle4000_Nclusgt4000'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_clusphi_data, l_hM_clusphi_sim, [0.1,0.08,0.15,0.13], 'data', False, 1.8, 'Cluster #phi', '', False, simlegtext, './DataSimComp/{}/Cluster_Phi'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_cluseta_data, l_hM_cluseta_sim, [0.1,0.08,0.15,0.13], 'data', False, 1.8, 'Cluster #eta', '', False, simlegtext, './DataSimComp/{}/Cluster_Eta'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_clusphisize_data, l_hM_clusphisize_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Cluster #phi size', '', False, simlegtext, './DataSimComp/{}/Cluster_PhiSize'.format(plotdir))

    Draw_1Dhist_datasimcomp(hM_dEta_reco_data, l_hM_dEta_reco_sim, [0.08,0.08,0.15,0.13], 'data', True, 250, 'Reco-tracklet #Delta#eta', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_dEta'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_altrange_data, l_hM_dEta_reco_altrange_sim, [0.08,0.08,0.15,0.13], 'data', True, 250, 'Reco-tracklet #Delta#eta', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_dEta_altrange'.format(plotdir))
    # Draw_1Dhist_datasimcomp_v2(hM_dEta_reco_altrange_Nclusle4000_data, hM_dEta_reco_altrange_Nclusgt4000_data, l_hM_dEta_reco_altrange_sim, [0.08,0.08,0.15,0.13], 'unity', True, 50, 'Reco-tracklet #Delta#eta', '', ['# of clusters (inner) < 4000', '# of clusters (inner) > 4000', 'Simulation'], False, simlegtext, './DataSimComp/{}/RecoTracklet_dEta_altrange_Nclusle4000_Nclusgt4000'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_data, l_hM_dPhi_reco_sim, [0.08,0.08,0.15,0.13], 'data', True, 500, 'Reco-tracklet #Delta#phi', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_dPhi'.format(plotdir))
    hM_dPhi_reco_altrange_data.GetXaxis().SetMaxDigits(2)
    for hM_dPhi_reco_altrange_sim in l_hM_dPhi_reco_altrange_sim:
        hM_dPhi_reco_altrange_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_altrange_data, l_hM_dPhi_reco_altrange_sim, [0.1,0.08,0.15,0.13], 'data', True, 150, 'Reco-tracklet #Delta#phi', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_dPhi_altrange'.format(plotdir))
    # Draw_1Dhist_datasimcomp_v2(hM_dPhi_reco_altrange_Nclusle4000_data, hM_dPhi_reco_altrange_Nclusgt4000_data, l_hM_dPhi_reco_altrange_sim, [0.1,0.08,0.15,0.13], 'unity', True, 50, 'Reco-tracklet #Delta#phi', '', ['# of clusters (inner) < 4000', '# of clusters (inner) > 4000', 'Simulation'], False, './DataSimComp/{}/RecoTracklet_dPhi_altrange_Nclusle4000_Nclusgt4000'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dR_reco_data, l_hM_dR_reco_sim, [0.08,0.08,0.15,0.13], 'data', True, 3, 'Reco-tracklet #DeltaR', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_dR'.format(plotdir))
    # Draw_1Dhist_datasimcomp_v2(hM_dR_reco_Nclusle4000_data, hM_dR_reco_Nclusgt4000_data, hM_dR_reco_sim, [0.08,0.08,0.15,0.13], 'unity', True, 50, 'Reco-tracklet #DeltaR', '', ['# of clusters (inner) < 4000', '# of clusters (inner) > 4000', 'Simulation'], False, './DataSimComp/{}/RecoTracklet_dR_Nclusle4000_Nclusgt4000'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Eta_reco_data, l_hM_Eta_reco_sim, [0.08,0.08,0.15,0.13], 'data', False, 1.8, 'Reco-tracklet #eta', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_Eta'.format(plotdir))
    # Draw_1Dhist_datasimcomp_v2(hM_Eta_reco_Nclusle4000_data, hM_Eta_reco_Nclusgt4000_data, hM_Eta_reco_sim, [0.08,0.08,0.15,0.13], 'unity', True, 750, 'Reco-tracklet #eta', '', ['# of clusters (inner) < 4000', '# of clusters (inner) > 4000', 'Simulation'], False, './DataSimComp/{}/RecoTracklet_Eta_Nclusle4000_Nclusgt4000'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_Phi_reco_data, l_hM_Phi_reco_sim, [0.08,0.08,0.15,0.13], 'data', False, 1.8, 'Reco-tracklet #phi', '', False, simlegtext, './DataSimComp/{}/RecoTracklet_Phi'.format(plotdir))
    hM_Phi_reco_Nclusle4000_data.GetYaxis().SetMaxDigits(2)
    hM_Phi_reco_Nclusgt4000_data.GetYaxis().SetMaxDigits(2)
    for hM_Phi_reco_sim in l_hM_Phi_reco_sim:
        hM_Phi_reco_sim.GetYaxis().SetMaxDigits(2)
    # Draw_1Dhist_datasimcomp_v2(hM_Phi_reco_Nclusle4000_data, hM_Phi_reco_Nclusgt4000_data, l_hM_Phi_reco_sim, [0.08,0.08,0.15,0.13], 'unity', False, 1.7, 'Reco-tracklet #phi', '', ['# of clusters (inner) < 4000', '# of clusters (inner) > 4000', 'Simulation'], False, './DataSimComp/{}/RecoTracklet_Phi_Nclusle4000_Nclusgt4000'.format(plotdir))

    # Draw_2Dhist_datasimcomp(hdata, hsim, logz, norm, rmargin, XaxisName, YaxisName, outname)
    Draw_2Dhist_datasimcomp(hM_cluseta_clusphisize_data, l_hM_cluseta_clusphisize_sim[0], False, 'data', 0.1, 'Cluster #eta', 'Cluster #phi size', './DataSimComp/{}/ClusEta_ClusPhiSize'.format(plotdir))
    Draw_2Dhist_datasimcomp(hM_clusphi_clusphisize_data, l_hM_clusphi_clusphisize_sim[0], False, 'data', 0.1, 'Cluster #phi', 'Cluster #phi size', './DataSimComp/{}/ClusPhi_ClusPhiSize'.format(plotdir))


