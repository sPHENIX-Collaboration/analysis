#ifndef COLUMNCHECK_H
#define COLUMNCHECK_H


#include "../ClusHistogram/ClusHistogram.h"

#include "../EvtVtxZTracklet/structure.h"

#include "../Constants.h"

class ColumnCheck : public ClusHistogram{
    public:
        ColumnCheck(
            int process_id_in,
            int runnumber_in,
            int run_nEvents_in,
            std::string input_directory_in,
            std::string input_file_name_in,
            std::string output_directory_in,

            std::string output_file_name_suffix_in,
            std::pair<double, double> vertexXYIncm_in,
            double SetMbinFloat_in,
            
            std::pair<double, double> VtxZRange_in,
            bool IsZClustering_in,
            bool BcoFullDiffCut_in,
            std::pair<bool, std::pair<double, double>> isClusQA_in, // note : {adc, phi size}

            bool ColMulMask_in
        );

        void MainProcess() override; 
        // void MakeDist();
        void EndRun() override;

    protected:
        void EvtCleanUp() override;
        void PrepareOutPutFileName() override;
        void PrepareOutPutRootFile() override;
        void PrepareHistograms() override;

        // note : for the constructor
        bool IsZClustering;
        std::pair<double, double> VtxZRange;
        double SetMbinFloat;

        // note : for histogram
        std::map<std::string,TH1D*> h1D_map;
        std::map<std::string, TH2D*> h2D_map;

        // note : ----------------- for constants -----------------
        double typeA_sensor_half_length_incm = Constants::typeA_sensor_half_length_incm;
        double typeB_sensor_half_length_incm = Constants::typeB_sensor_half_length_incm;
        int Semi_inclusive_bin = Constants::Semi_inclusive_bin;

        // int nZbin = Constants::nZbin;
        // double Zmin = Constants::Zmin;
        // double Zmax = Constants::Zmax;
};

#endif