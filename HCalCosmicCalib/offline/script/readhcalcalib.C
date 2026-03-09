#include <cdbobjects/CDBTTree.h>

R__LOAD_LIBRARY(libcdbobjects.so)

void readhcalcalib(){
  ofstream ohcal_precalib_file("script/ohcal_precalib.txt", std::ios::trunc);
  ofstream ihcal_precalib_file("script/ihcal_precalib.txt", std::ios::trunc);

  CDBTTree *cdbttree_ohcal = new CDBTTree("ohcal_precalib.root");
  CDBTTree *cdbttree_ihcal = new CDBTTree("ihcal_precalib.root");
  if (!cdbttree_ohcal){
    std::cout << "ohcal CDBTTree not found!" << std::endl;
    return;
  }
  if (!cdbttree_ihcal){
    std::cout << "ihcal CDBTTree not found!" << std::endl;
    return;
  }

  for(int ieta = 0; ieta<24;ieta++){
    for(int iphi = 0; iphi<64; iphi++){
      //int towerid = ieta*64+iphi;
      int towerid = iphi + (ieta << 16U);
      float ihcalcalib = cdbttree_ihcal->GetFloatValue(towerid,"HCALIN_calib_ADC_to_ETower");
      float ohcalcalib = cdbttree_ohcal->GetFloatValue(towerid,"HCALOUT_calib_ADC_to_ETower");
      //replace this
      ihcal_precalib_file << ieta << " " << iphi << " " << ihcalcalib << std::endl;
      ohcal_precalib_file << ieta << " " << iphi << " " << ohcalcalib << std::endl;
    }
  }
  
}
