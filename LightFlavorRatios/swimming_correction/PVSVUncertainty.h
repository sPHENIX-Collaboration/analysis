#ifndef PVSVUNCERTAINTY_H
#define PVSVUNCERTAINTY_H

struct PVSVUncertainty
{
  // uncertainty on reco calculation
  TH1F* PV_sigma;
  TH1F* PV_sigma_xy;
  TH1F* SV_sigma;
  TH1F* track_1_PV_DCA_sigma;
  TH1F* track_2_PV_DCA_sigma;
  TH1F* track_1_PV_DCA_xy_sigma;
  TH1F* track_2_PV_DCA_xy_sigma;

  PVSVUncertainty(const std::string& mother_name,const std::string& mother_title,int sigma_nBins,float sigma_min,float sigma_max)
  {
    PV_sigma = new TH1F((mother_name+"_PV_sigma").c_str(),(mother_title+" primary vertex reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
    PV_sigma_xy = new TH1F((mother_name+"_PV_sigma_xy").c_str(),(mother_title+" primary vertex xy reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
    SV_sigma = new TH1F((mother_name+"_SV_sigma").c_str(),(mother_title+" secondary vertex reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
    track_1_PV_DCA_sigma = new TH1F((mother_name+"_track_1_PV_DCA_sigma").c_str(),(mother_title+" track 1 PV DCA reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
    track_2_PV_DCA_sigma = new TH1F((mother_name+"_track_2_PV_DCA_sigma").c_str(),(mother_title+" track 2 PV DCA reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
    track_1_PV_DCA_xy_sigma = new TH1F((mother_name+"_track_1_PV_DCA_xy_sigma").c_str(),(mother_title+" track 1 PV DCA xy reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
    track_2_PV_DCA_xy_sigma = new TH1F((mother_name+"_track_2_PV_DCA_xy_sigma").c_str(),(mother_title+" track 2 PV DCA xy reco uncertainty").c_str(),sigma_nBins,sigma_min,sigma_max);
  }

  PVSVUncertainty(TFile* infile, const std::string& mother_name)
  {
    PV_sigma = (TH1F*)infile->Get((mother_name+"_PV_sigma").c_str());
    PV_sigma_xy = (TH1F*)infile->Get((mother_name+"_PV_sigma_xy").c_str());
    SV_sigma = (TH1F*)infile->Get((mother_name+"_SV_sigma").c_str());
    track_1_PV_DCA_sigma = (TH1F*)infile->Get((mother_name+"_track_1_PV_DCA_sigma").c_str());
    track_2_PV_DCA_sigma = (TH1F*)infile->Get((mother_name+"_track_2_PV_DCA_sigma").c_str());
    track_1_PV_DCA_xy_sigma = (TH1F*)infile->Get((mother_name+"_track_1_PV_DCA_xy_sigma").c_str());
    track_2_PV_DCA_xy_sigma = (TH1F*)infile->Get((mother_name+"_track_2_PV_DCA_xy_sigma").c_str());
  }

  ~PVSVUncertainty()
  {
    delete PV_sigma;
    delete PV_sigma_xy;
    delete SV_sigma;
    delete track_1_PV_DCA_sigma;
    delete track_2_PV_DCA_sigma;
    delete track_1_PV_DCA_xy_sigma;
    delete track_2_PV_DCA_xy_sigma;
  }

  PVSVUncertainty(const PVSVUncertainty& c)
  {
    PV_sigma = new TH1F(*c.PV_sigma);
    PV_sigma_xy = new TH1F(*c.PV_sigma_xy);
    SV_sigma = new TH1F(*c.SV_sigma);
    track_1_PV_DCA_sigma = new TH1F(*c.track_1_PV_DCA_sigma);
    track_2_PV_DCA_sigma = new TH1F(*c.track_2_PV_DCA_sigma);
    track_1_PV_DCA_xy_sigma = new TH1F(*c.track_1_PV_DCA_xy_sigma);
    track_2_PV_DCA_xy_sigma = new TH1F(*c.track_2_PV_DCA_xy_sigma);
  }

  void Write()
  {
    PV_sigma->Write();
    PV_sigma_xy->Write();
    SV_sigma->Write();
    track_1_PV_DCA_sigma->Write();
    track_2_PV_DCA_sigma->Write();
    track_1_PV_DCA_xy_sigma->Write();
    track_2_PV_DCA_xy_sigma->Write();
  }
};

#endif // PVSVUNCERTAINTY_H
