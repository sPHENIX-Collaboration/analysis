#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phhepmc/PHGenIntegral.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <TLorentzVector.h>
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#include <iostream>

#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

#include <g4eval/SvtxEvalStack.h>
#include <sstream>

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include "TruthPhotonJet.h"

using namespace std;

TruthPhotonJet::TruthPhotonJet(const std::string &name)
  : SubsysReco("TRUTHPHOTONJET")
{
  initialize_values();

  outfilename = name;

  //add other initializers here
  //default use isocone algorithm
  use_isocone = 0;

  eval_tracked_jets = 0;

  //default use 0.3 jet cone
  jet_cone_size = 3;
  nevents = 0;

  //default to barrel
  etalow = -1;
  etahigh = 1;

  //default jetminptcut
  minjetpt = 4.;

  //default mincluscut
  mincluspt = 0.5;

  //default to photon-jet instead of dijet
  _is_dijet_process = 0;
}

int TruthPhotonJet::Init(PHCompositeNode *topnode)
{
  cout << "GATHERING JETS: " << jet_cone_size << endl;
  cout << "GATHERING IN ETA: [" << etalow
       << "," << etahigh << "]" << endl;
  cout << "EVALUATING TRACKED JETS: " << eval_tracked_jets << endl;
  cout << "USING ISOLATION CONE: " << use_isocone << endl;

  file = new TFile(outfilename.c_str(), "RECREATE");

  ntruthconstituents_h = new TH1F("ntruthconstituents", "", 200, 0, 200);
  histo = new TH1F("histo", "histo", 100, -3, 3);

  tree = new TTree("tree", "a tree");
  tree->Branch("nevents", &nevents, "nevents/I");

  Set_Tree_Branches();

  return 0;
}

