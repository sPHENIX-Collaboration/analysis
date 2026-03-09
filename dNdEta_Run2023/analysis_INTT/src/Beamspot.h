#include "TObject.h"
#include "TH2F.h"
#include "TProfile.h"

struct Beamspot : TObject
{
  double x;
  double y;
  double z;
  double sigma_x;
  double sigma_y;
  double sigma_z;
  double xy_correlation;

  double covariance[21];
  double dzdx;
  double dzdy;
  double zwidth;

  int Ntracklets;
  TH2F* d0phi0dist;
  TH2F* d0phi0dist_bkgrm;
  TProfile* d0phi0dist_bkgrm_prof;
  TH1F* z0dist;
  TH2F* pcadist;

  Beamspot() : x(0.),y(0.),z(0.),sigma_x(0.),sigma_y(0.),sigma_z(0.),Ntracklets(0.)
  {
    for(int i=0;i<21;i++) covariance[i] = 1.;
    d0phi0dist = new TH2F("d0phi0dist","tracklet phi0 vs. d0;#phi [radian];DCA [cm]",200,-M_PI,M_PI,200,-1.,1.);
    z0dist = new TH1F("z0","z0",200,-40.,40.);
    pcadist = new TH2F("pcadist","PCA",200,-.5,.5,200,-.5,.5);
  }

  void identify()
  {
    std::cout << "Beamspot: " << std::endl;
    std::cout << "(x,y,z) = (" << x << " +- " << sigma_x << ", " << y << " +- " << sigma_y << ", " << z << " +- " << sigma_z << ")" << std::endl;
    std::cout << "Ntracklets: " << Ntracklets << std::endl;
  }
};
