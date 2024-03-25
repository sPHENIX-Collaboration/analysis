#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TF2.h"
#include "TGraphErrors.h"

#include <Eigen/Dense>

#include "Hit.h"
#include "Beamspot.h"

Beamspot fit_PCAD0Phi0(std::vector<std::pair<std::shared_ptr<Hit>,std::shared_ptr<Hit>>> tracklets, Beamspot BS_init, double d0_cut)
{
  Beamspot reco_BS;

  int Ntracklets = 0;

  std::cout << "processing " << tracklets.size() << " tracklets..." << std::endl;

  for(auto& tracklet : tracklets)
  {
    double x1 = tracklet.first->posX();
    double y1 = tracklet.first->posY();
    double z1 = tracklet.first->posZ();
    double x2 = tracklet.second->posX();
    double y2 = tracklet.second->posY();
    double z2 = tracklet.second->posZ();

    int size1 = tracklet.first->PhiSize();
    int size2 = tracklet.second->PhiSize();

    //std::cout << "tracklet: (" << x1 << ", " << y1 << ", " << z1 <<") -> (" << x2 << ", " << y2 << ", " << z2 << ")" << std::endl;

    // cluster position uncertainties
    double stripwidth = 0.0078; // cm
    double rphierr1 = stripwidth*size1;
    double rphierr2 = stripwidth*size2;
    double r1 = sqrt(x1*x1+y1*y1);
    double r2 = sqrt(x2*x2+y2*y2);
    double phi1 = atan2(y1,x1);
    double phi2 = atan2(y2,x2);
    double sigma2_x1 = pow(rphierr1*cos(phi1),2.);
    double sigma2_y1 = pow(rphierr1*sin(phi1),2.);
    double sigma2_x2 = pow(rphierr2*cos(phi2),2.);
    double sigma2_y2 = pow(rphierr2*sin(phi2),2.);

    // phi0 (angle of momentum vector at PCA) and uncertainty
    double px = x2-x1;
    double py = y2-y1;
    double sigma2_px = sigma2_x2+sigma2_x1;
    double sigma2_py = sigma2_y2+sigma2_y1;
    double phi0 = atan2(py,px);
    double sigma2_phi0 = (sigma2_px*py*py+sigma2_py*px*px)/pow(px*px+py*py,2.); // the aftermath of a bit of algebra
    //std::cout << "phi0: " << phi0 << " +- " << sqrt(sigma2_phi0) << std::endl;

    // PCA and uncertainty
    double pca_x = x1 + ((BS_init.x-x1)*cos(phi0)+(BS_init.y-y1)*sin(phi0))*cos(phi0);
    double pca_y = y1 + ((BS_init.x-x1)*cos(phi0)+(BS_init.y-y1)*sin(phi0))*sin(phi0);
    //std::cout << "PCA: (" << pca_x << " +- " << sqrt(sigma2_pca_x) << ", " << pca_y << " +- " << sqrt(sigma2_pca_y) << std::endl;

    double dca_origin = sqrt(pca_x*pca_x+pca_y*pca_y);
    double dzdr = (z2-z1)/(r2-r1);
    double z0 = z1-dzdr*(r1-dca_origin);

    double d0 = sqrt(pow(pca_x-BS_init.x,2.)+pow(pca_y-BS_init.y,2.));
    if(d0>d0_cut) continue;
    double phi_pca = atan2(pca_y-BS_init.y,pca_x-BS_init.x);
    double oppositephi_pca = phi_pca+M_PI;
    if(oppositephi_pca>M_PI) oppositephi_pca -= 2*M_PI;

    reco_BS.d0phi0dist->Fill(phi_pca,d0);
    reco_BS.d0phi0dist->Fill(oppositephi_pca,-d0);

    reco_BS.pcadist->Fill(pca_x,pca_y);
    reco_BS.z0dist->Fill(z0);

    Ntracklets++;
  }

  TF1 z_f("gaus","gaus",-60.,60.);

  //TF2 d0phi0_f("d0phi0_f","[0]+[1]*TMath::Gaus(y,[2]*TMath::Cos(x-[3]),[4])",-M_PI,M_PI,-1.,1.);

  TGraphErrors* g = new TGraphErrors();

  for(int i=0;i<reco_BS.d0phi0dist->GetNbinsY();i++)
  {
    TH1D* slice = reco_BS.d0phi0dist->ProjectionY("slice",i,i+1);
    int maxbin = slice->GetMaximumBin();
    double best_d0 = slice->GetBinCenter(maxbin);
    double low_d0 = slice->GetBinCenter(slice->FindFirstBinAbove(0.5*slice->GetMaximum(),1,maxbin-10,maxbin));
    double high_d0 = slice->GetBinCenter(slice->FindLastBinAbove(0.5*slice->GetMaximum(),1,maxbin,maxbin+10));
    double d0_error;
    if(best_d0-low_d0>high_d0-best_d0) d0_error = best_d0-low_d0;
    else d0_error = high_d0-best_d0;
    std::cout << "d0 error: " << d0_error << std::endl;
    double slice_phi = -M_PI+i*(2.*M_PI)/200.;
    g->AddPoint(slice_phi,best_d0);
    g->SetPointError(i,M_PI/100.,d0_error);
  }
  
  TF1 d0cos("d0cos","[0]*TMath::Cos(x-[1])",-M_PI,M_PI);
  g->Fit(&d0cos);
  g->Write("maxgraph");
/*
  d0phi0_f.SetParameter(0,(float)Ntracklets/(reco_BS.d0phi0dist->GetNbinsX()*reco_BS.d0phi0dist->GetNbinsY()));
  d0phi0_f.SetParLimits(0,0.,1e9);
  d0phi0_f.SetParameter(1,1.);
  d0phi0_f.SetParameter(2,1.);
  d0phi0_f.SetParameter(3,0.);
  d0phi0_f.SetParLimits(3,-M_PI,M_PI);
  d0phi0_f.SetParameter(4,.001);
  d0phi0_f.SetParLimits(4,0.,1.);
*/
  reco_BS.z0dist->Fit(&z_f);
  //reco_BS.d0phi0dist->Fit(&d0phi0_f,"LI");

  // build reconstructed beamspot: x and y first

  double BS_d0 = d0cos.GetParameter(0);
  double BS_phi0 = d0cos.GetParameter(1);
  double BS_sigmad0 = d0cos.GetParError(0);
  double BS_sigmaphi0 = d0cos.GetParError(1);

  reco_BS.x = BS_d0*cos(BS_phi0)+BS_init.x;
  reco_BS.y = BS_d0*sin(BS_phi0)+BS_init.y;
  reco_BS.sigma_x = sqrt(pow(cos(BS_phi0),2.)*pow(BS_sigmad0,2.)+pow(BS_d0*sin(BS_phi0),2.)*pow(BS_sigmaphi0,2.));
  reco_BS.sigma_y = sqrt(pow(sin(BS_phi0),2.)*pow(BS_sigmad0,2.)+pow(BS_d0*cos(BS_phi0),2.)*pow(BS_sigmaphi0,2.));
  
  reco_BS.Ntracklets = Ntracklets;

  reco_BS.z = z_f.GetParameter("Mean");
  reco_BS.sigma_z = z_f.GetParameter("Sigma");

  return reco_BS;
}

