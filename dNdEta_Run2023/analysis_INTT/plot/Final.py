#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TFile, TCanvas, TPad, gPad, TLegend, TColor, gROOT, TGraphErrors, TGraphAsymmErrors, TGaxis, kHAlignLeft, kVAlignTop, kHAlignRight, kVAlignBottom
import numpy
import math
import glob
from array import array
import json
from plotUtil import GetHistogram
from measurements.measurements import *
import numpy as np
from closure import GetMbinNum

gROOT.SetBatch(True)

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.22
RightMargin = 0.06
TopMargin = 0.05
BottomMargin = 0.13
FillAlpha = 0.5

include_rawhijing = False

# compute uncertainty - weighted average (! valid only for uncorrelated uncertainties, for example RHIC combination !)
def weighted_average(values, uncertainties):
    """
    Computes the weighted average and its uncertainty for a given set of values and uncertainties.
    
    Parameters:
    values (list or array): Data points.
    uncertainties (list or array): Corresponding uncertainties of the data points.
    
    Returns:
    tuple: Weighted average and its uncertainty.
    """
    values = np.array(values)
    uncertainties = np.array(uncertainties)
    
    # Compute weights
    weights = 1 / uncertainties**2
    
    # Compute weighted average
    weighted_avg = np.sum(weights * values) / np.sum(weights)
    
    # Compute uncertainty in weighted average
    weighted_uncertainty = np.sqrt(1 / np.sum(weights))
    
    return weighted_avg, weighted_uncertainty
    
def average(values, uncertainties):
    values = np.array(values)
    uncertainties = np.array(uncertainties)
    
    avg = np.mean(values)
    avg_err = np.mean(uncertainties)
    
    return avg, avg_err


def msrmnt_sphenix_generator_auau_200gev(fnameprefix):
    centralitybin = sphenix_centrality_interval()
    centnpart, centnparterr = sphenix_centralitynpart()
    
    dNdEta_eta0, dNdEta_eta0_divnpart2, Centrality, dNdEta_eta0_err, dNdEta_eta0_divnpart2_err, Centrality_err = [], [], [], [], [], []
    
    for i in range(len(centralitybin) - 1):
        # hM_dNdEtafinal = GetHistogram('./corrections/HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/Centrality{}to{}_Zvtxm10p0to10p0_noasel/correction_hists.root'.format(centralitybin[i],centralitybin[i+1]), 'h1WGhadron')
        hM_dNdEtafinal = GetHistogram('./corrections/{}/Centrality{}to{}_Zvtxm10p0to10p0_noasel/correction_hists.root'.format(fnameprefix,centralitybin[i],centralitybin[i+1]), 'h1WGhadron')

        dNdEta_eta0_centrality = (hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))) / 3.
        list_dNdEta_eta0 = [hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))]
        list_dNdEta_eta0_err = [hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0.2))]
        dNdEta_eta0_centrality, dNdEta_eta0_err_centrality = average(list_dNdEta_eta0, list_dNdEta_eta0_err)
        # print ('dNdEta_eta0_centrality={}, dNdta_eta0_err_centrality={}'.format(dNdEta_eta0_centrality, dNdEta_eta0_err_centrality))
        rel_err_dNdEta = dNdEta_eta0_err_centrality / dNdEta_eta0_centrality
        rel_err_npart = centnparterr[i] / centnpart[i]
        dNdEta_eta0_divnpart2_err_centrality = dNdEta_eta0_centrality / (centnpart[i] / 2.) * math.sqrt(rel_err_dNdEta**2 + rel_err_npart**2) # error propagation; TODO: include the uncertainty for <npart>
        
        dNdEta_eta0.append(dNdEta_eta0_centrality)
        dNdEta_eta0_divnpart2.append(dNdEta_eta0_centrality / (centnpart[i] / 2.))
        Centrality.append((centralitybin[i] + centralitybin[i+1]) / 2.)
        dNdEta_eta0_err.append(dNdEta_eta0_err_centrality)
        dNdEta_eta0_divnpart2_err.append(0)
        Centrality_err.append(0)


    dNdEta_eta0 = array('f', dNdEta_eta0) 
    dNdEta_eta0_divnpart2 = array('f', dNdEta_eta0_divnpart2)
    Centrality = array('f', Centrality)
    dNdEta_eta0_err = array('f', dNdEta_eta0_err)
    dNdEta_eta0_divnpart2_err = array('f', dNdEta_eta0_divnpart2_err)
    Centrality_err = array('f', Centrality_err)
    centnpart = array('f', centnpart)
    centnparterr = array('f', centnparterr)
    gsphenix_rawhijing_auau_200gev = TGraphErrors(len(Centrality), Centrality, dNdEta_eta0, Centrality_err, dNdEta_eta0_err)
    gsphenix_rawhijing_divnpart2_auau_200gev = TGraphErrors(len(centnpart), centnpart, dNdEta_eta0_divnpart2, centnparterr, dNdEta_eta0_divnpart2_err)

    return gsphenix_rawhijing_auau_200gev, gsphenix_rawhijing_divnpart2_auau_200gev

def msrmnt_generator_dNdetaNpart2_auau_200gev(filepath):
    centnpart, centnparterr = sphenix_centralitynpart()
    npart_max = max(centnpart)
    npart_min = min(centnpart)
    
    file = TFile(filepath, 'READ')
    g_truthdNdEta = file.Get('g_truthdNdEta')
    
    # exclude points outside npart_min and npart_max
    # Iterate in reverse order to avoid index issues when removing points
    for i in range(g_truthdNdEta.GetN()-1, -1, -1):
        if g_truthdNdEta.GetX()[i] < npart_min or g_truthdNdEta.GetX()[i] > npart_max:
            g_truthdNdEta.RemovePoint(i)
    
    return g_truthdNdEta


