#include "massRecoAnalysis.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>


int massRecoAnalysis::process_event(PHCompositeNode * /**topNode*/)
{
  // Loop over tracks and check for close DCA match with all other tracks
  for(auto tr1_it = m_svtxTrackMap->begin(); tr1_it != m_svtxTrackMap->end(); ++tr1_it)
    {
      auto id1 = tr1_it->first;
      auto tr1 = tr1_it->second;
      if(tr1->get_quality() > _qual_cut) continue;

      // calculate number silicon tracks
      double this_dca_cut    = track_dca_cut;
      TrackSeed *siliconseed = tr1->get_silicon_seed();
      if(!siliconseed)
	{
	  this_dca_cut *= 5;
	  if(Verbosity()>2){std::cout << "silicon seed not found" << std::endl;}
	  if(_require_mvtx){continue;}
	}
      Acts::Vector3 pos1(tr1->get_x(), tr1->get_y(), tr1->get_z());
      Acts::Vector3 mom1(tr1->get_px(), tr1->get_py(), tr1->get_pz());
      Acts::Vector3 dcaVals1 = calculateDca(tr1, mom1, pos1);
      // first dca cuts
      if(dcaVals1(0) < this_dca_cut or dcaVals1(1) < this_dca_cut) continue;

      // look for close DCA matches with all other such tracks
      for(auto tr2_it = std::next(tr1_it); tr2_it != m_svtxTrackMap->end(); ++tr2_it)
	{
	  auto id2 = tr2_it->first;
	  auto tr2 = tr2_it->second;
	  if(tr2->get_quality() > _qual_cut) continue;

          // calculate number silicon tracks
          TrackSeed *siliconseed2 = tr2->get_silicon_seed();
          double this_dca_cut2    = track_dca_cut;
          if(!siliconseed2)
	    { 
	      this_dca_cut2 *= 5;
	      if(Verbosity()>2){std::cout << "silicon seed not found" << std::endl;}
	      if(_require_mvtx){continue;}
	    }

          // dca xy and dca z cut here compare to track dca cut
          Acts::Vector3 pos2(tr2->get_x(), tr2->get_y(), tr2->get_z());
          Acts::Vector3 mom2(tr2->get_px(), tr2->get_py(), tr2->get_pz());
          Acts::Vector3 dcaVals2 = calculateDca(tr2, mom2, pos2);

          if(dcaVals2(0) < this_dca_cut2 or dcaVals2(1) < this_dca_cut2) continue;
	  
	  // find DCA of these two tracks
	  if(Verbosity() > 3) std::cout << "Check DCA for tracks " << id1 << " and  " << id2 << std::endl;

          if(tr1->get_charge() == tr2->get_charge()) continue;
	  
	  // declare these variables to pass into findPCAtwoTracks by reference
          double pair_dca;
          Acts::Vector3 pca_rel1;
          Acts::Vector3 pca_rel2;
	  double invariantMass;
	  double invariantPt;

	  findPcaTwoTracks(tr1, tr2, pca_rel1, pca_rel2, pair_dca);

	  // tracks with small relative pca are k short candidates
          if(abs(pair_dca) < pair_dca_cut)
	    {
	      fillHistogram(tr1,tr2,recomass,invariantMass,invariantPt);
	      fillNtp(tr1,tr2,dcaVals1,dcaVals2,pca_rel1,pca_rel2,pair_dca,invariantMass,invariantPt);

	       if(Verbosity() > 2 )
	       	{
		  std::cout << "Accepted Track Pair" << std::endl;
		  std::cout << " invariant mass: " << invariantMass<<std::endl;
		  std::cout << " track1 dca_cut: " << this_dca_cut<< " track2 dca_cut: " << this_dca_cut2 <<std::endl;
		  std::cout << " dca3dxy1,dca3dz1,phi1: " << dcaVals1 << std::endl;
		  std::cout << " dca3dxy2,dca3dz2,phi2: " << dcaVals2 << std::endl;
		  std::cout << " Relative PCA = "<< abs(pair_dca) << " pca_cut = " << pair_dca_cut <<std::endl;
		  std::cout << " charge 1: " << tr1->get_charge() << " charge2: "<< tr2->get_charge() << std::endl;
		}
            }
	}
    }
  return 0;
}

