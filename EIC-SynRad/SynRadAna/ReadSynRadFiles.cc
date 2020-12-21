#include "ReadSynRadFiles.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>  // for GenParticle
#include <HepMC/GenVertex.h>
#include <HepMC/PdfInfo.h>       // for PdfInfo
#include <HepMC/SimpleVector.h>  // for FourVector
#include <HepMC/Units.h>         // for GEV, MM

// eicsmear classes
#include <eicsmear/erhic/EventMC.h>
#include <eicsmear/erhic/ParticleMC.h>  // for ParticleMC
#include <eicsmear/erhic/Pid.h>         // for Pid

// General Root and C++ classes
#include <TBranch.h>  // for TBranch
#include <TChain.h>
#include <TVector3.h>  // for TVector3

#include <algorithm>  // copy
#include <cassert>
#include <iostream>  // for operator<<, endl, basic_ostream
#include <iterator>  // ostream_operator
#include <string>
#include <vector>  // for vector
#include <vector>

#include <boost/tokenizer.hpp>

class PHCompositeNode;
class PHHepMCGenEvent;

using namespace std;

///////////////////////////////////////////////////////////////////

ReadSynRadFiles::ReadSynRadFiles(const string &name)
  : SubsysReco(name)
  , filename("")
  , nEntries(1)
  , entry(1)
  , GenEvent(nullptr)
  , _node_name("PHHepMCGenEvent")
{
  hepmc_helper.set_embedding_id(1);  // default embedding ID to 1
  return;
}

///////////////////////////////////////////////////////////////////

ReadSynRadFiles::~ReadSynRadFiles()
{
}

///////////////////////////////////////////////////////////////////

bool ReadSynRadFiles::OpenInputFile(const string &name)
{
  filename = name;
  m_csv_input.open(filename);
  assert(m_csv_input.is_open());

  // skip header
  string line;
  std::getline(m_csv_input, line);

  return true;
}

int ReadSynRadFiles::Init(PHCompositeNode *topNode)
{
  /* Create node tree */
  CreateNodeTree(topNode);
  return 0;
}

