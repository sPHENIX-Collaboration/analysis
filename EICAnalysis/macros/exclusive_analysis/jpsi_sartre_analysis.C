#include <vector>

/* README */
/*

  Macro Purpose:
    To analyze vector meson production events from Sartre
  What this Macro currently does:
    Per event, this macro will search for best candidates for the decayed electron and positron pair. Every event analyzed will have particle theta, eta, phi, etc. filled into a vector under 'Public variables'. Per event we feed this macro, there are a variety of possibilities
    1) One track is found
    * If only one track is found, we test if that track is from a positron. If so, we fill positron event data and add a null entry to the two electron vectors
    2) Two tracks are found
    * If two tracks are found, we test if there are two electrons (using bending information). If two electrons are found, we add null entries for all three particle vectors. If two tracks are found, one positron one electron, we assume the electron is a decay product. We nullify the scattered electron entry and fill in the decay products
    3) Three tracks are found
    * If three tracks are found, we test both electron positron pair possibilities and take the pair which generates an invariant mass closest to that of the J/Psi
  
    



 */
/* ------------------------------------------------------------------------- */
/* Public variables */
TFile *f;
TTree *t;
TString file_name;

std::vector<std::vector<float> > jpsi_electron_vect;
std::vector<std::vector<float> > jpsi_positron_vect;
std::vector<std::vector<float> > scattered_electron_vect;
// Has 8 vectors inside
// 1. theta
// 2. eta
// 3. phi
// 4. energy
// 5. pt
// 6. ptotal
// 7. charge
// 8. found (0 if not found, 1 if found)

std::vector<float> theta;
std::vector<float> eta;
std::vector<float> phi;
std::vector<float> energy;
std::vector<float> pt;
std::vector<float> ptotal;
std::vector<float> charge;
std::vector<float> found;

std::vector<float> invariant_mass_track;

// Cuts //
const float energy_cut = 0.3;
const float ep_cut = 0.7;
/* ------------------------------------------------------------------------- */
/* Public methods */
void openFile(TString);
void closeFile();
float get_invariant_mass(float,float,float,float,float,float);
int get_em_showers();
void get_track_data(TString);
void voidify(char);
void fillify(char,int);
void pushify();

/* Public analysis methods */
std::vector<float> get_invariant_mass_vector();

/* -------------------------------------------------------------------------- */
int jpsi_sartre_analysis()
{
  pushify();
  get_track_data("JPsi_sartre_studies/sample/20x250_100Events.root");
  return 0;
}

void openFile(TString FILE_NAME)
{
   f = new TFile("/sphenix/user/gregtom3/data/Summer2018/" + FILE_NAME);
  t=(TTree*)f->Get("event_cluster");
}

void closeFile()
{
  delete t; delete f; f=NULL; t=NULL;
}

float get_invariant_mass(float pt1, float pt2, float phi1, float phi2, float eta1, float eta2)
{
  float Msquared = 2*pt1*pt2*(cosh(eta1-eta2)-cos(phi1-phi2));
  return (sqrt(Msquared));
}

void get_track_data(TString file)
{
  openFile(file);
  
  std::vector<float> return_theta;
  std::vector<float> return_eta;
  std::vector<float> return_phi;
  std::vector<float> return_energy;
  std::vector<float> return_pt;
  std::vector<float> return_ptotal;
  std::vector<float> return_charge;
  
  std::vector<float>* theta_pointer=&theta;
  std::vector<float>* eta_pointer=&eta;
  std::vector<float>* phi_pointer=&phi;
  std::vector<float>* energy_pointer=&energy;
  std::vector<float>* pt_pointer=&pt;
  std::vector<float>* ptotal_pointer=&ptotal;
  std::vector<float>* charge_pointer=&charge;

  t->SetBranchAddress("em_cluster_theta",&theta_pointer);
  t->SetBranchAddress("em_track_eta",&eta_pointer);
  t->SetBranchAddress("em_track_phi",&phi_pointer);
  t->SetBranchAddress("em_cluster_e",&energy_pointer);
  t->SetBranchAddress("em_track_ptrans",&pt_pointer);
  t->SetBranchAddress("em_track_ptotal",&ptotal_pointer);
  t->SetBranchAddress("em_track_charge",&charge_pointer);


  
  Int_t nentries = Int_t(t->GetEntries());
  for(Int_t entryInChain=0; entryInChain<nentries; entryInChain++)
    {
      Int_t entryInTree = t->LoadTree(entryInChain);
      if (entryInTree < 0) break;
      t->GetEntry(entryInChain); // Now lets look at a single event
     
      int number_em_showers=get_em_showers();
      switch(number_em_showers)
	{
	case 1: 
	  case_1_track();
	  break;
	case 2:
	  case_2_track();
	  break;
	case 3:
	  case_3_track();
	  break;
	}
    }
  closeFile();
}

