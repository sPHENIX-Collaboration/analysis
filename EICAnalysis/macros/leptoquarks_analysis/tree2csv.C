/**
 * Save ROOT::TTree as CSV file
 *
 * Written by nils.feege@stonybrook.edu
 */

int tree2csv( TString filelist, TString csv_out = TString("output_raw.txt") )
{
  /* Create a TChain from trees in multiple input files */
  TChain chain("candidates");

  /* open stream from input file list */
  ifstream in;
  in.open( filelist );

  /* add files from input list to TChain */
  TString fname;
  while (1) {
    in >> fname;
    if (!in.good()) break;
    cout << "Adding input file " << fname << endl;
    chain.Add(fname);
  }

  /* close stream from input file list */
  in.close();

  /* particle selection */
  TCut select_jetcandidates("abs(jet_eta)<1.0 && jet_ptrans>5 && ( (evtgen_is_tau == 0) || (evtgen_is_tau==1 && sqrt( (evtgen_tau_eta-jet_eta)*(evtgen_tau_eta-jet_eta) + (evtgen_tau_phi-jet_phi)*(evtgen_tau_phi-jet_phi) ) < 0.1) )");

  /* select columns to keep */
  TString cols_keep("*");

  /* Set up TTreePlayer to print output to ascii file */
  ((TTreePlayer*)(chain.GetPlayer()))->SetScanRedirect(true);
  ((TTreePlayer*)(chain.GetPlayer()))->SetScanFileName( csv_out );
  chain.SetScanField(0);

  /* Call the Scan command that writes the tree content.
   *
   * Options:
   *
   * colsize=X --> set column size (length of character). This should correspond to the longest branch name.
   */
  chain.Scan( cols_keep , select_jetcandidates , "colsize=30");

  return 0;
}