int TruthPhotonJet::process_event(PHCompositeNode *topnode)
{
  if(nevents % 50 == 0)
    cout << "at event number " << nevents << endl;

  //get the nodes from the NodeTree

  //get the requested size jets
  ostringstream truthjetsize;

  truthjetsize.str("");
  truthjetsize << "AntiKt_Truth_r";

  //cout<<"Gathering Jets:  "<<recojetsize.str().c_str()<<endl;
  //cout<<"Gathering Jets:  "<<truthjetsize.str().c_str()<<endl;
  if (jet_cone_size == 2)
  {
    truthjetsize << "02";
  }
  else if (jet_cone_size == 3)
  {
    truthjetsize << "03";
  }
  else if (jet_cone_size == 4)
  {
    truthjetsize << "04";
  }
  else if (jet_cone_size == 5)
  {
    truthjetsize << "05";
  }
  else if (jet_cone_size == 6)
  {
    truthjetsize << "06";
  }

  else if (jet_cone_size == 7)
  {
    truthjetsize << "07";
  }
  //if its some other number just set it to 0.4

  else
  {
    truthjetsize << "04";
  }

  JetMap *truth_jets =
      //findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r04");
      findNode::getClass<JetMap>(topnode, truthjetsize.str().c_str());

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode, "G4TruthInfo");

  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  if (!truth_jets)
  {
    cout << "no truth jets" << endl;
    return 0;
  }
  if (!truthinfo)
  {
    cout << "no truth track info" << endl;
    return 0;
  }
  JetEvalStack *_jetevalstack = new JetEvalStack(topnode, truthjetsize.str().c_str(), truthjetsize.str().c_str());
  JetTruthEval *trutheval = _jetevalstack->get_truth_eval();

  /***********************************************

  GET ALL THE HEPMC EVENT LEVEL TRUTH PARTICLES

  ************************************************/
  if (Verbosity() > 1)
    cout << "GETTING HEPMC" << endl;

  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topnode, "PHHepMCGenEventMap");

  if (!hepmceventmap)
  {
    cout << "hepmc event node is missing, BAILING" << endl;
    return 0;
  }

  //0 corresponds to the hepmc pythia event. 1 is gold gold embedded background
  PHHepMCGenEvent *hepmcevent = hepmceventmap->get(1);

  if (!hepmcevent)
  {
    cout << PHWHERE << "no hepmcevent pointer, bailing" << endl;
    return 0;
  }

  HepMC::GenEvent *truthevent = hepmcevent->getEvent();
  if (!truthevent)
  {
    cout << PHWHERE << "no evt pointer under phhepmvgenevent found " << endl;
    return 0;
  }

  //get the interacting protons
  if (truthevent->valid_beam_particles())
  {
    HepMC::GenParticle *part1 = truthevent->beam_particles().first;

    //beam energy
    beam_energy = fabs(part1->momentum().pz());
  }

  //Parton info
  HepMC::PdfInfo *pdfinfo = truthevent->pdf_info();

  partid1 = pdfinfo->id1();
  partid2 = pdfinfo->id2();
  x1 = pdfinfo->x1();
  x2 = pdfinfo->x2();

  if (Verbosity() > 1)
    cout << "Looping over HEPMC particles" << endl;

  numparticlesinevent = 0;
  process_id = truthevent->signal_process_id();
  for (HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin(); iter != truthevent->particles_end(); ++iter)
  {
    truthenergy = (*iter)->momentum().e();
    truthpid = (*iter)->pdg_id();
    trutheta = (*iter)->momentum().pseudoRapidity();
    truthphi = (*iter)->momentum().phi();
    truthpx = (*iter)->momentum().px();
    truthpy = (*iter)->momentum().py();
    truthpz = (*iter)->momentum().pz();
    truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);

    truthtree->Fill();
    numparticlesinevent++;
  }

  if (Verbosity() > 1)
    cout << "LOOPING OVER G4 TRUTH PARTICLES" << endl;

  cluseventenergy = 0;
  cluseventphi = 0;
  cluseventpt = 0;
  cluseventeta = 0;
  float lastenergy = 0;
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
  {
    PHG4Particle *truth = iter->second;

    truthpx = truth->get_px();
    truthpy = truth->get_py();
    truthpz = truth->get_pz();
    truthp = sqrt(truthpx * truthpx + truthpy * truthpy + truthpz * truthpz);
    truthenergy = truth->get_e();

    TLorentzVector vec;
    vec.SetPxPyPzE(truthpx, truthpy, truthpz, truthenergy);

    truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);

    truthphi = vec.Phi();
    trutheta = vec.Eta();
    truthpid = truth->get_pid();

    if (truthpid == 22 && truthenergy > lastenergy && trutheta < etahigh && trutheta > etalow)
    {
      lastenergy = truthenergy;
      cluseventenergy = truthenergy;
      cluseventpt = truthpt;
      cluseventphi = truthphi;
      cluseventeta = trutheta;
    }
    truth_g4particles->Fill();
  }

  //cout<<"CLUSTER Properties : " << cluseventenergy<< " " <<cluseventphi<< 
  //" " << cluseventeta<<endl;

  if (Verbosity() > 1)
    cout << "LOOPING OVER TRUTH JETS" << endl;
  /***************************************

   TRUTH JETS

   ***************************************/

  float hardest_jet = 0;
  float subleading_jet = 0;
  int numtruthjets = 0;

  hardest_jetpt = 0;
  hardest_jeteta = 0;
  hardest_jetphi = 0;
  hardest_jetenergy = 0;
  for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end(); ++iter)
  {
    Jet *jet = iter->second;

    truthjetpt = jet->get_pt();
    if (truthjetpt < minjetpt)
      continue;

    truthjeteta = jet->get_eta();

    //make the width extra just to be safe
    if (truthjeteta < (etalow - 1) || truthjeteta > (etahigh + 1))
      continue;

    truthjetpx = jet->get_px();
    truthjetpy = jet->get_py();
    truthjetpz = jet->get_pz();
    truthjetphi = jet->get_phi();
    truthjetmass = jet->get_mass();
    truthjetp = jet->get_p();
    truthjetenergy = jet->get_e();
    truthjetxf = truthjetpz / beam_energy;  //just the energy of one beam, so don't need 1/2

    //check that the jet isn't just a photon or something like this
    //needs at least 2 constituents and that 90% of jet isn't from one photon
    std::set<PHG4Particle *> truthjetcomp =
        trutheval->all_truth_particles(jet);
    int _ntruthconstituents = 0;
    int photonflag = 0;
    for (std::set<PHG4Particle *>::iterator iter2 = truthjetcomp.begin();
         iter2 != truthjetcomp.end();
         ++iter2)
    {
      PHG4Particle *truthpart = *iter2;
      if (!truthpart)
      {
        cout << "no truth particles in the jet??" << endl;
        break;
      }
      _ntruthconstituents++;

      TLorentzVector constvec;
      constvec.SetPxPyPzE(truthpart->get_px(), 
			  truthpart->get_py(), 
			  truthpart->get_pz(),
			  truthpart->get_e());

      int constpid = truthpart->get_pid();

      // If this jet contains the truth prompt photon, skip it
    if(fabs(cluseventphi - constvec.Phi()) < 0.02 && 
       fabs(cluseventeta - constvec.Eta()) < 0.02 &&
       constpid == 22)
      {
	photonflag = 1;
	break;
      }
    }
    ntruthconstituents_h->Fill(_ntruthconstituents);

    // If it contains the truth direct photon, skip this jet
    if(photonflag)
      continue;

    if (_ntruthconstituents < 3)
      continue;

    numtruthjets++;

    // if it is photon-jet process then we want the hardest away-side jet
    if (!_is_dijet_process)
    {
      if (truthjetpt > hardest_jet)
      {
        hardest_jet = truthjetpt;
        hardest_jetpt = truthjetpt;
        hardest_jetenergy = truthjetenergy;
        hardest_jeteta = truthjeteta;
        hardest_jetphi = truthjetphi;
	ntruthconstituents = _ntruthconstituents;
      }
    }

    //if it is dijet process we want to overwrite
    //the highest pt photon with the highest pt jet
    if (_is_dijet_process)
    {
      if (truthjetpt > hardest_jet)
      {
        hardest_jet = truthjetpt;
        cluseventenergy = truthjetenergy;
        cluseventpt = truthjetpt;
        cluseventphi = truthjetphi;
        cluseventeta = truthjeteta;
      }
    }

    truthjettree->Fill();
  }


  //cout<<"Jet properties : " << hardest_jetenergy << "  " << truthjetphi
  //  << " " << truthjeteta << endl;

  if (_is_dijet_process && numtruthjets > 1)
  {
    for (JetMap::Iter iter2 = truth_jets->begin(); iter2 != truth_jets->end();
         ++iter2)
    {
      Jet *jet = iter2->second;
      float subtruthjetpt = jet->get_pt();
      if (subtruthjetpt < minjetpt)
        continue;

      float subtruthjeteta = jet->get_eta();
      //make the width extra just to be safe
      if (subtruthjeteta < (etalow - 1) || subtruthjeteta > (etahigh + 1))
        continue;

      float subtruthjetphi = jet->get_phi();
      float subtruthjetenergy = jet->get_e();

      //check that the jet isn't just a photon or something like this
      //needs at least 2 constituents and that 90% of jet isn't from one photon
      std::set<PHG4Particle *> truthjetcomp =
          trutheval->all_truth_particles(jet);
      int ntruthconstituents = 0;

      for (std::set<PHG4Particle *>::iterator iter3 = truthjetcomp.begin();
           iter3 != truthjetcomp.end();
           ++iter3)
      {
        PHG4Particle *truthpart = *iter3;
        if (!truthpart)
        {
          cout << "no truth particles in the jet??" << endl;
          break;
        }
        ntruthconstituents++;
      }

      if (ntruthconstituents < 2)
        continue;

      if (subtruthjetpt > subleading_jet && subtruthjetpt < cluseventpt)
      {
        subleading_jet = subtruthjetpt;
        hardest_jetpt = subtruthjetpt;
        hardest_jetenergy = subtruthjetenergy;
        hardest_jeteta = subtruthjeteta;
        hardest_jetphi = subtruthjetphi;
      }
    }
  }

  eventdphi = cluseventphi - hardest_jetphi;
  event_tree->Fill();
    
  nevents++;
  tree->Fill();



  return 0;
  }