def msrmnt_sphenix_data_auau_200gev(approach='cms'): # approach: 'cms', 'phobos', 'combined'
    centralitybin = sphenix_centrality_interval()
    centnpart, centnparterr = sphenix_centralitynpart()
    
    dNdEta_eta0, dNdEta_eta0_divnpart2, Centrality, dNdEta_eta0_err, dNdEta_eta0_divnpart2_err, Centrality_err = [], [], [], [], [], []
    
    for i in range(len(centralitybin) - 1):
        if approach == 'cms':
            hM_dNdEtafinal = GetHistogram('./systematics/Centrality{}to{}_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality{}to{}_Zvtxm10p0to10p0_noasel.root'.format(centralitybin[i],centralitybin[i+1],centralitybin[i],centralitybin[i+1]), 'hM_final')
        elif approach == 'phobos':
            mbin = GetMbinNum('Centrality{}to{}'.format(centralitybin[i],centralitybin[i+1]))
            hM_dNdEtafinal = GetHistogram('/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p026/completed/vtxZ_-10_10cm_MBin{}/Final_Mbin{}_00054280/Final_Mbin{}_00054280.root'.format(mbin,mbin,mbin), 'h1D_dNdEta_reco')
        elif approach == 'combined':
            # hM_dNdEtafinal_cms = GetHistogram('./systematics/Centrality{}to{}_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality{}to{}_Zvtxm10p0to10p0_noasel.root'.format(centralitybin[i],centralitybin[i+1],centralitybin[i],centralitybin[i+1]), 'hM_final')
            # mbin = GetMbinNum('Centrality{}to{}'.format(centralitybin[i],centralitybin[i+1]))
            # hM_dNdEtafinal_phobos = GetHistogram('/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/EvtVtxZ/FinalResult/completed/vtxZ_-10_10cm_MBin{}/Final_Mbin{}_00054280/Final_Mbin{}_00054280.root'.format(mbin,mbin,mbin), 'h1D_dNdEta_reco')
            fcombined = TFile('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/systematics/combined/combined.root')
            tgae_dNdEtafinal = fcombined.Get('tgae_combine_Centrality{}to{}'.format(centralitybin[i],centralitybin[i+1]))
            hM_dNdEtafinal = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/systematics/combined/combined.root', 'h_combined_Centrality{}to{}'.format(centralitybin[i],centralitybin[i+1]))
        else:
            sys.exit('Approach {} is not supported. Use default approach of cms'.format(approach))

        if approach == 'cms' or approach == 'phobos':
            dNdEta_eta0_centrality = (hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))) / 3.
            list_dNdEta_eta0 = [hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))]
            list_dNdEta_eta0_err = [hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0.2))]
            dNdEta_eta0_centrality, dNdEta_eta0_err_centrality = average(list_dNdEta_eta0, list_dNdEta_eta0_err)
            # print ('dNdEta_eta0_centrality={}, dNdta_eta0_err_centrality={}'.format(dNdEta_eta0_centrality, dNdEta_eta0_err_centrality))
            rel_err_dNdEta = dNdEta_eta0_err_centrality / dNdEta_eta0_centrality
            rel_err_npart = centnparterr[i] / centnpart[i]
            dNdEta_eta0_divnpart2_err_centrality = dNdEta_eta0_centrality / (centnpart[i] / 2.) * math.sqrt(rel_err_dNdEta**2 + rel_err_npart**2) 
            
            # print('Approach {}: centrality {}-{}, dNdeta +- uncertainty = {:.3f} +- {:.3f} ({:.3f}%), dNdeta/(0.5 * <npart>) +- uncertainty = {:.3f} +- {:.3f} ({:.3f}%)'.format(approach, 
            #                                                                                                                                                                      centralitybin[i], 
            #                                                                                                                                                                      centralitybin[i+1], 
            #                                                                                                                                                                      dNdEta_eta0_centrality, 
            #                                                                                                                                                                      dNdEta_eta0_err_centrality, 
            #                                                                                                                                                                      dNdEta_eta0_err_centrality / dNdEta_eta0_centrality * 100, 
            #                                                                                                                                                                      dNdEta_eta0_centrality / (centnpart[i] / 2.), 
            #                                                                                                                                                                      dNdEta_eta0_divnpart2_err_centrality, 
            #                                                                                                                                                                      dNdEta_eta0_divnpart2_err_centrality / (dNdEta_eta0_centrality / (centnpart[i] / 2.)) * 100))
            
            dNdEta_eta0.append(dNdEta_eta0_centrality)
            dNdEta_eta0_divnpart2.append(dNdEta_eta0_centrality / (centnpart[i] / 2.))
            Centrality.append((centralitybin[i] + centralitybin[i+1]) / 2.)
            dNdEta_eta0_err.append(dNdEta_eta0_err_centrality)
            dNdEta_eta0_divnpart2_err.append(dNdEta_eta0_divnpart2_err_centrality)
            Centrality_err.append(0)
        elif approach == 'combined':            
            # use the histogram to get the indices of the bins, then use the indices to get the values and errors from the TGraphAsymmErrors
            # print ('Eta = 0, dNdEta combined = {:.3f} +- {:.3f}'.format(tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(0)-1], tgae_dNdEtafinal.GetEYlow()[hM_dNdEtafinal.GetXaxis().FindBin(0)-1]))
            # print ('Eta = -0.2, dNdEta combined = {:.3f} +- {:.3f}'.format(tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(-0.2)-1], tgae_dNdEtafinal.GetEYlow()[hM_dNdEtafinal.GetXaxis().FindBin(-0.2)-1]))
            # print ('Eta = 0.2, dNdEta combined = {:.3f} +- {:.3f}'.format(tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(0.2)-1], tgae_dNdEtafinal.GetEYlow()[hM_dNdEtafinal.GetXaxis().FindBin(0.2)-1]))
            dNdEta_eta0_centrality = (tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(0)-1] + tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(-0.2)-1] + tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(0.2)-1]) / 3.
            list_dNdEta_eta0 = [tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(0)-1], tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(-0.2)-1], tgae_dNdEtafinal.GetY()[hM_dNdEtafinal.GetXaxis().FindBin(0.2)-1]]
            list_dNdEta_eta0_err = [tgae_dNdEtafinal.GetEYlow()[hM_dNdEtafinal.GetXaxis().FindBin(0)-1], tgae_dNdEtafinal.GetEYlow()[hM_dNdEtafinal.GetXaxis().FindBin(-0.2)-1], tgae_dNdEtafinal.GetEYlow()[hM_dNdEtafinal.GetXaxis().FindBin(0.2)-1]]
            dNdEta_eta0_centrality, dNdEta_eta0_err_centrality = average(list_dNdEta_eta0, list_dNdEta_eta0_err)
            rel_err_dNdEta = dNdEta_eta0_err_centrality / dNdEta_eta0_centrality
            rel_err_npart = centnparterr[i] / centnpart[i]
            dNdEta_eta0_divnpart2_err_centrality = dNdEta_eta0_centrality / (centnpart[i] / 2.) * math.sqrt(rel_err_dNdEta**2 + rel_err_npart**2)
            # print in the Latex table format
            precision = '.2f'  # Default precision, can be changed to '.2f' or any other format
            # print ('{}\\%-{}\\% & ${:{}}\\pm{:{}}$ & ${:{}}\\pm{:{}}$ & ${:{}}\\pm{:{}}$ \\\\'.format(
            #     centralitybin[i], centralitybin[i+1], 
            #     dNdEta_eta0_centrality, precision, dNdEta_eta0_err_centrality, precision, 
            #     centnpart[i], precision, centnparterr[i], precision,
            #     dNdEta_eta0_centrality / (centnpart[i] / 2.), precision, dNdEta_eta0_divnpart2_err_centrality, precision))
            
            dNdEta_eta0.append(dNdEta_eta0_centrality)
            dNdEta_eta0_divnpart2.append(dNdEta_eta0_centrality / (centnpart[i] / 2.))
            Centrality.append((centralitybin[i] + centralitybin[i+1]) / 2.)
            dNdEta_eta0_err.append(dNdEta_eta0_err_centrality)
            dNdEta_eta0_divnpart2_err.append(dNdEta_eta0_divnpart2_err_centrality)
            Centrality_err.append(0)
        else:
            sys.exit('Approach {} is not supported. Use default approach of cms'.format(approach))

    dNdEta_eta0 = array('f', dNdEta_eta0)
    dNdEta_eta0_divnpart2 = array('f', dNdEta_eta0_divnpart2)
    Centrality = array('f', Centrality)
    dNdEta_eta0_err = array('f', dNdEta_eta0_err)
    dNdEta_eta0_divnpart2_err = array('f', dNdEta_eta0_divnpart2_err)
    Centrality_err = array('f', Centrality_err)
    centnpart = array('f', centnpart)
    centnparterr = array('f', centnparterr)
    gsphenix_data_auau_200gev = TGraphAsymmErrors(len(Centrality), Centrality, dNdEta_eta0, Centrality_err, Centrality_err, dNdEta_eta0_err, dNdEta_eta0_err)
    gsphenix_data_divnpart2_auau_200gev = TGraphAsymmErrors(len(centnpart), centnpart, dNdEta_eta0_divnpart2, centnparterr, centnparterr, dNdEta_eta0_divnpart2_err, dNdEta_eta0_divnpart2_err)

    return gsphenix_data_auau_200gev, gsphenix_data_divnpart2_auau_200gev
    