int get_em_showers()
{
  int count=0;
  for(unsigned j = 0; j<theta.size(); j++)
    {
      if(ptotal.at(j)!=0)
	{
	  if(energy.at(j)>energy_cut && energy.at(j)/ptotal.at(j)>ep_cut)
	    count++;
	}
    }
  return count;
}

void case_1_track()
{
  /* Possibilities */
  // 1. Scattered electron
  // 2. JPsi Electron
  // 3. JPsi Positron
  // 4. Scattered proton

  for(unsigned j = 0; j<theta.size(); j++)
    {
      if(ptotal.at(j)!=0)
	{
	  if(energy.at(j)>energy_cut && energy.at(j)/ptotal.at(j)>ep_cut && charge.at(j)==1)
	    {
	      // Positron found (Case 3)
	      fillify('P',j);
	      voidify('S');
	      voidify('E');
	      return;
	    }
	}
    }
  // Positron not found (Case 1,2,4)
  voidify('P');
  voidify('E');
  voidify('S');
}
void case_2_track()
{
  
  /* Possibilites */
  // 1. Scattered Electron-JPsi Electron
  // 2. Scattered Electron-JPsi Positron
  // 3. JPsi Electron - JPsi Positron
  // First, lets see if we can find the positron
  bool positron_found = false;
  for(unsigned j = 0; j<theta.size(); j++)
    {
      if(ptotal.at(j)!=0)
	{
	  if(energy.at(j)>energy_cut && energy.at(j)/ptotal.at(j)>ep_cut && charge.at(j)==1)
	    positron_found = true;
	}
    }
  
  if(!positron_found)
    {
      // Case 1 (Scattered Electron - JPsi Electron)
      voidify('P');
      voidify('E');
      voidify('S');
    }

  // Positron found, assume case 3 for now
  else
    {
      for(unsigned j = 0; j<theta.size(); j++)
	{
	  if(ptotal.at(j)!=0)
	    {
	      if(energy.at(j)>energy_cut && energy.at(j)/ptotal.at(j)>ep_cut)
		{
		  if(charge.at(j)==-1)
		    {
		      fillify('E',j);
		    }
		  else if(charge.at(j)==1)
		    {
		      fillify('P',j);
		    }
		}
	    }
	}
      voidify('S');
    }
}  
void case_3_track()
{
  /* Possibilites */
  // 1. Scattered Electron - JPsi Electron - JPsi Positron
  unsigned index_positron;
  unsigned index_electron1=999;
  unsigned index_electron2;
  // Fill the vectors with the three decay candidates
  for(unsigned i = 0; i<theta.size(); i++)
    {
      if(energy.at(i)>energy_cut && energy.at(i)/ptotal.at(i)>ep_cut)
	{
	  if(charge.at(i)==1)
	    index_positron=i;
	  else if(charge.at(i)==-1&&index_electron1==999)
	    index_electron1=i;
	  else if(charge.at(i)==-1)
	    index_electron2=i;
	}
    }
  
  //index_positron is the vector index of positron data
  //index_electron1 is the vector index of electron 1 data (could be scattered)
  //index_electron2 is the vector index of electron 2 data (could be scattered)
  
  // Test which pair gives better J/Psi reconstruction
  double JPsi_rest_mass=3.096916;
  
  float rest_mass_1 = get_invariant_mass(pt.at(index_positron),pt.at(index_electron1),phi.at(index_positron),phi.at(index_electron1),eta.at(index_positron),eta.at(index_electron1));
  
  float rest_mass_2 = get_invariant_mass(pt.at(index_positron),pt.at(index_electron2),phi.at(index_positron),phi.at(index_electron2),eta.at(index_positron),eta.at(index_electron2));

  unsigned scattered_index;
  unsigned decay_index;

  if(abs(rest_mass_1-JPsi_rest_mass)<abs(rest_mass_2-JPsi_rest_mass))
    {
      scattered_index=index_electron2;
      decay_index=index_electron1;
    }
  else
    {
      scattered_index=index_electron1;
      decay_index=index_electron2;
    }
  
  fillify('S',scattered_index);
  fillify('E',decay_index);
  fillify('P',index_positron);
}

