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

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -n]")
    parser.add_option("-f", "--histfile", dest="histfname", type="string", default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/dRcut0p5/hists_merged.root', help="Input file name")
    parser.add_option("-d", "--plotdir", dest="plotdir", type="string", default='./RecoTracklet/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/dRcut0p5/', help="Plot directory")
    parser.add_option("-s", "--isdata", dest="isdata", action="store_true", default=False, help="Is data")

    (opt, args) = parser.parse_args()

    histfname = opt.histfname
    plotdir = opt.plotdir
    isdata = opt.isdata

    os.makedirs(plotdir, exist_ok=True)

    print('Plotting: [Histogram file, plot directory]=[{}, {}]'.format(histfname, plotdir))

    hM_NClusLayer1 = GetHistogram(histfname, 'hM_NClusLayer1')
    hM_NPrototkl = GetHistogram(histfname, 'hM_NPrototkl')
    hM_NRecotkl_Raw = GetHistogram(histfname, 'hM_NRecotkl_Raw')

    hM_dEta_proto = GetHistogram(histfname, 'hM_dEta_proto')
    hM_dPhi_proto = GetHistogram(histfname, 'hM_dPhi_proto')
    hM_dPhi_proto_altrange = GetHistogram(histfname, 'hM_dPhi_proto_altrange')
    hM_dR_proto = GetHistogram(histfname, 'hM_dR_proto')
    hM_dR_proto_altrange = GetHistogram(histfname, 'hM_dR_proto_altrange')
    hM_dR_proto_LogX = GetHistogram(histfname, 'hM_dR_proto_LogX')
    hM_Eta_proto = GetHistogram(histfname, 'hM_Eta_proto')
    hM_Phi_proto = GetHistogram(histfname, 'hM_Phi_proto')

    hM_dEta_reco = GetHistogram(histfname, 'hM_dEta_reco')
    hM_dEta_reco_altrange = GetHistogram(histfname, 'hM_dEta_reco_altrange')
    hM_dPhi_reco = GetHistogram(histfname, 'hM_dPhi_reco')
    hM_dPhi_reco_altrange = GetHistogram(histfname, 'hM_dPhi_reco_altrange')
    hM_dR_reco = GetHistogram(histfname, 'hM_dR_reco')
    hM_dR_reco_altrange = GetHistogram(histfname, 'hM_dR_reco_altrange')
    hM_dR_reco_LogX = GetHistogram(histfname, 'hM_dR_reco_LogX')
    hM_Eta_reco = GetHistogram(histfname, 'hM_Eta_reco')
    hM_Phi_reco = GetHistogram(histfname, 'hM_Phi_reco')

    hM_Eta_vtxZ_proto_incl = GetHistogram(histfname, 'hM_Eta_vtxZ_proto_incl')
    hM_Eta_vtxZ_reco_incl = GetHistogram(histfname, 'hM_Eta_vtxZ_reco_incl')

    hM_clusphi_clusphisize = GetHistogram(histfname, 'hM_clusphi_clusphisize')
    hM_cluseta_clusphisize = GetHistogram(histfname, 'hM_cluseta_clusphisize')

    # Draw_1Dhist(hist, IsData, norm1, logy, ymaxscale, XaxisName, Ytitle_unit, outname):
    # Do the plots
    Draw_1Dhist(hM_NClusLayer1, isdata, False, True, 1.3, 'Number of clusters (Layer 3+4)', '', plotdir+'NClusLayer1')
    Draw_1Dhist(hM_NPrototkl, isdata, False, True, 1.3, 'Number of proto-tracklets', '', plotdir+'NProtoTracklets')
    Draw_1Dhist(hM_NRecotkl_Raw, isdata, False, True, 1.3, 'Number of reco-tracklets', '', plotdir+'NRecoTracklets_Raw')

    Draw_1Dhist(hM_dEta_proto, isdata, False, False, 1.3, 'Proto-tracklet #Delta#eta', '', plotdir+'ProtoTracklet_dEta')
    Draw_1Dhist(hM_dPhi_proto, isdata, False, False, 1.3, 'Proto-tracklet #Delta#phi', '', plotdir+'ProtoTracklet_dPhi')
    Draw_1Dhist(hM_dPhi_proto_altrange, isdata, False, False, 1.3, 'Proto-tracklet #Delta#phi', '', plotdir+'ProtoTracklet_dPhi_altrange')
    Draw_1Dhist(hM_Eta_proto, isdata, False, False, 1.3, 'Proto-tracklet #eta', '', plotdir+'ProtoTracklet_Eta')
    Draw_1Dhist(hM_Phi_proto, isdata, False, False, 1.3, 'Proto-tracklet #phi', '', plotdir+'ProtoTracklet_Phi')
    Draw_1Dhist(hM_dR_proto, isdata, False, True, 1.3, 'Proto-tracklet #DeltaR', '', plotdir+'ProtoTracklet_dR')
    Draw_1Dhist(hM_dR_proto_altrange, isdata, False, True, 1.3, 'Proto-tracklet #DeltaR', '', plotdir+'ProtoTracklet_dR_altrange')
    Draw_1Dhist(hM_dR_proto_LogX, isdata, True, True, 1.3, 'Proto-tracklet #DeltaR', '', plotdir+'ProtoTracklet_dR_logX')

    Draw_1Dhist(hM_dEta_reco, isdata, False, True, 5, 'Reco-tracklet #Delta#eta', '', plotdir+'RecoTracklet_dEta')
    Draw_1Dhist(hM_dEta_reco_altrange, isdata, False, True, 5, 'Reco-tracklet #Delta#eta', '', plotdir+'RecoTracklet_dEta_altrange')
    Draw_1Dhist(hM_dPhi_reco, isdata, False, True, 10, 'Reco-tracklet #Delta#phi', '', plotdir+'RecoTracklet_dPhi')
    Draw_1Dhist(hM_dPhi_reco_altrange, isdata, False, True, 5, 'Reco-tracklet #Delta#phi', '', plotdir+'RecoTracklet_dPhi_altrange')
    Draw_1Dhist(hM_Eta_reco, isdata, False, False, 1.3, 'Reco-tracklet #eta', '', plotdir+'RecoTracklet_Eta')
    Draw_1Dhist(hM_Phi_reco, isdata, False, False, 1.3, 'Reco-tracklet #phi', '', plotdir+'RecoTracklet_Phi')
    Draw_1Dhist(hM_dR_reco, isdata, False, True, 1.3, 'Reco-tracklet #DeltaR', '', plotdir+'RecoTracklet_dR')
    Draw_1Dhist(hM_dR_reco_altrange, isdata, False, True, 10, 'Reco-tracklet #DeltaR', '', plotdir+'RecoTracklet_dR_altrange')
    Draw_1Dhist(hM_dR_reco_LogX, isdata, True, True, 1.3, 'Reco-tracklet #DeltaR', '', plotdir+'RecoTracklet_dR_logX')

    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, drawopt, outname):
    Draw_2Dhist(hM_Eta_vtxZ_proto_incl, isdata, False, False, 0.16, 'Proto-tracklet #eta', 'Primary vertex V_{z} [cm]', 'colz', plotdir+'ProtoTracklet_Eta_vtxZ')
    Draw_2Dhist(hM_Eta_vtxZ_reco_incl, isdata, False, False, 0.14, 'Reco-tracklet #eta', 'Primary vertex V_{z} [cm]', 'colz', plotdir+'RecoTracklet_Eta_vtxZ')
    Draw_2Dhist(hM_clusphi_clusphisize, isdata, True, False, 0.16, 'Cluster #phi', 'Cluster #phi size', 'colz', plotdir+'Cluster_Phi_ClusSize')
    Draw_2Dhist(hM_cluseta_clusphisize, isdata, True, False, 0.14, 'Cluster #eta', 'Cluster #phi size', 'colz', plotdir+'Cluster_Eta_ClusSize')
