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
TopMargin = 0.28
BottomMargin = 0.13
FillAlpha = 0.5

include_rawhijing = False

# compute uncertainty - weighted average
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
    

def msrmnt_sphenix_rawhijing_auau_200gev():
    centralitybin = sphenix_centrality_interval()
    centnpart, centnparterr = sphenix_centralitynpart()
    
    dNdEta_eta0, dNdEta_eta0_divnpart2, Centrality, dNdEta_eta0_err, dNdEta_eta0_divnpart2_err, Centrality_err = [], [], [], [], [], []
    
    for i in range(len(centralitybin) - 1):
        hM_dNdEtafinal = GetHistogram('./corrections/HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/Centrality{}to{}_Zvtxm10p0to10p0_noasel/correction_hists.root'.format(centralitybin[i],centralitybin[i+1]), 'h1WGhadron')

        dNdEta_eta0_centrality = (hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))) / 3.
        list_dNdEta_eta0 = [hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))]
        list_dNdEta_eta0_err = [hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0.2))]
        dNdEta_eta0_centrality, dNdEta_eta0_err_centrality = weighted_average(list_dNdEta_eta0, list_dNdEta_eta0_err)
        # print ('dNdEta_eta0_centrality={}, dNdta_eta0_err_centrality={}'.format(dNdEta_eta0_centrality, dNdEta_eta0_err_centrality))
        rel_err_dNdEta = dNdEta_eta0_err_centrality / dNdEta_eta0_centrality
        rel_err_npart = centnparterr[i] / centnpart[i]
        dNdEta_eta0_divnpart2_err_centrality = dNdEta_eta0_centrality / (centnpart[i] / 2.) * math.sqrt(rel_err_dNdEta**2 + rel_err_npart**2) # error propagation; TODO: include the uncertainty for <npart>
        
        dNdEta_eta0.append(dNdEta_eta0_centrality)
        dNdEta_eta0_divnpart2.append(dNdEta_eta0_centrality / (centnpart[i] / 2.))
        Centrality.append((centralitybin[i] + centralitybin[i+1]) / 2.)
        dNdEta_eta0_err.append(dNdEta_eta0_err_centrality)
        dNdEta_eta0_divnpart2_err.append(dNdEta_eta0_divnpart2_err_centrality)
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


