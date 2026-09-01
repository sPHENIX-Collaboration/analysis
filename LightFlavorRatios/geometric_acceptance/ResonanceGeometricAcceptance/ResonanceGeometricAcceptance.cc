//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in ResonanceGeometricAcceptance.h.
//
// ResonanceGeometricAcceptance(const std::string &name = "ResonanceGeometricAcceptance")
// everything is keyed to ResonanceGeometricAcceptance, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// ResonanceGeometricAcceptance::~ResonanceGeometricAcceptance()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int ResonanceGeometricAcceptance::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int ResonanceGeometricAcceptance::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int ResonanceGeometricAcceptance::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int ResonanceGeometricAcceptance::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int ResonanceGeometricAcceptance::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int ResonanceGeometricAcceptance::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int ResonanceGeometricAcceptance::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void ResonanceGeometricAcceptance::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
// [[maybe_unused]] suppresses compiler warnings if topNode is not used in this method
//
//____________________________________________________________________________..

#include "ResonanceGeometricAcceptance.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/PHG4ParticleSvtxMap.h>
#include <trackbase_historic/SvtxPHG4ParticleMap.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <KFParticle.h>

#include <TFile.h>
#include <Math/Vector4D.h>
#include <TDatabasePDG.h>
#include <TMatrixD.h>