def msrmnt_dict():
    msrmntdict = {}

    msrmntdict['cms_pbpb_2p76'] = cms_pbpb_2p76()
    msrmntdict['cms_xexe_5p44'] = cms_xexe_5p44()
    msrmntdict['atlas_pbpb_2p76'] = atlas_pbpb_2p76()
    msrmntdict['alice_pbpb_2p76'] = alice_pbpb_2p76()
    msrmntdict['alice_pbpb_5p02'] = alice_pbpb_5p02()
    msrmntdict['alice_xexe_5p44'] = alice_xexe_5p44()
    msrmntdict['phobos_auau_0p2'] = phobos_auau_0p2()
    msrmntdict['phobos_auau_0p13'] = phobos_auau_0p13()
    msrmntdict['phobos_auau_0p0624'] = phobos_auau_0p0624()
    msrmntdict['phobos_auau_0p0196'] = phobos_auau_0p0196()
    msrmntdict['phenix_auau_0p2'] = phenix_auau_0p2()
    msrmntdict['phenix_auau_0p13'] = phenix_auau_0p13()
    msrmntdict['phenix_auau_0p0624'] = phenix_auau_0p0624()
    msrmntdict['phenix_auau_0p039'] = phenix_auau_0p039()
    msrmntdict['phenix_auau_0p027'] = phenix_auau_0p027()
    msrmntdict['phenix_auau_0p0196'] = phenix_auau_0p0196()
    msrmntdict['phenix_auau_0p0145'] = phenix_auau_0p0145()
    msrmntdict['phenix_auau_0p0077'] = phenix_auau_0p0077()
    msrmntdict['brahms_auau_0p2'] = brahms_auau_0p2()
    
    # sphenix_rawhijing_auau_200gev, sphenix_rawhijing_divnpart2_auau_200gev = msrmnt_sphenix_generator_auau_200gev('HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0')
    # sphenix_rawepos_auau_200gev, sphenix_rawepos_divnpart2_auau_200gev = msrmnt_sphenix_generator_auau_200gev('EPOS_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0')
    # sphenix_rawampt_auau_200gev, sphenix_rawampt_divnpart2_auau_200gev = msrmnt_sphenix_generator_auau_200gev('AMPT_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0')
    sphenix_rawhijing_divnpart2_auau_200gev = msrmnt_generator_dNdetaNpart2_auau_200gev('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/generator_dNdEtaNpart2/truthdNdEta_HIJING.root')
    sphenix_rawepos_divnpart2_auau_200gev = msrmnt_generator_dNdetaNpart2_auau_200gev('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/generator_dNdEtaNpart2/truthdNdEta_EPOS.root')
    sphenix_rawampt_divnpart2_auau_200gev = msrmnt_generator_dNdetaNpart2_auau_200gev('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/generator_dNdEtaNpart2/truthdNdEta_AMPT.root')
    # sphnx_sim_auau_200gev, sphnx_sim_divnpart2_auau_200gev = msrmnt_sphenix_sim_auau_200gev()
    sphnx_data_auau_200gev_cmsapp, sphnx_data_divnpart2_auau_200gev_cmsapp = msrmnt_sphenix_data_auau_200gev('cms')
    sphnx_data_auau_200gev_phobosapp, sphnx_data_divnpart2_auau_200gev_phobosapp = msrmnt_sphenix_data_auau_200gev('phobos')
    sphnx_data_auau_200gev_combined, sphnx_data_divnpart2_auau_200gev_combined = msrmnt_sphenix_data_auau_200gev('combined')
    
    # msrmntdict['sphenix_rawhijing_auau_200gev'] = sphenix_rawhijing_auau_200gev
    # msrmntdict['sphenix_rawepos_auau_200gev'] = sphenix_rawepos_auau_200gev
    # msrmntdict['sphenix_rawampt_auau_200gev'] = sphenix_rawampt_auau_200gev
    # msrmntdict['sphenix_sim_auau_200gev'] = sphnx_sim_auau_200gev
    msrmntdict['sphenix_data_auau_200gev_cmsapp'] = sphnx_data_auau_200gev_cmsapp
    msrmntdict['sphenix_data_auau_200gev_cmsapp_redraw'] = sphnx_data_auau_200gev_cmsapp
    msrmntdict['sphenix_data_auau_200gev_phobosapp'] = sphnx_data_auau_200gev_phobosapp
    msrmntdict['sphenix_data_auau_200gev_phobosapp_redraw'] = sphnx_data_auau_200gev_phobosapp
    msrmntdict['sphenix_data_auau_200gev_combined'] = sphnx_data_auau_200gev_combined
    msrmntdict['sphenix_data_auau_200gev_combined_redraw'] = sphnx_data_auau_200gev_combined
    
    # dndeta / (<npart> / 2)
    msrmntdict['phenix_auau_0p2_divnpart2'] = phenix_auau_0p2_divnpart2() # PHENIX only provide dndeta / (<npart>)
    msrmntdict['brahms_auau_0p2_divnpart2'] = brahms_auau_0p2_divnpart2()
    msrmntdict['phobos_auau_0p2_divnpart2'] = phobos_auau_0p2_divnpart2()
    msrmntdict['sphenix_rawhijing_auau_0p2_divnpart2'] = sphenix_rawhijing_divnpart2_auau_200gev
    msrmntdict['sphenix_rawepos_auau_0p2_divnpart2'] = sphenix_rawepos_divnpart2_auau_200gev
    msrmntdict['sphenix_rawampt_auau_0p2_divnpart2'] = sphenix_rawampt_divnpart2_auau_200gev
    # msrmntdict['sphenix_sim_auau_0p2_divnpart2'] = sphnx_sim_divnpart2_auau_200gev
    msrmntdict['sphenix_data_auau_0p2_divnpart2_cmsapp'] = sphnx_data_divnpart2_auau_200gev_cmsapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_cmsapp_redraw'] = sphnx_data_divnpart2_auau_200gev_cmsapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_phobosapp'] = sphnx_data_divnpart2_auau_200gev_phobosapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_phobosapp_redraw'] = sphnx_data_divnpart2_auau_200gev_phobosapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_combined'] = sphnx_data_divnpart2_auau_200gev_combined
    msrmntdict['sphenix_data_auau_0p2_divnpart2_combined_redraw'] = sphnx_data_divnpart2_auau_200gev_combined
    
    msrmntdict['rhic_combine_dndetadivnpart2'] = combine_RHIC()
    msrmntdict['rhic_combine_dndetadivnpart2_redraw'] = combine_RHIC()
    
    msrmntdict['empty'] = TGraphErrors()
    return msrmntdict

