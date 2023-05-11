int
mergefiles()
{
  TChain chain("pid_rich_small");

  TString base("eval_RICH_PID_p");
  for ( float p = 2; p < 71; p+=2 )
    {
      TString file = base;
      file += p;
      file.Append(".root");

      cout << "Adding file: " << file << endl;
      chain.Add( file );
    }

  TFile *output = new TFile("output.root","RECREATE");
  chain.CloneTree(-1,"fast");
  output->Write(); 
  delete output;

  chain.Draw("theta_mean:ptot","ptot>1","profs");

  return 1;
}
