#include "../vtxZDist_comp.h"

R__LOAD_LIBRARY(../libvtxZDist_comp.so)

int Run_vtxZDist_comp()
{
    std::string sPH_labeling = "Internal";
    std::string MC_labeling = "Simulation";
    std::string output_folder_name = "vtxZ_comp_withVtxZQA"; // note : auto generated
    // std::string output_folder_name = "vtxZ_comp_noVtxZQA"; // note : auto generated

    std::string data_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/completed/VtxZDist/completed";

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> data_input_directory_pair_vec = {
        {
            data_directory + "/Data_vtxZDist_VtxZQA_EvtBcoFullDiffCut61_00054280_merged.root",
            {"Data, |MbdVtxZ| < 60 cm, |BcoFullDiff| > 61", "data_VtxZQA"}
        }
    };

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> MC_input_directory_pair_vec = {
        {
            "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run4/EvtVtxZ/completed/VtxZDist/completed/MC_vtxZDist_VtxZQA_merged.root",
            {"HIJING, |MbdVtxZ| < 60 cm", "HIJING_noZWeight_VtxZQA"}
        }
    };

    std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map = {
        {
            "Data",
            {
                // std::make_tuple(0.22, 0.8, "Old vertex XY"),
                std::make_tuple(0.22, 0.76, "With INTT vtxZ QA")

                // std::make_tuple(0.22, 0.76, "No INTT vtxZ QA")
            }
        },

        {
            "MC",
            {
                // std::make_tuple(0.22, 0.8, "Truth avg VtxXY"),
                std::make_tuple(0.22, 0.76, "With INTT vtxZ QA"),

                std::make_tuple(0.22, 0.8, "MC no VtxZ weight")
                // std::make_tuple(0.22, 0.76, "No INTT vtxZ QA")
            }
        },


        {
            "Comp",
            {
                // std::make_tuple(0.22, 0.8, "Data, Old vertex XY"),
                // std::make_tuple(0.22, 0.76, "MC, Truth avg vtxXY"),
                std::make_tuple(0.22, 0.76, "With INTT vtxZ QA"),
                
                std::make_tuple(0.22, 0.8, "MC no VtxZ weight")
                // std::make_tuple(0.22, 0.76, "No INTT vtxZ QA")
            }
        }
    };
    

    std::string output_directory = data_directory + "/" + output_folder_name;
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

    VZDC -> MakeDataPlot("hist");
    VZDC -> MakeMCPlot("hist");
    VZDC -> MakeComparisonPlot();
    VZDC -> MakeVtxZCheckPlot();
    VZDC -> PrepareINTTvtxZReWeight();

    return 0;
}