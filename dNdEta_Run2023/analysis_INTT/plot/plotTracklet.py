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

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -n]")
    parser.add_option("--histdir", dest="histdir", type="string", default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/dRcut0p5/hists_merged.root', help="Input file name")
    parser.add_option("--plotdir", dest="plotdir", type="string", default='./RecoTracklet/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/dRcut0p5/', help="Plot directory")
    parser.add_option("--isdata", dest="isdata", action="store_true", default=False, help="Is data")

    (opt, args) = parser.parse_args()

    histdir = opt.histdir
    plotdir = opt.plotdir
    isdata = opt.isdata
    
    if os.path.isfile("{}/hists_merged.root".format(histdir)):
        os.system("rm {}/hists_merged.root".format(histdir))
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(histdir, histdir))
    else:
        os.system("hadd -f -j 20 {}/hists_merged.root {}/hists_*.root".format(histdir, histdir))

    os.makedirs(plotdir, exist_ok=True)


    hM_Eta_vtxZ_reco_incl = GetHistogram('{}/hists_merged.root'.format(histdir), 'hM_Eta_vtxZ_reco_incl')
    hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10 = GetHistogram('{}/hists_merged.root'.format(histdir), 'hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10 = GetHistogram('{}/hists_merged.root'.format(histdir), 'hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')

    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname)
    Draw_2Dhist(hM_Eta_vtxZ_reco_incl, isdata, False, False, 0.16, 'Reco-tracklet #eta', 'INTT Z_{vtx} [cm]', '', 'colz', plotdir+'/RecoTracklet_Eta_vtxZ')
    Draw_2Dhist(hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10, isdata, False, False, 0.16, 'Reco-tracklet #eta', 'INTT Z_{vtx} [cm]', '', 'colz', plotdir+'/RecoTracklet_Eta_vtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
    Draw_2Dhist(hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10, isdata, False, False, 0.16, 'Reco-tracklet #eta', 'Reco-tracklet #phi', '', 'colz', plotdir+'/RecoTracklet_Eta_Phi_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10')
