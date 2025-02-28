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

def Draw_1Dhist_datasimcomp_evtsel(hdatas, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, evtseltexts, outname):
    # make sure the number of histograms to compare are the same
    if len(hdatas) != len(hsims):
        print('Number of histograms to compare are not the same')
        sys.exit(1)
        
    nhists = len(hdatas)
    hcolor = ['#4a536b','#608BC1','#B03052']

    for i in range(nhists):
        hdatas[i].Sumw2()
        hsims[i].Sumw2()

    binwidth = hdatas[0].GetXaxis().GetBinWidth(1)

    # Get the maximum bin content 
    maxbincontent = -1
    minbincontent = 1E9
    if norm == 'unity':
        for i in range(nhists):
            hdatas[i].Scale(1.0 / hdatas[i].Integral(-1,-1))
            hsims[i].Scale(1.0 / hsims[i].Integral(-1,-1))
            maxbinc = max(hdatas[i].GetMaximum(), hsims[i].GetMaximum())
            minbinc = min(hdatas[i].GetMinimum(0), hsims[i].GetMinimum(0))
            if maxbinc > maxbincontent:
                maxbincontent = maxbinc
            if minbinc < minbincontent:
                minbincontent = minbinc
    elif norm == 'data': # normalized to sim to data 
        for i in range(nhists):
            hsims[i].Scale(hdatas[i].Integral(-1,-1) / hsims[i].Integral(-1,-1))
            maxbinc = max(hdatas[i].GetMaximum(), hsims[i].GetMaximum())
            minbinc = min(hdatas[i].GetMinimum(0), hsims[i].GetMinimum(0))
            if maxbinc > maxbincontent:
                maxbincontent = maxbinc
            if minbinc < minbincontent:
                minbincontent = minbinc
    else:
        if norm != 'none':
            print('Invalid normalization option: {}'.format(norm))
            sys.exit(1)

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
            hsim.SetLineColor(TColor.GetColor(hcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe')
        else:
            hsim.SetLineColor(TColor.GetColor(hcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe same')

    for i, hdata in enumerate(hdatas):
        hdata.SetMarkerStyle(20)
        hdata.SetMarkerSize(0.6)
        hdata.SetMarkerColor(TColor.GetColor(hcolor[i]))
        hdata.SetLineColor(TColor.GetColor(hcolor[i]))
        hdata.SetLineWidth(2)
        hdata.Draw('same PE1')
    
    # dummy hist for legend
    hdummy_data = hdatas[0].Clone('hdummy_data')
    hdummy_data.SetMarkerStyle(20)
    hdummy_data.SetMarkerSize(0.6)
    hdummy_data.SetLineWidth(2)
    hdummy_data.SetMarkerColor(kBlack)
    hdummy_data.SetLineColor(kBlack)
    hdummy_sim = hsims[0].Clone('hdummy_sim')
    hdummy_sim.SetLineWidth(2)
    hdummy_sim.SetLineColor(kBlack)
    # Legend
    shift = 0.43 if prelim else 0.73
    legylow = 0.15 + 0.0 * (len(hsims))
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.02)
    leg.SetTextSize(0.05)
    leg.SetFillStyle(0)
    # prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    prelimtext = 'Preliminary' if prelim else 'Internal'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.SetNColumns(2)
    leg.AddEntry(hdummy_data, 'Data', "pe")
    leg.AddEntry(hdummy_sim, 'HIJING', "le")
    leg.Draw('same')
    
    # event selection text
    legylow_evtselshift = 0.05 * len(evtseltexts)
    leg2 = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow-legylow_evtselshift,
                    (1-RightMargin)-0.3, (1-TopMargin)-legylow)
    leg2.SetTextSize(0.04)
    leg2.SetFillStyle(0)
    for i, evtseltext in enumerate(evtseltexts):
        leg2.AddEntry(hsims[i], evtseltext, 'l')
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
        hM_ratio = hdatas[i].Clone('hM_ratio')
        hM_ratio.Divide(hsim)
        # hM_ratio.SetMarkerStyle(20)
        # hM_ratio.SetMarkerSize(0.7)
        # hM_ratio.SetMarkerColor(TColor.GetColor(hcolor[i]))
        hM_ratio.SetLineColor(TColor.GetColor(hcolor[i]))
        hM_ratio.SetLineWidth(2)
        if i == 0:
            hM_ratio.GetYaxis().SetRangeUser(0.001, 1.999)
            # hM_ratio.GetYaxis().SetRangeUser(0.501, 1.499)
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
            hM_ratio.Draw('l hist')
        else:
            hM_ratio.Draw('l hist same')
            
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
    plotdir = './DataSimComp/EvtSelComp'
    if not os.path.exists(plotdir):
        os.makedirs(plotdir)
    
    datafilestocomp = [
        '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run54280_HotChannel_BCO/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_TrigBit13InttZvtx30cm/hists_merged.root',
        '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run54280_HotChannel_BCO/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_TrigBit12InttZvtx10cm/hists_merged.root',
        '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run54280_HotChannel_BCO/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_TrigBit12InttZvtx10cm_clusPhisizele6/hists_merged.root'
    ]
    
    simfilestocomp = [
        '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana443_20241102/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_TrigBit13InttZvtx30cm/hists_merged.root',
        '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana443_20241102/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_TrigBit12InttZvtx10cm/hists_merged.root',
        '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana443_20241102/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_TrigBit12InttZvtx10cm_clusPhisizele6/hists_merged.root'
    ]
    
    hdatas_NClusLayer1_zvtxwei = []
    hsims_NClusLayer1_zvtxwei = []
    hdatas_clusphi_zvtxwei = []
    hsims_clusphi_zvtxwei = []
    hdatas_cluseta_zvtxwei = []
    hsims_cluseta_zvtxwei = []
    hdatas_dEta_reco_altrange = []
    hsims_dEta_reco_altrange = []
    hdatas_dR_reco = []
    hsims_dR_reco = []
    hdatas_dPhi_reco_altrange = []
    hsims_dPhi_reco_altrange = []
    hdatas_Eta_reco = []
    hsims_Eta_reco = []
    hdatas_Phi_reco = []
    hsims_Phi_reco = []
    
    for datafile in datafilestocomp:
        hdatas_NClusLayer1_zvtxwei.append(GetHistogram(datafile, 'hM_NClusLayer1_zvtxwei'))
        hdatas_clusphi_zvtxwei.append(GetHistogram(datafile, 'hM_clusphi_zvtxwei'))
        hdatas_cluseta_zvtxwei.append(GetHistogram(datafile, 'hM_cluseta_zvtxwei'))
        hdatas_dEta_reco_altrange.append(GetHistogram(datafile, 'hM_dEta_reco_altrange'))
        hdatas_dR_reco.append(GetHistogram(datafile, 'hM_dR_reco'))
        hdatas_dPhi_reco_altrange.append(GetHistogram(datafile, 'hM_dPhi_reco_altrange'))
        hdatas_Eta_reco.append(GetHistogram(datafile, 'hM_Eta_reco'))
        hdatas_Phi_reco.append(GetHistogram(datafile, 'hM_Phi_reco'))
    for simfile in simfilestocomp:
        hsims_NClusLayer1_zvtxwei.append(GetHistogram(simfile, 'hM_NClusLayer1_zvtxwei'))
        hsims_clusphi_zvtxwei.append(GetHistogram(simfile, 'hM_clusphi_zvtxwei'))
        hsims_cluseta_zvtxwei.append(GetHistogram(simfile, 'hM_cluseta_zvtxwei'))
        hsims_dEta_reco_altrange.append(GetHistogram(simfile, 'hM_dEta_reco_altrange'))
        hsims_dR_reco.append(GetHistogram(simfile, 'hM_dR_reco'))
        hsims_dPhi_reco_altrange.append(GetHistogram(simfile, 'hM_dPhi_reco_altrange'))
        hsims_Eta_reco.append(GetHistogram(simfile, 'hM_Eta_reco'))
        hsims_Phi_reco.append(GetHistogram(simfile, 'hM_Phi_reco'))
            
    print (len(hdatas_NClusLayer1_zvtxwei), len(hsims_NClusLayer1_zvtxwei))
    padmargin = [0.08,0.06,0.15,0.32]
    # Draw_1Dhist_datasimcomp_evtsel(hdatas, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, evtseltexts, outname)
    Draw_1Dhist_datasimcomp_evtsel(hdatas_NClusLayer1_zvtxwei, hsims_NClusLayer1_zvtxwei, padmargin, 'data', True, 100, 'Number of clusters (inner)', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_NClusLayer1_zvtxwei')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_clusphi_zvtxwei, hsims_clusphi_zvtxwei, padmargin, 'data', False, 1.6, 'Cluster #phi', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_clusphi_zvtxwei')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_cluseta_zvtxwei, hsims_cluseta_zvtxwei, padmargin, 'data', False, 1.7, 'Cluster #eta', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_cluseta_zvtxwei')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_dEta_reco_altrange, hsims_dEta_reco_altrange, padmargin, 'data', True, 100, '#Delta#eta', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_dEta_reco_altrange')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_dR_reco, hsims_dR_reco, padmargin, 'data', True, 100, '#DeltaR', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_dR_reco')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_dPhi_reco_altrange, hsims_dPhi_reco_altrange, padmargin, 'data', True, 100, '#Delta#phi', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_dPhi_reco_altrange')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_Eta_reco, hsims_Eta_reco, padmargin, 'data', False, 1.7, 'Reco-tracklet #eta', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_Eta_reco')
    Draw_1Dhist_datasimcomp_evtsel(hdatas_Phi_reco, hsims_Phi_reco, padmargin, 'data', False, 1.6, 'Reco-tracklet #phi', '', False, ['Trigger bit 13 (MBD S&N#geq2, |Z_{vtx}|<30cm)', 'Trigger bit 12 (MBD S&N#geq2, |Z_{vtx}|<10cm)', 'Trigger bit 12, cluster#phi size<6, ADC>35'], plotdir+'/EvtSelComp_Phi_reco')