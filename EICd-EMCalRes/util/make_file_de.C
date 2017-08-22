int make_file_de(const char *filename_de)
{
	TFile *file_de = new TFile(filename_de, "RECREATE");
	TNtuple *tree_de = new TNtuple("TREE_DE", "TNtuple of (dE/E, E) pairs", "de_over_e:d_de_over_e:e");
	file_de->Write();
	file_de->Close();
	return 0;
}
