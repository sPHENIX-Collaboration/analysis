//
// plot all the charge histograms from the laser output
//

int icv = 0;
TCanvas *ac[10];
TH1 *h_mbdq[128];

void plot_laser_q(const std::string rootfname = "LASER-00040000-0000.root")
{
  gStyle->SetOptStat(0);

  ac[icv] = new TCanvas("laser_q","Laser Charge",1600,800);
  //ac[icv]->Divide(16,8,-1,-1);
  ac[icv]->Divide(16,8,0.0001,0.0001);

  TFile *tfile = new TFile(rootfname.c_str(),"READ"); 
  
  TString name;
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h_mbdq"; name += ipmt;
    h_mbdq[ipmt] = (TH1*)tfile->Get( name );

    double mean = h_mbdq[ipmt]->GetMean();
    double rms = h_mbdq[ipmt]->GetRMS();

    h_mbdq[ipmt]->GetXaxis()->SetRangeUser(mean-5*rms,mean+5*rms);

    ac[icv]->cd(ipmt+1);
    h_mbdq[ipmt]->SetLineColor(2);
    h_mbdq[ipmt]->Draw();
  }

  ac[icv]->SaveAs(".png");
}

