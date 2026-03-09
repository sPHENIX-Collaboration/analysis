vector<string> read_list(string folder_direction, string MC_list_name)
{
    vector<string> file_list;
    string list_buffer;
    ifstream data_list;
	data_list.open((folder_direction + "/" + MC_list_name).c_str());

   file_list.clear();

	while (1)
	{
		data_list >> list_buffer;
		if (!data_list.good())
		{
			break;
		}

        if (list_buffer[0] == '#') {continue;}

		file_list.push_back(list_buffer);
	}
	cout<<"size in the" <<MC_list_name<<": "<<file_list.size()<<endl;

    return file_list;
}

int CorrectionRange()
{
    std::string file_list_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p026/completed";
    std::string file_list_name = "file_list_AccEffiCorr.txt";
    std::string h1D_name = "h1D_RotatedBkg_alpha_correction";
    std::pair<double, double> range = std::make_pair(-1.1, 1.1);

    std::vector<std::string> file_list = read_list(file_list_directory, file_list_name);

    TH1D * h1D_AccEffiCorrection = new TH1D("h1D_AccEffiCorrection", "h1D_AccEffiCorrection", 100, 0.7, 1.3);

    for (int i = 0; i < file_list.size(); i++)
    {
        TFile * file_in = TFile::Open(file_list[i].c_str());
        TH1D * h1D = (TH1D*)file_in->Get(h1D_name.c_str());

        std::cout << "Processing " << file_list[i] << std::endl;

        for (int bin_i = 0; bin_i < h1D->GetNbinsX(); bin_i++)
        {
            double bin_content = h1D->GetBinContent(bin_i + 1);
            double bin_center = h1D->GetBinCenter(bin_i + 1);

            if (bin_center < range.first || bin_center > range.second) {continue;}

            h1D_AccEffiCorrection->Fill(bin_content);

            std::cout<<bin_center<<" "<<bin_content<<std::endl;

        }

        std::cout<<std::endl;

    }

    h1D_AccEffiCorrection -> Draw("hist");


    return 0;

}