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

std::map<std::string, int> GetInputTreeBranchesMap(TChain * m_tree_in)
{
    std::map<std::string, int> branch_map;
    TObjArray * branch_list = m_tree_in -> GetListOfBranches();
    for (int i = 0; i < branch_list -> GetEntries(); i++)
    {
        TBranch * branch = dynamic_cast<TBranch*>(branch_list->At(i));
        branch_map[branch -> GetName()] = 1;
    }
    return branch_map;
}


float SystUncRange()
{
    std::string file_list_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p026/completed";
    // std::string file_list_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed";
    std::string file_list_name = "file_list.txt";

    std::vector<std::string> file_list = read_list(file_list_directory, file_list_name);

    TChain * chain = new TChain("tree");
    for (int i = 0; i < file_list.size(); i++)
    {
        chain -> Add(file_list[i].c_str());
    }

    cout<<"chain -> GetEntries() : "<<chain -> GetEntries()<<endl;

    std::pair<double,double> *UncRange_StatUnc = 0;
    std::pair<double,double> *UncRange_RunSegment = 0;
    std::pair<double,double> *UncRange_ClusAdc = 0;
    std::pair<double,double> *UncRange_GeoOffset = 0;
    std::pair<double,double> *UncRange_DeltaPhi = 0;
    std::pair<double,double> *UncRange_ClusPhiSize = 0;
    std::pair<double,double> *UncRange_Strangeness = 0;
    std::pair<double,double> *UncRange_Generator = 0;
    std::pair<double,double> *UncRange_Final = 0;

    std::map<std::string, int> branch_map = GetInputTreeBranchesMap(chain);

    if(branch_map.find("UncRange_StatUnc") != branch_map.end()) {chain -> SetBranchAddress("UncRange_StatUnc", &UncRange_StatUnc);}
    if(branch_map.find("UncRange_RunSegment") != branch_map.end()) {chain -> SetBranchAddress("UncRange_RunSegment", &UncRange_RunSegment);}
    if(branch_map.find("UncRange_ClusAdc") != branch_map.end()) {chain -> SetBranchAddress("UncRange_ClusAdc", &UncRange_ClusAdc);}
    if(branch_map.find("UncRange_GeoOffset") != branch_map.end()) {chain -> SetBranchAddress("UncRange_GeoOffset", &UncRange_GeoOffset);}
    if(branch_map.find("UncRange_DeltaPhi") != branch_map.end()) {chain -> SetBranchAddress("UncRange_DeltaPhi", &UncRange_DeltaPhi);}
    if(branch_map.find("UncRange_ClusPhiSize") != branch_map.end()) {chain -> SetBranchAddress("UncRange_ClusPhiSize", &UncRange_ClusPhiSize);}
    if(branch_map.find("UncRange_Strangeness") != branch_map.end()) {chain -> SetBranchAddress("UncRange_Strangeness", &UncRange_Strangeness);}
    if(branch_map.find("UncRange_Generator") != branch_map.end()) {chain -> SetBranchAddress("UncRange_Generator", &UncRange_Generator);}
    if(branch_map.find("UncRange_Final") != branch_map.end()) {chain -> SetBranchAddress("UncRange_Final", &UncRange_Final);}

    std::pair<double,double> MinMax_StatUnc = {9999999, -9999999};
    std::pair<double,double> MinMax_RunSegment = {9999999, -9999999};
    std::pair<double,double> MinMax_ClusAdc = {9999999, -9999999};
    std::pair<double,double> MinMax_GeoOffset = {9999999, -9999999};
    std::pair<double,double> MinMax_DeltaPhi = {9999999, -9999999};
    std::pair<double,double> MinMax_ClusPhiSize = {9999999, -9999999};
    std::pair<double,double> MinMax_Strangeness = {9999999, -9999999};
    std::pair<double,double> MinMax_Generator = {9999999, -9999999};
    std::pair<double,double> MinMax_Final = {9999999, -9999999};

    for (int i = 0; i < chain -> GetEntries(); i++)
    {
        chain -> GetEntry(i);

        cout<<"i: "<<i<<", file: "<<file_list[i]<<endl;

        if (branch_map.find("UncRange_StatUnc") != branch_map.end()) 
        {
            if (UncRange_StatUnc->first != UncRange_StatUnc->first || UncRange_StatUnc->second != UncRange_StatUnc->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_StatUnc is nan"<<std::endl;
            }

            if (MinMax_StatUnc.first > UncRange_StatUnc -> first)
            {
                MinMax_StatUnc.first = UncRange_StatUnc -> first;
            }
            if (MinMax_StatUnc.second < UncRange_StatUnc -> second)
            {
                MinMax_StatUnc.second = UncRange_StatUnc -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_RunSegment") != branch_map.end()) 
        {
            if (UncRange_RunSegment->first != UncRange_RunSegment->first || UncRange_RunSegment->second != UncRange_RunSegment->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_RunSegment is nan"<<std::endl;
            }

            if (MinMax_RunSegment.first > UncRange_RunSegment -> first)
            {
                MinMax_RunSegment.first = UncRange_RunSegment -> first;
            }
            if (MinMax_RunSegment.second < UncRange_RunSegment -> second)
            {
                MinMax_RunSegment.second = UncRange_RunSegment -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_ClusAdc") != branch_map.end()) 
        {
            if (UncRange_ClusAdc->first != UncRange_ClusAdc->first || UncRange_ClusAdc->second != UncRange_ClusAdc->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_ClusAdc is nan"<<std::endl;
            }

            if (MinMax_ClusAdc.first > UncRange_ClusAdc -> first)
            {
                MinMax_ClusAdc.first = UncRange_ClusAdc -> first;
            }
            if (MinMax_ClusAdc.second < UncRange_ClusAdc -> second)
            {
                MinMax_ClusAdc.second = UncRange_ClusAdc -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_GeoOffset") != branch_map.end()) 
        {
            if (UncRange_GeoOffset->first != UncRange_GeoOffset->first || UncRange_GeoOffset->second != UncRange_GeoOffset->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_GeoOffset is nan"<<std::endl;
            }

            if (MinMax_GeoOffset.first > UncRange_GeoOffset -> first)
            {
                MinMax_GeoOffset.first = UncRange_GeoOffset -> first;
            }
            if (MinMax_GeoOffset.second < UncRange_GeoOffset -> second)
            {
                MinMax_GeoOffset.second = UncRange_GeoOffset -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_DeltaPhi") != branch_map.end()) 
        {
            if (UncRange_DeltaPhi->first != UncRange_DeltaPhi->first || UncRange_DeltaPhi->second != UncRange_DeltaPhi->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_DeltaPhi is nan"<<std::endl;
            }

            if (MinMax_DeltaPhi.first > UncRange_DeltaPhi -> first)
            {
                MinMax_DeltaPhi.first = UncRange_DeltaPhi -> first;
            }
            if (MinMax_DeltaPhi.second < UncRange_DeltaPhi -> second)
            {
                MinMax_DeltaPhi.second = UncRange_DeltaPhi -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_ClusPhiSize") != branch_map.end()) 
        {
            if (UncRange_ClusPhiSize->first != UncRange_ClusPhiSize->first || UncRange_ClusPhiSize->second != UncRange_ClusPhiSize->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_ClusPhiSize is nan"<<std::endl;
            }

            if (MinMax_ClusPhiSize.first > UncRange_ClusPhiSize -> first)
            {
                MinMax_ClusPhiSize.first = UncRange_ClusPhiSize -> first;
            }
            if (MinMax_ClusPhiSize.second < UncRange_ClusPhiSize -> second)
            {
                MinMax_ClusPhiSize.second = UncRange_ClusPhiSize -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_Strangeness") != branch_map.end()) 
        {
            if (UncRange_Strangeness->first != UncRange_Strangeness->first || UncRange_Strangeness->second != UncRange_Strangeness->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_Strangeness is nan"<<std::endl;
            }

            if (MinMax_Strangeness.first > UncRange_Strangeness -> first)
            {
                MinMax_Strangeness.first = UncRange_Strangeness -> first;
            }
            if (MinMax_Strangeness.second < UncRange_Strangeness -> second)
            {
                MinMax_Strangeness.second = UncRange_Strangeness -> second;
            }
        }

        // Division: -----------------------------------------------------------------------------------------------------------------------------

        if (branch_map.find("UncRange_Generator") != branch_map.end()) 
        {
            if (UncRange_Generator->first != UncRange_Generator->first || UncRange_Generator->second != UncRange_Generator->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_Generator is nan"<<std::endl;
            }

            if (MinMax_Generator.first > UncRange_Generator -> first)
            {
                MinMax_Generator.first = UncRange_Generator -> first;
            }
            if (MinMax_Generator.second < UncRange_Generator -> second)
            {
                MinMax_Generator.second = UncRange_Generator -> second;
            }
        }



        // Division: -----------------------------------------------------------------------------------------------------------------------------
        
        if (branch_map.find("UncRange_Final") != branch_map.end()) 
        {
            if (UncRange_Final->first != UncRange_Final->first || UncRange_Final->second != UncRange_Final->second)
            {
                std::cout<<"In file: "<<file_list[i]<<", UncRange_Final is nan"<<std::endl;
            }

            if (MinMax_Final.first > UncRange_Final -> first)
            {
                MinMax_Final.first = UncRange_Final -> first;
            }
            if (MinMax_Final.second < UncRange_Final -> second)
            {
                MinMax_Final.second = UncRange_Final -> second;
            }
        }
    }

    std::cout<<"========================================================================================="<<std::endl;
    std::cout<<Form("StatUnc,     min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_StatUnc.first, 100 * MinMax_StatUnc.second, 100 * MinMax_StatUnc.first, 100 * MinMax_StatUnc.second)<<std::endl;
    std::cout<<Form("RunSegment,  min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_RunSegment.first, 100 * MinMax_RunSegment.second, 100 * MinMax_RunSegment.first, 100 * MinMax_RunSegment.second)<<std::endl;
    std::cout<<Form("ClusAdc,     min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_ClusAdc.first, 100 * MinMax_ClusAdc.second, 100 * MinMax_ClusAdc.first, 100 * MinMax_ClusAdc.second)<<std::endl;
    std::cout<<Form("GeoOffset,   min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_GeoOffset.first, 100 * MinMax_GeoOffset.second, 100 * MinMax_GeoOffset.first, 100 * MinMax_GeoOffset.second)<<std::endl;
    std::cout<<Form("DeltaPhi,    min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_DeltaPhi.first, 100 * MinMax_DeltaPhi.second, 100 * MinMax_DeltaPhi.first, 100 * MinMax_DeltaPhi.second)<<std::endl;
    std::cout<<Form("ClusPhiSize, min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_ClusPhiSize.first, 100 * MinMax_ClusPhiSize.second, 100 * MinMax_ClusPhiSize.first, 100 * MinMax_ClusPhiSize.second)<<std::endl;
    std::cout<<Form("Strangeness, min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_Strangeness.first, 100 * MinMax_Strangeness.second, 100 * MinMax_Strangeness.first, 100 * MinMax_Strangeness.second)<<std::endl;
    std::cout<<Form("Generator,   min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_Generator.first, 100 * MinMax_Generator.second, 100 * MinMax_Generator.first, 100 * MinMax_Generator.second)<<std::endl;
    std::cout<<Form("Final,       min: %.4f%%, max: %.4f%%, [%.4f--%.4f]", 100 * MinMax_Final.first, 100 * MinMax_Final.second, 100 * MinMax_Final.first, 100 * MinMax_Final.second)<<std::endl;
    std::cout<<"========================================================================================="<<std::endl;

    return 6.6667;
}