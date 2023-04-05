#ifndef MACRO_CEMCSPACAL_C
#define MACRO_CEMCSPACAL_C

#include "emcalpositiondependentcalibration/CaloEvaluatorPositionCorrection.h"
#include "emcalpositiondependentcalibration/RawClusterPositionCorrectionFull.h"

namespace Enable
{
    bool CEMC_CLUSTER_FULL = false;
    bool CEMC_EVAL_POSITION_CORRECTION = false;
}  // namespace Enable

void CEMC_Clusters_Full(const string &calib_path="CEMC/PositionRecalibrationFull/")
{
    int verbosity = std::max(Enable::VERBOSITY, Enable::CEMC_VERBOSITY);

    Fun4AllServer *se = Fun4AllServer::instance();

    if (G4CEMC::Cemc_clusterizer == G4CEMC::kCemcTemplateClusterizer)
    {
        RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
        ClusterBuilder->Detector("CEMC");
        ClusterBuilder->Verbosity(verbosity);
        ClusterBuilder->set_threshold_energy(0.030);  // This threshold should be the same as in CEMCprof_Thresh**.root file below
        std::string emc_prof = getenv("CALIBRATIONROOT");
        emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
        ClusterBuilder->LoadProfile(emc_prof);
        se->registerSubsystem(ClusterBuilder);
    }
    else if (G4CEMC::Cemc_clusterizer == G4CEMC::kCemcGraphClusterizer)
    {
        RawClusterBuilderGraph *ClusterBuilder = new RawClusterBuilderGraph("EmcRawClusterBuilderGraph");
        ClusterBuilder->Detector("CEMC");
        ClusterBuilder->Verbosity(verbosity);
        se->registerSubsystem(ClusterBuilder);
    }
    else
    {
        cout << "CEMC_Clusters - unknown clusterizer setting!" << endl;
        exit(1);
    }

    RawClusterPositionCorrectionFull *clusterCorrection = new RawClusterPositionCorrectionFull("CEMC");
    if (Enable::XPLOAD)
    {
        clusterCorrection->Get_eclus_CalibrationParameters().ReadFromCDB("CEMCRECALIB");
        clusterCorrection->Get_ecore_CalibrationParameters().ReadFromCDB("CEMC_ECORE_RECALIB");
    }
    else
    {
        clusterCorrection->Get_eclus_CalibrationParameters().ReadFromFile("CEMC_RECALIB", "xml", 0, 0,
                //raw location
                string(getenv("CALIBRATIONROOT")) + string("/") + calib_path);
        clusterCorrection->Get_ecore_CalibrationParameters().ReadFromFile("CEMC_ECORE_RECALIB", "xml", 0, 0,
                //raw location
                string(getenv("CALIBRATIONROOT")) + string("/") + calib_path);
    }

    clusterCorrection->Verbosity(verbosity);
    se->registerSubsystem(clusterCorrection);

    return;
}

void CEMC_Eval_Position_Correction(const std::string &outputfile)
{
    int verbosity = std::max(Enable::VERBOSITY, Enable::CEMC_VERBOSITY);

    Fun4AllServer *se = Fun4AllServer::instance();

    CaloEvaluatorPositionCorrection *eval = new CaloEvaluatorPositionCorrection("CALOEVALUATORPOSITIONCORRECTION", "CEMC", outputfile);
    eval->Verbosity(verbosity);
    se->registerSubsystem(eval);

    return;
}

#endif
