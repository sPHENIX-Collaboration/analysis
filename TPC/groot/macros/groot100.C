void groot100(int JobNumber, int RunNumber, int SkipEvents, int PRUN_EVTS)
{
  cout << "Prepare to skip " << SkipEvents << " Events" << endl;
  gSystem->Load("libgroot");

  char filename[500];
  sprintf(filename, "data/beam_000000%d-0000.evt", RunNumber);
  cout << "Analyzing file named: " << filename << endl;
  poncsopen(filename);
  prun(2);

  Quiver::BlobTimeCut      = false;    // default is false
  Quiver::PadBlobs         = true;     // default is true
  ABlob::RecalibrateCharge = true;     // default is false

  Quiver::PaddingLimit = 4;     //default is 1
  ABlob::GaussPosition = true;  //default is false

  cout << "Prepare to skip " << SkipEvents << " Events" << endl;
  setFirstEventNr(SkipEvents);
  prun(PRUN_EVTS);
  //prun(SkipEvents+10);

  groot* Tree = groot::instance();

  char outfile[500];
  sprintf(outfile,"scratch/Analysis_%010d_%04d.root", RunNumber, JobNumber);
  cout << "Outputting Histograms to: " << outfile << endl;
  Tree->SaveTheHistograms(outfile);
}
