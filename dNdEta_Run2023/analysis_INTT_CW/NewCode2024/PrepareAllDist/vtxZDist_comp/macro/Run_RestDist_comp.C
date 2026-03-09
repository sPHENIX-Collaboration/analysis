#include "../vtxZDist_comp.h"

R__LOAD_LIBRARY(../libvtxZDist_comp.so)

// note : with vtxZ range cut, and no ClusQA
int comp1()
{
    std::string sPH_labeling = "Internal";
    std::string MC_labeling = "Simulation";

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> data_input_directory_pair_vec = {
        {
            std::string("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed/RestDist/completed") + 
            "/Data_RestDist_vtxZQA_EvtBcoFullDiffCut61_vtxZRangeM10p0to10p0_00054280_merged_merged_001.root"
            ,
            
            {"Data, |Reco. VtxZ| < 10 cm, |BcoFullDiff| > 61", "Data"}
        }
    };

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> MC_input_directory_pair_vec = {
        {
            std::string("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/RestDist/completed") + 
            "/MC_RestDist_vtxZQA_VtxZReWeighting_vtxZRangeM10p0to10p0_merged.root",
            {"HIJING, |Reco. VtxZ| < 10 cm", "HIJING_ZWeight"}
        }
    };

    std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map = {
        {
            "Data",
            {
                // std::make_tuple(0.22, 0.8, "Old vertex XY"),
                std::make_tuple(0.22, 0.76, "w/ INTT vtxZ QA")
            }
        },

        {
            "MC",
            {
                // std::make_tuple(0.22, 0.8, "Truth avg VtxXY"),
                std::make_tuple(0.22, 0.76, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.72, "MC vtxZ Reweighted")
            }
        },


        {
            "Comp",
            {
                // std::make_tuple(0.22, 0.8, "Data, Old vertex XY"),
                // std::make_tuple(0.22, 0.76, "MC, Truth avg vtxXY"),
                std::make_tuple(0.22, 0.72, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.68, "MC vtxZ Reweighted")
            }
        }
    };
    

    std::string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/RestDist/completed/RestComp_VtxZCut_NoClusQA";

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
    // VZDC -> MakeVtxZCheckPlot();
    // VZDC -> PrepareINTTvtxZReWeight();

    return 1;
}

// note : with vtxZ range cut, and w/ ClusQA
int comp2()
{
    std::string sPH_labeling = "Internal";
    std::string MC_labeling = "Simulation";

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> data_input_directory_pair_vec = {
        {
            std::string("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed/RestDist/completed") + 
            "/Data_RestDist_vtxZQA_EvtBcoFullDiffCut61_vtxZRangeM10p0to10p0_ClusQAAdc35PhiSize40_00054280_merged_merged_001.root",
            
            {"Data, |MbdVtxZ| < 10 cm, |BcoFullDiff| > 61", "Data"}
        }
    };

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> MC_input_directory_pair_vec = {
        {
            std::string("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/RestDist/completed") + 
            "/MC_RestDist_vtxZQA_VtxZReWeighting_vtxZRangeM10p0to10p0_ClusQAAdc35PhiSize40_merged.root",

            {"HIJING, |MbdVtxZ| < 10 cm", "HIJING_ZWeight_withClusQA"}
        }
    };

    std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map = {
        {
            "Data",
            {
                // std::make_tuple(0.22, 0.8, "Old vertex XY"),
                std::make_tuple(0.22, 0.76, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.72, "w/ ClusQA")
            }
        },

        {
            "MC",
            {
                // std::make_tuple(0.22, 0.8, "Truth avg VtxXY"),
                std::make_tuple(0.22, 0.76, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.72, "w/ ClusQA"),
                std::make_tuple(0.22, 0.68, "MC vtxZ Reweighted")
            }
        },


        {
            "Comp",
            {
                // std::make_tuple(0.22, 0.8, "Data, Old vertex XY"),
                // std::make_tuple(0.22, 0.76, "MC, Truth avg vtxXY"),
                std::make_tuple(0.22, 0.72, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.68, "w/ ClusQA"),
                std::make_tuple(0.22, 0.64, "MC vtxZ Reweighted")
            }
        }
    };
    

    std::string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/RestDist/completed/RestComp_VtxZCut_ClusQA";

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
    // VZDC -> MakeVtxZCheckPlot();
    // VZDC -> PrepareINTTvtxZReWeight();

    return 2;
}

// note : No vtxZ cut, No ClusQA
int comp3()
{
    std::string sPH_labeling = "Internal";
    std::string MC_labeling = "Simulation";

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> data_input_directory_pair_vec = {
        {
            std::string("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed/RestDist/completed") + 
            "/Data_RestDist_vtxZQA_EvtBcoFullDiffCut61_00054280_merged_merged_001.root",
            
            {"Data, |MbdVtxZ| < 60 cm, |BcoFullDiff| > 61", "data"}
        }
    };

    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> MC_input_directory_pair_vec = {
        {
            std::string("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/RestDist/completed") + 
            "/MC_RestDist_vtxZQA_VtxZReWeighting_merged.root",

            {"HIJING, |MbdVtxZ| < 60 cm", "HIJING_ZWeight_noVtxZCut_noClusQA"}
        }
    };

    std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map = {
        {
            "Data",
            {
                // std::make_tuple(0.22, 0.8, "Old vertex XY"),
                std::make_tuple(0.22, 0.76, "w/ INTT vtxZ QA"),
                // std::make_tuple(0.22, 0.72, "MC vtxZ Reweighted")
                // std::make_tuple(0.22, 0.72, "w/ ClusQA")
            }
        },

        {
            "MC",
            {
                // std::make_tuple(0.22, 0.8, "Truth avg VtxXY"),
                std::make_tuple(0.22, 0.76, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.72, "MC vtxZ Reweighted")
                // std::make_tuple(0.22, 0.72, "w/ ClusQA")
            }
        },


        {
            "Comp",
            {
                // std::make_tuple(0.22, 0.8, "Data, Old vertex XY"),
                // std::make_tuple(0.22, 0.76, "MC, Truth avg vtxXY"),
                std::make_tuple(0.22, 0.72, "w/ INTT vtxZ QA"),
                std::make_tuple(0.22, 0.68, "MC vtxZ Reweighted")
                // std::make_tuple(0.22, 0.68, "w/ ClusQA")
            }
        }
    };
    

    std::string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/RestDist/completed/RestComp_NoVtxZCut_NoClusQA";

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
    // VZDC -> MakeVtxZCheckPlot();
    // VZDC -> PrepareINTTvtxZReWeight();

    return 2;
}

int Run_RestDist_comp()
{
    comp1();
    comp2();
    comp3();
    return 4;
}