void analyze(int nevts=20,
	     char *prdfname="/gpfs/mnt/gpfs02/sphenix/data/data01/t1044-2016a/cosmics/cosmics_00001858-0000.prdf",
	     char *outfile = "signal.root")
{
  gSystem->Load("/gpfs/mnt/gpfs02/sphenix/sim/sim01/sPHENIX/sunywrk/abhisek/HCAL/build/lib/libhcal.so");
  //IHCAL high gain channels
  int hbd_channels[] = {64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94};

  //IHCAL low gain channels
  //int hbd_channels[] = {65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95};
  
  //OHCAL high gain channels
  //int hbd_channels[] = {112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142};

   //OHCAL low gain channels
  //int hbd_channels[] = {113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143};

  int nch = sizeof(hbd_channels)/sizeof(int);
  hcal *hcaldet = new hcal();
  hcaldet->setPrdf(prdfname);
  hcaldet->SetCableMap(nch,hbd_channels);
  hcaldet->set_verbosity( false );
  hcaldet->set_display( true );
  hcaldet->set_positive_polarity( false );
  hcaldet->evLoop(nevts);
  hcaldet->Save(outfile);
}

