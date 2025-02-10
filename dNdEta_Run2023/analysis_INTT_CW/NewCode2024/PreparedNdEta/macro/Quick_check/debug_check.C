std::vector<std::string> ReadList(std::string filename)
{
    std::vector<std::string> list;
    std::ifstream file(filename);
    std::string str;
    while (std::getline(file, str))
    {
        list.push_back(str);
    }

    std::cout<<"list.size(): "<<list.size()<<std::endl;

    return list;
}

// note : to split the input vector randomly into N vectors
std::vector<std::vector<std::string>> RandomSplit(std::vector<std::string> list, int N)
{
    std::vector<std::vector<std::string>> output(N);
    std::random_device rd;
    std::mt19937 g(rd());
    // std::shuffle(list.begin(), list.end(), g);

    for (int i = 0; i < list.size(); i++)
    {
        output[i % N].push_back(list[i]);
    }

    return output;
}

std::string get_merged_suffix(int file_index, int total_file)
{
    if (total_file <= 0){
        std::cout<<"wtf"<<std::endl;
        exit(1);
    }
    else if (total_file == 1){
        return "";
    }
    else if (total_file > 1){

        std::string job_index = std::to_string( file_index + 1 );
        int job_index_len = 3;
        job_index.insert(0, job_index_len - job_index.size(), '0');

        return "_" + job_index;
    }

    return "";
}

int debug_check()
{
    int N_merged_files = 2;
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_TrackHist/completed";
    string input_filename = "MC_TrackletHistogram_vtxZReweight_INTT_vtxZ_QA_SecondRun_00142.root"; // note : xxxxx_00001.root

    string input_filename_no_number = input_filename.substr(0, input_filename.find_last_of("_"));

    std::cout<<"input_directory: "<<input_directory<<std::endl;
    std::cout<<"input_filename: "<<input_filename<<std::endl;
    std::cout<<"input_filename_no_number: "<<input_filename_no_number<<std::endl;

    system(Form("ls %s/%s_*.root > %s/file_list.txt", input_directory.c_str(), input_filename_no_number.c_str(), input_directory.c_str()));

    std::vector<std::string> list = ReadList(Form("%s/file_list.txt", input_directory.c_str()));
    for (int i = 0; i < list.size(); i++)
    {
        string filename = list[i];

        // std::cout<<"filename: "<<filename<<std::endl;
        if (filename.find(Form("%s_merged",input_filename_no_number.c_str())) != std::string::npos)
        {
            std::cout<<"removing : "<<filename<<std::endl;
            list.erase(std::remove(list.begin(), list.end(), filename), list.end());
            // system(Form("rm %s", filename.c_str()));

            i -= 1;
        }
    }
    std::cout<<std::endl;
    std::cout<<"N files post removing: "<<list.size()<<std::endl;

    TFile * file_out = new TFile("Debug_check.root", "recreate");
    TCanvas * c1 = new TCanvas("c1","c1",2400,800);

    vector<TGraph *> single_bin_gr_vec; single_bin_gr_vec.clear();

    std::vector<std::vector<std::string>> list_splitted = RandomSplit(list, N_merged_files);


    int file_count = 0;
    for (int i = 0; i < N_merged_files; i++)
    {
        std::cout<<"list_splitted["<<i<<"].size(): "<<list_splitted[i].size()<<std::endl;

        single_bin_gr_vec.push_back(new TGraph());
        single_bin_gr_vec.back() -> SetName(Form("single_bin_NHadron_%d", i));
        single_bin_gr_vec.back() -> SetMarkerStyle(20);
        single_bin_gr_vec.back() -> SetMarkerSize(0.6);
        single_bin_gr_vec.back() -> SetMarkerColor(i + 2);

        for (auto filename : list_splitted[i])
        {
            TFile * file = TFile::Open(filename.c_str());
            TH1D * h1D = (TH1D*)file->Get("debug_h1D_NHadron_OneEtaBin_Inclusive100");

            std::cout<<"filename: "<<filename<<std::endl;
            
            single_bin_gr_vec.back() -> SetPoint(single_bin_gr_vec.back()->GetN(), file_count, h1D->GetMean());

            file_count++;
        }

        single_bin_gr_vec.back() -> GetXaxis() -> SetLimits(0, 200);

        c1 -> cd();
        if (i == 0){
            single_bin_gr_vec.back() -> Draw("APL");
        }
        else{
            single_bin_gr_vec.back() -> Draw("PL same");
        }
    
    }

    



    

    // for (int i = 0; i < list.size(); i++)
    // {
    //     std::cout<<"list["<<i<<"]: "<<list[i]<<std::endl;
    //     TFile * file = TFile::Open(list[i].c_str());
        
    //     TH1D * h1D = (TH1D*)file->Get("debug_h1D_NHadron_OneEtaBin_Inclusive100");
    //     single_bin_gr -> SetPoint(single_bin_gr->GetN(), i, h1D->GetMean());
    // }

    
    // c1 -> cd();
    // single_bin_gr -> SetMarkerStyle(20);
    // single_bin_gr -> SetMarkerSize(0.4);
    // single_bin_gr -> Draw("APL");
    

    file_out -> cd(); 
    c1 -> Write("single_bin_NHadron");
    file_out -> Close();

    c1 -> Print("single_bin_NHadron.pdf");

    system(Form("rm %s/file_list.txt", input_directory.c_str()));

    return 3;
}