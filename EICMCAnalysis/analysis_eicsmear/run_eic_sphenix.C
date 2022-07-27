int
run_eic_sphenix()
{
  gSystem->Load("libeicsmear");

  gROOT->LoadMacro("eic_sphenix.C");

  SmearTree(BuildEicSphenix(), "data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.root");

  return 0;
}