def msrmnt_sphenix_data_auau_200gev(approach='cms'): # approach: 'cms', 'phobos', 'combined'
    centralitybin = sphenix_centrality_interval()
    centnpart, centnparterr = sphenix_centralitynpart()
    
    dNdEta_eta0, dNdEta_eta0_divnpart2, Centrality, dNdEta_eta0_err, dNdEta_eta0_divnpart2_err, Centrality_err = [], [], [], [], [], []
    
    for i in range(len(centralitybin) - 1):
        if approach == 'cms':
            hM_dNdEtafinal = GetHistogram('./systematics/Centrality{}to{}_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality{}to{}_Zvtxm10p0to10p0_noasel.root'.format(centralitybin[i],centralitybin[i+1],centralitybin[i],centralitybin[i+1]), 'hM_final')
        elif approach == 'phobos':
            mbin = GetMbinNum('Centrality{}to{}'.format(centralitybin[i],centralitybin[i+1]))
            hM_dNdEtafinal = GetHistogram('/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/EvtVtxZ/FinalResult/completed/vtxZ_-10_10cm_MBin{}/Final_Mbin{}_00054280/Final_Mbin{}_00054280.root'.format(mbin,mbin,mbin), 'h1D_dNdEta_reco')
        elif approach == 'combined':
            hM_dNdEtafinal_cms = GetHistogram('./systematics/Centrality{}to{}_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality{}to{}_Zvtxm10p0to10p0_noasel.root'.format(centralitybin[i],centralitybin[i+1],centralitybin[i],centralitybin[i+1]), 'hM_final')
            mbin = GetMbinNum('Centrality{}to{}'.format(centralitybin[i],centralitybin[i+1]))
            hM_dNdEtafinal_phobos = GetHistogram('/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/EvtVtxZ/FinalResult/completed/vtxZ_-10_10cm_MBin{}/Final_Mbin{}_00054280/Final_Mbin{}_00054280.root'.format(mbin,mbin,mbin), 'h1D_dNdEta_reco')
        else:
            sys.exit('Approach {} is not supported. Use default approach of cms'.format(approach))

        if approach == 'cms' or approach == 'phobos':
            dNdEta_eta0_centrality = (hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)) + hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))) / 3.
            list_dNdEta_eta0 = [hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinContent(hM_dNdEtafinal.FindBin(0.2))]
            list_dNdEta_eta0_err = [hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(-0.2)), hM_dNdEtafinal.GetBinError(hM_dNdEtafinal.FindBin(0.2))]
            dNdEta_eta0_centrality, dNdEta_eta0_err_centrality = weighted_average(list_dNdEta_eta0, list_dNdEta_eta0_err)
            # print ('dNdEta_eta0_centrality={}, dNdta_eta0_err_centrality={}'.format(dNdEta_eta0_centrality, dNdEta_eta0_err_centrality))
            rel_err_dNdEta = dNdEta_eta0_err_centrality / dNdEta_eta0_centrality
            rel_err_npart = centnparterr[i] / centnpart[i]
            dNdEta_eta0_divnpart2_err_centrality = dNdEta_eta0_centrality / (centnpart[i] / 2.) * math.sqrt(rel_err_dNdEta**2 + rel_err_npart**2) 
            
            dNdEta_eta0.append(dNdEta_eta0_centrality)
            dNdEta_eta0_divnpart2.append(dNdEta_eta0_centrality / (centnpart[i] / 2.))
            Centrality.append((centralitybin[i] + centralitybin[i+1]) / 2.)
            dNdEta_eta0_err.append(dNdEta_eta0_err_centrality)
            dNdEta_eta0_divnpart2_err.append(dNdEta_eta0_divnpart2_err_centrality)
            Centrality_err.append(0)
        elif approach == 'combined':
            dNdEta_eta0_centrality_cms = (hM_dNdEtafinal_cms.GetBinContent(hM_dNdEtafinal_cms.FindBin(0)) + hM_dNdEtafinal_cms.GetBinContent(hM_dNdEtafinal_cms.FindBin(-0.2)) + hM_dNdEtafinal_cms.GetBinContent(hM_dNdEtafinal_cms.FindBin(0.2))) / 3.
            list_dNdEta_eta0_cms = [hM_dNdEtafinal_cms.GetBinContent(hM_dNdEtafinal_cms.FindBin(0)), hM_dNdEtafinal_cms.GetBinContent(hM_dNdEtafinal_cms.FindBin(-0.2)), hM_dNdEtafinal_cms.GetBinContent(hM_dNdEtafinal_cms.FindBin(0.2))]
            list_dNdEta_eta0_err_cms = [hM_dNdEtafinal_cms.GetBinError(hM_dNdEtafinal_cms.FindBin(0)), hM_dNdEtafinal_cms.GetBinError(hM_dNdEtafinal_cms.FindBin(-0.2)), hM_dNdEtafinal_cms.GetBinError(hM_dNdEtafinal_cms.FindBin(0.2))]
            dNdEta_eta0_centrality_cms, dNdEta_eta0_err_centrality_cms = weighted_average(list_dNdEta_eta0_cms, list_dNdEta_eta0_err_cms)
            
            dNdEta_eta0_centrality_phobos = (hM_dNdEtafinal_phobos.GetBinContent(hM_dNdEtafinal_phobos.FindBin(0)) + hM_dNdEtafinal_phobos.GetBinContent(hM_dNdEtafinal_phobos.FindBin(-0.2)) + hM_dNdEtafinal_phobos.GetBinContent(hM_dNdEtafinal_phobos.FindBin(0.2))) / 3.
            list_dNdEta_eta0_phobos = [hM_dNdEtafinal_phobos.GetBinContent(hM_dNdEtafinal_phobos.FindBin(0)), hM_dNdEtafinal_phobos.GetBinContent(hM_dNdEtafinal_phobos.FindBin(-0.2)), hM_dNdEtafinal_phobos.GetBinContent(hM_dNdEtafinal_phobos.FindBin(0.2))]
            list_dNdEta_eta0_err_phobos = [hM_dNdEtafinal_phobos.GetBinError(hM_dNdEtafinal_phobos.FindBin(0)), hM_dNdEtafinal_phobos.GetBinError(hM_dNdEtafinal_phobos.FindBin(-0.2)), hM_dNdEtafinal_phobos.GetBinError(hM_dNdEtafinal_phobos.FindBin(0.2))]
            dNdEta_eta0_centrality_phobos, dNdEta_eta0_err_centrality_phobos = weighted_average(list_dNdEta_eta0_phobos, list_dNdEta_eta0_err_phobos)
            
            dNdEta_eta0_centrality = (dNdEta_eta0_centrality_cms + dNdEta_eta0_centrality_phobos) / 2.
            dNdEta_eta0_err_centrality = max(dNdEta_eta0_err_centrality_cms, dNdEta_eta0_err_centrality_phobos)
            rel_err_dNdEta = dNdEta_eta0_err_centrality / dNdEta_eta0_centrality
            rel_err_npart = centnparterr[i] / centnpart[i]
            dNdEta_eta0_divnpart2_err_centrality = dNdEta_eta0_centrality / (centnpart[i] / 2.) * math.sqrt(rel_err_dNdEta**2 + rel_err_npart**2)
            
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
    
    sphenix_rawhijing_auau_200gev, sphenix_rawhijing_divnpart2_auau_200gev = msrmnt_sphenix_rawhijing_auau_200gev()
    # sphnx_sim_auau_200gev, sphnx_sim_divnpart2_auau_200gev = msrmnt_sphenix_sim_auau_200gev()
    sphnx_data_auau_200gev_cmsapp, sphnx_data_divnpart2_auau_200gev_cmsapp = msrmnt_sphenix_data_auau_200gev('cms')
    sphnx_data_auau_200gev_phobosapp, sphnx_data_divnpart2_auau_200gev_phobosapp = msrmnt_sphenix_data_auau_200gev('phobos')
    sphnx_data_auau_200gev_combined, sphnx_data_divnpart2_auau_200gev_combined = msrmnt_sphenix_data_auau_200gev('combined')
    
    msrmntdict['sphenix_rawhijing_auau_200gev'] = sphenix_rawhijing_auau_200gev
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
    # msrmntdict['sphenix_sim_auau_0p2_divnpart2'] = sphnx_sim_divnpart2_auau_200gev
    msrmntdict['sphenix_data_auau_0p2_divnpart2_cmsapp'] = sphnx_data_divnpart2_auau_200gev_cmsapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_cmsapp_redraw'] = sphnx_data_divnpart2_auau_200gev_cmsapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_phobosapp'] = sphnx_data_divnpart2_auau_200gev_phobosapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_phobosapp_redraw'] = sphnx_data_divnpart2_auau_200gev_phobosapp
    msrmntdict['sphenix_data_auau_0p2_divnpart2_combined'] = sphnx_data_divnpart2_auau_200gev_combined
    msrmntdict['sphenix_data_auau_0p2_divnpart2_combined_redraw'] = sphnx_data_divnpart2_auau_200gev_combined
    return msrmntdict