def combine_RHIC():
    # combine PHENIX, BRAHMS, PHOBOS, and sPHENIX for dNdeta / (<npart> / 2)
    # print out the dNdeta for each experiment, then the npart/2 for each experiment
    # print out the combined dNdeta and npart/2
    # start with PHENIX
    
    centralitybin_phenix = [[0, 5], [5, 10], [10 ,15], [15, 20], [20, 25], [25, 30], [30, 35], [35, 40], [40, 45], [45, 50], [50, 55], [55, 60], [60, 65], [65, 70]]
    centralitybin_phobos = [[0, 3], [3, 6], [6,10], [10, 15],[15, 20], [20, 25], [25, 30], [30, 35], [35, 40], [40, 45], [45, 50], [50, 55], [55, 60], [60, 65], [65, 70]]
    centralitybin_brahms = [[0, 5], [5, 10], [10, 20], [20, 30], [30, 40], [40, 50]]
    centralitybin_sphenix = centralitybin_phobos
    sphenix_npart, sphenix_nparterr = sphenix_centralitynpart()
    sphnx_data_auau_200gev_combined, sphnx_data_divnpart2_auau_200gev_combined = msrmnt_sphenix_data_auau_200gev('combined')
    
    # centralitybin_combine = [[0,3], [3,6], [6,10], [10,15], [15,20], [20,25], [25,30], [30,35], [35,40], [40,45], [45,50], [50,55], [55,60], [60,65], [65,70]]
    centralitybin_combine = [[0,3], [0,5], [3,6], [5,10], [6,10], [10,15], [15,20], [20,25], [25,30], [30,35], [35,40], [40,45], [45,50], [50,55], [55,60], [60,65], [65,70]]
    
    gr_phenix_auau_0p2 = phenix_auau_0p2()
    phenix_dndeta, phenix_dndeta_err = [], []
    for i in range(gr_phenix_auau_0p2.GetN()):
        phenix_dndeta.append(gr_phenix_auau_0p2.GetY()[i])
        phenix_dndeta_err.append(gr_phenix_auau_0p2.GetErrorY(i))

    # add 60-65% and 65-70% points for phenix that are not in the graph and HepData
    phenix_dndeta.append(37.5)
    phenix_dndeta.append(25.6)
    phenix_dndeta_err.append(5.4)
    phenix_dndeta_err.append(4.5)
    
    gr_brahms_auau_0p2_divnpart2 = brahms_auau_0p2_divnpart2()
    
    # print('------------------------------------')
    # for i in range(len(phenix_dndeta)):
    #     print ('PHENIX: Centrality = {}-{}%, dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(centralitybin_phenix[i][0], centralitybin_phenix[i][1], phenix_auau_0p2_dndetadivnpart2[-(i+1)], phenix_auau_0p2_dndetadivnpart2_err[-(i+1)], phenix_auau_0p2_npart[-(i+1)],phenix_auau_0p2_nparterr[-(i+1)]))
    
    # print('------------------------------------')
    # for i in range(len(phobos_auau_0p2_raw)):
    #     print ('PHOBOS: Centrality = {}-{}%, dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(centralitybin_phobos[i][0], centralitybin_phobos[i][1], phobos_auau_0p2_dndetadivnpart2[-(i+1)], phobos_auau_0p2_dndetadivnpart2_err[-(i+1)], phobos_auau_0p2_npart[-(i+1)],phobos_auau_0p2_nparterr[-(i+1)]))
        
    # print('------------------------------------')
    # for i in range(len(brahms_auau_0p2_raw)):
    #     print ('BRAHMS: Centrality = {}-{}%, dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(centralitybin_brahms[i][0], centralitybin_brahms[i][1], gr_brahms_auau_0p2_divnpart2.GetY()[len(gr_brahms_auau_0p2_divnpart2.GetY())-i-1], gr_brahms_auau_0p2_divnpart2.GetErrorY(len(gr_brahms_auau_0p2_divnpart2.GetY())-i-1), brahms_auau_0p2_npart[-(i+1)], brahms_auau_0p2_nparterr[-(i+1)]))
    
    # print('------------------------------------')
    # for i in range(len(sphnx_data_auau_200gev_combined.GetY())):
    #     print ('sPHENIX: Centrality = {}-{}%, dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(centralitybin_sphenix[i][0], centralitybin_sphenix[i][1], sphnx_data_divnpart2_auau_200gev_combined.GetY()[i], sphnx_data_divnpart2_auau_200gev_combined.GetErrorY(i), sphenix_npart[i], sphenix_nparterr[i]))
        
    # print('------------------------------------')
    
    rhic_combine_dndetadivnpart2 = []
    rhic_combine_dndetadivnpart2_err = []
    rhic_combine_npart = []
    rhic_combine_nparterr = []
    
    # combine the data points
    for i in range(len(centralitybin_combine)):
        # print('Centrality = {}-{} %'.format(centralitybin_combine[i][0], centralitybin_combine[i][1]))
        # check if centralitybin_combine[i] is in centralitybin_phenix, centralitybin_phobos, centralitybin_brahms, and centralitybin_sphenix
        # if it is, then combine the data points using the function weighted_average(values, uncertainties)
        
        values_dndeta = []
        uncertainties_dndeta = []
        values_npart = []
        uncertainties_npart = []
        
        if centralitybin_combine[i] in centralitybin_phenix:
            phenix_index = centralitybin_phenix.index(centralitybin_combine[i])
            values_dndeta.append(phenix_auau_0p2_dndetadivnpart2[-(phenix_index+1)])
            uncertainties_dndeta.append(phenix_auau_0p2_dndetadivnpart2_err[-(phenix_index+1)])
            values_npart.append(phenix_auau_0p2_npart[-(phenix_index+1)])
            uncertainties_npart.append(phenix_auau_0p2_nparterr[-(phenix_index+1)])
            # print ('PHENIX: dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(phenix_auau_0p2_dndetadivnpart2[-(phenix_index+1)], phenix_auau_0p2_dndetadivnpart2_err[-(phenix_index+1)], phenix_auau_0p2_npart[-(phenix_index+1)], phenix_auau_0p2_nparterr[-(phenix_index+1)]))
            
        if centralitybin_combine[i] in centralitybin_phobos:
            phobos_index = centralitybin_phobos.index(centralitybin_combine[i])
            values_dndeta.append(phobos_auau_0p2_dndetadivnpart2[-(phobos_index+1)])
            uncertainties_dndeta.append(phobos_auau_0p2_dndetadivnpart2_err[-(phobos_index+1)])
            values_npart.append(phobos_auau_0p2_npart[-(phobos_index+1)])
            uncertainties_npart.append(phobos_auau_0p2_nparterr[-(phobos_index+1)])
            # print ('PHOBOS: dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(phobos_auau_0p2_dndetadivnpart2[-(phobos_index+1)], phobos_auau_0p2_dndetadivnpart2_err[-(phobos_index+1)], phobos_auau_0p2_npart[-(phobos_index+1)], phobos_auau_0p2_nparterr[-(phobos_index+1)]))
                   
        if centralitybin_combine[i] in centralitybin_brahms:
            brahms_index = centralitybin_brahms.index(centralitybin_combine[i])
            values_dndeta.append(gr_brahms_auau_0p2_divnpart2.GetY()[len(gr_brahms_auau_0p2_divnpart2.GetY())-i-1])
            uncertainties_dndeta.append(gr_brahms_auau_0p2_divnpart2.GetErrorY(len(gr_brahms_auau_0p2_divnpart2.GetY())-i-1))
            values_npart.append(brahms_auau_0p2_npart[-(brahms_index+1)])
            uncertainties_npart.append(brahms_auau_0p2_nparterr[-(brahms_index+1)])
            # print ('BRAHMS: dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(gr_brahms_auau_0p2_divnpart2.GetY()[len(gr_brahms_auau_0p2_divnpart2.GetY())-i-1], gr_brahms_auau_0p2_divnpart2.GetErrorY(len(gr_brahms_auau_0p2_divnpart2.GetY())-i-1), brahms_auau_0p2_npart[-(brahms_index+1)], brahms_auau_0p2_nparterr[-(brahms_index+1)]))
                   
        if centralitybin_combine[i] in centralitybin_sphenix:
            sphenix_index = centralitybin_sphenix.index(centralitybin_combine[i])
            values_dndeta.append(sphnx_data_divnpart2_auau_200gev_combined.GetY()[sphenix_index])
            uncertainties_dndeta.append(sphnx_data_divnpart2_auau_200gev_combined.GetErrorY(sphenix_index))
            values_npart.append(sphenix_npart[sphenix_index])
            uncertainties_npart.append(sphenix_nparterr[sphenix_index])
            # print ('sPHENIX: dNdeta/(0.5npart) = {:.3f}+-{:.3f}, npart = {:.3f}+-{:.3f}'.format(sphnx_data_divnpart2_auau_200gev_combined.GetY()[sphenix_index], sphnx_data_divnpart2_auau_200gev_combined.GetErrorY(sphenix_index), sphenix_npart[sphenix_index], sphenix_nparterr[sphenix_index]))
            
        dndeta_combined, dndeta_combined_err = weighted_average(values_dndeta, uncertainties_dndeta)
        npart_combined, npart_combined_err = weighted_average(values_npart, uncertainties_npart)
        # print ('Combined: dNdeta/(0.5npart) = ${:.2f}\pm{:.2f}~({:.2f}\%)$, npart = {:.3f}+-{:.3f}'.format(dndeta_combined, dndeta_combined_err, dndeta_combined_err/dndeta_combined*100, npart_combined, npart_combined_err))
        # print('------------------------------------')
        
        rhic_combine_dndetadivnpart2.append(dndeta_combined)
        rhic_combine_dndetadivnpart2_err.append(dndeta_combined_err)
        rhic_combine_npart.append(npart_combined)
        rhic_combine_nparterr.append(npart_combined_err)
        
    # make a TGraphAsymmErrors for the combined data points
    gr_rhic_combined = TGraphAsymmErrors(len(centralitybin_combine), array('f', rhic_combine_npart), array('f', rhic_combine_dndetadivnpart2), array('f', rhic_combine_nparterr), array('f', rhic_combine_nparterr), array('f', rhic_combine_dndetadivnpart2_err), array('f', rhic_combine_dndetadivnpart2_err))
    
    return gr_rhic_combined
    