void massRecoAnalysis::fillNtp(SvtxTrack *track1, SvtxTrack *track2, Acts::Vector3 dcavals1, Acts::Vector3 dcavals2,
			       Acts::Vector3 pca_rel1, Acts::Vector3 pca_rel2, double pair_dca, double invariantMass, double invariantPt)
{
  double px1          = track1->get_px();
  double py1          = track1->get_py();
  double pz1          = track1->get_pz();
  auto tpcSeed1       = track1->get_tpc_seed();
  size_t tpcClusters1 = tpcSeed1->size_cluster_keys();
  double eta1         = asinh(pz1 / sqrt(pow(px1, 2) + pow(py1, 2)));

  double px2          = track2->get_px();
  double py2          = track2->get_py();
  double pz2          = track2->get_pz();
  auto tpcSeed2       = track2->get_tpc_seed();
  size_t tpcClusters2 = tpcSeed2->size_cluster_keys();
  double eta2         = asinh(pz2 / sqrt(pow(px2, 2) + pow(py2, 2)));

  auto vtxid      = track1->get_vertex_id();
  auto svtxVertex = m_vertexMap->get(vtxid);
  if(!svtxVertex){ return; }
  
  float reco_info[] = {track1->get_x(), track1->get_y(), track1->get_z(), track1->get_px(), track1->get_py(), track1->get_pz(), (float) dcavals1(0), (float) dcavals1(1), (float) dcavals1(2), (float) pca_rel1(0), (float) pca_rel1(1), (float) pca_rel1(2), (float) eta1,  (float) track1->get_charge(), (float) tpcClusters1, track2->get_x(), track2->get_y(), track2->get_z(),  track2->get_px(), track2->get_py(), track2->get_pz(), (float) dcavals2(0), (float) dcavals2(1), (float) dcavals2(2), (float) pca_rel2(0), (float) pca_rel2(1), (float) pca_rel2(2), (float) eta2, (float) track2->get_charge(), (float) tpcClusters2, svtxVertex->get_x(), svtxVertex->get_y(), svtxVertex->get_z(), (float) pair_dca,(float) invariantMass, (float) invariantPt};

  ntp_reco_info->Fill(reco_info);
}

void massRecoAnalysis::fillHistogram(SvtxTrack *track1, SvtxTrack *track2, TH1D *massreco, double& invariantMass, double& invariantPt)
{
  double E1 = sqrt(pow(track1->get_px(),2) + pow(track1->get_py(),2) + pow(track1->get_pz(),2) + pow(decaymass,2));
  double E2 = sqrt(pow(track2->get_px(),2) + pow(track2->get_py(),2) + pow(track2->get_pz(),2) + pow(decaymass,2));

  TLorentzVector v1 (track1->get_px(),track1->get_py(),track1->get_pz(),E1);
  TLorentzVector v2 (track2->get_px(),track2->get_py(),track2->get_pz(),E2);

  TLorentzVector tsum;
  tsum = v1 + v2;
  invariantMass = tsum.M();
  invariantPt   = tsum.Pt();

  

  if(Verbosity() > 2)
    {
      std::cout << "px1: " << track1->get_px()<< " py1: "<<track1->get_py() << " pz1: "<< track1->get_pz()<< " E1: "<<E1 << std::endl;
      std::cout << "px2: " << track2->get_px()<< " py2: "<<track2->get_py() << " pz2: "<< track2->get_pz()<< " E2: "<<E2 << std::endl;
      std::cout << "tsum: " <<tsum(0)<<" "<<tsum(1)<<" " <<tsum(2)<< " " <<tsum(3) << std::endl;
      std::cout << "invariant mass: " << invariantMass << " invariant Pt: "<< invariantPt<< std::endl;
    }   

  if(invariantPt<pt_cut)
    {
      massreco->Fill(invariantMass);
    }
}

void massRecoAnalysis::findPcaTwoTracks(SvtxTrack *track1, SvtxTrack *track2, Acts::Vector3& pca1, Acts::Vector3& pca2, double& dca)
{
  TLorentzVector v1;
  TLorentzVector v2;

  double px1 = track1->get_px();
  double py1 = track1->get_py();
  double pz1 = track1->get_pz();
  double px2 = track2->get_px();
  double py2 = track2->get_py();
  double pz2 = track2->get_pz();
  Float_t E1 = sqrt(pow(px1,2) + pow(py1,2) + pow(pz1,2) + pow(decaymass,2));
  Float_t E2 = sqrt(pow(px2,2) + pow(py2,2) + pow(pz2,2) + pow(decaymass,2));
  
  v1.SetPxPyPzE(px1,py1,pz1,E1);
  v2.SetPxPyPzE(px2,py2,pz2,E2);	
  
  //give method tracks and calculate lorentz vector here
  Eigen::Vector3d a1 = Eigen::Vector3d(track1->get_x(),track1->get_y(),track1->get_z());
  Eigen::Vector3d a2 = Eigen::Vector3d(track2->get_x(),track2->get_y(),track2->get_z());

  Eigen::Vector3d b1(v1.Px(), v1.Py(), v1.Pz());
  Eigen::Vector3d b2(v2.Px(), v2.Py(), v2.Pz());

  // The shortest distance between two skew lines described by
  //  a1 + c * b1
  //  a2 + d * b2
  // where a1, a2, are vectors representing points on the lines, b1, b2 are direction vectors, and c and d are scalars
  // dca = (b1 x b2) .(a2-a1) / |b1 x b2|

  // bcrossb/mag_bcrossb is a unit vector perpendicular to both direction vectors b1 and b2
  auto bcrossb     = b1.cross(b2);
  auto mag_bcrossb = bcrossb.norm();
  // a2-a1 is the vector joining any arbitrary points on the two lines
  auto aminusa = a2 - a1;

  // The DCA of these two lines is the projection of a2-a1 along the direction of the perpendicular to both 
  // remember that a2-a1 is longer than (or equal to) the dca by definition
  dca = 999;
  if( mag_bcrossb != 0)
    dca = bcrossb.dot(aminusa) / mag_bcrossb;
  else
    return;   // same track, skip combination
  
  // get the points at which the normal to the lines intersect the lines, where the lines are perpendicular
  double X = b1.dot(b2) - b1.dot(b1) * b2.dot(b2) / b2.dot(b1);
  double Y = (a2.dot(b2) - a1.dot(b2)) - (a2.dot(b1) - a1.dot(b1)) * b2.dot(b2) / b2.dot(b1) ;
  double c = Y/X;

  double F = b1.dot(b1) / b2.dot(b1); 
  double G = - (a2.dot(b1) - a1.dot(b1)) / b2.dot(b1);
  double d = c * F + G;

  // then the points of closest approach are:
  pca1 = a1 + c*b1;
  pca2 = a2 + d*b2;

  return;  
}

