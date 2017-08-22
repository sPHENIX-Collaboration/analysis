int make_file_pa(const char *filename_pa)
{
	TFile *file_pa = new TFile(filename_pa, "RECREATE");
	TTree *tree_pa = new TTree("TREE_PA", "Collection of Fit Parameters");
	Float_t constant_term,
	        constant_term_uncertainty,
	        one_over_sqrt_term,
	        one_over_sqrt_term_uncertainty;
	char *filename_de[256];
	tree_pa->Branch("cnst", &constant_term, "cnst/F");
	tree_pa->Branch("d_cnst", &constant_term_uncertainty, "d_cnst/F");
	tree_pa->Branch("sqrt", &one_over_sqrt_term, "sqrt/F");
	tree_pa->Branch("d_sqrt", &one_over_sqrt_term_uncertainty, "d_sqrt/F");
	tree_pa->Branch("filename_origin", filename_de, "filename_origin/C");
	file_pa->Write();
	file_pa->Close();
	return 0;
}
