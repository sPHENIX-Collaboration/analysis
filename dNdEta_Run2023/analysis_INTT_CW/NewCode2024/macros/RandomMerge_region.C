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
std::vector<std::vector<std::string>> RandomSplit(std::vector<std::string> list, int N, bool IsShuffle = true, int segment_index = 0)
{
    std::vector<std::vector<std::string>> output(N);
    std::random_device rd;
    std::mt19937 g(rd());
    if (IsShuffle){std::shuffle(list.begin(), list.end(), g);}

    int N_each = int(list.size()) / N;

    for (int i = 0; i < list.size(); i++)
    {
        // output[i % N].push_back(list[i]);
        // int corresponding_file_index = i / N_each;
        // corresponding_file_index = (corresponding_file_index >= N) ? N - 1 : corresponding_file_index;
        if (i < segment_index){ // note : 0 - 75, < 76
            output[0].push_back(list[i]);
        }
        else {
            output[1].push_back(list[i]);
        }
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

int RandomMerge_region(
    string input_directory,
    string input_filename,
    int segment_index
)
{
    bool IsShuffle = 0;
    int N_merged_files = 2;

    if ( IsShuffle == 1 || N_merged_files != 2){
        std::cout<<"IsShuffle == 1"<<std::endl;
        std::cout<<"N_merged_files != 2"<<std::endl;
        exit(1);
    }

    std::cout<<std::endl;
    // std::cout<<"!!! Be careful, the code can currently only handle the maximal number of files is 10000, [00000 - 09999] !!!"<<std::endl;

    // bool IsShuffle = false;
    // int N_merged_files = 1;
    // string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/completed/RestDist/completed";
    // string input_filename = "MC_RestDist_vtxZQA_VtxZReWeighting_vtxZRangeM10p0to10p0_ClusQAAdc35PhiSize500_00000.root"; // note : xxxxx_00001.root

    string input_filename_no_number = input_filename.substr(0, input_filename.find_last_of("_"));

    std::cout<<"input_directory: "<<input_directory<<std::endl;
    std::cout<<"input_filename: "<<input_filename<<std::endl;
    std::cout<<"input_filename_no_number: "<<input_filename_no_number<<std::endl;
    std::cout<<std::endl;

    system(Form("ls %s/%s_0*.root > %s/file_list.txt", input_directory.c_str(), input_filename_no_number.c_str(), input_directory.c_str())); // todo: the maximal number of files is 10000
    // system(Form("ls %s/%s > %s/file_list.txt", input_directory.c_str(), input_filename.c_str(), input_directory.c_str())); // todo: the maximal number of files is 10000

    std::vector<std::string> list = ReadList(Form("%s/file_list.txt", input_directory.c_str()));
    for (int i = 0; i < list.size(); i++)
    {
        string filename = list[i];

        // std::cout<<"filename: "<<filename<<std::endl;
        if (filename.find(Form("%s_merged",input_filename_no_number.c_str())) != std::string::npos)
        {
            std::cout<<"removing : "<<filename<<std::endl;
            list.erase(std::remove(list.begin(), list.end(), filename), list.end());
            system(Form("rm %s", filename.c_str()));

            i -= 1;
        }
    }
    std::cout<<std::endl;
    std::cout<<"N files post removing: "<<list.size()<<std::endl;

    // for (auto filename : list){
    //     std::cout<<"filename: "<<filename<<std::endl;
    // }

    std::vector<std::vector<std::string>> list_splitted = RandomSplit(list, N_merged_files, IsShuffle, segment_index);

    // for (int i = 0; i < N_merged_files; i++)
    // {
    //     std::cout<<"list_splitted["<<i<<"].size(): "<<list_splitted[i].size()<<std::endl;

    //     std::string all_in_one = "";

    //     for (auto filename : list_splitted[i])
    //     {
    //         std::cout<<"filename: "<<filename<<std::endl;
    //         all_in_one += filename + " ";
    //     }

    //     std::cout<<std::endl;
    //     system(Form("time hadd %s/%s_merged%s.root %s", input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str(), all_in_one.c_str()));

    //     system(Form("mkdir -p %s/merged_files_%s_%s", input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str()));

    //     for (auto filename : list_splitted[i])
    //     {
    //         system(Form("mv %s %s/merged_files_%s_%s", filename.c_str(), input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str()));
    //     }
    // }

    for (int i = 0; i < N_merged_files; i++)
    {
        std::cout<<std::endl;
        std::cout<<"list_splitted["<<i<<"].size(): "<<list_splitted[i].size()<<std::endl;

        // std::string all_in_one = "";

        // for (auto filename : list_splitted[i])
        // {
        //     std::cout<<"filename: "<<filename<<std::endl;
        //     all_in_one += filename + " ";
        // }

        system(Form("mkdir -p %s/merged_files_%s_%s", input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str()));

        for (auto filename : list_splitted[i])
        {
            std::cout<<"filename: "<<filename<<std::endl;
            system(Form("mv %s %s/merged_files_%s_%s", filename.c_str(), input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str()));
        }

        std::string multi_thread_str = (list_splitted[i].size() > 16) ? "-j 8" : "";

        std::cout<<std::endl;
        system(Form("time hadd %s %s/%s_merged%s.root %s/merged_files_%s_%s/*.root", multi_thread_str.c_str(), input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str(), input_directory.c_str(), input_filename_no_number.c_str(), get_merged_suffix(i, N_merged_files).c_str()));
    }

    system(Form("rm %s/file_list.txt", input_directory.c_str()));

    return 3;
}