int ReadSynRadFiles::process_event(PHCompositeNode *topNode)
{
  /* Create GenEvent */
  HepMC::GenEvent *evt = new HepMC::GenEvent();

  /* define the units (Pythia uses GeV and mm) */
  evt->use_units(HepMC::Units::GEV, HepMC::Units::CM);

  /* Loop over all particles for this event in input file and fill
   * vector with HepMC particles */
  vector<HepMC::GenParticle *> hepmc_particles;
  vector<unsigned> origin_index;
  //  /* add HepMC particles to Hep MC vertices; skip first two particles
  //   * in loop, assuming that they are the beam particles */
  vector<HepMC::GenVertex *> hepmc_vertices;

  typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

  vector<string> vec;
  string line;

  int SumPhoton(0);
  double SumFlux(0);

  if (Verbosity())
  {
    cout << "ReadSynRadFiles::process_event - reading " << nEntries << " lines from " << filename << endl;
  }
  for (int ii = 1; ii <= nEntries; ii++)
  {
    if (not std::getline(m_csv_input, line))
    {
      cout << "ReadSynRadFiles::process_event - "
           << "input file end reached" << endl;

      return Fun4AllReturnCodes::ABORTRUN;
    }

    Tokenizer tok(line);
    vec.assign(tok.begin(), tok.end());

    if (vec.size() != 14 and vec.size() != 15)
    {
      cout << "ReadSynRadFiles::process_event - "
           << "invalid input :" << line << endl;

      return Fun4AllReturnCodes::ABORTRUN;
    }

    //    Pos_X_[cm]  Pos_Y_[cm]  Pos_Z_[cm]  Pos_u Pos_v Dir_X Dir_Y Dir_Z Dir_theta_[rad] Dir_phi_[rad] LowFluxRatio  Energy_[eV] Flux_[photon/s] Power_[W]

    int id = 0;
    const double Pos_X_cm = stod(vec[id++]);
    const double Pos_Y_cm = stod(vec[id++]);
    const double Pos_Z_cm = stod(vec[id++]);
    const double Pos_u = stod(vec[id++]);
    const double Pos_v = stod(vec[id++]);
    const double Dir_X = stod(vec[id++]);
    const double Dir_Y = stod(vec[id++]);
    const double Dir_Z = stod(vec[id++]);
    const double Dir_theta_rad = stod(vec[id++]);
    const double Dir_phi_rad = stod(vec[id++]);
    const double LowFluxRatio = stod(vec[id++]);
    const double Energy_eV = stod(vec[id++]);
    const double Flux_photon_s = stod(vec[id++]);
    const double Power_W = stod(vec[id++]);
    assert(id == 14);

    if (Verbosity())
    {
      cout << "ReadSynRadFiles::process_event - " << line << " -> " << endl
           << Pos_X_cm << ", "
           << Pos_Y_cm << ", "
           << Pos_Z_cm << ", "
           << Pos_u << ", "
           << Pos_v << ", "
           << Dir_X << ", "
           << Dir_Y << ", "
           << Dir_Z << ", "
           << Dir_theta_rad << ", "
           << Dir_phi_rad << ", "
           << LowFluxRatio << ", "
           << Energy_eV << ", "
           << Flux_photon_s << ", "
           << Power_W << ". "
           << endl;
    }

    double xz_sign = +1;
    //assert(m_reverseXZ);
    if (m_reverseXZ)
    {
      xz_sign = -1;

      static bool once = true;

      if (once)
      {
        cout << "ReadSynRadFiles::process_event - reverse x z axis direction for input photons" << endl;
        once = false;
      }
    }

    const double E_GeV = Energy_eV / 1e9;
    //    const double E_GeV = Energy_eV / 1e3;
    const double px = E_GeV * Dir_X;
    const double py = E_GeV * Dir_Y;
    const double pz = E_GeV * Dir_Z;

    /* Create HepMC particle record */
    HepMC::GenParticle *hepmcpart = new HepMC::GenParticle(HepMC::FourVector(px * xz_sign, py, pz * xz_sign, E_GeV), 22);

    hepmcpart->set_status(1);

    /* add particle information */
    hepmcpart->setGeneratedMass(0);

    /* append particle to vector */
    hepmc_particles.push_back(hepmcpart);
    origin_index.push_back(ii);

    HepMC::GenVertex *hepmcvtx = new HepMC::GenVertex(HepMC::FourVector(Pos_X_cm * xz_sign,
                                                                        Pos_Y_cm,
                                                                        Pos_Z_cm * xz_sign,
                                                                        0));
    hepmc_vertices.push_back(hepmcvtx);
    hepmcvtx->add_particle_out(hepmcpart);

    ++SumPhoton;
    SumFlux += Flux_photon_s;
  }

  /* Check if hepmc_particles and origin_index vectors are the same size */
  if (hepmc_particles.size() != origin_index.size())
  {
    cout << "ReadSynRadFiles::process_event - Lengths of HepMC particles and Origin index vectors do not match!" << endl;

    delete evt;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  /* Add HepMC vertices to event */
  for (unsigned v = 0; v < hepmc_vertices.size(); v++)
    evt->add_vertex(hepmc_vertices.at(v));

  // save weights
  auto &weightcontainer = evt->weights();
  weightcontainer.push_back(SumFlux);
  weightcontainer.push_back(SumPhoton);

  /* pass HepMC to PHNode*/
  PHHepMCGenEvent *success = hepmc_helper.insert_event(evt);
  if (!success)
  {
    cout << "ReadSynRadFiles::process_event - Failed to add event to HepMC record!" << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  entry++;
  /* Done */
  return 0;
}

int ReadSynRadFiles::CreateNodeTree(PHCompositeNode *topNode)
{
  hepmc_helper.create_node_tree(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}
