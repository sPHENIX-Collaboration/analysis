// -- c++ includes -- //
#include <string>
#include <fstream>
#include <string>

// -- module: phparameter includes -- //
#include <phparameter/PHParameters.h>

R__LOAD_LIBRARY(libphparameter.so)

void Construct_Recalibs_Files(string inputFile="../example/LO_positiondependent_calibs_phot.txt", string outDir=string(getenv("CALIBRATIONROOT")) + string("/CEMC/PositionRecalibrationFull"))
{
    const int nbins_eta = 384;
    const int nbins_phi = 64;

    PHParameters *param = new PHParameters("CEMC_RECALIB");

    param->set_string_param("description", Form("Position based recalibrations for CEMC showers, inputfiles located at $CALIBRATIONROOT/CEMC/PositionRecalibrationFull"));

    // The calibrations are made in for 1 sector (4 blocks in phi x 24 blocks in eta) 
    // Each block is divided into 16 by 16 bins for a 2x2 cemc block 
    double calibrations[nbins_eta][nbins_phi];
    ifstream stream;
    stream.open(inputFile);

    cout<<"reading"<<endl;
    if(stream.is_open()){
        int row = 0;
        while(row!=nbins_eta){
            int col=0;
            while(col!=nbins_phi){
                double value;
                stream>>value;
                calibrations[row][col]=value;
                col++;
            }
            row++;
        }

    }
    else cout<<"no recalibs open, can't do anything"<<endl;

    cout<<"setting params"<<endl;
    for(int row=0; row<nbins_eta; row++){
        for(int col=0; col<nbins_phi; col++){
            string recalib_const_name(Form("recalib_const_eta%i_phi%i",row,col));
            param->set_double_param(recalib_const_name,calibrations[row][col]);
        }
    }

    param->set_int_param("number_of_bins_eta",nbins_eta);
    param->set_int_param("number_of_bins_phi",nbins_phi);

    cout<<"write to xml file, located at the path below"<<endl;
    param->WriteToFile("xml",outDir);
}
