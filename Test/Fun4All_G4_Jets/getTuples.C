void getTuples() {
	/// open the output file.
	
	TFile f("g4jeteval.root");
	
	/// if you want to look at the contents
	/// of the file, do...
	/// 
	/// root[] f.ls();
	/// TFile**		g4jeteval.root	
	///  TFile*		g4jeteval.root	
	///   KEY: TNtuple	ntp_recojet;1	reco jet => max truth jet
	///   KEY: TNtuple	ntp_truthjet;1	truth jet => best reco jet
	/// 
	/// ...so, g4jeteval.root contains a TNtuple
	/// named ntp_recojet and a TNtuple named ntp_truthjet.
	
	/// retrieve pointers to the file contents.
	
	TNtuple *reco = f.Get("ntp_recojet;1");
	TNtuple *truth = f.Get("ntp_truthjet;1");
	
	/// you can check the contents of a TNtuple via...
	///
	/// root[] reco->Show();
	/// ======> EVENT:-1
	///  event           = 0
	///  id              = 0
	///  ncomp           = 0
	///  eta             = 0
	///  phi             = 0
	///  e               = 0
	///  pt              = 0
	///  gid             = 0
	///  gncomp          = 0
	///  geta            = 0
	///  gphi            = 0
	///  ge              = 0
	///  gpt             = 0
	///  efromtruth      = 0
	/// 
	/// ...in this case, the TNtuple contains info on two jets:
	/// with 'g' prefix (for GEANT?) is a truth jet,
	/// without 'g' prefix is a reconstructed jet.
	/// same thing with the TNtuple for truth. i am not sure
	/// exactly what the double info means...
	
	/// anyway, you can histogram the leaves or whatever.
	/// these are your TNtuples; you can do what you want.
	
	reco->Draw("pt");
	
	/// if the TNtuple's high-level data processing tools
	/// are not sufficient for your purposes,
	/// you can get at the raw numbers by looping over
	/// the entries.
	
	Float_t pt;
	reco->SetBranchAddress("pt", &pt);
	for (Int_t i = 0; i < reco->GetEntries(); ++i) {
		reco->GetEntry(i);
		printf("%f\n", pt);
	}
	
	/// finally, although ROOT's documentation does not make this
	/// at all clear (i needed to reference the source code
	/// and trace calls back to a use of TClass::New()... :P)
	/// getting an object from file *does* mean that you own it.
	/// so, if you are being a good citizen then you should clean up
	/// after yourself.
	
	delete reco;
	delete truth;
}