def gstyle(g, mstyle, msize, color, lwidth, alpha):
    g.SetMarkerStyle(mstyle)
    g.SetMarkerSize(msize)
    g.SetMarkerColor(color)
    g.SetLineColor(color)
    g.SetLineWidth(lwidth)
    g.SetFillColorAlpha(color, alpha)


def dNdeta_eta0_Summary(docombine, jsonpath, canvname, axisrange = [-1, 71, 20, 1200], canvsize = [800, 600], grlegpos = [0.35, 0.18, 0.9, 0.45], prelimtext='Internal'):
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
    gframe.GetYaxis().SetTitle('#frac{dN_{ch}}{d#eta}#lbar_{#eta=0}')
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
    gr_leg = TLegend(gPad.GetLeftMargin(), 1 - gPad.GetTopMargin() + 0.02, gPad.GetLeftMargin() + 0.35, 0.97)
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
            gstyle(gr, par['markerstyle'], par['markersize'], TColor.GetColor(par['markercolor']), par['linewidth'], par['alpha'])
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
    
    
def dNdeta_eta0_divnpart2_Summary(docombine, jsonpath, canvname, axisrange = [0, 400, 1.49, 5.01], canvsize = [800, 600], grlegpos = [0.35, 0.18, 0.9, 0.45], prelimtext='Internal'):
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
    gPad.SetLogy(0)
    # Create a frame for the graph
    gframe = TH1F('gframe_' + canvname, 'gframe_' + canvname, 1, axisrange[0], axisrange[1])
    gframe.GetXaxis().SetTitle('#LTN_{part}#GT')
    gframe.GetYaxis().SetTitle('#LT2/N_{part}#GT #frac{dN_{ch}}{d#eta}#lbar_{#eta=0}')
    gframe.GetYaxis().SetTitleOffset(1.9)
    gframe.GetXaxis().SetRangeUser(axisrange[0], axisrange[1])
    gframe.SetAxisRange(axisrange[2], axisrange[3], 'Y')
    gframe.Draw()
    # Create the legend for the TGraphs
    # gr_leg = TLegend(grlegpos[0], grlegpos[1], grlegpos[2], grlegpos[3])
    gr_leg = TLegend(gPad.GetLeftMargin(), 1 - gPad.GetTopMargin() + 0.02, gPad.GetLeftMargin() + 0.35, 0.97)
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
            gstyle(gr, par['markerstyle'], par['markersize'], TColor.GetColor(par['markercolor']), par['linewidth'], par['alpha'])
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
        c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_combine.png'.format(canvname))
        c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}_combine.pdf'.format(canvname))
    else:
        c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}.png'.format(canvname))
        c.SaveAs(plotpath + 'dNdEta_eta0_divnpart2_{}.pdf'.format(canvname))


if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('--combine', action='store_true', dest='combine', default=False, help='Combine two analyses')
    
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))
    
    combine = opt.combine
    
    plotpath = './dNdEtaFinal/'
    os.makedirs(plotpath, exist_ok=True)

    dNdeta_eta0_Summary(combine, './measurements/RHIC_plotparam.json', 'RHIC')
    dNdeta_eta0_divnpart2_Summary(combine, './measurements/RHIC_dndetadivnpart2_plotparam.json', 'RHIC')
    