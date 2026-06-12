void VertexResolution()
{
  TFile* KS0DataFile = new TFile("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/data/analysisMacros/KShort_fullDataset_finalCuts_0p2pTCut_rapidity1p0Cut_BCOcut_.root");
  TTree* KS0DataTree = (TTree*)KS0DataFile->Get("DecayTree");

  TFile* XiDataFile = new TFile("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/data/analysisMacros/Ximinus_fullDataset_finalCuts_0p2pTCut_rapidity1p0Cut_BCOcut_.root");
  TTree* XiDataTree = (TTree*)XiDataFile->Get("DecayTree");

  TH2F *h_PV_x_y_KS0 = new TH2F("h_PV_x_y_KS0",";Primary Vertex x [cm];Primary Vertex y [cm]",50,-0.2,0.2,50,-0.2,0.2);
  TH2F *h_PV_x_y_Xi = new TH2F("h_PV_x_y_Xi",";Primary Vertex x [cm];Primary Vertex y [cm]",50,-0.2,0.2,50,-0.2,0.2);
  TH2F *h_PV_z_r_KS0 = new TH2F("h_PV_z_r_KS0",";Primary Vertex z [cm];Primary Vertex r [cm]",50,-10,10,50,0,0.2);
  TH2F *h_PV_z_r_Xi = new TH2F("h_PV_z_r_Xi",";Primary Vertex z [cm];Primary Vertex r [cm]",50,-10,10,50,0,0.2);

  TH2F *h_PVcov_x_y_KS0 = new TH2F("h_PVcov_x_y_KS0",";Primary Vertex #sigma_{x};Primary Vertex #sigma_{y}",50,0,0.01,50,0,0.01);
  TH2F *h_PVcov_x_y_Xi = new TH2F("h_PVcov_x_y_Xi",";Primary Vertex #sigma_{x};Primary Vertex #sigma_{y}",50,0,0.01,50,0,0.01);
  TH2F *h_PVcov_x_z_KS0 = new TH2F("h_PVcov_x_z_KS0",";Primary Vertex #sigma_{x};Primary Vertex #sigma_{z}",50,0,0.01,50,0,0.01);
  TH2F *h_PVcov_x_z_Xi = new TH2F("h_PVcov_x_z_Xi",";Primary Vertex #sigma_{x};Primary Vertex #sigma_{z}",50,0,0.01,50,0,0.01);

  TH1F *h_PVvolume_r_KS0 = new TH1F("h_PVvolume_r_KS0",";Primary Vertex Volume Radius [cm];Entries",50,0,0.01);
  TH1F *h_PVvolume_r_Xi = new TH1F("h_PVvolume_r_Xi",";Primary Vertex Volume Radius [cm];Entries",50,0,0.01);
  TH1F *h_PVcov_r_KS0 = new TH1F("h_PVcov_r_KS0",";Primary Vertex #sigma_{R};Entries",50,0,0.01); 
  TH1F *h_PVcov_r_Xi = new TH1F("h_PVcov_r_Xi",";Primary Vertex #sigma_{R};Entries",50,0,0.01); 
  TH1F *h_PVcov_rxy_KS0 = new TH1F("h_PVcov_rxy_KS0",";Primary Vertex #sigma_{Rxy};Entries",50,0,0.01); 
  TH1F *h_PVcov_rxy_Xi = new TH1F("h_PVcov_rxy_Xi",";Primary Vertex #sigma_{Rxy};Entries",50,0,0.01); 

  TH2F *h_PVcov_r_nTracks_KS0 = new TH2F("h_PVcov_r_nTracks_KS0",";Number of Vertex Tracks;Primary Vertex #sigma_{R}",10,0,10,50,0,0.01);
 
  TH2F *h_SV_x_y_KS0 = new TH2F("h_SV_x_y_KS0",";Secondary Vertex x [cm];Secondary Vertex y [cm]",50,-5,5,50,-5,5);
  TH2F *h_SV_x_y_Xi = new TH2F("h_SV_x_y_Xi",";Secondary Vertex x [cm];Secondary Vertex y [cm]",50,-5,5,50,-5,5);
  TH2F *h_SV_z_r_KS0 = new TH2F("h_SV_z_r_KS0",";Secondary Vertex z [cm];Secondary Vertex r [cm]",50,-10,10,50,0,5);
  TH2F *h_SV_z_r_Xi = new TH2F("h_SV_z_r_Xi",";Secondary Vertex z [cm];Secondary Vertex r [cm]",50,-10,10,50,0,5);

  TH2F *h_SVcov_x_y_KS0 = new TH2F("h_SVcov_x_y_KS0",";Secondary Vertex #sigma_{x};Secondary Vertex #sigma_{y}",50,0,0.004,50,0,0.004);
  TH2F *h_SVcov_x_y_Xi = new TH2F("h_SVcov_x_y_Xi",";Secondary Vertex #sigma_{x};Secondary Vertex #sigma_{y}",50,0,0.004,50,0,0.004);
  TH2F *h_SVcov_x_z_KS0 = new TH2F("h_SVcov_x_z_KS0",";Secondary Vertex #sigma_{x};Secondary Vertex #sigma_{z}",50,0,0.004,50,0,0.004);
  TH2F *h_SVcov_x_z_Xi = new TH2F("h_SVcov_x_z_Xi",";Secondary Vertex #sigma_{x};Secondary Vertex #sigma_{z}",50,0,0.004,50,0,0.004);

  TH1F *h_SVvolume_r_KS0 = new TH1F("h_SVvolume_r_KS0",";Secondary Vertex Volume Radius [cm];Entries",50,0,0.01);
  TH1F *h_SVvolume_r_Xi = new TH1F("h_SVvolume_r_Xi",";Secondary Vertex Volume Radius [cm];Entries",50,0,0.01);
  TH1F *h_SVcov_r_KS0 = new TH1F("h_SVcov_r_KS0",";Secondary Vertex #sigma_{R};Entries",50,0,0.01); 
  TH1F *h_SVcov_r_Xi = new TH1F("h_SVcov_r_Xi",";Secondary Vertex #sigma_{R};Entries",50,0,0.01); 
  TH1F *h_SVcov_rxy_KS0 = new TH1F("h_SVcov_rxy_KS0",";Secondary Vertex #sigma_{Rxy};Entries",50,0,0.01); 
  TH1F *h_SVcov_rxy_Xi = new TH1F("h_SVcov_rxy_Xi",";Secondary Vertex #sigma_{Rxy};Entries",50,0,0.01); 
  
  TH2F *h_Lambda_x_y_Xi = new TH2F("h_Lambda_x_y_Xi",";#Lambda^{0} Vertex x [cm];#Lambda^{0} Vertex y [cm]",50,-5,5,50,-5,5);
  TH2F *h_Lambda_z_r_Xi = new TH2F("h_Lambda_z_r_Xi",";#Lambda^{0} Vertex z [cm];#Lambda^{0} Vertex r [cm]",50,-10,10,50,0,5);

  TH2F *h_Lambdacov_x_y_Xi = new TH2F("h_Lambdacov_x_y_Xi",";#Lambda^{0} Vertex #sigma_{x};#Lambda^{0} Vertex #sigma_{y}",50,0,0.004,50,0,0.004);
  TH2F *h_Lambdacov_x_z_Xi = new TH2F("h_Lambdacov_x_z_Xi",";#Lambda^{0} Vertex #sigma_{x};#Lambda^{0} Vertex #sigma_{z}",50,0,0.004,50,0,0.004);

  TH1F *h_Lambdavolume_r_Xi = new TH1F("h_Lambdavolume_r_Xi",";#Lambda^{0} Vertex Volume Radius [cm];Entries",50,0,0.01);
  TH1F *h_Lambdacov_r_Xi = new TH1F("h_Lambdacov_r_Xi",";#Lambda^{0} Vertex #sigma_{R};Entries",50,0,0.01); 
  TH1F *h_Lambdacov_rxy_Xi = new TH1F("h_Lambdacov_rxy_Xi",";#Lambda^{0} Vertex #sigma_{Rxy};Entries",50,0,0.01); 

  TH1F *h_pion_track1_r_KS0 = new TH1F("h_pion_track1_r_KS0",";#pi^{#pm} Track 1 #sigma_{R};Entries",50,0,0.01);
  TH1F *h_pion_track2_r_KS0 = new TH1F("h_pion_track2_r_KS0",";#pi^{#pm} Track 1 #sigma_{R};Entries",50,0,0.01);
  TH1F *h_pion_track1_rxy_KS0 = new TH1F("h_pion_track1_rxy_KS0",";#pi^{#pm} Track 1 #sigma_{Rxy};Entries",50,0,0.01);
  TH1F *h_pion_track2_rxy_KS0 = new TH1F("h_pion_track2_rxy_KS0",";#pi^{#pm} Track 1 #sigma_{Rxy};Entries",50,0,0.01);

  TH1F *h_pion_track_1_r_Xi = new TH1F("h_pion_track_1_r_Xi",";#pi^{#pm} Track 1 #sigma_{R};Entries",50,0,0.01);
  TH1F *h_proton_track_2_r_Xi = new TH1F("h_proton_track_2_r_Xi",";p^{#pm} Track 2 #sigma_{R};Entries",50,0,0.01);
  TH1F *h_pion_track_3_r_Xi = new TH1F("h_pion_track_3_r_Xi",";#pi^{#pm} Track 3 #sigma_{R};Entries",50,0,0.01);
  TH1F *h_pion_track_1_rxy_Xi = new TH1F("h_pion_track_1_rxy_Xi",";#pi^{#pm} Track 1 #sigma_{Rxy};Entries",50,0,0.01);
  TH1F *h_proton_track_2_rxy_Xi = new TH1F("h_proton_track_2_rxy_Xi",";p^{#pm} Track 2 #sigma_{Rxy};Entries",50,0,0.01);
  TH1F *h_pion_track_3_rxy_Xi = new TH1F("h_pion_track_3_rxy_Xi",";#pi^{#pm} Track 3 #sigma_{Rxy};Entries",50,0,0.01);

  std::string Xi_charge_cut = "Lambda0_track_1_charge == track_3_charge";
 
  /* 
  KS0DataTree->Draw("primary_vertex_y:primary_vertex_x>>h_PV_x_y_KS0");
  XiDataTree->Draw("primary_vertex_y:primary_vertex_x>>h_PV_x_y_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("sqrt(pow(primary_vertex_x,2) + pow(primary_vertex_y,2)):primary_vertex_z>>h_PV_z_r_KS0");
  XiDataTree->Draw("sqrt(pow(primary_vertex_x,2) + pow(primary_vertex_y,2)):primary_vertex_z>>h_PV_z_r_Xi",Xi_charge_cut.c_str());

  KS0DataTree->Draw("sqrt(primary_vertex_Covariance[2]):sqrt(primary_vertex_Covariance[0])>>h_PVcov_x_y_KS0");
  XiDataTree->Draw("sqrt(primary_vertex_Covariance[2]):sqrt(primary_vertex_Covariance[0])>>h_PVcov_x_y_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("sqrt(primary_vertex_Covariance[5]):sqrt(primary_vertex_Covariance[0])>>h_PVcov_x_z_KS0");
  XiDataTree->Draw("sqrt(primary_vertex_Covariance[5]):sqrt(primary_vertex_Covariance[0])>>h_PVcov_x_z_Xi",Xi_charge_cut.c_str());

  KS0DataTree->Draw("pow((3*primary_vertex_volume)/(4*pi),(1./3.))>>h_PVvolume_r_KS0");
  XiDataTree->Draw("pow((3*primary_vertex_volume)/(4*pi),(1./3.))>>h_PVvolume_r_Xi",Xi_charge_cut.c_str());
  */
  KS0DataTree->Draw("pow(primary_vertex_Covariance[0] + primary_vertex_Covariance[2] + primary_vertex_Covariance[5],1/2)>>h_PVcov_r_KS0");
  /*
  XiDataTree->Draw("pow(primary_vertex_Covariance[0] + primary_vertex_Covariance[2] + primary_vertex_Covariance[5],1/2)>>h_PVcov_r_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("pow(primary_vertex_Covariance[0] + primary_vertex_Covariance[2],1/2)>>h_PVcov_rxy_KS0");
  */
  XiDataTree->Draw("pow(primary_vertex_Covariance[0] + primary_vertex_Covariance[2],1/2)>>h_PVcov_rxy_Xi",Xi_charge_cut.c_str());
  /*
  KS0DataTree->Draw("pow(primary_vertex_Covariance[0] + primary_vertex_Covariance[2] + primary_vertex_Covariance[5],1/2):nTracksOfVertex>>h_PVcov_r_nTracks_KS0");

  KS0DataTree->Draw("K_S0_y:K_S0_x>>h_SV_x_y_KS0");
  XiDataTree->Draw("Ximinus_y:Ximinus_x>>h_SV_x_y_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("sqrt(pow(K_S0_x,2) + pow(K_S0_y,2)):K_S0_z>>h_SV_z_r_KS0");
  XiDataTree->Draw("sqrt(pow(Ximinus_x,2) + pow(Ximinus_y,2)):Ximinus_z>>h_SV_z_r_Xi",Xi_charge_cut.c_str());

  KS0DataTree->Draw("sqrt(K_S0_Covariance[2]):sqrt(K_S0_Covariance[0])>>h_SVcov_x_y_KS0");
  XiDataTree->Draw("sqrt(Ximinus_Covariance[2]):sqrt(Ximinus_Covariance[0])>>h_SVcov_x_y_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("sqrt(K_S0_Covariance[5]):sqrt(K_S0_Covariance[0])>>h_SVcov_x_z_KS0");
  XiDataTree->Draw("sqrt(Ximinus_Covariance[5]):sqrt(Ximinus_Covariance[0])>>h_SVcov_x_z_Xi",Xi_charge_cut.c_str());

  KS0DataTree->Draw("pow((3*K_S0_vertex_volume)/(4*pi),(1./3.))>>h_SVvolume_r_KS0");
  XiDataTree->Draw("pow((3*Ximinus_vertex_volume)/(4*pi),(1./3.))>>h_SVvolume_r_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("pow(K_S0_Covariance[0] + K_S0_Covariance[2] + K_S0_Covariance[5],1/2)>>h_SVcov_r_KS0");
  XiDataTree->Draw("pow(Ximinus_Covariance[0] + Ximinus_Covariance[2] + Ximinus_Covariance[5],1/2)>>h_SVcov_r_Xi",Xi_charge_cut.c_str());
  KS0DataTree->Draw("pow(K_S0_Covariance[0] + K_S0_Covariance[2],1/2)>>h_SVcov_rxy_KS0");
  XiDataTree->Draw("pow(Ximinus_Covariance[0] + Ximinus_Covariance[2],1/2)>>h_SVcov_rxy_Xi",Xi_charge_cut.c_str());
  
  XiDataTree->Draw("Lambda0_y:Lambda0_x>>h_Lambda_x_y_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(pow(Lambda0_x,2) + pow(Lambda0_y,2)):Lambda0_z>>h_Lambda_z_r_Xi",Xi_charge_cut.c_str());

  XiDataTree->Draw("sqrt(Lambda0_Covariance[2]):sqrt(Lambda0_Covariance[0])>>h_Lambdacov_x_y_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(Lambda0_Covariance[5]):sqrt(Lambda0_Covariance[0])>>h_Lambdacov_x_z_Xi",Xi_charge_cut.c_str());

  XiDataTree->Draw("pow((3*Lambda0_vertex_volume)/(4*pi),(1./3.))>>h_Lambdavolume_r_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("pow(Lambda0_Covariance[0]+Lambda0_Covariance[2]+Lambda0_Covariance[5],1/2)>>h_Lambdacov_r_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("pow(Lambda0_Covariance[0]+Lambda0_Covariance[2],1/2)>>h_Lambdacov_rxy_Xi",Xi_charge_cut.c_str());

  KS0DataTree->Draw("sqrt(track_1_Covariance[0]+track_1_Covariance[2]+track_1_Covariance[5])>>h_pion_track1_r_KS0");
  KS0DataTree->Draw("sqrt(track_2_Covariance[0]+track_2_Covariance[2]+track_2_Covariance[5])>>h_pion_track2_r_KS0");
  KS0DataTree->Draw("sqrt(track_1_Covariance[0]+track_1_Covariance[2])>>h_pion_track1_rxy_KS0");
  KS0DataTree->Draw("sqrt(track_2_Covariance[0]+track_2_Covariance[2])>>h_pion_track2_rxy_KS0");

  XiDataTree->Draw("sqrt(Lambda0_track_1_Covariance[0]+Lambda0_track_1_Covariance[2]+Lambda0_track_1_Covariance[5])>>h_pion_track_1_r_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(Lambda0_track_2_Covariance[0]+Lambda0_track_2_Covariance[2]+Lambda0_track_2_Covariance[5])>>h_proton_track_2_r_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(track_3_Covariance[0]+track_3_Covariance[2]+track_3_Covariance[5])>>h_pion_track_3_r_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(Lambda0_track_1_Covariance[0]+Lambda0_track_1_Covariance[2])>>h_pion_track_1_rxy_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(Lambda0_track_2_Covariance[0]+Lambda0_track_2_Covariance[2])>>h_proton_track_2_rxy_Xi",Xi_charge_cut.c_str());
  XiDataTree->Draw("sqrt(track_3_Covariance[0]+track_3_Covariance[2])>>h_pion_track_3_rxy_Xi",Xi_charge_cut.c_str());
  */
  
  TF1* fitFunc1 = new TF1("gaus1", "gaus", 0.002,0.0075);
  fitFunc1->SetLineColor(kRed);
  fitFunc1->SetParName(0, "Const"); fitFunc1->SetParameter(0, 5e5); fitFunc1->SetParLimits(0, 0, 1e6);
  fitFunc1->SetParName(1, "Mean"); fitFunc1->SetParameter(1, 0.0049); fitFunc1->SetParLimits(1, 0.001, 0.01);
  fitFunc1->SetParName(2, "Width"); fitFunc1->SetParameter(2, 0.001); fitFunc1->SetParLimits(2, 1e-4, 0.01);
  h_PVcov_r_KS0->Fit(fitFunc1, "ER");
  TF1* fitFunc2 = new TF1("gaus2", "gaus", 0.001,0.007);
  fitFunc2->SetLineColor(kRed);
  fitFunc2->SetParName(0, "Const"); fitFunc2->SetParameter(0, 5.5e3); fitFunc2->SetParLimits(0, 0, 1e4);
  fitFunc2->SetParName(1, "Mean"); fitFunc2->SetParameter(1, 0.0039); fitFunc2->SetParLimits(1, 0.001, 0.01);
  fitFunc2->SetParName(2, "Width"); fitFunc2->SetParameter(2, 0.001); fitFunc2->SetParLimits(2, 1e-4, 0.01);
  h_PVcov_rxy_Xi->Fit(fitFunc2, "ER");
  /*
  TF1* fitFunc3 = new TF1("gaus3", "gaus", 0.0,0.003);
  fitFunc3->SetLineColor(kRed);
  fitFunc3->SetParName(0, "Const"); fitFunc3->SetParameter(0, 9e5); fitFunc3->SetParLimits(0, 0, 5e6);
  fitFunc3->SetParName(1, "Mean"); fitFunc3->SetParameter(1, 0.001); fitFunc3->SetParLimits(1, 1e-4, 0.003);
  fitFunc3->SetParName(2, "Width"); fitFunc3->SetParameter(2, 0.001); fitFunc3->SetParLimits(2, 1e-4, 0.01);
  h_SVvolume_r_KS0->Fit(fitFunc3, "ER");
  TF1* fitFunc4 = new TF1("gaus4", "gaus", 0.0,0.005);
  fitFunc4->SetLineColor(kRed);
  fitFunc4->SetParName(0, "Const"); fitFunc4->SetParameter(0, 3.5e3); fitFunc4->SetParLimits(0, 0, 1e4);
  fitFunc4->SetParName(1, "Mean"); fitFunc4->SetParameter(1, 0.002); fitFunc4->SetParLimits(1, 0.001, 0.004);
  fitFunc4->SetParName(2, "Width"); fitFunc4->SetParameter(2, 0.001); fitFunc4->SetParLimits(2, 1e-4, 0.01);
  h_SVvolume_r_Xi->Fit(fitFunc4, "ER");
  TF1* fitFunc5 = new TF1("gaus5", "gaus", 0.0,0.005);
  fitFunc5->SetLineColor(kRed);
  fitFunc5->SetParName(0, "Const"); fitFunc5->SetParameter(0, 3.5e3); fitFunc5->SetParLimits(0, 0, 1e4);
  fitFunc5->SetParName(1, "Mean"); fitFunc5->SetParameter(1, 0.002); fitFunc5->SetParLimits(1, 0.001, 0.004);
  fitFunc5->SetParName(2, "Width"); fitFunc5->SetParameter(2, 0.001); fitFunc5->SetParLimits(2, 1e-4, 0.01);
  h_Lambdavolume_r_Xi->Fit(fitFunc5, "ER");

  double fitMean1 = fitFunc1->GetParameter(1);
  double fitWidth1 = fitFunc1->GetParameter(2);
  double fitMean2 = fitFunc2->GetParameter(1);
  double fitWidth2 = fitFunc2->GetParameter(2);
  double fitMean3 = fitFunc3->GetParameter(1);
  double fitWidth3 = fitFunc3->GetParameter(2);
  double fitMean4 = fitFunc4->GetParameter(1);
  double fitWidth4 = fitFunc4->GetParameter(2);
  double fitMean5 = fitFunc5->GetParameter(1);
  double fitWidth5 = fitFunc5->GetParameter(2);

  std::cout << "K_S0 PV mean and sigma : " << fitMean1 << "+/-" << fitWidth1 << std::endl;
  std::cout << "Xi PV mean and sigma : " << fitMean2 << "+/-" << fitWidth2 << std::endl;
  std::cout << "K_S0 SV mean and sigma : " << fitMean3 << "+/-" << fitWidth3 << std::endl;
  std::cout << "Xi SV mean and sigma : " << fitMean4 << "+/-" << fitWidth4 << std::endl;
  std::cout << "Xi Lambda mean and sigma : " << fitMean5 << "+/-" << fitWidth5 << std::endl;
  */

  TFile* fout = new TFile("VertexDistributionHistograms_temp.root","RECREATE");
  fout->cd();

  /*
  h_PV_x_y_KS0->Write();
  h_PV_x_y_Xi->Write();
  h_PV_z_r_KS0->Write();
  h_PV_z_r_Xi->Write();
  h_PVcov_x_y_KS0->Write();
  h_PVcov_x_y_Xi->Write();
  h_PVcov_x_z_KS0->Write();
  h_PVcov_x_z_Xi->Write();
  */
  h_PVcov_r_KS0->Write();
  /*
  h_PVcov_r_Xi->Write();
  h_PVcov_rxy_KS0->Write();
  */
  h_PVcov_rxy_Xi->Write();
  /*
  h_PVvolume_r_KS0->Write();
  h_PVvolume_r_Xi->Write();
  h_SV_x_y_KS0->Write();
  h_SV_x_y_Xi->Write();
  h_SV_z_r_KS0->Write();
  h_SV_z_r_Xi->Write();
  h_SVcov_x_y_KS0->Write();
  h_SVcov_x_y_Xi->Write();
  h_SVcov_x_z_KS0->Write();
  h_SVcov_x_z_Xi->Write();
  h_SVcov_r_KS0->Write();
  h_SVcov_r_Xi->Write();
  h_SVcov_rxy_KS0->Write();
  h_SVcov_rxy_Xi->Write();
  h_SVvolume_r_KS0->Write();
  h_SVvolume_r_Xi->Write();
  h_Lambda_x_y_Xi->Write();
  h_Lambda_z_r_Xi->Write();
  h_Lambdacov_x_y_Xi->Write();
  h_Lambdacov_x_z_Xi->Write();
  h_Lambdavolume_r_Xi->Write();
  h_Lambdacov_r_Xi->Write();
  h_Lambdacov_rxy_Xi->Write();
  h_PVcov_r_nTracks_KS0->Write();
  h_pion_track1_r_KS0->Write();
  h_pion_track2_r_KS0->Write();
  h_pion_track1_rxy_KS0->Write();
  h_pion_track2_rxy_KS0->Write();
  h_pion_track_1_r_Xi->Write();
  h_proton_track_2_r_Xi->Write();
  h_pion_track_3_r_Xi->Write();
  h_pion_track_1_rxy_Xi->Write();
  h_proton_track_2_rxy_Xi->Write();
  h_pion_track_3_rxy_Xi->Write();
  */
  fout->Close();
 
  std::cout << "Completed" << std::endl;
}