int TruthPhotonJet::End(PHCompositeNode *topnode)
{
  std::cout << " DONE PROCESSING " << endl;

  //get the integrated luminosity

  PHGenIntegral *phgen = findNode::getClass<PHGenIntegral>(topnode, "PHGenIntegral");
  if (phgen)
  {
    intlumi = phgen->get_Integrated_Lumi();
    nprocessedevents = phgen->get_N_Processed_Event();
    nacceptedevents = phgen->get_N_Generator_Accepted_Event();
    xsecprocessedevents = phgen->get_CrossSection_Processed_Event();
    xsecacceptedevents = phgen->get_CrossSection_Generator_Accepted_Event();
    
    runinfo->Fill();
  }
  


  file->Write();
  file->Close();
  return 0;
}

void TruthPhotonJet::Set_Tree_Branches()
{
  runinfo = new TTree("runinfo", "a tree with pythia run info like int lumi");
  runinfo->Branch("intlumi", &intlumi, "intlumi/F");
  runinfo->Branch("nprocessedevents", &nprocessedevents, "nprocessedevents/F");
  runinfo->Branch("nacceptedevents", &nacceptedevents, "nacceptedevents/F");
  runinfo->Branch("xsecprocessedevents", &xsecprocessedevents, "xsecprocessedevents/F");
  runinfo->Branch("xsecacceptedevents", &xsecacceptedevents, "xsecacceptedevents/F");

  truthjettree = new TTree("truthjettree", "a tree with truth jets");
  truthjettree->Branch("truthjetpt", &truthjetpt, "truthjetpt/F");
  truthjettree->Branch("truthjetpx", &truthjetpx, "truthjetpx/F");
  truthjettree->Branch("truthjetpy", &truthjetpy, "truthjetpy/F");
  truthjettree->Branch("truthjetpz", &truthjetpz, "truthjetpz/F");
  truthjettree->Branch("truthjetphi", &truthjetphi, "truthjetphi/F");
  truthjettree->Branch("truthjeteta", &truthjeteta, "truthjeteta/F");
  truthjettree->Branch("truthjetmass", &truthjetmass, "truthjetmass/F");
  truthjettree->Branch("truthjetp", &truthjetp, "truthjetp/F");
  truthjettree->Branch("truthjetenergy", &truthjetenergy, "truthjetenergy/F");
  truthjettree->Branch("nevents", &nevents, "nevents/I");
  truthjettree->Branch("process_id", &process_id, "process_id/I");
  truthjettree->Branch("x1", &x1, "x1/F");
  truthjettree->Branch("x2", &x2, "x2/F");
  truthjettree->Branch("partid1", &partid1, "partid1/I");
  truthjettree->Branch("partid2", &partid2, "partid2/I");
  truthjettree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  truthjettree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  truthjettree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  truthjettree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  truthjettree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  truthjettree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");
  truthjettree->Branch("truthjetxf", &truthjetxf, "truthjetxf/F");

  truth_g4particles = new TTree("truthtree_g4", "a tree with all truth g4 particles");
  truth_g4particles->Branch("truthpx", &truthpx, "truthpx/F");
  truth_g4particles->Branch("truthpy", &truthpy, "truthpy/F");
  truth_g4particles->Branch("truthpz", &truthpz, "truthpz/F");
  truth_g4particles->Branch("truthp", &truthp, "truthp/F");
  truth_g4particles->Branch("truthenergy", &truthenergy, "truthenergy/F");
  truth_g4particles->Branch("truthphi", &truthphi, "truthphi/F");
  truth_g4particles->Branch("trutheta", &trutheta, "trutheta/F");
  truth_g4particles->Branch("truthpt", &truthpt, "truthpt/F");
  truth_g4particles->Branch("truthpid", &truthpid, "truthpid/I");
  truth_g4particles->Branch("nevents", &nevents, "nevents/I");
  truth_g4particles->Branch("process_id", &process_id, "process_id/I");
  truth_g4particles->Branch("x1", &x1, "x1/F");
  truth_g4particles->Branch("x2", &x2, "x2/F");
  truth_g4particles->Branch("partid1", &partid1, "partid1/I");
  truth_g4particles->Branch("partid2", &partid2, "partid2/I");
  truth_g4particles->Branch("E_4x4", &E_4x4, "E_4x4/F");
  truth_g4particles->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  truth_g4particles->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  truth_g4particles->Branch("E_2x2", &E_2x2, "E_2x2/F");
  truth_g4particles->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  truth_g4particles->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  truthtree = new TTree("truthtree", "a tree with all truth pythia particles");
  truthtree->Branch("truthpx", &truthpx, "truthpx/F");
  truthtree->Branch("truthpy", &truthpy, "truthpy/F");
  truthtree->Branch("truthpz", &truthpz, "truthpz/F");
  truthtree->Branch("truthp", &truthp, "truthp/F");
  truthtree->Branch("truthenergy", &truthenergy, "truthenergy/F");
  truthtree->Branch("truthphi", &truthphi, "truthphi/F");
  truthtree->Branch("trutheta", &trutheta, "trutheta/F");
  truthtree->Branch("truthpt", &truthpt, "truthpt/F");
  truthtree->Branch("truthpid", &truthpid, "truthpid/I");
  truthtree->Branch("nevents", &nevents, "nevents/I");
  truthtree->Branch("numparticlesinevent", &numparticlesinevent, "numparticlesinevent/I");
  truthtree->Branch("process_id", &process_id, "process_id/I");
  truthtree->Branch("x1", &x1, "x1/F");
  truthtree->Branch("x2", &x2, "x2/F");
  truthtree->Branch("partid1", &partid1, "partid1/I");
  truthtree->Branch("partid2", &partid2, "partid2/I");

  event_tree = new TTree("eventtree", "A tree with 2 to 2 event info");
  event_tree->Branch("x1", &x1, "x1/F");
  event_tree->Branch("x2", &x2, "x2/F");
  event_tree->Branch("partid1", &partid1, "partid1/I");
  event_tree->Branch("partid2", &partid2, "partid2/I");
  event_tree->Branch("process_id", &process_id, "process_id/I");
  event_tree->Branch("nevents", &nevents, "nevents/I");
  event_tree->Branch("cluseventenergy", &cluseventenergy, "cluseventenergy/F");
  event_tree->Branch("cluseventeta", &cluseventeta, "cluseventeta/F");
  event_tree->Branch("cluseventpt", &cluseventpt, "cluseventpt/F");
  event_tree->Branch("cluseventphi", &cluseventphi, "cluseventphi/F");
  event_tree->Branch("hardest_jetpt", &hardest_jetpt, "hardest_jetpt/F");
  event_tree->Branch("hardest_jetphi", &hardest_jetphi, "hardest_jetphi/F");
  event_tree->Branch("hardest_jeteta", &hardest_jeteta, "hardest_jeteta/F");
  event_tree->Branch("hardest_jetenergy", &hardest_jetenergy, "hardest_jetenergy/F");
  event_tree->Branch("ntruthconstituents", &ntruthconstituents, "ntruthconstituents/I");
  event_tree->Branch("eventdphi", &eventdphi, "eventdphi/F");
  event_tree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  event_tree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  event_tree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  event_tree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  event_tree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  event_tree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");
}

