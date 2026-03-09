// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TRUTHTOSVTXTRACK_H
#define TRUTHTOSVTXTRACK_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/ActsGeometry.h>
#include <ffaobjects/EventHeaderv1.h>

#include <phool/getClass.h>

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertexMap.h>
#include <ffarawobjects/Gl1Packet.h>
#include <trackbase_historic/SvtxPHG4ParticleMap_v1.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <trackbase_historic/SvtxTrack_v2.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v1.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/TrackSeed.h>

#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>

#include <ffaobjects/EventHeaderv1.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Shower.h>
#include <g4main/PHG4HitDefs.h>

#include <fun4all/SubsysReco.h>

#include <globalvertex/GlobalVertexMap.h>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TString.h>
#include <TPad.h>
#include <TLorentzVector.h>

class PHCompositeNode;

class TruthToSvtxTrack : public SubsysReco
{
public:
    TruthToSvtxTrack(
        const std::string & name_in = "TruthToSvtxTrack"
    );


    ~TruthToSvtxTrack() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
    */
    int Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
    */
    int InitRun(PHCompositeNode * /*topNode*/) override;

    /* Called for each event.
        This is where you do the real work.
    */
    int process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    int EndRun(const int runnumber) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    /// Reset
    int Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;

private:
    int createTracksFromTruth(PHCompositeNode* topNode);

    PHG4TruthInfoContainer *m_truth_info = nullptr;
};

#endif // TRUTHTOSVTXTRACK_H