int main(int argc, char** argv)
{
  if(argc!=4)
  {
    std::cout << "Usage: ./BeamspotReco [infile] [outfile] [dphi_cut]" << std::endl;
    return 0;
  }

  std::string infile = argv[1];
  std::string outfile = argv[2];
  double dphi_cut = atof(argv[3]);

  TFile* inf = TFile::Open(infile.c_str());
  TTree* events = (TTree*)inf->Get("EventTree");

  TFile* outf = new TFile(outfile.c_str(),"RECREATE");
  TTree* t = new TTree("reco_beamspot","reco_beamspot");

  int Nclusters;
  std::vector<double>* clusters_x = 0;
  std::vector<double>* clusters_y = 0;
  std::vector<double>* clusters_z = 0;
  std::vector<int>* clusters_layer = 0;
  std::vector<int>* clusters_phisize = 0;

  events->SetBranchAddress("NClus",&Nclusters);
  events->SetBranchAddress("ClusX",&clusters_x);
  events->SetBranchAddress("ClusY",&clusters_y);
  events->SetBranchAddress("ClusZ",&clusters_z);
  events->SetBranchAddress("ClusLayer",&clusters_layer);
  events->SetBranchAddress("ClusPhiSize",&clusters_phisize);

  std::vector<std::pair<std::shared_ptr<Hit>,std::shared_ptr<Hit>>> tracklets;

  // assemble tracklets that pass dphi cut
  for(int i=0;i<events->GetEntries();i++)
  {
    std::vector<std::shared_ptr<Hit>> layer1_clusters;
    std::vector<std::shared_ptr<Hit>> layer2_clusters;

    events->GetEntry(i);

    std::cout << "event " << i << std::endl;

    for(int j=0;j<Nclusters;j++)
    {
      if(clusters_layer->at(j)==3 || clusters_layer->at(j)==4)
      {
        layer1_clusters.push_back(std::make_shared<Hit>(clusters_x->at(j),clusters_y->at(j),clusters_z->at(j),0.,0.,0.,0,clusters_phisize->at(j)));
      }
      if(clusters_layer->at(j)==5 || clusters_layer->at(j)==6)
      {
        layer2_clusters.push_back(std::make_shared<Hit>(clusters_x->at(j),clusters_y->at(j),clusters_z->at(j),0.,0.,0.,0,clusters_phisize->at(j)));
      }
    }
    for(auto& cluster1 : layer1_clusters)
    {
      for(auto& cluster2: layer2_clusters)
      {
        if(fabs(cluster1->Phi()-cluster2->Phi())<dphi_cut && fabs(cluster1->posZ()-cluster2->posZ())<5.)
        {
          tracklets.push_back(std::make_pair(cluster1,cluster2));
        }
      }
    }
  }

  Beamspot origin;
  Beamspot BS = fit_PCAD0Phi0(tracklets,origin,100.);
  //BS = fit_PCAD0Phi0(tracklets,BS,0.5);
  //BS = fit_PCAD0Phi0(tracklets,BS,0.2);
  //BS = fit_PCAD0Phi0(tracklets,BS,0.1);

  t->Branch("x",&BS.x);
  t->Branch("y",&BS.y);
  t->Branch("z",&BS.z);
  t->Branch("sigma_x",&BS.sigma_x);
  t->Branch("sigma_y",&BS.sigma_y);
  t->Branch("sigma_z",&BS.sigma_z);
  t->Branch("xy_correlation",&BS.xy_correlation);
  t->Branch("Ntracklets",&BS.Ntracklets);

  t->Fill();
  t->Write();

  BS.d0phi0dist->Write("d0phi0dist",TObject::kOverwrite);
  BS.z0dist->Write("z0dist",TObject::kOverwrite);
  BS.pcadist->Write("pcadist",TObject::kOverwrite);

  BS.identify();

  outf->Close();

  return 0;
}