massRecoAnalysis::massRecoAnalysis(const std::string &name): SubsysReco(name){}

Acts::Vector3 massRecoAnalysis::calculateDca(SvtxTrack *track, Acts::Vector3 momentum, Acts::Vector3 position)
    {
     Acts::Vector3 outVals(999,999,999);
     auto vtxid = track->get_vertex_id();
     if(!m_vertexMap)
       {
	 std::cout << "Could not find m_vertexmap "  << std::endl;
	 return outVals;
       }
     auto svtxVertex = m_vertexMap->get(vtxid);
     if(!svtxVertex)
       { 
	 std::cout << "Could not find vtxid in m_vertexMap " << vtxid  << std::endl;
	 return outVals; 
       }
     Acts::Vector3 vertex(svtxVertex->get_x(), svtxVertex->get_y(), svtxVertex->get_z());   
     position -= vertex;
     Acts::Vector3 r = momentum.cross(Acts::Vector3(0.,0.,1.));
     float phi       = atan2(r(1), r(0));
  
     Acts::RotationMatrix3 rot;
     rot(0,0) = cos(phi);
     rot(0,1) = -sin(phi);
     rot(0,2) = 0;
     rot(1,0) = sin(phi);
     rot(1,1) = cos(phi);
     rot(1,2) = 0;
     rot(2,0) = 0;
     rot(2,1) = 0;
     rot(2,2) = 1;
  
     Acts::Vector3 pos_R = rot * position;
     double dca3dxy      = pos_R(0);
     double dca3dz       = pos_R(2);

     outVals(0) = abs(dca3dxy);
     outVals(1) = abs(dca3dz);
     outVals(2) = phi;

     if(Verbosity() > 2)
       {
	 std::cout << " pre-position: "<<position << std::endl;
	 std::cout << " vertex: "<<vertex << std::endl; 
	 std::cout << " vertex subtracted-position: "<<position << std::endl;
       }

     return outVals;
    }

int massRecoAnalysis::InitRun(PHCompositeNode *topNode)
{
  char fileName[500];
  sprintf(fileName, "eval_output/ntp_mass_out_%i.root",process);
  fout = new TFile(fileName,"recreate");
  ntp_reco_info = new TNtuple("ntp_reco_info","decay_pairs","x1:y1:z1:px1:py1:pz1:dca3dxy1:dca3dz1:phi1:pca_relx_1:pca_rely_1:pca_relz_1:eta1:charge1:tpcClusters_1:x2:y2:z2:px2:py2:pz2:dca3dxy2:dca3dz2:phi2:pca_relx_2:pca_rely_2:pca_relz_2:eta2:charge2:tpcClusters_2:vertex_x:vertex_y:vertex_z:pair_dca:invariant_mass:invariant_pt");
  getNodes(topNode);
  
  char name[500];
  sprintf(name, "recomass");
  recomass = new TH1D(name,name,1000,0.0,1);  //root histogram arguments: name,title,bins,minvalx,maxvalx

  return 0;
}

int massRecoAnalysis::End(PHCompositeNode * /**topNode*/)
{
  fout->cd();
  ntp_reco_info->Write();
  recomass->Write();
  fout->Close();

  return 0;
}

int massRecoAnalysis::getNodes(PHCompositeNode *topNode)
{
  m_svtxTrackMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!m_svtxTrackMap)
    {
      std::cout << PHWHERE << "No SvtxTrackMap on node tree, exiting." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
      
  m_vertexMap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!m_vertexMap)
    {
      std::cout << PHWHERE << "No vertexMap on node tree, exiting." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
