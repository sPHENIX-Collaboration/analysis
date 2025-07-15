#include "../vtxZDist_comp.h"

R__LOAD_LIBRARY(../libvtxZDist_comp.so)

int Run_vtxZDist_comp()
{
    bool isVtxZQA = false;
    std::string data_file_index = "_001"; // todo: <---- change it "002" // note : the first and second half of the data
    std::string MC_generator = "AMPT"; // note : HIJING+StrangeEnhance
    std::string sPH_labeling = "Internal";
    
    std::string data_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed/VtxZDist/completed";
    std::string MC_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_AMPT_MDC2_ana472_20250310/Run7/EvtVtxZ/completed/VtxZDist/completed";
    
    std::string MC_labeling = "Simulation";
    std::string With_and_No = (isVtxZQA) ? "With" : "No";
    std::string Nothing_and_No = (isVtxZQA) ? "" : "No";

    // std::string output_folder_name = "vtxZ_comp_withVtxZQA" + data_file_index; // note : auto generated
    std::string output_folder_name = Form("vtxZ_comp_%sVtxZQA",With_and_No.c_str()) + data_file_index; // note : auto generated

    

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> data_input_directory_pair_vec = {
        {
            data_directory + Form("/Data_vtxZDist_%sVtxZQA_EvtBcoFullDiffCut61_00054280_merged_merged%s.root", Nothing_and_No.c_str(), data_file_index.c_str()),
            {"Data, |MbdVtxZ| < 60 cm, |BcoFullDiff| > 61", Form("data_%sVtxZQA", With_and_No.c_str())}
        }
    };

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> MC_input_directory_pair_vec = {
        {
            MC_directory + Form("/MC_vtxZDist_%sVtxZQA_merged.root", Nothing_and_No.c_str()),
            {Form("%s, |MbdVtxZ| < 60 cm",MC_generator.c_str()), Form("%s_noZWeight_%sVtxZQA", MC_generator.c_str(), With_and_No.c_str())}
        }
    };

    std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map = {
        {
            "Data",
            {
                // std::make_tuple(0.22, 0.8, "Old vertex XY"),
                // std::make_tuple(0.22, 0.76, "With INTT vtxZ QA")

                std::make_tuple(0.22, 0.76, Form("%s INTT vtxZ QA", With_and_No.c_str()))
            }
        },

        {
            "MC",
            {
                // std::make_tuple(0.22, 0.8, "Truth avg VtxXY"),
                // std::make_tuple(0.22, 0.76, "With INTT vtxZ QA"),

                std::make_tuple(0.22, 0.8, "MC no VtxZ weight"),
                std::make_tuple(0.22, 0.76, Form("%s INTT vtxZ QA", With_and_No.c_str()))
            }
        },


        {
            "Comp",
            {
                // std::make_tuple(0.22, 0.8, "Data, Old vertex XY"),
                // std::make_tuple(0.22, 0.76, "MC, Truth avg vtxXY"),
                // std::make_tuple(0.22, 0.76, "With INTT vtxZ QA"),
                
                std::make_tuple(0.22, 0.8, "MC no VtxZ weight"),
                std::make_tuple(0.22, 0.76, Form("%s INTT vtxZ QA", With_and_No.c_str()))
            }
        }
    };
    

    std::string output_directory = MC_directory + "/" + output_folder_name;
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi", output_directory.c_str(), output_directory.c_str()));

    bool WithVtxZReWeighting = false;

    vtxZDist_comp * VZDC = new vtxZDist_comp(
        sPH_labeling,
        MC_labeling,
        data_input_directory_pair_vec,
        MC_input_directory_pair_vec,
        labelling_vec_map,
        output_directory,
        WithVtxZReWeighting
    );

    cout<<"11111"<<endl;
    VZDC -> MakeDataPlot("hist");
    cout<<"22222"<<endl;
    VZDC -> MakeMCPlot("hist");
    cout<<"33333"<<endl;
    VZDC -> MakeComparisonPlot();
    cout<<"44444"<<endl;
    VZDC -> MakeVtxZCheckPlot();
    cout<<"55555"<<endl;
    VZDC -> PrepareINTTvtxZReWeight();
    cout<<"66666"<<endl;

    return 777;
}