void fillify(char c, unsigned idx)
{
  switch(c)
    {
    case 'S':
      scattered_electron_vect[0].push_back(theta.at(idx));
      scattered_electron_vect[1].push_back(eta.at(idx));
      scattered_electron_vect[2].push_back(phi.at(idx));
      scattered_electron_vect[3].push_back(energy.at(idx));
      scattered_electron_vect[4].push_back(pt.at(idx));
      scattered_electron_vect[5].push_back(ptotal.at(idx));
      scattered_electron_vect[6].push_back(charge.at(idx));
      scattered_electron_vect[7].push_back(1);
      break;
    case 'E':
      jpsi_electron_vect[0].push_back(theta.at(idx));
      jpsi_electron_vect[1].push_back(eta.at(idx));
      jpsi_electron_vect[2].push_back(phi.at(idx));
      jpsi_electron_vect[3].push_back(energy.at(idx));
      jpsi_electron_vect[4].push_back(pt.at(idx));
      jpsi_electron_vect[5].push_back(ptotal.at(idx));
      jpsi_electron_vect[6].push_back(charge.at(idx));
      jpsi_electron_vect[7].push_back(1);
      break;
    case 'P':
      jpsi_positron_vect[0].push_back(theta.at(idx));
      jpsi_positron_vect[1].push_back(eta.at(idx));
      jpsi_positron_vect[2].push_back(phi.at(idx));
      jpsi_positron_vect[3].push_back(energy.at(idx));
      jpsi_positron_vect[4].push_back(pt.at(idx));
      jpsi_positron_vect[5].push_back(ptotal.at(idx));
      jpsi_positron_vect[6].push_back(charge.at(idx));
      jpsi_positron_vect[7].push_back(1);
      break;
    }
}

void voidify(char c)
{
  switch(c)
    {
    case 'S':
      scattered_electron_vect[0].push_back(NULL);
      scattered_electron_vect[1].push_back(NULL);
      scattered_electron_vect[2].push_back(NULL);
      scattered_electron_vect[3].push_back(NULL);
      scattered_electron_vect[4].push_back(NULL);
      scattered_electron_vect[5].push_back(NULL);
      scattered_electron_vect[6].push_back(NULL);
      scattered_electron_vect[7].push_back(0);
      break;
    case 'E':
      jpsi_electron_vect[0].push_back(NULL);
      jpsi_electron_vect[1].push_back(NULL);
      jpsi_electron_vect[2].push_back(NULL);
      jpsi_electron_vect[3].push_back(NULL);
      jpsi_electron_vect[4].push_back(NULL);
      jpsi_electron_vect[5].push_back(NULL);
      jpsi_electron_vect[6].push_back(NULL);
      jpsi_electron_vect[7].push_back(0);
      break;
    case 'P':
      jpsi_positron_vect[0].push_back(NULL);
      jpsi_positron_vect[1].push_back(NULL);
      jpsi_positron_vect[2].push_back(NULL);
      jpsi_positron_vect[3].push_back(NULL);
      jpsi_positron_vect[4].push_back(NULL);
      jpsi_positron_vect[5].push_back(NULL);
      jpsi_positron_vect[6].push_back(NULL);
      jpsi_positron_vect[7].push_back(0);
      break;
    }
}

void pushify()
{
  for(int k = 0; k < 8; k++)
    {
      jpsi_electron_vect.push_back(std::vector<float>());
      jpsi_positron_vect.push_back(std::vector<float>());
      scattered_electron_vect.push_back(std::vector<float>());
    }
}

std::vector<float> get_invariant_mass_vector()
{
  for(unsigned index = 0 ; index<jpsi_electron_vect.at(0).size(); index++)
    {
      if(jpsi_electron_vect.at(7).at(index)==1 && jpsi_positron_vect.at(7).at(index)==1)
	{
	  float theta_1=jpsi_electron_vect.at(0).at(index);
	  float eta_1=jpsi_electron_vect.at(1).at(index);
	  float phi_1=jpsi_electron_vect.at(2).at(index);
	  float energy_1=jpsi_electron_vect.at(3).at(index);
	  float pt_1=jpsi_electron_vect.at(4).at(index);
	  float ptotal_1=jpsi_electron_vect.at(5).at(index);
	  float charge_1=jpsi_electron_vect.at(6).at(index);

	  float theta_2=jpsi_positron_vect.at(0).at(index);
	  float eta_2=jpsi_positron_vect.at(1).at(index);
	  float phi_2=jpsi_positron_vect.at(2).at(index);
	  float energy_2=jpsi_positron_vect.at(3).at(index);
	  float pt_2=jpsi_positron_vect.at(4).at(index);
	  float ptotal_2=jpsi_positron_vect.at(5).at(index);
	  float charge_2=jpsi_positron_vect.at(6).at(index);

	  invariant_mass_track.push_back(get_invariant_mass(pt_1,pt_2,phi_1,phi_2,eta_1,eta_2));
	  
	  
	}
    }
}
