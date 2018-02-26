int
compare_eictree_evtgenana()
{
  /* INPUT: EICTree created with eic-smear */
  TString fname_eictree("/gpfs/mnt/gpfs04/sphenix/user/nfeege/data/copy_eic_data_PYTHIA_ep/TREES/pythia.ep.10x250.1Mevents.RadCor=0.root");

  /* INPUT: tree generated in Fun4All with DISKinematicsReco (truth) */
  TString fname_evtgenana("/gpfs/mnt/gpfs04/sphenix/user/nfeege/sphenixsw/devel/analysis/EICAnalysis/data/EventGenAna_pythiaeRHIC_DIS_10x250_1M.root");

  /* get Fun4All tree from input file */
  TFile *file_evtgenana = new TFile (fname_evtgenana, "OPEN");
  TTree *tree_evtgenana = (TTree*)file_evtgenana->Get("event_truth");

  /* add eic-smear tree as friend */
  tree_evtgenana->AddFriend("EICTree", fname_eictree);

  /* plots correlations */
  TString drawmode("colz");
  //TString drawmode("");

  TCanvas *cy = new TCanvas();
  tree_evtgenana->Draw("evtgen_y:EICTree.trueY","",drawmode);

  TCanvas *cx = new TCanvas();
  tree_evtgenana->Draw("evtgen_x:EICTree.trueX","",drawmode);

  TCanvas *cq2 = new TCanvas();
  tree_evtgenana->Draw("evtgen_Q2:EICTree.trueQ2","",drawmode);

  TCanvas *cs = new TCanvas();
  tree_evtgenana->Draw("evtgen_s:EICTree.sHat","",drawmode);

  TCanvas *cw = new TCanvas();
  tree_evtgenana->Draw("evtgen_W*evtgen_W:EICTree.trueW2","",drawmode);

}