void TruthPhotonJet::initialize_values()
{
  event_tree = 0;
  tree = 0;

  truth_g4particles = 0;
  truthtree = 0;

  truthjettree = 0;

  histo = 0;

  _truthjetmass = -999;
  clus_energy = -999;
  clus_eta = -999;
  clus_phi = -999;
  clus_pt = -999;
  clus_px = -999;
  clus_py = -999;
  clus_pz = -999;
  clus_theta = -999;
  clus_x = -999;
  clus_y = -999;
  clus_z = -999;
  clus_t = -999;

  tr_px = -999;
  tr_py = -999;
  tr_pz = -999;
  tr_p = -999;
  tr_pt = -999;
  tr_phi = -999;
  tr_eta = -999;
  charge = -999;
  chisq = -999;
  ndf = -999;
  dca = -999;
  tr_x = -999;
  tr_y = -999;
  tr_z = -999;
  truthtrackpx = -999;
  truthtrackpy = -999;
  truthtrackpz = -999;
  truthtrackp = -999;
  truthtracke = -999;
  truthtrackpt = -999;
  truthtrackphi = -999;
  truthtracketa = -999;
  truthtrackpid = -999;
  truth_is_primary = false;

  truthjetpt = -999;
  truthjetpx = -999;
  truthjetpy = -999;
  truthjetpz = -999;
  truthjetphi = -999;
  truthjeteta = -999;
  truthjetmass = -999;
  truthjetp = -999;
  truthjetenergy = -999;
  recojetpt = -999;
  recojetpx = -999;
  recojetpy = -999;
  recojetpz = -999;
  recojetphi = -999;
  recojeteta = -999;
  recojetmass = -999;
  recojetp = -999;
  recojetenergy = -999;
  recojetid = -999;
  truthjetid = -999;

  _recojetid = -999;
  _recojetpt = -999;
  _recojetpx = -999;
  _recojetpy = -999;
  _recojetpz = -999;
  _recojetphi = -999;
  _recojeteta = -999;
  _recojetmass = -999;
  _recojetp = -999;
  _recojetenergy = -999;
  jetdphi = -999;
  jetpout = -999;
  jetdeta = -999;
  _truthjetid = -999;
  _truthjetpt = -999;
  _truthjetpx = -999;
  _truthjetpy = -999;
  _truthjetpz = -999;
  _truthjetphi = -999;
  _truthjeteta = -999;
  _truthjetmass = -999;
  _truthjetp = -999;
  _truthjetenergy = -999;

  _trecojetid = -999;
  _trecojetpt = -999;
  _trecojetpx = -999;
  _trecojetpy = -999;
  _trecojetpz = -999;
  _trecojetphi = -999;
  _trecojeteta = -999;
  _trecojetmass = -999;
  _trecojetp = -999;
  _trecojetenergy = -999;
  tjetdphi = -999;
  tjetpout = -999;
  tjetdeta = -999;
  _ttruthjetid = -999;
  _ttruthjetpt = -999;
  _ttruthjetpx = -999;
  _ttruthjetpy = -999;
  _ttruthjetpz = -999;
  _ttruthjetphi = -999;
  _ttruthjeteta = -999;
  _ttruthjetmass = -999;
  _ttruthjetp = -999;
  _ttruthjetenergy = -999;

  intlumi = -999;
  nprocessedevents = -999;
  nacceptedevents = -999;
  xsecprocessedevents = -999;
  xsecacceptedevents = -999;

  _tr_px = -999;
  _tr_py = -999;
  _tr_pz = -999;
  _tr_p = -999;
  _tr_pt = -999;
  _tr_phi = -999;
  _tr_eta = -999;
  _charge = -999;
  _chisq = -999;
  _ndf = -999;
  _dca = -999;
  _tr_x = -999;
  _tr_y = -999;
  _tr_z = -999;
  haddphi = -999;
  hadpout = -999;
  haddeta = -999;
  _truth_is_primary = false;
  _truthtrackpx = -999;
  _truthtrackpy = -999;
  _truthtrackpz = -999;
  _truthtrackp = -999;
  _truthtracke = -999;
  _truthtrackpt = -999;
  _truthtrackphi = -999;
  _truthtracketa = -999;
  _truthtrackpid = -999;

  eventdphi = -999;
  truthpx = -999;
  truthpy = -999;
  truthpz = -999;
  truthp = -999;
  truthphi = -999;
  trutheta = -999;
  truthpt = -999;
  truthenergy = -999;
  truthpid = -999;
  numparticlesinevent = -999;
  process_id = -999;

  clustruthpx = -999;
  clustruthpy = -999;
  clustruthpz = -999;
  clustruthenergy = -999;
  clustruthpt = -999;
  clustruthphi = -999;
  clustrutheta = -999;
  clustruthpid = -999;

  beam_energy = 0;
  x1 = 0;
  x2 = 0;
  partid1 = 0;
  partid2 = 0;

  hardest_jetpt = 0;
  hardest_jetphi = 0;
  hardest_jeteta = 0;
  hardest_jetenergy = 0;
}