//____________________________________________________________________________..
ResonanceGeometricAcceptance::ResonanceGeometricAcceptance(const std::string &name):
 SubsysReco(name)
{
  //std::cout << "ResonanceGeometricAcceptance::ResonanceGeometricAcceptance(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
ResonanceGeometricAcceptance::~ResonanceGeometricAcceptance()
{
  //std::cout << "ResonanceGeometricAcceptance::~ResonanceGeometricAcceptance() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  //std::cout << "ResonanceGeometricAcceptance::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::InitRun([[maybe_unused]] PHCompositeNode *topNode)
{
  //std::cout << "ResonanceGeometricAcceptance::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  m_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  m_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  m_truth = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  m_truth_reco_map = findNode::getClass<PHG4ParticleSvtxMap>(topNode, "PHG4ParticleSvtxMap");
  m_reco_truth_map = findNode::getClass<SvtxPHG4ParticleMap>(topNode, "SvtxPHG4ParticleMap");

  if(!m_trackmap)
  {
    std::cout << PHWHERE << " cannot find SvtxTrackMap named " << m_trackmap_name << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!m_vertexmap)
  {
    std::cout << PHWHERE << " cannot find SvtxVertexMap named " << m_trackmap_name << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!m_truth)
  {
    std::cout << PHWHERE << " cannot find PHG4TruthInfoContainer named G4TruthInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!m_truth_reco_map)
  {
    std::cout << PHWHERE << " cannot find PHG4ParticleSvtxMap named PHG4ParticleSvtxMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!m_reco_truth_map)
  {
    std::cout << PHWHERE << " cannot find SvtxPHG4ParticleMap named SvtxPHG4ParticleMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  outfile = new TFile(m_outfile_name.c_str(),"RECREATE");

  all_candidates_vspt = makeHistogram(m_mother_name+"_all_candidates","All "+m_mother_name+" candidates in fiducial region with selected decay",pt_bins);
  all_candidates_vseta = makeHistogram(m_mother_name+"_all_candidates","All "+m_mother_name+" candidates in fiducial region with selected decay",eta_bins);
  all_candidates_vsphi = makeHistogram(m_mother_name+"_all_candidates","All "+m_mother_name+" candidates in fiducial region with selected decay",phi_bins);
  all_candidates_vsrapidity = makeHistogram(m_mother_name+"_all_candidates","All "+m_mother_name+" candidates in fiducial region with selected decay",rapidity_bins);

  passing_candidates_vspt = makeHistogram(m_mother_name+"_passing_candidates",m_mother_name+" candidates passing geometric acceptance cuts",pt_bins);
  passing_candidates_vseta = makeHistogram(m_mother_name+"_passing_candidates",m_mother_name+" candidates passing geometric acceptance cuts",eta_bins);
  passing_candidates_vsphi = makeHistogram(m_mother_name+"_passing_candidates",m_mother_name+" candidates passing geometric acceptance cuts",phi_bins);
  passing_candidates_vsrapidity = makeHistogram(m_mother_name+"_passing_candidates",m_mother_name+" candidates passing geometric acceptance cuts",rapidity_bins);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::process_event([[maybe_unused]] PHCompositeNode *topNode)
{
  //std::cout << "ResonanceGeometricAcceptance::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  std::map<int,std::vector<PHG4Particle*>> all_daughter_map;
  std::map<int,std::vector<PHG4Particle*>> passing_daughter_map;

  // find mothers
  const auto truth_particle_range = m_truth->GetParticleRange();
  for(auto particle_iter = truth_particle_range.first; particle_iter != truth_particle_range.second; ++particle_iter)
  {
    auto id_and_particle = *particle_iter;
    PHG4Particle* particle = id_and_particle.second;

    // select mothers with specified PDG ID which are primary
    const bool is_mother = (particle->get_pid() == m_mother_pdgid) || (m_include_conjugate && particle->get_pid() == -1*m_mother_pdgid);
    bool is_primary;
    if(m_mother_pdgid == 310)
    {
      PHG4Particle* parent = m_truth->GetParticle(particle->get_parent_id());
      if(parent)
      {
        const bool parent_is_K0 = (abs(parent->get_pid()) == 311);
        const bool parent_is_primary = (parent->get_track_id() == parent->get_primary_id());
        is_primary = (particle->get_track_id() == particle->get_primary_id()) || (parent_is_K0 && parent_is_primary);
      }
      else
      {
        is_primary = (particle->get_track_id() == particle->get_primary_id());
      }
    }
    else
    {
      is_primary = (particle->get_track_id() == particle->get_primary_id());
    }

    if(is_mother && is_primary)
    {
      all_daughter_map.insert({particle->get_track_id(),{}});
      passing_daughter_map.insert({particle->get_track_id(),{}});
    }
  }

  if(Verbosity()>0)
  {
    std::cout << "found " << all_daughter_map.size() << " truth mothers" << std::endl;
  }

  // find daughters and pair with mothers
  for(auto particle_iter = truth_particle_range.first; particle_iter != truth_particle_range.second; ++particle_iter)
  {
    auto id_and_particle = *particle_iter;
    PHG4Particle* particle = id_and_particle.second;

    if(Verbosity()>5)
    {
      std::cout << "processing truth track " << particle->get_track_id() << " (PDGID " << particle->get_pid() << ")" << std::endl;
    }

    PHG4Particle* parent = m_truth->GetParticle(particle->get_parent_id());
    if(!parent)
    {
      if(Verbosity()>5)
      {
        std::cout << "no parent" << std::endl;
      }
      continue;
    }
    if((parent->get_pid() == m_mother_pdgid) || (m_include_conjugate && parent->get_pid() == -1*m_mother_pdgid))
    {
      if(Verbosity()>5)
      {
        std::cout << "has mother of PDGID " << m_mother_pdgid << std::endl;
      }
      const bool is_daughter = std::find(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),particle->get_pid()) != m_daughter_pdgids.end();
      const bool is_conjugate_daughter = std::find(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),-1*particle->get_pid()) != m_daughter_pdgids.end();
      const bool mother_is_primary = (all_daughter_map.count(parent->get_track_id())>0);
      if((is_daughter || (m_include_conjugate && is_conjugate_daughter)) && mother_is_primary)
      {
        if(Verbosity()>5)
        {
          std::cout << "is a daughter of a primary mother" << std::endl;
        }

        all_daughter_map[parent->get_track_id()].push_back(particle);

        // get all reco matches for daughter which have silicon hits
        std::set<SvtxTrack*> matches_with_si = get_reco_matches_with_silicon_hits(particle);
        if(matches_with_si.size()>0)
        {
          passing_daughter_map[parent->get_track_id()].push_back(particle);
        }
      }
    }
/*
    if(parent && get_reco_track(particle)) 
    {
      SvtxTrack* reco_track = get_reco_track(particle);
      std::cout << "truth track " << particle->get_track_id() << " paired with reco track " << reco_track->get_id() << " has parent PDGID " << parent->get_pid() << std::endl;
    }
*/
  }

  if(Verbosity()>0)
  {
    std::cout << "passing daughter pass complete" << std::endl;
  }

  // fill denominator with primary mothers which decayed in the correct channel and satisfy fiducial cuts given by HistogramInfo variables
  for(const auto& [mother_track_id, daughters] : all_daughter_map)
  {
    PHG4Particle* mother = m_truth->GetParticle(mother_track_id);
    ROOT::Math::PxPyPzEVector mother_lorentzvector(mother->get_px(),mother->get_py(),mother->get_pz(),mother->get_e());

    PHG4VtxPoint* gvertex = m_truth->GetVtx(mother->get_vtx_id());

    bool correct_decay_channel = has_all_daughters(daughters);
    bool within_zvertex_limits = (gvertex->get_z()>=zvertex_limits.first && gvertex->get_z()<=zvertex_limits.second);

    bool within_pt_limits = (mother_lorentzvector.Pt()>=pt_limits.first && mother_lorentzvector.Pt()<=pt_limits.second);
    bool within_eta_limits = (mother_lorentzvector.Eta()>=eta_limits.first && mother_lorentzvector.Eta()<=eta_limits.second);
    bool within_phi_limits = (mother_lorentzvector.Phi()>=phi_limits.first && mother_lorentzvector.Phi()<=phi_limits.second);
    bool within_rapidity_limits = (mother_lorentzvector.Rapidity()>=rapidity_limits.first && mother_lorentzvector.Rapidity()<=rapidity_limits.second);

    if(correct_decay_channel && within_zvertex_limits && (within_pt_limits && within_eta_limits && within_phi_limits && within_rapidity_limits))
    {
      all_candidates_vspt->Fill(mother_lorentzvector.Pt());
      all_candidates_vseta->Fill(mother_lorentzvector.Eta());
      all_candidates_vsphi->Fill(mother_lorentzvector.Phi());
      all_candidates_vsrapidity->Fill(mother_lorentzvector.Rapidity());
    }
  }

  if(Verbosity()>0)
  {
    std::cout << "filled " << m_mother_name << " truth histograms" << std::endl;
    std::cout << "current nEntries: " << all_candidates_vspt->GetEntries() << std::endl;
  }

  // If reco vertex exists, fill numerator with primary mothers whose daughters are reconstructible as KFParticle SVs within the fiducial volume.
  // This means that they must satisfy all of the following:
  // - each daughter must have a reconstructed track
  // - the vertex charge must be correct
  // - the PID hypotheses available to KFParticle must match with the reco-charge-modified PDG IDs of the daughters
  // - the mother crossing must match all the daughter crossings.
  // In addition, the mother momentum produced by KFParticle's internal Kalman filter can significantly differ from that obtained from the raw sum of four-vectors; 
  // therefore, the fiducial volume cuts must be evaluated relative to the KFParticle mother momentum.
  if(!m_vertexmap->empty())
  {
    for(const auto& [mother_track_id, daughters] : passing_daughter_map)
    {
      bool correct_decay_channel = has_all_daughters(daughters);
      if(!correct_decay_channel)
      {
        continue;
      }
      std::vector<std::vector<SvtxTrack*>> reco_combinations = get_all_reco_combinations(daughters);

      for(std::vector<SvtxTrack*> combination : reco_combinations)
      {
        bool charge_swaps_ok = check_charge_swaps(daughters,combination);
        bool crossings_match = check_crossings_match(combination);

        if(correct_decay_channel && charge_swaps_ok && crossings_match)
        {
          std::vector<KFParticle> kfp_reco_daughters;
          for(size_t itr=0; itr<combination.size(); itr++)
          {
            kfp_reco_daughters.push_back(make_KFParticle(daughters[itr],combination[itr]));
          }

          // build mother KFParticle

          KFParticle kfp_reco_mother;
          kfp_reco_mother.SetConstructMethod(2);
          for(KFParticle& kfp_daughter : kfp_reco_daughters)
          {
            kfp_reco_mother.AddDaughter(kfp_daughter);
          }
/*
          for(KFParticle& kfp_daughter : kfp_reco_daughters)
          {
            kfp_daughter.SetProductionVertex(kfp_reco_mother);
          }
*/
          kfp_reco_mother.SetPDG(get_mother_PDGID(daughters));
          float mother_mass, mother_mass_err;
          kfp_reco_mother.GetMass(mother_mass,mother_mass_err);
          float mother_pt, mother_pt_err;
          kfp_reco_mother.GetPt(mother_pt,mother_pt_err);

          // associate mother with best possible reconstructed vertex

          int best_vertex_id = get_best_reco_vertex(kfp_reco_mother,combination[0]->get_crossing());
          if(Verbosity()>5)
          {
            std::cout << "best_vertex_id: " << best_vertex_id << std::endl;
          }
          if(best_vertex_id == -1)
          {
            continue;
          }
        
          SvtxVertex* pv = m_vertexmap->get(best_vertex_id);
          KFParticle kfpv = make_KFParticle_Vertex(pv);
          //kfp_reco_mother.SetProductionVertex(kfpv);
          //kfp_reco_mother.TransportToDecayVertex();

          if(Verbosity()>5)
          {
            std::cout << "reco vertex z: " << kfpv.GetZ() << std::endl;
          }

          // get mother kinematic variables, compare with fiducial cuts

          float reco_mother_pt, reco_mother_pterr;
          kfp_reco_mother.GetPt(reco_mother_pt,reco_mother_pterr);
          float reco_mother_eta, reco_mother_etaerr;
          kfp_reco_mother.GetEta(reco_mother_eta,reco_mother_etaerr);
          float reco_mother_phi, reco_mother_phierr;
          kfp_reco_mother.GetPhi(reco_mother_phi,reco_mother_phierr);
          float reco_mother_rapidity = kfp_reco_mother.GetRapidity();

          bool within_zvertex_limits = (kfpv.GetZ()>=zvertex_limits.first && kfpv.GetZ()<=zvertex_limits.second);

          bool within_pt_limits = (reco_mother_pt>=pt_limits.first && reco_mother_pt<=pt_limits.second);
          bool within_eta_limits = (reco_mother_eta>=eta_limits.first && reco_mother_eta<=eta_limits.second);
          bool within_phi_limits = (reco_mother_phi>=phi_limits.first && reco_mother_phi<=phi_limits.second);
          bool within_rapidity_limits = (reco_mother_rapidity>=rapidity_limits.first && reco_mother_rapidity<=rapidity_limits.second);

          if(within_zvertex_limits && within_pt_limits && within_eta_limits && within_phi_limits && within_rapidity_limits)
          {
            if(Verbosity()>1)
            {
              identify(kfp_reco_mother);
              for(KFParticle& kfpd : kfp_reco_daughters) identify(kfpd);
            }

            passing_candidates_vspt->Fill(reco_mother_pt);
            passing_candidates_vseta->Fill(reco_mother_eta);
            passing_candidates_vsphi->Fill(reco_mother_phi);
            passing_candidates_vsrapidity->Fill(reco_mother_rapidity);
          }
        }
      }
    }
  }

  if(Verbosity()>0)
  {
    std::cout << "filled " << m_mother_name << " reco histograms" << std::endl;
    std::cout << "current nEntries: " << passing_candidates_vspt->GetEntries() << std::endl;
  }

  all_daughter_map.clear();
  passing_daughter_map.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<std::vector<SvtxTrack*>> ResonanceGeometricAcceptance::get_all_reco_combinations(const std::vector<PHG4Particle*>& daughters) const
{
  std::vector<std::vector<SvtxTrack*>> reco_tracks;
  for(PHG4Particle* daughter : daughters)
  {
    std::set<SvtxTrack*> reco_track_set = get_reco_matches_with_silicon_hits(daughter);
    if(reco_track_set.empty()) return {};
    std::vector<SvtxTrack*> reco_track_vector;
    reco_track_vector.insert(reco_track_vector.end(),reco_track_set.begin(),reco_track_set.end());
    reco_tracks.push_back(reco_track_vector);
  }

  if(Verbosity()>5)
  {
    std::cout << "reco tracks for each daughter:" << std::endl;
    std::cout << "{";
    for(size_t i=0;i<reco_tracks.size();i++)
    {
      std::cout << "{";
      for(size_t j=0;j<reco_tracks[i].size();j++)
      {
        std::cout << reco_tracks[i][j]->get_id();
        if(j<reco_tracks[i].size()-1) std::cout << ", ";
      }
      std::cout << "}";
      if(i<reco_tracks.size()-1) std::cout << ", ";
    }
    std::cout << "}" << std::endl;
  }

  std::vector<std::vector<SvtxTrack*>> reco_combinations;
  if(reco_tracks.size()==0) return {};
  for(SvtxTrack* track : reco_tracks[0])
  {
    reco_combinations.push_back({track});
  }
  if(reco_tracks.size()>1)
  {
    for(size_t iv = 1; iv<reco_tracks.size(); iv++)
    {
      std::vector<std::vector<SvtxTrack*>> reco_iteration;
      for(size_t icomb = 0; icomb<reco_combinations.size(); icomb++)
      {
        for(size_t itr = 0; itr<reco_tracks[iv].size(); itr++)
        {
          std::vector<SvtxTrack*> reco_combination = reco_combinations[icomb];
          reco_combination.push_back(reco_tracks[iv][itr]);
          reco_iteration.push_back(reco_combination);
        }
      }
      reco_combinations = reco_iteration;
    }
  }

  if(Verbosity()>5)
  {
    std::cout << "reco combinations:" << std::endl;
    std::cout << "{";
    for(size_t i=0;i<reco_combinations.size();i++)
    {
      std::cout << "{";
      for(size_t j=0;j<reco_combinations[i].size();j++)
      {
        std::cout << reco_combinations[i][j]->get_id();
        if(j<reco_combinations[i].size()-1) std::cout << ", ";
      }
      std::cout << "}";
      if(i<reco_combinations.size()-1) std::cout << ", ";
    }
    std::cout << "}" << std::endl;
  }
  return reco_combinations;
}

int ResonanceGeometricAcceptance::get_mother_PDGID(const std::vector<PHG4Particle*>& daughters) const
{
  bool all_conjugate_daughters = std::all_of(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),[&](int pdgid){ return has_daughter(-1*pdgid,daughters); });
  if(all_conjugate_daughters) 
  {
    return -1*m_mother_pdgid;
  }
  else
  {
    return m_mother_pdgid;
  }
}

void ResonanceGeometricAcceptance::identify(KFParticle particle) const
{
  std::cout << "-----------------------------------------------------------------------" << std::endl;
  std::cout << "Track ID: " << particle.Id() << std::endl;
  std::cout << "PDG ID: " << particle.GetPDG() << ", charge: " << (int) particle.GetQ() << ", mass: " << particle.GetMass() << " GeV" << std::endl;
  std::cout << "(px,py,pz) = (" << particle.GetPx() << " +/- " << std::sqrt(particle.GetCovariance(3, 3)) << ", ";
  std::cout << particle.GetPy() << " +/- " << std::sqrt(particle.GetCovariance(4, 4)) << ", ";
  std::cout << particle.GetPz() << " +/- " << std::sqrt(particle.GetCovariance(5, 5)) << ") GeV" << std::endl;
  std::cout << "(x,y,z) = (" << particle.GetX() << " +/- " << std::sqrt(particle.GetCovariance(0, 0)) << ", ";
  std::cout << particle.GetY() << " +/- " << std::sqrt(particle.GetCovariance(1, 1)) << ", ";
  std::cout << particle.GetZ() << " +/- " << std::sqrt(particle.GetCovariance(2, 2)) << ") cm\n"
            << std::endl;
  std::cout << "-----------------------------------------------------------------------" << std::endl;
}

int ResonanceGeometricAcceptance::get_best_reco_vertex(KFParticle mother, const int crossing) const
{
  int best_masserr_vertex_id = -1;
  float best_mass_err = FLT_MAX;

  int best_dcachi2_vertex_id = -1;
  float best_dcachi2 = FLT_MAX;

  for(auto vtx_iter = m_vertexmap->begin(); vtx_iter != m_vertexmap->end(); ++vtx_iter)
  {
    SvtxVertex* vertex = (*vtx_iter).second;
    if(vertex->get_beam_crossing() != crossing)
    {
      continue;
    }
    KFParticle mother_copy = mother;
    KFParticle kfv = make_KFParticle_Vertex(vertex);
    mother_copy.SetProductionVertex(kfv);

    if(mother_copy.GetErrMass() < best_mass_err)
    {
      best_masserr_vertex_id = vertex->get_id();
      best_mass_err = mother_copy.GetErrMass();
    }

    if(mother_copy.GetDeviationFromVertex(kfv) < best_dcachi2)
    {
      best_dcachi2_vertex_id = vertex->get_id();
      best_dcachi2 = mother_copy.GetDeviationFromVertex(kfv);
    }
  }

  if(Verbosity()>5)
  {
    std::cout << "best vertex ID by DCA chi2: " << best_dcachi2_vertex_id << ", by mass err: " << best_masserr_vertex_id << std::endl;
  }

  return best_dcachi2_vertex_id;
}

KFParticle ResonanceGeometricAcceptance::make_KFParticle_Vertex(SvtxVertex* vertex) const
{
  double parameters[6] = {vertex->get_x(), vertex->get_y(), vertex->get_z()};
  double covariance[21];
  for(int i=0;i<21;i++)
  {
    covariance[i] = 0.;
  }
  int index = 0;
  for(int i=0;i<3;i++)
  {
    for(int j=0;j<=i;j++)
    {
      covariance[index] = vertex->get_error(i,j);
      index++;
    }
  }

  KFParticle kfv;
  kfv.Create(parameters,covariance,0,-1);
  kfv.Chi2() = vertex->get_chisq();
  kfv.NDF() = vertex->get_ndof();

  return kfv;
}

KFParticle ResonanceGeometricAcceptance::make_KFParticle(PHG4Particle* particle, SvtxTrack* track) const
{
  double mass = TDatabasePDG::Instance()->GetParticle(particle->get_pid())->Mass();

  double parameters[6] = {track->get_x(), track->get_y(), track->get_z(), track->get_px(), track->get_py(), track->get_pz()};
  double covariance[21];
  int index = 0;
  for(int i=0;i<6;i++)
  {
    for(int j=0;j<=i;j++)
    {
      covariance[index] = track->get_error(i,j);
      index++;
    }
  }
  double chi2 = track->get_chisq();
  double ndf = track->get_ndf();
  int charge = track->get_charge();

  KFParticle kfp;
  kfp.SetConstructMethod(2);
  kfp.Create(parameters,covariance,charge,mass);
  kfp.Chi2() = chi2;
  kfp.NDF() = ndf;
  kfp.SetPDG(particle->get_pid());
  kfp.SetId(track->get_id());

  return kfp;
}
/*
bool ResonanceGeometricAcceptance::truth_track_is_best_match_to_reco_track(const PHG4Particle* particle) const
{
  SvtxTrack* reco_track = get_reco_track(particle);
  PHG4Particle* truth_track = nullptr;
  std::map<float, std::set<int>> truth_set = m_reco_truth_map->get(reco_track->get_id());
  std::cout << "getting truth track ID for reco track " << reco_track->get_id() << std::endl;
  for(auto [weight, trackset] : truth_set)
  {
    std::cout << "reco track " << reco_track->get_id() << " pairs with weight " << weight << " on the following truth tracks:" << std::endl;
    for(int trackID : trackset)
    {
      std::cout << trackID << ", ";
    }
    std::cout << std::endl;
  }
  if (!truth_set.empty())
  {
    std::pair<float, std::set<int>> best_weight = *truth_set.rbegin();
    int best_truth_id = *best_weight.second.rbegin();
    truth_track = m_truth->GetParticle(best_truth_id);
  }
  std::cout << "reco track " << reco_track->get_id() << " pairs with truth track " << truth_track->get_track_id() << std::endl;
  return (truth_track->get_track_id() == particle->get_track_id());
}
*/
std::set<SvtxTrack*> ResonanceGeometricAcceptance::get_reco_matches_with_silicon_hits(const PHG4Particle* particle) const
{
  std::set<SvtxTrack*> reco_matches = get_best_reco_matches(particle);

  std::set<SvtxTrack*> reco_matches_with_silicon_hits;

  for(SvtxTrack* reco_match : reco_matches)
  {
    TrackSeed* si_seed = reco_match->get_silicon_seed();
    if(si_seed && si_seed->size_cluster_keys()>0)
    {
      if(Verbosity()>5)
      {
        std::cout << "matched reco track with ID " << reco_match->get_id() << " has " << si_seed->size_cluster_keys() << " silicon hits" << std::endl;
      }
      reco_matches_with_silicon_hits.insert(reco_match);
    }
  }

  return reco_matches_with_silicon_hits;
}

std::set<SvtxTrack*> ResonanceGeometricAcceptance::get_best_reco_matches(const PHG4Particle* particle) const
{
  std::set<SvtxTrack*> reco_matches;
  std::map<float, std::set<unsigned int>> reco_set = m_truth_reco_map->get(particle->get_track_id());
  if(Verbosity()>5)
  {
    std::cout << "getting reco track ID for truth track " << particle->get_track_id() << " (PDGID " << particle->get_pid() << ")" << std::endl;
  }
  for(auto [weight, trackset] : reco_set)
  {
    if(Verbosity()>5)
    {
      std::cout << "truth track " << particle->get_track_id() << " pairs with weight " << weight << " on the following reco tracks:" << std::endl;
      for(unsigned int trackID : trackset)
      {
        std::cout << trackID << ", ";
      }
      std::cout << std::endl;
    }
  }
  if (!reco_set.empty())
  {
    for(const auto& [weight, track_set] : reco_set)
    {
      if(weight>m_truth_weight_threshold)
      {
        for(unsigned int trackID : track_set)
        {
          reco_matches.insert(m_trackmap->get(trackID));
        }
      }
    }
  }
  return reco_matches;
}

bool ResonanceGeometricAcceptance::has_all_daughters(const std::vector<PHG4Particle*>& daughters) const
{
  bool all_daughters = std::all_of(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),[&](int pdgid){ return has_daughter(pdgid,daughters); });
  bool all_conjugate_daughters = std::all_of(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),[&](int pdgid){ return has_daughter(-1*pdgid,daughters); });
  return (all_daughters || (m_include_conjugate && all_conjugate_daughters));
}

bool ResonanceGeometricAcceptance::has_daughter(const int pdgid, const std::vector<PHG4Particle*>& daughters) const
{
  return (std::find_if(daughters.begin(),daughters.end(),
    [&](PHG4Particle* daughter){ return (daughter->get_pid() == pdgid); }) 
    != daughters.end());
}

bool ResonanceGeometricAcceptance::check_charge_swaps(const std::vector<PHG4Particle*>& daughters, const std::vector<SvtxTrack*>& reco_daughters) const
{
  if(Verbosity()>5)
  {
    std::cout << "charge swap check:" << std::endl;
  }
  // get the set of "effective" PDG IDs after any charge swaps
  if(daughters.size() != reco_daughters.size())
  {
    std::cout << "ERROR: truth and reco daughters have different number of tracks! Something has gone very wrong here..." << std::endl;
    return false;
  }
  std::vector<int> reco_daughter_PDGIDs;
  for(size_t i=0; i<daughters.size(); i++)
  {
    int gflavor = daughters[i]->get_pid();
    SvtxTrack* reco_track = reco_daughters[i];
    const int reco_charge = reco_track->get_charge();
    if(Verbosity()>5)
    {
      std::cout << "daughter_gflavor: " << gflavor << ", charge: " << reco_charge << std::endl;
    }
    const int truth_charge = gflavor > 0 ? 1 : -1;

    // charges match if their product is 1
    if(reco_charge*truth_charge == 1)
    {
      reco_daughter_PDGIDs.push_back(gflavor);
    }
    else
    {
      reco_daughter_PDGIDs.push_back(-1*gflavor);
    }
  }

  // check to see that we still have the right mix of flavors to form a secondary vertex
  bool has_all_daughters = std::all_of(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),
    [&](int flavor){ return std::find(reco_daughter_PDGIDs.begin(),reco_daughter_PDGIDs.end(),flavor) != reco_daughter_PDGIDs.end(); });
  bool has_all_opposite_daughters = std::all_of(m_daughter_pdgids.begin(),m_daughter_pdgids.end(),
    [&](int flavor){ return std::find(reco_daughter_PDGIDs.begin(),reco_daughter_PDGIDs.end(),-1*flavor) != reco_daughter_PDGIDs.end(); });

  if(has_all_daughters && Verbosity()>5) 
  {
    std::cout << "has all daughters" << std::endl;
  }
  if(has_all_opposite_daughters && Verbosity()>5) 
  {
    std::cout << "has all opposite daughters" << std::endl;
  }

  bool charge_swap_ok = (has_all_daughters || (m_include_conjugate && has_all_opposite_daughters));
  if(Verbosity()>5)
  {
    std::cout << "charge_swap_ok: " << charge_swap_ok << std::endl;
  }
  return charge_swap_ok;
}

bool ResonanceGeometricAcceptance::check_crossings_match(const std::vector<SvtxTrack*>& reco_daughters) const
{
  if(Verbosity()>5)
  {
    std::cout << "crossing check:" << std::endl;
    for(SvtxTrack* reco_daughter : reco_daughters)
    {
      std::cout << "daughter crossing: " << reco_daughter->get_crossing() << std::endl;
    }
  }
  bool crossings_match = std::all_of(reco_daughters.begin(),reco_daughters.end(),[&](SvtxTrack* reco_track){ return reco_track->get_crossing() == reco_daughters[0]->get_crossing(); });
  if(Verbosity()>5)
  {
    std::cout << "crossings_match: " << crossings_match << std::endl;
  }
  return crossings_match;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  //std::cout << "ResonanceGeometricAcceptance::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::EndRun(const int runnumber)
{
  std::cout << "ResonanceGeometricAcceptance::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::End([[maybe_unused]] PHCompositeNode *topNode)
{
  //std::cout << "ResonanceGeometricAcceptance::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  if(Verbosity()>0)
  {
    std::cout << m_mother_name << " (passing / all) = " << passing_candidates_vspt->GetEntries() << " / " << all_candidates_vspt->GetEntries() << std::endl;
  }

  outfile->cd();
  all_candidates_vspt->Write();
  all_candidates_vseta->Write();
  all_candidates_vsphi->Write();
  all_candidates_vsrapidity->Write();
  passing_candidates_vspt->Write();
  passing_candidates_vseta->Write();
  passing_candidates_vsphi->Write();
  passing_candidates_vsrapidity->Write();
  outfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ResonanceGeometricAcceptance::Reset([[maybe_unused]] PHCompositeNode *topNode)
{
  //std::cout << "ResonanceGeometricAcceptance::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void ResonanceGeometricAcceptance::Print(const std::string &what) const
{
  std::cout << "ResonanceGeometricAcceptance::Print(const std::string &what) const Printing info for " << what << std::endl;
}
