#include <trackbase/ActsGeometry.h>

bool writer = true;

const int nlayers = 7;

namespace 
{
  template<class T> inline constexpr T square(const T& x) { return x*x;}
  template<class T> inline constexpr T get_r(const T&x ,const T&y)
  { return std::sqrt(square(x)+square(y)); }
  inline const double get_angle(const Acts::Vector3&x, const Acts::Vector3& y)
  { return std::acos(x.dot(y)/(x.norm() * y.norm())); }
}

void AnalyzeResiduals(std::string infile, const bool use_helical = false)
{
  TFile *file = TFile::Open(infile.c_str());
  if(!file)
    {
      std::cout << "wrong file name" << std::endl;
      return;
    }
  auto tree = (TTree*)file->Get("residualtree");
  
  float px, py, pz, pt, eta, phi, deltapt, quality, pcax, pcay, pcaz;
  int nhits, nmaps, nintt, ntpc, nmms, charge;
  
  std::vector<float>* cluslx=0;
  std::vector<float>* cluslz=0;
  std::vector<float>* cluselx=0;
  std::vector<float>* cluselz=0;
  std::vector<float>* clusgx=0;
  std::vector<float>* clusgy=0;
  std::vector<float>* clusgz=0;
  std::vector<int>* cluslayer=0;
  std::vector<int>* clussize=0;
  std::vector<uint32_t>* clushitsetkey = 0;

  std::vector<float>* idealsurfcenterx=0;
  std::vector<float>* idealsurfcentery=0;
  std::vector<float>* idealsurfcenterz=0;
  std::vector<float>* idealsurfnormx=0;
  std::vector<float>* idealsurfnormy=0;
  std::vector<float>* idealsurfnormz=0;
  std::vector<float>* missurfcenterx=0;
  std::vector<float>* missurfcentery=0;
  std::vector<float>* missurfcenterz=0;
  std::vector<float>* missurfnormx=0;
  std::vector<float>* missurfnormy=0;
  std::vector<float>* missurfnormz=0;
  std::vector<float>* clusgxideal=0;
  std::vector<float>* clusgyideal=0;
  std::vector<float>* clusgzideal=0;
  std::vector<float>* missurfalpha=0;
  std::vector<float>* missurfbeta=0;
  std::vector<float>* missurfgamma=0;
  std::vector<float>* idealsurfalpha=0;
  std::vector<float>* idealsurfbeta=0;
  std::vector<float>* idealsurfgamma=0;

  std::vector<float>* statelx=0;
  std::vector<float>* statelz=0;
  std::vector<float>* stateelx=0;
  std::vector<float>* stateelz=0;
  std::vector<float>* stategx=0;
  std::vector<float>* stategy=0;
  std::vector<float>* stategz=0;
  std::vector<float>* statepx=0;
  std::vector<float>* statepy=0;
  std::vector<float>* statepz=0;
  std::vector<float>* statepl=0;

  std::vector<float>* statelxglobderivdx=0;
  std::vector<float> *statelxglobderivdy=0;
  std::vector<float> *statelxglobderivdz=0;
  std::vector<float> *statelxglobderivdalpha=0;
  std::vector<float> *statelxglobderivdbeta=0;
  std::vector<float> *statelxglobderivdgamma=0;

  std::vector<float> *statelxlocderivd0=0;
  std::vector<float> *statelxlocderivz0=0;
  std::vector<float> *statelxlocderivphi=0;
  std::vector<float> *statelxlocderivtheta=0;
  std::vector<float> *statelxlocderivqop=0;
  
  std::vector<float> *statelzglobderivdx=0;
  std::vector<float> *statelzglobderivdy=0;
  std::vector<float> *statelzglobderivdz=0;
  std::vector<float> *statelzglobderivdalpha=0;
  std::vector<float> *statelzglobderivdbeta=0;
  std::vector<float> *statelzglobderivdgamma=0;

  std::vector<float> *statelzlocderivd0=0;
  std::vector<float> *statelzlocderivz0=0;
  std::vector<float> *statelzlocderivphi=0;
  std::vector<float> *statelzlocderivtheta=0;
  std::vector<float> *statelzlocderivqop=0;

  tree->SetBranchAddress("px",&px);
  tree->SetBranchAddress("py",&py);
  tree->SetBranchAddress("pz",&pz);
  tree->SetBranchAddress("pt",&pt);
  tree->SetBranchAddress("eta",&eta);
  tree->SetBranchAddress("phi",&phi);
  tree->SetBranchAddress("deltapt",&deltapt);
  tree->SetBranchAddress("quality",&quality);
  tree->SetBranchAddress("nhits",&nhits);
  tree->SetBranchAddress("nmaps",&nmaps);
  tree->SetBranchAddress("nintt",&nintt);
  tree->SetBranchAddress("ntpc",&ntpc);
  tree->SetBranchAddress("nmms",&nmms);
  tree->SetBranchAddress("quality",&quality);
  tree->SetBranchAddress("pcax", &pcax);
  tree->SetBranchAddress("pcay",&pcay);
  tree->SetBranchAddress("pcaz", &pcaz);

  tree->SetBranchAddress("clushitsetkey",&clushitsetkey);
  tree->SetBranchAddress("cluslx",&cluslx);
  tree->SetBranchAddress("cluslz",&cluslz);
  tree->SetBranchAddress("cluselx",&cluselx);
  tree->SetBranchAddress("cluselz",&cluselz);
  tree->SetBranchAddress("clusgx",&clusgx);
  tree->SetBranchAddress("clusgy",&clusgy);
  tree->SetBranchAddress("clusgz",&clusgz);
  tree->SetBranchAddress("cluslayer",&cluslayer);
  tree->SetBranchAddress("clussize",&clussize);
  tree->SetBranchAddress("idealsurfcenterx",&idealsurfcenterx);
  tree->SetBranchAddress("idealsurfcentery",&idealsurfcentery);
  tree->SetBranchAddress("idealsurfcenterz",&idealsurfcenterz);
  tree->SetBranchAddress("missurfcenterx",&missurfcenterx);
  tree->SetBranchAddress("missurfcentery",&missurfcentery);
  tree->SetBranchAddress("missurfcenterz",&missurfcenterz);
  tree->SetBranchAddress("idealsurfnormx",&idealsurfnormx);
  tree->SetBranchAddress("idealsurfnormy",&idealsurfnormy);
  tree->SetBranchAddress("idealsurfnormz",&idealsurfnormz);
  tree->SetBranchAddress("missurfnormx",&missurfnormx);
  tree->SetBranchAddress("missurfnormy",&missurfnormy);
  tree->SetBranchAddress("missurfnormz",&missurfnormz);
  tree->SetBranchAddress("clusgxideal",&clusgxideal);
  tree->SetBranchAddress("clusgyideal", &clusgyideal);
  tree->SetBranchAddress("clusgzideal",&clusgzideal);
  tree->SetBranchAddress("missurfalpha",&missurfalpha);
  tree->SetBranchAddress("missurfbeta",&missurfbeta);
  tree->SetBranchAddress("missurfgamma",&missurfgamma);
  tree->SetBranchAddress("idealsurfalpha",&idealsurfalpha);
  tree->SetBranchAddress("idealsurfbeta",&idealsurfbeta);
  tree->SetBranchAddress("idealsurfgamma",&idealsurfgamma);

  tree->SetBranchAddress("statelx",&statelx);
   tree->SetBranchAddress("statelz",&statelz);
  tree->SetBranchAddress("stateelx",&stateelx);
  tree->SetBranchAddress("stateelz",&stateelz);
  tree->SetBranchAddress("stategx",&stategx);
  tree->SetBranchAddress("stategy",&stategy);
  tree->SetBranchAddress("stategz",&stategz);
  tree->SetBranchAddress("statepx",&statepx);
  tree->SetBranchAddress("statepy",&statepy);
  tree->SetBranchAddress("statepz",&statepz);
  tree->SetBranchAddress("statepl",&statepl);

  tree->SetBranchAddress("statelxglobderivdx", &statelxglobderivdx);
  tree->SetBranchAddress("statelxglobderivdy", &statelxglobderivdy);
  tree->SetBranchAddress("statelxglobderivdz", &statelxglobderivdz); 
  tree->SetBranchAddress("statelxglobderivdalpha", &statelxglobderivdalpha);
  tree->SetBranchAddress("statelxglobderivdbeta", &statelxglobderivdbeta);
  tree->SetBranchAddress("statelxglobderivdgamma", &statelxglobderivdgamma);

  tree->SetBranchAddress("statelxlocderivd0",&statelxlocderivd0);
  tree->SetBranchAddress("statelxlocderivz0",&statelxlocderivz0);
  tree->SetBranchAddress("statelxlocderivphi",&statelxlocderivphi);
  tree->SetBranchAddress("statelxlocderivtheta",&statelxlocderivtheta);
  tree->SetBranchAddress("statelxlocderivqop",&statelxlocderivqop);

  tree->SetBranchAddress("statelzglobderivdx", &statelzglobderivdx);
  tree->SetBranchAddress("statelzglobderivdy", &statelzglobderivdy);
  tree->SetBranchAddress("statelzglobderivdz", &statelzglobderivdz); 
  tree->SetBranchAddress("statelzglobderivdalpha", &statelzglobderivdalpha);
  tree->SetBranchAddress("statelzglobderivdbeta", &statelzglobderivdbeta);
  tree->SetBranchAddress("statelzglobderivdgamma", &statelzglobderivdgamma);

  tree->SetBranchAddress("statelzlocderivd0",&statelzlocderivd0);
  tree->SetBranchAddress("statelzlocderivz0",&statelzlocderivz0);
  tree->SetBranchAddress("statelzlocderivphi",&statelzlocderivphi);
  tree->SetBranchAddress("statelzlocderivtheta",&statelzlocderivtheta);
  tree->SetBranchAddress("statelzlocderivqop",&statelzlocderivqop);
  
  TFile *outfile = new TFile("residualoutfile.root","recreate");
  
  TH2 *h_residuallayerx = new TH2F("residuallayerx",";x residual [mm];layer",1000,-1,1,60,0,60);
  TH2 *h_residuallayerz = new TH2F("residuallayerz",";z residual [mm];layer",1000,-1,1,60,0,60);
  TH2 *h_residualcluspulllayerx = new TH2F("residualcluspulllayerx",";x residual / clus error;layer",400,-10,10,60,0,60);
  TH2 *h_residualcluspulllayerz = new TH2F("residualcluspulllayerz",";z residual / clus error;layer",400,-10,10,60,0,60);
  TH2 *h_residualstatepulllayerx = new TH2F("residualstatepulllayerx",";x residual / state error;layer",400,-10,10,60,0,60);
  TH2 *h_residualstatepulllayerz = new TH2F("residualstatepulllayerz",";z residual / state error;layer",400,-10,10,60,0,60);

  // as a function of layer
  TH2 *h_residualxclusr[nlayers];
  TH2 *h_residualxclusphi[nlayers];
  TH2 *h_residualgxclusphi[nlayers];
  TH2 *h_residualgyclusphi[nlayers];
  TH2 *h_residualgzclusphi[nlayers];
  TH2 *h_residualxclusz[nlayers];
  TH2 *h_residualzclusr[nlayers];
  TH2 *h_residualzclusphi[nlayers];
  TH2 *h_residualzclusz[nlayers];
  TH2 *h_surfradiusdiffvsphi[nlayers];
  TH2 *h_surfxdiffvsphi[nlayers];
  TH2 *h_surfydiffvsphi[nlayers];
  TH2 *h_surfzdiffvsphi[nlayers];
  TH2 *h_surfphidiffvsphi[nlayers];
  TH2 *h_surfadiffvsphi[nlayers];
  TH2 *h_surfbdiffvsphi[nlayers];
  TH2 *h_surfgdiffvsphi[nlayers];
  TH2 *h_surfadiffvsz[nlayers];
  TH2 *h_surfbdiffvsz[nlayers];
  TH2 *h_surfgdiffvsz[nlayers];
  TH3 *h_residualxclusphiz[nlayers];
  TH3 *h_residualzclusphiz[nlayers];

  for(int i=0; i<nlayers; i++)
    {
      int zlow = -105;
      int zhigh = 105;
      int rlow = 30;
      int rhigh = 85;
      
      if(i<3){
	zlow = -10; zhigh = 10; rlow = 0; rhigh = 4;
      } else if (i < 7) {
	zlow = -30; zhigh = 30; rlow = 4; rhigh = 14;
      } 
      
      ostringstream name;
      name.str("");
      name << "surfphidiffvsphi_"<<i;
      h_surfphidiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Ideal - misaligned phi [deg]",120,-180,180,1080,-2,2);

      name.str("");
      name << "surfadiffvsphi_"<<i;
      h_surfadiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Ideal - misaligned angle to x axis [mrad]",120,-180,180,100,-10,10);
      
      name.str("");
      name << "surfbdiffvsphi_"<<i;
      h_surfbdiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Ideal - misaligned angle to y axis [mrad]",120,-180,180,100,-10,10);

name.str("");
      name << "surfgdiffvsphi_"<<i;
      h_surfgdiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Ideal - misaligned angle to z axis [mrad]",120,-180,180,100,-10,10);
    
      name.str("");
      name << "surfadiffvsz_"<<i;
      h_surfadiffvsz[i] = new TH2F(name.str().c_str(),";Ideal z_{surf} [mm]; Ideal - misaligned angle to x axis [mrad]",120,-180,180,100,-10,10);
      
      name.str("");
      name << "surfbdiffvsz_"<<i;
      h_surfbdiffvsz[i] = new TH2F(name.str().c_str(),";Ideal z_{surf} [mm]; Ideal - misaligned angle to y axis [mrad]",120,-180,180,100,-10,10);

      name.str("");
      name << "surfgdiffvsz_"<<i;
      h_surfgdiffvsz[i] = new TH2F(name.str().c_str(),";Ideal z_{surf} [deg]; Ideal - misaligned angle to z axis [mrad]",120,-180,180,100,-10,10);

      name.str("");
      name << "surfradiusdiffvsphi_"<<i;
      h_surfradiusdiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Ideal radius - misalign radius [mm]",120,-180,180,1000,-1,1);

      name.str("");
      name << "surfxdiffvsphi_"<<i;
      h_surfxdiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Misalign x - ideal x [mm]",120,-180,180,1000,-1,1);
      
      name.str("");
      name << "surfydiffvsphi_"<<i;
      h_surfydiffvsphi[i] = new TH2F(name.str().c_str(),";Ideal #phi_{surf} [deg]; Misalign y - ideal y [mm]", 120,-180,180,1000,-1,1);

      name.str("");
      name << "surfzdiffvsphi_"<<i;
      h_surfzdiffvsphi[i] = new TH2F(name.str().c_str(), ";Ideal #phi_{surf} [deg]; Misalign z - ideal z [mm]",120,-180,180,2500,-10,10);

      name.str("");
      name << "residualxclusphiz_"<<i;
      //h_residualxclusphiz[i] = new TH3F(name.str().c_str(),";#phi [deg]; z [mm]; x residual [mm]",180,-180,180,100,-200,200,1000,-1,1);
      name.str("");
      name << "residualzclusphiz_"<<i;
      //h_residualzclusphiz[i] = new TH3F(name.str().c_str(),";#phi [deg]; z[mm]; z residual [mm]",180,-180,180,100,-200,200,1000,-1,1);

      name.str("");
      name<<"residualgxclusphi_"<<i;
      h_residualgxclusphi[i] = new TH2F(name.str().c_str(),";#phi [deg]; x_{glob} residual [mm]", 180,-180,180,2000,-1,1);
      
      name.str("");
      name << "residualgyclusphi_"<<i;
      h_residualgyclusphi[i] = new TH2F(name.str().c_str(),";#phi [deg]; y_{glob} residual [mm]",180,-180,180,2000,-1,1);
      
      name.str("");
      name << "residualgzclusphi_"<<i;
      h_residualgzclusphi[i] = new TH2F(name.str().c_str(),";#phi [deg]; z_{glob} residual [mm]",180,-180,180,2000,-10,10);

      name.str("");
      name << "residualxclusr_"<<i;
      h_residualxclusr[i] = new TH2F(name.str().c_str(),";r [cm]; x residual [mm]",
				     170,rlow,rhigh,2000,-1,1);
      name.str("");
      name << "residualxclusphi_"<<i;
      h_residualxclusphi[i] = new TH2F(name.str().c_str(),";phi [deg]; x residual [mm]",
				       180,-180,180,2000,-1,1);
      name.str("");
      name << "residualxclusz_"<<i;
      h_residualxclusz[i] = new TH2F(name.str().c_str(),";z [cm]; x residual [mm]",
				    210,zlow,zhigh,2000,-1,1);
      name.str("");
      name<<"residualzclusr_"<<i;
      h_residualzclusr[i] = new TH2F(name.str().c_str(),";r [cm]; z residual [mm]",
				     170,rlow,rhigh,2000,-1,1);
      name.str("");
      name << "residualzclusphi_"<<i;
      h_residualzclusphi[i] = new TH2F(name.str().c_str(),";phi [deg]; z residual [mm]",
				       180,-180,180,2000,-1,1);
      name.str("");
      name << "residualzclusz_"<<i;
      h_residualzclusz[i] = new TH2F(name.str().c_str(),";z [cm]; z residual [mm]",
				     210,zlow,zhigh,2000,-1,1);
    }



  TH2F *h_residualhitsetkeyx = new TH2F("residualhitsetkeyx",";x residual [mm];hitsetkey",1000,-1,1,1200,0,1200);
  TH2F *h_residualhitsetkeyz = new TH2F("residualhitsetkeyz",";z residual [mm];hitsetkey",1000,-1,1,1200,0,1200);
  TH2F *h_residualcluspullhitsetkeyx = new TH2F("residualcluspullhitsetkeyx",";x residual / clus error;hitsetkey",400,-10,10,1200,0,1200);
  TH2F *h_residualcluspullhitsetkeyz = new TH2F("residualcluspullhitsetkeyz",";z residual / clus error;hitsetkey",400,-10,10,1200,0,1200);
  TH2F *h_residualstatepullhitsetkeyx = new TH2F("residualstatepullhitsetkeyx",";x residual / state error;hitsetkey",400,-10,10,1200,0,1200);
  TH2F *h_residualstatepullhitsetkeyz = new TH2F("residualstatepullhitsetkeyz",";z residual / state error;hitsetkey",400,-10,10,1200,0,1200);

  TH2F *h_xresidualgdx = new TH2F("xresidualgdx",";dx_{resid}/dx_{glob};x_{resid} [mm]",1000,-1.3,1.3,1000,-1,1);
  TH2F *h_xresidualgdy = new TH2F("xresidualgdy",";dx_{resid}/dy_{glob};x_{resid} [mm]",1000,-1.3,1.3,1000,-1,1);
  TH2F *h_xresidualgdz = new TH2F("xresidualgdz",";dx_{resid}/dz_{glob};x_{resid} [mm]",1000,-0.001,0.001,1000,-1,1);
  TH2F *h_xresidualdalpha = new TH2F("xresidualdalpha",";dx_{resid}/d#alpha;x_{resid} [mm]",1000,-600,600,1000,-1,1);
  TH2F *h_xresidualdbeta = new TH2F("xresidualdbeta",";dx_{resid}/d#beta;x_{resid} [mm]",1000,-600,600,1000,-1,1);
  TH2F *h_xresidualdgamma = new TH2F("xresidualdgamma",";dx_{resid}/d#gamma;x_{resid} [mm]",1000,-20,20,1000,-1,1);
  TH2F *h_xresidualdd0 = new TH2F("xresidualdd0",";dx_{resid}/dd_{0}; x_{resid} [mm]",1000,0.9,1.1,1000,-1,1);
  TH2F *h_xresidualdz0 = new TH2F("xresidualdz0",";dx_{resid}/dz_{0}; x_{resid} [mm]",1000,-0.002,0.002,1000,-1,1);
  TH2F *h_xresidualdphi = new TH2F("xresidualdphi",";dx_{resid}/d#phi; x_{resid} [mm]", 1000,-100,100,1000,-1,1);
  TH2F *h_xresidualdtheta = new TH2F("xresidualdtheta",";dx_{resid}/d#theta; x_{resid} [mm]",1000,-10,10,1000,-1,1);
  TH2F *h_xresidualdqop = new TH2F("xresidualdqop",";dx_{resid}/dqop; x_{resid} [mm]",1000,-20,10,1000,-1,1);
  

  TH2F *h_zresidualgdx = new TH2F("zresidualgdx",";dz_{resid}/dx_{glob};z_{resid} [mm]",1000,-1.3,1.3,1000,-1,1);
  TH2F *h_zresidualgdy = new TH2F("zresidualgdy",";dz_{resid}/dy_{glob};z_{resid} [mm]",1000,-1.3,1.3,1000,-1,1);
  TH2F *h_zresidualgdz = new TH2F("zresidualgdz",";dz_{resid}/dz_{glob};z_{resid} [mm]",1000,-1.3,1.3,1000,-1,1);
  TH2F *h_zresidualdalpha = new TH2F("zresidualdalpha",";dz_{resid}/d#alpha;z_{resid} [mm]",1000,-600,600,1000,-1,1);
  TH2F *h_zresidualdbeta = new TH2F("zresidualdbeta",";dz_{resid}/d#beta;z_{resid} [mm]",1000,-600,600,1000,-1,1);
  TH2F *h_zresidualdgamma = new TH2F("zresidualdgamma",";dz_{resid}/d#gamma;z_{resid} [mm]",1000,-20,20,1000,-1,1);
  TH2F *h_zresidualdd0 = new TH2F("zresidualdd0",";dz_{resid}/dd_{0}; z_{resid} [mm]",1000,-1,1,1000,-1,1);
  TH2F *h_zresidualdz0 = new TH2F("zresidualdz0",";dz_{resid}/dz_{0}; z_{resid} [mm]",1000,0.9,1.1,1000,-1,1);
  TH2F *h_zresidualdphi = new TH2F("zresidualdphi",";dz_{resid}/d#phi; z_{resid} [mm]", 1000,-100,100,1000,-1,1);
  TH2F *h_zresidualdtheta = new TH2F("zresidualdtheta",";dz_{resid}/d#theta; z_{resid} [mm]",5000,-600,10,1000,-1,1);
  TH2F *h_zresidualdqop = new TH2F("zresidualdqop",";dz_{resid}/dqop; z_{resid} [mm]",1000,-2,2,1000,-1,1);

  TH2F *h_dxdxvsphi = new TH2F("dxdxvsphi",";#phi [deg];dx_{resid}/dx_{glob}",120,-180,180,1000,-1.3,1.3);
  TH2F *h_dxdyvsphi = new TH2F("dxdyvsphi",";#phi [deg];dx_{resid}/dy_{glob}",120,-180,180,1000,-1.3,1.3);
  TH2F *h_dxdzvsphi = new TH2F("dxdzvsphi",";#phi [deg];dx_{resid}/dz_{glob}",120,-180,180,1000,-0.002,0.002);
  TH2F *h_dxdalphavsphi = new TH2F("dxdalphavsphi",";#phi [deg];dx_{resid}/d#alpha",120,-180,180,1000,-600,600);
  TH2F *h_dxdbetavsphi = new TH2F("dxdbetavsphi",";#phi [deg];dx_{resid}/d#beta",120,-180,180,1000,-600,600);
  TH2F *h_dxdgammavsphi = new TH2F("dxdgammavsphi",";#phi [deg];dx_{resid}/d#gamma",120,-180,180,1000,-20,20);
  TH2F *h_dxdd0vsphi = new TH2F("dxdd0vsphi",";#phi [deg];dx_{resid}/dd_{0}",120,-180,180,1000,0.9,1.1);
  TH2F *h_dxdz0vsphi = new TH2F("dxdz0vsphi",";#phi [deg];dx_{resid}/dz_{0}",120,-180,180,1000,-0.002,0.002);
  TH2F *h_dxdphivsphi = new TH2F("dxdphivsphi",";#phi [deg];dx_{resid}/d#phi",120,-180,180,1000,-100,100);
  TH2F *h_dxdthetavsphi = new TH2F("dxdthetavsphi",";#phi [deg];dx_{resid}/d#theta",120,-180,180,1000,-10,10);
  TH2F *h_dxdqopvsphi = new TH2F("dxdqopvsphi",";#phi [deg];dx_{resid}/dqop",120,-180,180,1000,-1,1);

  TH2F *h_dzdxvsphi = new TH2F("dzdxvsphi",";#phi [deg];dz_{resid}/dx_{glob}",120,-180,180,1000,-1.3,1.3);
  TH2F *h_dzdyvsphi = new TH2F("dzdyvsphi",";#phi [deg];dz_{resid}/dy_{glob}",120,-180,180,1000,-1.3,1.3);
  TH2F *h_dzdzvsphi = new TH2F("dzdzvsphi",";#phi [deg];dz_{resid}/dz_{glob}",120,-180,180,1000,-1.3,1.3);
  TH2F *h_dzdalphavsphi = new TH2F("dzdalphavsphi",";#phi [deg];dz_{resid}/d#alpha",120,-180,180,1000,-600,600);
  TH2F *h_dzdbetavsphi = new TH2F("dzdbetavsphi",";#phi [deg];dz_{resid}/d#beta",120,-180,180,1000,-600,600);
  TH2F *h_dzdgammavsphi = new TH2F("dzdgammavsphi",";#phi [deg];dz_{resid}/d#gamma",120,-180,180,1000,-20,20);
  TH2F *h_dzdd0vsphi = new TH2F("dzdd0vsphi",";#phi [deg];dz_{resid}/dd_{0}",120,-180,180,1000,-1,1);
  TH2F *h_dzdz0vsphi = new TH2F("dzdz0vsphi",";#phi [deg];dz_{resid}/dz_{0}",120,-180,180,1000,0.9,1.1);
  TH2F *h_dzdphivsphi = new TH2F("dzdphivsphi",";#phi [deg];dz_{resid}/d#phi",120,-180,180,1000,-100,100);
  TH2F *h_dzdthetavsphi = new TH2F("dzdthetavsphi",";#phi [deg];dz_{resid}/d#theta",120,-180,180,1000,-10,10);
  TH2F *h_dzdqopvsphi = new TH2F("dzdqopvsphi",";#phi [deg];dz_{resid}/dqop",120,-180,180,1000,-1,1);

 TH2F *h_dxdxvseta = new TH2F("dxdxvseta",";#eta ;dx_{resid}/dx_{glob}",100,-1,1,1000,-1.3,1.3);
  TH2F *h_dxdyvseta = new TH2F("dxdyvseta",";#eta ;dx_{resid}/dy_{glob}",100,-1,1,1000,-1.3,1.3);
  TH2F *h_dxdzvseta = new TH2F("dxdzvseta",";#eta ;dx_{resid}/dz_{glob}",100,-1,1,1000,-1.3,1.3);
  TH2F *h_dxdalphavseta = new TH2F("dxdalphavseta",";#eta ;dx_{resid}/d#alpha",100,-1,1,1000,-600,600);
  TH2F *h_dxdbetavseta = new TH2F("dxdbetavseta",";#eta ;dx_{resid}/d#beta",100,-1,1,1000,-600,600);
  TH2F *h_dxdgammavseta = new TH2F("dxdgammavseta",";#eta ;dx_{resid}/d#gamma",100,-1,1,1000,-20,20);
  TH2F *h_dxdd0vseta = new TH2F("dxdd0vseta",";#eta ;dx_{resid}/dd_{0}",100,-1,1,1000,0.9,1.1);
  TH2F *h_dxdz0vseta = new TH2F("dxdz0vseta",";#eta ;dx_{resid}/dz_{0}",100,-1,1,1000,-0.002,0.002);
  TH2F *h_dxdphivseta = new TH2F("dxdphivseta",";#eta ;dx_{resid}/d#phi",100,-1,1,1000,-100,100);
  TH2F *h_dxdthetavseta = new TH2F("dxdthetavseta",";#eta ;dx_{resid}/d#theta",100,-1,1,1000,-10,10);
  TH2F *h_dxdqopvseta = new TH2F("dxdqopvseta",";#eta ;dx_{resid}/dqop",100,-1,1,1000,-1,1);

  TH2F *h_dzdxvseta = new TH2F("dzdxvseta",";#eta ;dz_{resid}/dx_{glob}",100,-1,1,1000,-1.3,1.3);
  TH2F *h_dzdyvseta = new TH2F("dzdyvseta",";#eta ;dz_{resid}/dy_{glob}",100,-1,1,1000,-1.3,1.3);
  TH2F *h_dzdzvseta = new TH2F("dzdzvseta",";#eta ;dz_{resid}/dz_{glob}",100,-1,1,1000,-1.3,1.3);
  TH2F *h_dzdalphavseta = new TH2F("dzdalphavseta",";#eta ;dz_{resid}/d#alpha",100,-1,1,1000,-600,600);
  TH2F *h_dzdbetavseta = new TH2F("dzdbetavseta",";#eta ;dz_{resid}/d#beta",100,-1,1,1000,-600,600);
  TH2F *h_dzdgammavseta = new TH2F("dzdgammavseta",";#eta ;dz_{resid}/d#gamma",100,-1,1,1000,-20,20);
  TH2F *h_dzdd0vseta = new TH2F("dzdd0vseta",";#eta ;dz_{resid}/dd_{0}",100,-1,1,1000,-1,1);
  TH2F *h_dzdz0vseta = new TH2F("dzdz0vseta",";#eta ;dz_{resid}/dz_{0}",100,-1,1,1000,0.9,1.1);
  TH2F *h_dzdphivseta = new TH2F("dzdphivseta",";#eta ;dz_{resid}/d#phi",100,-1,1,1000,-100,100);
  TH2F *h_dzdthetavseta = new TH2F("dzdthetavseta",";#eta ;dz_{resid}/d#theta",100,-1,1,1000,-10,10);
  TH2F *h_dzdqopvseta = new TH2F("dzdqopvseta",";#eta ;dz_{resid}/dqop",100,-1,1,1000,-1,1);
  TH2F *h_sensorlayercounts = new TH2F("sensorlayercounts",";sensor number; layer",1200,0,1200,58,0,58);

  TH2F *h_lowcountsxy = new TH2F("lowcountsxy",";x_{clus} [cm];y_{clus} [cm];",
				 10000,-20,20,10000,-20,20);

  TH2F *h_lowcountsrz = new TH2F("lowcountsrz",";z_{clus} [cm];r_{clus} [cm];",
				 10000,-40,40,100,0,20);

  int badent = 0;
  int sensornum = 0;
  std::map<uint32_t, int> hitsetkey_sensornum_map, hitsetkey_numcounts_map;
  std::cout << "iterating " << tree->GetEntries() << std::endl;
  for(int i=0; i<tree->GetEntries(); i++)
    {
      if(i > 25000)
	break;
      tree->GetEntry(i);
      for(int j=0; j<cluslx->size(); j++)
	{
	  unsigned int layer = cluslayer->at(j);

	  if(layer > 6) continue;
	  float xresidual = cluslx->at(j) - statelx->at(j);
	  float zresidual = cluslz->at(j) - statelz->at(j);
	  //convert to mm
	  xresidual *= 10;
	  zresidual *= 10;
	  float celx = cluselx->at(j);
	  float celz = cluselz->at(j);
	  float cgz = clusgz->at(j);
	  celx *= 10;
	  celz *= 10;
	  cgz *= 10;
	  uint32_t hitsetkey = clushitsetkey->at(j);

	  auto iter = hitsetkey_sensornum_map.find(hitsetkey);
	  if(iter == hitsetkey_sensornum_map.end())
	    {
	      hitsetkey_sensornum_map.insert(std::make_pair(hitsetkey,sensornum));
	      hitsetkey_numcounts_map.insert(std::make_pair(hitsetkey, 1));
	      sensornum++;
	    } 
	  else
	    {
	      hitsetkey_numcounts_map.find(hitsetkey)->second++;
	    }
	  
	  int sensornum = hitsetkey_sensornum_map.find(hitsetkey)->second;
	  if(sensornum > 130)
	    {
	      h_lowcountsxy->Fill(clusgx->at(j), clusgy->at(j));
	      h_lowcountsrz->Fill(clusgz->at(j),sqrt(clusgx->at(j)*clusgx->at(j)+clusgy->at(j)*clusgy->at(j)));
	    }
	  h_sensorlayercounts->Fill(sensornum, layer);
	  TVector3 clusvec;
	  clusvec.SetXYZ(clusgx->at(j), clusgy->at(j), clusgz->at(j));
	  float clusphi = clusvec.Phi() * 180 / 3.14159;
	  float cluseta = clusvec.Eta();
	  float statelxglobderivdxv = statelxglobderivdx->at(j);
	  
	  float xresidualcluspull = xresidual / celx;
	  float zresidualcluspull = zresidual / celz;
	  float xresidualstatepull = xresidual / (10*stateelx->at(j));
	  float zresidualstatepull = zresidual / (10*stateelz->at(j));

	  float statelxglobdx = statelxglobderivdx->at(j);
	  float statelxglobdy = statelxglobderivdy->at(j);
	  float statelxglobdz = statelxglobderivdz->at(j);
	  float statelxglobdalpha = statelxglobderivdalpha->at(j);
	  float statelxglobdbeta = statelxglobderivdbeta->at(j);
	  float statelxglobdgamma = statelxglobderivdgamma->at(j);
	  float statelzglobdx = statelxglobderivdx->at(j);
	  float statelzglobdy = statelxglobderivdy->at(j);
	  float statelzglobdz = statelxglobderivdz->at(j);
	  float statelzglobdalpha = statelzglobderivdalpha->at(j);
	  float statelzglobdbeta = statelzglobderivdbeta->at(j);
	  float statelzglobdgamma = statelzglobderivdgamma->at(j);
	  
	  if(use_helical)
	    {
	      statelxglobdalpha *= 10;
	      statelxglobdbeta *= 10;
	      statelxglobdgamma *= 10;
	      statelzglobdalpha *= 10;
	      statelzglobdbeta *= 10;
	      statelzglobdgamma *= 10;
	    }
	  
	  Acts::Vector3 missurfnorm(missurfnormx->at(j),
				    missurfnormy->at(j),
				    missurfnormz->at(j));
        
	  Acts::Vector3 idealsurfnorm(idealsurfnormx->at(j),
				      idealsurfnormy->at(j),
				      idealsurfnormz->at(j));

	  float alpha = idealsurfalpha->at(j)-missurfalpha->at(j);
	  float beta = idealsurfbeta->at(j)-missurfbeta->at(j);
	  float gamma = idealsurfgamma->at(j)-missurfgamma->at(j);

	  /// convert to millirad
	  alpha *= 1000;
	  beta *=1000;
	  gamma *=1000;
	  float idealsurfphi = 180/M_PI*atan2(idealsurfcentery->at(j), 
					      idealsurfcenterx->at(j));
	  float missurfphi = 180/M_PI*atan2(missurfcentery->at(j),
					    missurfcenterx->at(j));
	  h_surfphidiffvsphi[layer]->Fill(idealsurfphi, missurfphi-idealsurfphi);
	  h_surfadiffvsphi[layer]->Fill(idealsurfphi, alpha);
	  h_surfbdiffvsphi[layer]->Fill(idealsurfphi, beta);
	  h_surfgdiffvsphi[layer]->Fill(idealsurfphi, gamma);
	  h_surfadiffvsz[layer]->Fill(idealsurfcenterz->at(j)*10, alpha);
	  h_surfbdiffvsz[layer]->Fill(idealsurfcenterz->at(j)*10, beta);
	  h_surfgdiffvsz[layer]->Fill(idealsurfcenterz->at(j)*10, gamma);

	  h_surfradiusdiffvsphi[layer]->Fill(idealsurfphi,
					     get_r(missurfcenterx->at(j),
						   missurfcentery->at(j))*10 -
					     get_r(idealsurfcenterx->at(j),
						   idealsurfcentery->at(j))*10);
	  h_surfxdiffvsphi[layer]->Fill(idealsurfphi,
					missurfcenterx->at(j)*10 - 
					idealsurfcenterx->at(j)*10);
	  h_surfydiffvsphi[layer]->Fill(idealsurfphi,
					missurfcentery->at(j)*10 - 
					idealsurfcentery->at(j)*10);
        
	  h_surfzdiffvsphi[layer]->Fill(idealsurfphi,
					missurfcenterz->at(j)*10 - 
					idealsurfcenterz->at(j)*10);
	  h_residualxclusr[layer]->Fill(clusvec.Perp(), xresidual);
	  h_residualgxclusphi[layer]->Fill(clusphi, (clusgx->at(j) - stategx->at(j))*10);
	  h_residualgyclusphi[layer]->Fill(clusphi, (clusgy->at(j) - stategy->at(j))*10);
	  //std::cout << clusgz->at(j) << ", " << stategz->at(j) << std::endl;
	  h_residualgzclusphi[layer]->Fill(clusphi, (clusgz->at(j) - stategz->at(j))*10);
        
	  h_residualxclusphi[layer]->Fill(clusphi, xresidual);
	  h_residualxclusz[layer]->Fill(cgz, xresidual);
	  h_residualzclusr[layer]->Fill(clusvec.Perp(), zresidual);
	  h_residualzclusphi[layer]->Fill(clusphi, zresidual);
	  h_residualzclusz[layer]->Fill(cgz, zresidual);
	  //h_residualxclusphiz[layer]->Fill(clusphi,cgz,xresidual);
	  //h_residualzclusphiz[layer]->Fill(clusphi,cgz,zresidual);

	  h_dxdxvsphi->Fill(clusphi, statelxglobdx);
	  h_dxdyvsphi->Fill(clusphi, statelxglobdy);
	  h_dxdzvsphi->Fill(clusphi, statelxglobdz);
	  h_dxdalphavsphi->Fill(clusphi, statelxglobdalpha);
	  h_dxdbetavsphi->Fill(clusphi, statelxglobdbeta);
	  h_dxdgammavsphi->Fill(clusphi, statelxglobdgamma);
	  h_dxdd0vsphi->Fill(clusphi,statelxlocderivd0->at(j));
	  h_dxdz0vsphi->Fill(clusphi,statelxlocderivz0->at(j));
	  h_dxdphivsphi->Fill(clusphi,statelxlocderivphi->at(j));
	  h_dxdthetavsphi->Fill(clusphi,statelxlocderivtheta->at(j));
	  h_dxdqopvsphi->Fill(clusphi,statelxlocderivqop->at(j));

	  h_dzdxvsphi->Fill(clusphi, statelzglobdx);
	  h_dzdyvsphi->Fill(clusphi, statelzglobdy);
	  h_dzdzvsphi->Fill(clusphi, statelzglobdz);
	  h_dzdalphavsphi->Fill(clusphi, statelzglobdalpha);
	  h_dzdbetavsphi->Fill(clusphi, statelzglobdbeta);
	  h_dzdgammavsphi->Fill(clusphi, statelzglobdgamma);
	  h_dzdd0vsphi->Fill(clusphi,statelzlocderivd0->at(j));
	  h_dzdz0vsphi->Fill(clusphi,statelzlocderivz0->at(j));
	  h_dzdphivsphi->Fill(clusphi,statelzlocderivphi->at(j));
	  h_dzdthetavsphi->Fill(clusphi,statelzlocderivtheta->at(j));
	  h_dzdqopvsphi->Fill(clusphi,statelzlocderivqop->at(j));

	  h_dxdxvseta->Fill(cluseta, statelxglobdx);
	  h_dxdyvseta->Fill(cluseta, statelxglobdy);
	  h_dxdzvseta->Fill(cluseta, statelxglobdz);
	  h_dxdalphavseta->Fill(cluseta, statelxglobdalpha);
	  h_dxdbetavseta->Fill(cluseta, statelxglobdbeta);
	  h_dxdgammavseta->Fill(cluseta, statelxglobdgamma);
	  h_dxdd0vseta->Fill(cluseta,statelxlocderivd0->at(j));
	  h_dxdz0vseta->Fill(cluseta,statelxlocderivz0->at(j));
	  h_dxdphivseta->Fill(cluseta,statelxlocderivphi->at(j));
	  h_dxdthetavseta->Fill(cluseta,statelxlocderivtheta->at(j));
	  h_dxdqopvseta->Fill(cluseta,statelxlocderivqop->at(j));

	  h_dzdxvseta->Fill(cluseta, statelzglobdx);
	  h_dzdyvseta->Fill(cluseta, statelzglobdy);
	  h_dzdzvseta->Fill(cluseta, statelzglobdz);
	  h_dzdalphavseta->Fill(cluseta, statelzglobdalpha);
	  h_dzdbetavseta->Fill(cluseta, statelzglobdbeta);
	  h_dzdgammavseta->Fill(cluseta, statelzglobdgamma);
	  h_dzdd0vseta->Fill(cluseta,statelzlocderivd0->at(j));
	  h_dzdz0vseta->Fill(cluseta,statelzlocderivz0->at(j));
	  h_dzdphivseta->Fill(cluseta,statelzlocderivphi->at(j));
	  h_dzdthetavseta->Fill(cluseta,statelzlocderivtheta->at(j));
	  h_dzdqopvseta->Fill(cluseta,statelzlocderivqop->at(j));

	  h_xresidualgdx->Fill(statelxglobdx,xresidual);
	  h_xresidualgdy->Fill(statelxglobdy,xresidual);
	  h_xresidualgdz->Fill(statelxglobdz,xresidual);
	  h_xresidualdalpha->Fill(statelxglobdalpha,xresidual);
	  h_xresidualdbeta->Fill(statelxglobdbeta,xresidual);
	  h_xresidualdgamma->Fill(statelxglobdgamma,xresidual);
	  h_xresidualdd0->Fill(statelxlocderivd0->at(j),xresidual);
	  h_xresidualdz0->Fill(statelxlocderivz0->at(j),xresidual);
	  h_xresidualdphi->Fill(statelxlocderivphi->at(j),xresidual);
	  h_xresidualdtheta->Fill(statelxlocderivtheta->at(j),xresidual);
	  h_xresidualdqop->Fill(statelxlocderivqop->at(j),xresidual);
	  
	  h_zresidualgdx->Fill(statelzglobdx,zresidual);
	  h_zresidualgdy->Fill(statelzglobdy,zresidual);
	  h_zresidualgdz->Fill(statelzglobdz,zresidual);
	  h_zresidualdalpha->Fill(statelzglobdalpha,zresidual);
	  h_zresidualdbeta->Fill(statelzglobdbeta,zresidual);
	  h_zresidualdgamma->Fill(statelzglobdgamma,zresidual);
	  h_zresidualdd0->Fill(statelzlocderivd0->at(j),zresidual);
	  h_zresidualdz0->Fill(statelzlocderivz0->at(j),zresidual);
	  h_zresidualdphi->Fill(statelzlocderivphi->at(j),zresidual);
	  h_zresidualdtheta->Fill(statelzlocderivtheta->at(j),zresidual);
	  h_zresidualdqop->Fill(statelzlocderivqop->at(j),zresidual);
	  
	  h_residuallayerx->Fill(xresidual, layer);
	  h_residuallayerz->Fill(zresidual, layer);
	  h_residualcluspulllayerx->Fill(xresidualcluspull, layer);
	  h_residualcluspulllayerz->Fill(zresidualcluspull, layer);
	  h_residualstatepulllayerx->Fill(xresidualstatepull, layer);
	  h_residualstatepulllayerz->Fill(zresidualstatepull, layer);
        
	  h_residualhitsetkeyx->Fill(xresidual, sensornum);
	  h_residualhitsetkeyz->Fill(zresidual, sensornum);
	  h_residualcluspullhitsetkeyx->Fill(xresidualcluspull, sensornum);
	  h_residualcluspullhitsetkeyz->Fill(zresidualcluspull, sensornum);
	  h_residualstatepullhitsetkeyx->Fill(xresidualstatepull, sensornum);
	  h_residualstatepullhitsetkeyz->Fill(zresidualstatepull, sensornum);
	  	  
	}
    }
  std::cout << "badent " << badent<<std::endl;
  for(const auto& [key, counts] : hitsetkey_numcounts_map)
    {
      std::cout << "hitsetkey " << key << " has " << counts << std::endl;
    }

  TCanvas *can1 = new TCanvas("can1","can1",300,300,1000,700);
  can1->Divide(3,2);
  can1->cd(1);
  h_xresidualdalpha->Draw();
  can1->cd(2);
  h_xresidualdbeta->Draw();
  can1->cd(3);
  h_xresidualdgamma->Draw();
  can1->cd(4);
  h_xresidualgdx->Draw();
  can1->cd(5);
  h_xresidualgdy->Draw();
  can1->cd(6);
  h_xresidualgdz->Draw();

  
  TCanvas *can2 = new TCanvas("can2","can2",300,300,1000,700);
  can2->Divide(3,2);
  can2->cd(1);
  h_xresidualdd0->Draw();
  can2->cd(2);
  h_xresidualdz0->Draw();
  can2->cd(3);
  h_xresidualdphi->Draw();
  can2->cd(4);
  h_xresidualdtheta->Draw();
  can2->cd(5);
  h_xresidualdqop->Draw();
  
  
  TCanvas *can3 = new TCanvas("can3","can3",300,300,1000,700);
  can3->Divide(3,2);
  can3->cd(1);
  h_zresidualdalpha->Draw();
  can3->cd(2);
  h_zresidualdbeta->Draw();
  can3->cd(3);
  h_zresidualdgamma->Draw();
  can3->cd(4);
  h_zresidualgdx->Draw();
  can3->cd(5);
  h_zresidualgdy->Draw();
  can3->cd(6);
  h_zresidualgdz->Draw();

  
  TCanvas *can4 = new TCanvas("can4","can4",300,300,1000,700);
  can4->Divide(3,2);
  can4->cd(1);
  h_zresidualdd0->Draw();
  can4->cd(2);
  h_zresidualdz0->Draw();
  can4->cd(3);
  h_zresidualdphi->Draw();
  can4->cd(4);
  h_zresidualdtheta->Draw();
  can4->cd(5);
  h_zresidualdqop->Draw();

  
  TCanvas *can5 = new TCanvas("can5","can5",300,300,1000,700);
  can5->Divide(3,2);
  can5->cd(1);

  h_dxdxvsphi->Draw();
  can5->cd(2);
  h_dxdyvsphi->Draw();
  can5->cd(3);
  h_dxdzvsphi->Draw();
  can5->cd(4);
  h_dxdalphavsphi->Draw();
  can5->cd(5);
  h_dxdbetavsphi->Draw();
  can5->cd(6);
  h_dxdgammavsphi->Draw();
  
  TCanvas *can6 = new TCanvas("can6","can6",300,300,1000,700);
  can6->Divide(3,2);
  can6->cd(1);

  h_dxdd0vsphi->Draw();
  can6->cd(2);
  h_dxdz0vsphi->Draw();
  can6->cd(3);
  h_dxdphivsphi->Draw();
  can6->cd(4);
  h_dxdthetavsphi->Draw();
  can6->cd(5);
  h_dxdqopvsphi->Draw();
  
  TCanvas *can7 = new TCanvas("can7","can7",300,300,1000,700);
  can7->Divide(3,2);
  can7->cd(1);

  h_dzdxvsphi->Draw();
  can7->cd(2);
  h_dzdyvsphi->Draw();
  can7->cd(3);
  h_dzdzvsphi->Draw();
  can7->cd(4);
  h_dzdalphavsphi->Draw();
  can7->cd(5);
  h_dzdbetavsphi->Draw();
  can7->cd(6);
  h_dzdgammavsphi->Draw();
  
  TCanvas *can8 = new TCanvas("can8","can8",300,300,1000,700);
  can8->Divide(3,2);
  can8->cd(1);

  h_dzdd0vsphi->Draw();
  can8->cd(2);
  h_dzdz0vsphi->Draw();
  can8->cd(3);
  h_dzdphivsphi->Draw();
  can8->cd(4);
  h_dzdthetavsphi->Draw();
  can8->cd(5);
  h_dzdqopvsphi->Draw();

  
  TCanvas *can9 = new TCanvas("can9","can9",300,300,1000,700);
  can9->Divide(3,2);
  can9->cd(1);

  h_dxdxvseta->Draw();
  can9->cd(2);
  h_dxdyvseta->Draw();
  can9->cd(3);
  h_dxdzvseta->Draw();
  can9->cd(4);
  h_dxdalphavseta->Draw();
  can9->cd(5);
  h_dxdbetavseta->Draw();
  can9->cd(6);
  h_dxdgammavseta->Draw();

  TCanvas *can10 = new TCanvas("can10","can10",300,300,1000,700);
  can10->Divide(3,2);
  can10->cd(1);

  h_dxdd0vseta->Draw();
  can10->cd(2);
  h_dxdz0vseta->Draw();
  can10->cd(3);
  h_dxdphivseta->Draw();
  can10->cd(4);
  h_dxdthetavseta->Draw();
  can10->cd(5);
  h_dxdqopvseta->Draw();

  TCanvas *can11 = new TCanvas("can11","can11",300,300,1000,700);
  can11->Divide(3,2);
  can11->cd(1);

  h_dzdxvseta->Draw();
  can11->cd(2);
  h_dzdyvseta->Draw();
  can11->cd(3);
  h_dzdzvseta->Draw();
  can11->cd(4);
  h_dzdalphavseta->Draw();
  can11->cd(5);
  h_dzdbetavseta->Draw();
  can11->cd(6);
  h_dzdgammavseta->Draw();

  TCanvas *can12 = new TCanvas("can12","can12",300,300,1000,700);
  can12->Divide(3,2);
  can12->cd(1);

  h_dzdd0vseta->Draw();
  can12->cd(2);
  h_dzdz0vseta->Draw();
  can12->cd(3);
  h_dzdphivseta->Draw();
  can12->cd(4);
  h_dzdthetavseta->Draw();
  can12->cd(5);
  h_dzdqopvseta->Draw();
  
  if(writer)
    {
  outfile->cd();
  h_lowcountsxy->Write();
  h_lowcountsrz->Write();
  h_residuallayerx->Write();
  h_residuallayerz->Write();
  h_residualcluspulllayerx->Write();
  h_residualcluspulllayerz->Write();
  h_residualstatepulllayerx->Write();
  h_residualstatepulllayerz->Write();
  h_sensorlayercounts->Write();

 h_residualhitsetkeyx->Write();
  h_residualhitsetkeyz->Write();
  h_residualcluspullhitsetkeyx->Write();
  h_residualcluspullhitsetkeyz->Write();
  h_residualstatepullhitsetkeyx->Write();
  h_residualstatepullhitsetkeyz->Write();
  
  h_xresidualgdx->Write();
  h_xresidualgdy->Write();
  h_xresidualgdz->Write();
  h_xresidualdalpha->Write();
  h_xresidualdbeta->Write();
  h_xresidualdgamma->Write();
  h_xresidualdd0->Write();
  h_xresidualdz0->Write();
  h_xresidualdphi->Write();
  h_xresidualdtheta->Write();
  h_xresidualdqop->Write();

  h_zresidualgdx->Write();
  h_zresidualgdy->Write();
  h_zresidualgdz->Write();
  h_zresidualdalpha->Write();
  h_zresidualdbeta->Write();
  h_zresidualdgamma->Write();
  h_zresidualdd0->Write();
  h_zresidualdz0->Write();
  h_zresidualdphi->Write();
  h_zresidualdtheta->Write();
  h_zresidualdqop->Write();

  h_dxdxvsphi->Write();
  h_dxdyvsphi->Write();
  h_dxdzvsphi->Write();
  h_dxdalphavsphi->Write();
  h_dxdbetavsphi->Write();
  h_dxdgammavsphi->Write();
  h_dxdd0vsphi->Write();
  h_dxdz0vsphi->Write();
  h_dxdphivsphi->Write();
  h_dxdthetavsphi->Write();
  h_dxdqopvsphi->Write();
  
  h_dzdxvsphi->Write();
  h_dzdyvsphi->Write();
  h_dzdzvsphi->Write();
  h_dzdalphavsphi->Write();
  h_dzdbetavsphi->Write();
  h_dzdgammavsphi->Write();
  h_dzdd0vsphi->Write();
  h_dzdz0vsphi->Write();
  h_dzdphivsphi->Write();
  h_dzdthetavsphi->Write();
  h_dzdqopvsphi->Write();
  
  h_dxdxvseta->Write();
  h_dxdyvseta->Write();
  h_dxdzvseta->Write();
  h_dxdalphavseta->Write();
  h_dxdbetavseta->Write();
  h_dxdgammavseta->Write();
  h_dxdd0vseta->Write();
  h_dxdz0vseta->Write();
  h_dxdphivseta->Write();
  h_dxdthetavseta->Write();
  h_dxdqopvseta->Write();
  
  h_dzdxvseta->Write();
  h_dzdyvseta->Write();
  h_dzdzvseta->Write();
  h_dzdalphavseta->Write();
  h_dzdbetavseta->Write();
  h_dzdgammavseta->Write();
  h_dzdd0vseta->Write();
  h_dzdz0vseta->Write();
  h_dzdphivseta->Write();
  h_dzdthetavseta->Write();
  h_dzdqopvseta->Write();
  

  for(int i=0; i<nlayers; i++)
    {
      h_residualxclusr[i]->Write();
      h_residualxclusz[i]->Write();
      h_residualgxclusphi[i]->Write();
      h_residualgyclusphi[i]->Write();
      h_residualgzclusphi[i]->Write();
      h_residualzclusr[i]->Write();
      h_residualzclusz[i]->Write();
      h_residualxclusphi[i]->Write();
      h_residualzclusphi[i]->Write();
      
      //h_residualxclusphiz[i]->Write();
      //h_residualzclusphiz[i]->Write();
      h_surfradiusdiffvsphi[i]->Write();
      h_surfxdiffvsphi[i]->Write();
      h_surfphidiffvsphi[i]->Write();
      h_surfydiffvsphi[i]->Write();
      h_surfzdiffvsphi[i]->Write();
      h_surfadiffvsphi[i]->Write();
      h_surfbdiffvsphi[i]->Write();
      h_surfgdiffvsphi[i]->Write();

      h_surfadiffvsz[i]->Write();
      h_surfbdiffvsz[i]->Write();
      h_surfgdiffvsz[i]->Write();
    }

  outfile->Write();
  outfile->Close();

  
    }

  

}