def gstyle(g, mstyle, msize, color, lstyle, lwidth, alpha):
    g.SetMarkerStyle(mstyle)
    g.SetMarkerSize(msize)
    g.SetMarkerColor(color)
    g.SetLineColor(color)
    g.SetLineStyle(lstyle)
    g.SetLineWidth(lwidth)
    g.SetFillColorAlpha(color, alpha)


def dNdeta_eta0_Summary(docombine, jsonpath, canvname, axisrange = [-1, 71, 20, 1100], canvsize = [800, 600], grlegpos = [0.35, 0.18, 0.9, 0.45], prelimtext=''):
    xmin = axisrange[0]
    xmax = axisrange[1]
    ymin = axisrange[2]
    ymax = axisrange[3]
    # Create a canvas
    c = TCanvas('c_'+canvname, 'c_'+canvname, canvsize[0], canvsize[1])
    c.cd()
    # Set left margin and logarithmic y-axis
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetLogy()
    # Create a frame for the graph
    gframe = TH1F('gframe_' + canvname, 'gframe_' + canvname, 1, xmin, xmax)
    gframe.SetLabelOffset(999, 'X')
    gframe.SetTickLength(0, 'X')
    gframe.GetXaxis().SetTitle('Centrality [%]')
    gframe.GetYaxis().SetTitle('#LT#frac{dN_{ch}}{d#eta}#GT')
    gframe.GetYaxis().SetMoreLogLabels()
    gframe.GetYaxis().SetTitleOffset(2.1)
    gframe.SetAxisRange(ymin, ymax, 'Y')
    gframe.Draw()
    # Create and draw the x-axis on top
    axis = TGaxis(xmax, ymin, xmin, ymin, xmin, xmax, 511, '-')
    axis.SetLabelOffset(-0.032)
    axis.SetLabelFont(43)
    # print('gframe.GetYaxis().GetLabelSize()={}'.format(gframe.GetYaxis().GetLabelSize()))
    axis.SetLabelSize(30)
    axis.Draw()
    # Create and draw the y-axis on top
    axisup = TGaxis(xmax, ymax, xmin, ymax, xmin, xmax, 511, '+')
    axisup.SetLabelOffset(1)
    axisup.Draw()
    # Create the legend for the TGraphs
    # gr_leg = TLegend(grlegpos[0], grlegpos[1], grlegpos[2], grlegpos[3])
    # gr_leg = TLegend(gPad.GetLeftMargin()+0.05, (1 - TopMargin) - 0.35, gPad.GetLeftMargin()+0.35, (1 - TopMargin) - 0.18)
    gr_leg = TLegend(1 - gPad.GetRightMargin() - 0.55, gPad.GetBottomMargin() + 0.05, 1 - gPad.GetRightMargin() - 0.05, gPad.GetBottomMargin() + 0.2)
    gr_leg.SetNColumns(2)
    gr_leg.SetTextSize(0.033)
    gr_leg.SetFillStyle(0)
    # Load measurement dictionaries
    dict_msrmnt= msrmnt_dict()
    # Load the json file
    with open(jsonpath, 'r') as fp:
        grtags = json.load(fp)
        for tagName, par in grtags.items():
            if not include_rawhijing and 'rawhijing' in tagName:
                continue
            
            if docombine:
                if 'phobosapp' in tagName or 'cmsapp' in tagName:
                    continue
            else:
                if 'combined' in tagName:
                    continue
            
            gr = dict_msrmnt[tagName]
            gstyle(gr, par['markerstyle'], par['markersize'], TColor.GetColor(par['markercolor']), par['linestyle'], par['linewidth'], par['alpha'])
            gr.Draw(par['DrawOption'][0])
            if par['DrawOption'][1] != '':
                gr.Draw(par['DrawOption'][1])
            # add legend if par['Legendtext'] does not contain 'redraw'
            if 'redraw' not in tagName:
                gr_leg.AddEntry(gr, par['Legendtext'], par['Legendstyle'])

    gr_leg.Draw()
    sphnxleg = TLegend(gPad.GetLeftMargin()+0.05, (1 - TopMargin) - 0.18, gPad.GetLeftMargin()+0.1, (1 - TopMargin) - 0.06)
    sphnxleg.SetTextAlign(kHAlignLeft + kVAlignTop)
    sphnxleg.SetTextSize(0.045)
    sphnxleg.SetFillStyle(0)
    sphnxleg.AddEntry('', '#it{#bf{sPHENIX}} ' + prelimtext, '')
    sphnxleg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    sphnxleg.Draw()
    if (docombine):
        c.SaveAs(plotpath + 'dNdEta_eta0_{}_combine.png'.format(canvname))
        c.SaveAs(plotpath + 'dNdEta_eta0_{}_combine.pdf'.format(canvname))
    else:
        c.SaveAs(plotpath + 'dNdEta_eta0_{}.png'.format(canvname));
        c.SaveAs(plotpath + 'dNdEta_eta0_{}.pdf'.format(canvname));
    
    
def dNdeta_eta0_divnpart2_Summary(docombine, rhiccombine, plotstyle, jsonpath, canvname, axisrange = [0, 400, 1.49, 4.71], canvsize = [800, 600], grlegpos = [0.35, 0.18, 0.9, 0.45], prelimtext=''):
    # plotstyle:
    # 1 - all measurements and theories comparison
    # 2 - only measurement comparison
    # 3 - only sPHENIX measurement and theories
    
    xmin = axisrange[0]
    xmax = axisrange[1]
    ymin = axisrange[2]
    ymax = axisrange[3]
    # Create a canvas
    c = TCanvas('c_'+canvname, 'c_'+canvname, canvsize[0], canvsize[1])
    c.cd()
    # Set left margin and logarithmic y-axis
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin - 0.05)
    gPad.SetLogy(0)
    # Create a frame for the graph
    gframe = TH1F('gframe_' + canvname, 'gframe_' + canvname, 1, axisrange[0], axisrange[1])
    # gframe.GetXaxis().SetTitle('#LTN_{part}#GT')
    gframe.GetXaxis().SetTitle('N_{part}')
    gframe.GetYaxis().SetTitle('#LT#frac{dN_{ch}}{d#eta}#GT/(#LTN_{part}#GT/2)')
    gframe.GetYaxis().SetTitleOffset(1.55)
    gframe.GetXaxis().SetRangeUser(axisrange[0], axisrange[1])
    gframe.SetAxisRange(axisrange[2], axisrange[3], 'Y')
    gframe.Draw()
    # Create the legend for the TGraphs
    # gr_leg = TLegend(grlegpos[0], grlegpos[1], grlegpos[2], grlegpos[3])
    # gr_leg = TLegend(gPad.GetLeftMargin(), 1 - gPad.GetTopMargin() + 0.02, gPad.GetLeftMargin() + 0.72, 0.97)
    
    grleg_x1 = 1 - gPad.GetRightMargin() - 0.43
    grleg_y1 = gPad.GetBottomMargin() + 0.05 # default for non-RHIC combine
    grleg_x2 = 1 - gPad.GetRightMargin() - 0.04 # default for non-RHIC combine
    grleg_y2 = gPad.GetBottomMargin() + 0.24 # default for non-RHIC combine
    if rhiccombine:
        grleg_x1 = 1 - gPad.GetRightMargin() - 0.55
        grleg_y1 = gPad.GetBottomMargin() + 0.08 
        grleg_x2 = 1 - gPad.GetRightMargin() - 0.12
        grleg_y2 = gPad.GetBottomMargin() + 0.24
    else:
        if plotstyle == 1:
            grleg_x1 = 1 - gPad.GetRightMargin() - 0.43
            grleg_y1 = gPad.GetBottomMargin() + 0.05
            grleg_x2 = 1 - gPad.GetRightMargin() - 0.04
            grleg_y2 = gPad.GetBottomMargin() + 0.24
        elif plotstyle == 2: # only measurement comparison
            grleg_x1 = 1 - gPad.GetRightMargin() - 0.31
            grleg_y1 = gPad.GetBottomMargin() + 0.05 
            grleg_x2 = 1 - gPad.GetRightMargin() - 0.1
            grleg_y2 = gPad.GetBottomMargin() + 0.24 
        elif plotstyle == 3: 
            grleg_x1 = 1 - gPad.GetRightMargin() - 0.31
            grleg_y1 = gPad.GetBottomMargin() + 0.05
            grleg_x2 = 1 - gPad.GetRightMargin() - 0.1
            grleg_y2 = gPad.GetBottomMargin() + 0.24
        else:
            print ('Error: Invalid plotstyle provided. Must be 1, 2, or 3. Using default grleg_x1.')
            grleg_x1 = 1 - gPad.GetRightMargin() - 0.43
            grleg_y1 = gPad.GetBottomMargin() + 0.05
            grleg_x2 = 1 - gPad.GetRightMargin() - 0.04
            grleg_y2 = gPad.GetBottomMargin() + 0.24

    
    gr_leg = TLegend(grleg_x1, grleg_y1, grleg_x2, grleg_y2)
    gr_leg.SetNColumns(2) if plotstyle == 1 else gr_leg.SetNColumns(1)
    gr_leg.SetTextSize(0.033)
    gr_leg.SetFillStyle(0)
    
    leg_generator = TLegend(1 - gPad.GetRightMargin() - 0.545, 
                            gPad.GetBottomMargin() + 0.03, 
                            1 - gPad.GetRightMargin() - 0.02,
                            gPad.GetBottomMargin() + 0.08)
    leg_generator.SetNColumns(3)
    leg_generator.SetTextSize(0.033)
    leg_generator.SetFillStyle(0)
        
    print ('-----------------------------------------------')   
    # Load measurement dictionaries
    dict_msrmnt= msrmnt_dict()
    # Load the json file
    with open(jsonpath, 'r') as fp:
        grtags = json.load(fp)
        for tagName, par in grtags.items():     
            if docombine:
                if 'phobosapp' in tagName or 'cmsapp' in tagName:
                    continue
            else:
                if 'combined' in tagName:
                    continue
                
            if plotstyle == 2: # only measurement comparison
                if 'rawhijing' in tagName or 'rawepos' in tagName or 'rawampt' in tagName or 'empty' in tagName:
                    continue
            
            if plotstyle == 3: # only sPHENIX measurement and theories
                print (tagName)
                if tagName.startswith('phenix_') or tagName.startswith('brahms_') or tagName.startswith('phobos_') or 'empty' in tagName:
                    continue
            
            # print (tagName)
                
            gr = dict_msrmnt[tagName]
            gstyle(gr, par['markerstyle'], par['markersize'], TColor.GetColor(par['markercolor']), par['linestyle'], par['linewidth'], par['alpha'])
            gr.Draw(par['DrawOption'][0])
            if par['DrawOption'][1] != '':
                gr.Draw(par['DrawOption'][1])
                
            # add legend if par['Legendtext'] does not contain 'redraw'
            if not rhiccombine:
                if ('redraw' not in tagName):
                    gr_leg.AddEntry(gr, par['Legendtext'], par['Legendstyle'])
            else:
                if ('redraw' not in tagName and 'rawhijing' not in tagName and 'rawepos' not in tagName and 'rawampt' not in tagName):
                    gr_leg.AddEntry(gr, par['Legendtext'], par['Legendstyle'])
                
                if ('rawhijing' in tagName or 'rawepos' in tagName or 'rawampt' in tagName):
                    leg_generator.AddEntry(gr, par['Legendtext'], par['Legendstyle'])

    gr_leg.Draw()    
    if rhiccombine:
        leg_generator.Draw()
    
    sphnxleg = TLegend(gPad.GetLeftMargin()+0.05, (1 - TopMargin) - 0.18, gPad.GetLeftMargin()+0.1, (1 - TopMargin) - 0.06)
    sphnxleg.SetTextAlign(kHAlignLeft + kVAlignTop)
    sphnxleg.SetTextSize(0.045)
    sphnxleg.SetFillStyle(0)
    sphnxleg.AddEntry('', '#it{#bf{sPHENIX}} ' + prelimtext, '')
    sphnxleg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    sphnxleg.Draw()
    if (docombine):
        if not rhiccombine:
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_combine_style{}.png'.format(canvname, plotstyle))
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_combine_style{}.pdf'.format(canvname, plotstyle))
        else:
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_combine_rhiccombine_style{}.png'.format(canvname, plotstyle))
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_combine_rhiccombine_style{}.pdf'.format(canvname, plotstyle))
    else:
        if not rhiccombine:
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_style{}.png'.format(canvname, plotstyle))
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_style{}.pdf'.format(canvname, plotstyle))
        else:
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_rhiccombine_style{}.png'.format(canvname, plotstyle))
            c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_rhiccombine_style{}.pdf'.format(canvname, plotstyle))


if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('--combine', action='store_true', dest='combine', default=False, help='Combine two analyses')
    
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))
    
    combine = opt.combine
    
    plotpath = './dNdEtaFinal/'
    os.makedirs(plotpath, exist_ok=True)

    dNdeta_eta0_Summary(combine, './measurements/RHIC_plotparam.json', 'RHIC')
    dNdeta_eta0_divnpart2_Summary(combine, False, 1, './measurements/RHIC_dndetadivnpart2_plotparam.json', 'RHIC')
    dNdeta_eta0_divnpart2_Summary(combine, False, 2, './measurements/RHIC_dndetadivnpart2_plotparam.json', 'RHIC')
    dNdeta_eta0_divnpart2_Summary(combine, False, 3, './measurements/RHIC_dndetadivnpart2_plotparam.json', 'RHIC')
    dNdeta_eta0_divnpart2_Summary(combine, True, 1, './measurements/RHIC_combine_dndetadivnpart2_plotparam.json', 'RHIC')
    
    combine_RHIC()
    