
//author Ejiro Umaka
//email eumaka1@bnl.gov

#include "Detinfo.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>
#include <epd/EPDDefs.h>

#include <mbd/MbdGeom.h>
#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

#include <TFile.h>
#include <TH3F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>

#include <cassert>
#include <fstream>
#include <TGraph.h>
#include <vector>

//____________________________________________________________________________..
Detinfo::Detinfo(const std::string &name):
 SubsysReco(name)
{
  _event = 0;
  _outfile_name = "qa.root";

}

//____________________________________________________________________________..
Detinfo::~Detinfo()
{
  
}

//____________________________________________________________________________..
int Detinfo::Init(PHCompositeNode *topNode)
{
    
    std::cout << PHWHERE << " Opening file " << _outfile_name << std::endl;

    PHTFileServer::get().open(_outfile_name, "RECREATE");
    PHTFileServer::get().cd(_outfile_name);
    
    zdc = new TNtuple("zdc","zdc", "zdc_ch0_e:zdc_ch0_time:zdc_ch1_e:zdc_ch1_time:zdc_ch2_e:zdc_ch2_time:zdc_ch3_e:zdc_ch3_time:zdc_ch4_e:zdc_ch4_time:zdc_ch5_e:zdc_ch5_time:zdc_ch6_e:zdc_ch6_time:zdc_ch7_e:zdc_ch7_time:zdc_ch8_e:zdc_ch8_time:zdc_ch9_e:zdc_ch9_time:zdc_ch10_e:zdc_ch10_time:zdc_ch11_e:zdc_ch11_time:zdc_ch12_e:zdc_ch12_time:zdc_ch13_e:zdc_ch13_time:zdc_ch14_e:zdc_ch14_time:zdc_ch15_e:zdc_ch15_time");
    
    hzdctime = new TH1D("hzdctime", "hzdctime", 19, -0.5, 18.5);

    sepd = new TNtuple("sepd", "sepd", "epd_ch0_e:epd_ch0_time:epd_ch1_e:epd_ch1_time:epd_ch2_e:epd_ch2_time:epd_ch3_e:epd_ch3_time:epd_ch4_e:epd_ch4_time:epd_ch5_e:epd_ch5_time:epd_ch6_e:epd_ch6_time:epd_ch7_e:epd_ch7_time:epd_ch8_e:epd_ch8_time:epd_ch9_e:epd_ch9_time:epd_ch10_e:epd_ch10_time:epd_ch11_e:epd_ch11_time:epd_ch12_e:epd_ch12_time:epd_ch13_e:epd_ch13_time:epd_ch14_e:epd_ch14_time:epd_ch15_e:epd_ch15_time:epd_ch16_e:epd_ch16_time:epd_ch17_e:epd_ch17_time:epd_ch18_e:epd_ch18_time:epd_ch19_e:epd_ch19_time:epd_ch20_e:epd_ch20_time:epd_ch21_e:epd_ch21_time:epd_ch22_e:epd_ch22_time:epd_ch23_e:epd_ch23_time:epd_ch24_e:epd_ch24_time:epd_ch25_e:epd_ch25_time:epd_ch26_e:epd_ch26_time:epd_ch27_e:epd_ch27_time:epd_ch28_e:epd_ch28_time:epd_ch29_e:epd_ch29_time:epd_ch30_e:epd_ch30_time:epd_ch31_e:epd_ch31_time:epd_ch32_e:epd_ch32_time:epd_ch33_e:epd_ch33_time:epd_ch34_e:epd_ch34_time:epd_ch35_e:epd_ch35_time:epd_ch36_e:epd_ch36_time:epd_ch37_e:epd_ch37_time:epd_ch38_e:epd_ch38_time:epd_ch39_e:epd_ch39_time:epd_ch40_e:epd_ch40_time:epd_ch41_e:epd_ch41_time:epd_ch42_e:epd_ch42_time:epd_ch43_e:epd_ch43_time:epd_ch44_e:epd_ch44_time:epd_ch45_e:epd_ch45_time:epd_ch46_e:epd_ch46_time:epd_ch47_e:epd_ch47_time:epd_ch48_e:epd_ch48_time:epd_ch49_e:epd_ch49_time:epd_ch50_e:epd_ch50_time:epd_ch51_e:epd_ch51_time:epd_ch52_e:epd_ch52_time:epd_ch53_e:epd_ch53_time:epd_ch54_e:epd_ch54_time:epd_ch55_e:epd_ch55_time:epd_ch56_e:epd_ch56_time:epd_ch57_e:epd_ch57_time:epd_ch58_e:epd_ch58_time:epd_ch59_e:epd_ch59_time:epd_ch60_e:epd_ch60_time:epd_ch61_e:epd_ch61_time:epd_ch62_e:epd_ch62_time:epd_ch63_e:epd_ch63_time:epd_ch64_e:epd_ch64_time:epd_ch65_e:epd_ch65_time:epd_ch66_e:epd_ch66_time:epd_ch67_e:epd_ch67_time:epd_ch68_e:epd_ch68_time:epd_ch69_e:epd_ch69_time:epd_ch70_e:epd_ch70_time:epd_ch71_e:epd_ch71_time:epd_ch72_e:epd_ch72_time:epd_ch73_e:epd_ch73_time:epd_ch74_e:epd_ch74_time:epd_ch75_e:epd_ch75_time:epd_ch76_e:epd_ch76_time:epd_ch77_e:epd_ch77_time:epd_ch78_e:epd_ch78_time:epd_ch79_e:epd_ch79_time:epd_ch80_e:epd_ch80_time:epd_ch81_e:epd_ch81_time:epd_ch82_e:epd_ch82_time:epd_ch83_e:epd_ch83_time:epd_ch84_e:epd_ch84_time:epd_ch85_e:epd_ch85_time:epd_ch86_e:epd_ch86_time:epd_ch87_e:epd_ch87_time:epd_ch88_e:epd_ch88_time:epd_ch89_e:epd_ch89_time:epd_ch90_e:epd_ch90_time:epd_ch91_e:epd_ch91_time:epd_ch92_e:epd_ch92_time:epd_ch93_e:epd_ch93_time:epd_ch94_e:epd_ch94_time:epd_ch95_e:epd_ch95_time:epd_ch96_e:epd_ch96_time:epd_ch97_e:epd_ch97_time:epd_ch98_e:epd_ch98_time:epd_ch99_e:epd_ch99_time:epd_ch100_e:epd_ch100_time:epd_ch101_e:epd_ch101_time:epd_ch102_e:epd_ch102_time:epd_ch103_e:epd_ch103_time:epd_ch104_e:epd_ch104_time:epd_ch105_e:epd_ch105_time:epd_ch106_e:epd_ch106_time:epd_ch107_e:epd_ch107_time:epd_ch108_e:epd_ch108_time:epd_ch109_e:epd_ch109_time:epd_ch110_e:epd_ch110_time:epd_ch111_e:epd_ch111_time:epd_ch112_e:epd_ch112_time:epd_ch113_e:epd_ch113_time:epd_ch114_e:epd_ch114_time:epd_ch115_e:epd_ch115_time:epd_ch116_e:epd_ch116_time:epd_ch117_e:epd_ch117_time:epd_ch118_e:epd_ch118_time:epd_ch119_e:epd_ch119_time:epd_ch120_e:epd_ch120_time:epd_ch121_e:epd_ch121_time:epd_ch122_e:epd_ch122_time:epd_ch123_e:epd_ch123_time:epd_ch124_e:epd_ch124_time:epd_ch125_e:epd_ch125_time:epd_ch126_e:epd_ch126_time:epd_ch127_e:epd_ch127_time:epd_ch128_e:epd_ch128_time:epd_ch129_e:epd_ch129_time:epd_ch130_e:epd_ch130_time:epd_ch131_e:epd_ch131_time:epd_ch132_e:epd_ch132_time:epd_ch133_e:epd_ch133_time:epd_ch134_e:epd_ch134_time:epd_ch135_e:epd_ch135_time:epd_ch136_e:epd_ch136_time:epd_ch137_e:epd_ch137_time:epd_ch138_e:epd_ch138_time:epd_ch139_e:epd_ch139_time:epd_ch140_e:epd_ch140_time:epd_ch141_e:epd_ch141_time:epd_ch142_e:epd_ch142_time:epd_ch143_e:epd_ch143_time:epd_ch144_e:epd_ch144_time:epd_ch145_e:epd_ch145_time:epd_ch146_e:epd_ch146_time:epd_ch147_e:epd_ch147_time:epd_ch148_e:epd_ch148_time:epd_ch149_e:epd_ch149_time:epd_ch150_e:epd_ch150_time:epd_ch151_e:epd_ch151_time:epd_ch152_e:epd_ch152_time:epd_ch153_e:epd_ch153_time:epd_ch154_e:epd_ch154_time:epd_ch155_e:epd_ch155_time:epd_ch156_e:epd_ch156_time:epd_ch157_e:epd_ch157_time:epd_ch158_e:epd_ch158_time:epd_ch159_e:epd_ch159_time:epd_ch160_e:epd_ch160_time:epd_ch161_e:epd_ch161_time:epd_ch162_e:epd_ch162_time:epd_ch163_e:epd_ch163_time:epd_ch164_e:epd_ch164_time:epd_ch165_e:epd_ch165_time:epd_ch166_e:epd_ch166_time:epd_ch167_e:epd_ch167_time:epd_ch168_e:epd_ch168_time:epd_ch169_e:epd_ch169_time:epd_ch170_e:epd_ch170_time:epd_ch171_e:epd_ch171_time:epd_ch172_e:epd_ch172_time:epd_ch173_e:epd_ch173_time:epd_ch174_e:epd_ch174_time:epd_ch175_e:epd_ch175_time:epd_ch176_e:epd_ch176_time:epd_ch177_e:epd_ch177_time:epd_ch178_e:epd_ch178_time:epd_ch179_e:epd_ch179_time:epd_ch180_e:epd_ch180_time:epd_ch181_e:epd_ch181_time:epd_ch182_e:epd_ch182_time:epd_ch183_e:epd_ch183_time:epd_ch184_e:epd_ch184_time:epd_ch185_e:epd_ch185_time:epd_ch186_e:epd_ch186_time:epd_ch187_e:epd_ch187_time:epd_ch188_e:epd_ch188_time:epd_ch189_e:epd_ch189_time:epd_ch190_e:epd_ch190_time:epd_ch191_e:epd_ch191_time:epd_ch192_e:epd_ch192_time:epd_ch193_e:epd_ch193_time:epd_ch194_e:epd_ch194_time:epd_ch195_e:epd_ch195_time:epd_ch196_e:epd_ch196_time:epd_ch197_e:epd_ch197_time:epd_ch198_e:epd_ch198_time:epd_ch199_e:epd_ch199_time:epd_ch200_e:epd_ch200_time:epd_ch201_e:epd_ch201_time:epd_ch202_e:epd_ch202_time:epd_ch203_e:epd_ch203_time:epd_ch204_e:epd_ch204_time:epd_ch205_e:epd_ch205_time:epd_ch206_e:epd_ch206_time:epd_ch207_e:epd_ch207_time:epd_ch208_e:epd_ch208_time:epd_ch209_e:epd_ch209_time:epd_ch210_e:epd_ch210_time:epd_ch211_e:epd_ch211_time:epd_ch212_e:epd_ch212_time:epd_ch213_e:epd_ch213_time:epd_ch214_e:epd_ch214_time:epd_ch215_e:epd_ch215_time:epd_ch216_e:epd_ch216_time:epd_ch217_e:epd_ch217_time:epd_ch218_e:epd_ch218_time:epd_ch219_e:epd_ch219_time:epd_ch220_e:epd_ch220_time:epd_ch221_e:epd_ch221_time:epd_ch222_e:epd_ch222_time:epd_ch223_e:epd_ch223_time:epd_ch224_e:epd_ch224_time:epd_ch225_e:epd_ch225_time:epd_ch226_e:epd_ch226_time:epd_ch227_e:epd_ch227_time:epd_ch228_e:epd_ch228_time:epd_ch229_e:epd_ch229_time:epd_ch230_e:epd_ch230_time:epd_ch231_e:epd_ch231_time:epd_ch232_e:epd_ch232_time:epd_ch233_e:epd_ch233_time:epd_ch234_e:epd_ch234_time:epd_ch235_e:epd_ch235_time:epd_ch236_e:epd_ch236_time:epd_ch237_e:epd_ch237_time:epd_ch238_e:epd_ch238_time:epd_ch239_e:epd_ch239_time:epd_ch240_e:epd_ch240_time:epd_ch241_e:epd_ch241_time:epd_ch242_e:epd_ch242_time:epd_ch243_e:epd_ch243_time:epd_ch244_e:epd_ch244_time:epd_ch245_e:epd_ch245_time:epd_ch246_e:epd_ch246_time:epd_ch247_e:epd_ch247_time:epd_ch248_e:epd_ch248_time:epd_ch249_e:epd_ch249_time:epd_ch250_e:epd_ch250_time:epd_ch251_e:epd_ch251_time:epd_ch252_e:epd_ch252_time:epd_ch253_e:epd_ch253_time:epd_ch254_e:epd_ch254_time:epd_ch255_e:epd_ch255_time:epd_ch256_e:epd_ch256_time:epd_ch257_e:epd_ch257_time:epd_ch258_e:epd_ch258_time:epd_ch259_e:epd_ch259_time:epd_ch260_e:epd_ch260_time:epd_ch261_e:epd_ch261_time:epd_ch262_e:epd_ch262_time:epd_ch263_e:epd_ch263_time:epd_ch264_e:epd_ch264_time:epd_ch265_e:epd_ch265_time:epd_ch266_e:epd_ch266_time:epd_ch267_e:epd_ch267_time:epd_ch268_e:epd_ch268_time:epd_ch269_e:epd_ch269_time:epd_ch270_e:epd_ch270_time:epd_ch271_e:epd_ch271_time:epd_ch272_e:epd_ch272_time:epd_ch273_e:epd_ch273_time:epd_ch274_e:epd_ch274_time:epd_ch275_e:epd_ch275_time:epd_ch276_e:epd_ch276_time:epd_ch277_e:epd_ch277_time:epd_ch278_e:epd_ch278_time:epd_ch279_e:epd_ch279_time:epd_ch280_e:epd_ch280_time:epd_ch281_e:epd_ch281_time:epd_ch282_e:epd_ch282_time:epd_ch283_e:epd_ch283_time:epd_ch284_e:epd_ch284_time:epd_ch285_e:epd_ch285_time:epd_ch286_e:epd_ch286_time:epd_ch287_e:epd_ch287_time:epd_ch288_e:epd_ch288_time:epd_ch289_e:epd_ch289_time:epd_ch290_e:epd_ch290_time:epd_ch291_e:epd_ch291_time:epd_ch292_e:epd_ch292_time:epd_ch293_e:epd_ch293_time:epd_ch294_e:epd_ch294_time:epd_ch295_e:epd_ch295_time:epd_ch296_e:epd_ch296_time:epd_ch297_e:epd_ch297_time:epd_ch298_e:epd_ch298_time:epd_ch299_e:epd_ch299_time:epd_ch300_e:epd_ch300_time:epd_ch301_e:epd_ch301_time:epd_ch302_e:epd_ch302_time:epd_ch303_e:epd_ch303_time:epd_ch304_e:epd_ch304_time:epd_ch305_e:epd_ch305_time:epd_ch306_e:epd_ch306_time:epd_ch307_e:epd_ch307_time:epd_ch308_e:epd_ch308_time:epd_ch309_e:epd_ch309_time:epd_ch310_e:epd_ch310_time:epd_ch311_e:epd_ch311_time:epd_ch312_e:epd_ch312_time:epd_ch313_e:epd_ch313_time:epd_ch314_e:epd_ch314_time:epd_ch315_e:epd_ch315_time:epd_ch316_e:epd_ch316_time:epd_ch317_e:epd_ch317_time:epd_ch318_e:epd_ch318_time:epd_ch319_e:epd_ch319_time:epd_ch320_e:epd_ch320_time:epd_ch321_e:epd_ch321_time:epd_ch322_e:epd_ch322_time:epd_ch323_e:epd_ch323_time:epd_ch324_e:epd_ch324_time:epd_ch325_e:epd_ch325_time:epd_ch326_e:epd_ch326_time:epd_ch327_e:epd_ch327_time:epd_ch328_e:epd_ch328_time:epd_ch329_e:epd_ch329_time:epd_ch330_e:epd_ch330_time:epd_ch331_e:epd_ch331_time:epd_ch332_e:epd_ch332_time:epd_ch333_e:epd_ch333_time:epd_ch334_e:epd_ch334_time:epd_ch335_e:epd_ch335_time:epd_ch336_e:epd_ch336_time:epd_ch337_e:epd_ch337_time:epd_ch338_e:epd_ch338_time:epd_ch339_e:epd_ch339_time:epd_ch340_e:epd_ch340_time:epd_ch341_e:epd_ch341_time:epd_ch342_e:epd_ch342_time:epd_ch343_e:epd_ch343_time:epd_ch344_e:epd_ch344_time:epd_ch345_e:epd_ch345_time:epd_ch346_e:epd_ch346_time:epd_ch347_e:epd_ch347_time:epd_ch348_e:epd_ch348_time:epd_ch349_e:epd_ch349_time:epd_ch350_e:epd_ch350_time:epd_ch351_e:epd_ch351_time:epd_ch352_e:epd_ch352_time:epd_ch353_e:epd_ch353_time:epd_ch354_e:epd_ch354_time:epd_ch355_e:epd_ch355_time:epd_ch356_e:epd_ch356_time:epd_ch357_e:epd_ch357_time:epd_ch358_e:epd_ch358_time:epd_ch359_e:epd_ch359_time:epd_ch360_e:epd_ch360_time:epd_ch361_e:epd_ch361_time:epd_ch362_e:epd_ch362_time:epd_ch363_e:epd_ch363_time:epd_ch364_e:epd_ch364_time:epd_ch365_e:epd_ch365_time:epd_ch366_e:epd_ch366_time:epd_ch367_e:epd_ch367_time:epd_ch368_e:epd_ch368_time:epd_ch369_e:epd_ch369_time:epd_ch370_e:epd_ch370_time:epd_ch371_e:epd_ch371_time:epd_ch372_e:epd_ch372_time:epd_ch373_e:epd_ch373_time:epd_ch374_e:epd_ch374_time:epd_ch375_e:epd_ch375_time:epd_ch376_e:epd_ch376_time:epd_ch377_e:epd_ch377_time:epd_ch378_e:epd_ch378_time:epd_ch379_e:epd_ch379_time:epd_ch380_e:epd_ch380_time:epd_ch381_e:epd_ch381_time:epd_ch382_e:epd_ch382_time:epd_ch383_e:epd_ch383_time:epd_ch384_e:epd_ch384_time:epd_ch385_e:epd_ch385_time:epd_ch386_e:epd_ch386_time:epd_ch387_e:epd_ch387_time:epd_ch388_e:epd_ch388_time:epd_ch389_e:epd_ch389_time:epd_ch390_e:epd_ch390_time:epd_ch391_e:epd_ch391_time:epd_ch392_e:epd_ch392_time:epd_ch393_e:epd_ch393_time:epd_ch394_e:epd_ch394_time:epd_ch395_e:epd_ch395_time:epd_ch396_e:epd_ch396_time:epd_ch397_e:epd_ch397_time:epd_ch398_e:epd_ch398_time:epd_ch399_e:epd_ch399_time:epd_ch400_e:epd_ch400_time:epd_ch401_e:epd_ch401_time:epd_ch402_e:epd_ch402_time:epd_ch403_e:epd_ch403_time:epd_ch404_e:epd_ch404_time:epd_ch405_e:epd_ch405_time:epd_ch406_e:epd_ch406_time:epd_ch407_e:epd_ch407_time:epd_ch408_e:epd_ch408_time:epd_ch409_e:epd_ch409_time:epd_ch410_e:epd_ch410_time:epd_ch411_e:epd_ch411_time:epd_ch412_e:epd_ch412_time:epd_ch413_e:epd_ch413_time:epd_ch414_e:epd_ch414_time:epd_ch415_e:epd_ch415_time:epd_ch416_e:epd_ch416_time:epd_ch417_e:epd_ch417_time:epd_ch418_e:epd_ch418_time:epd_ch419_e:epd_ch419_time:epd_ch420_e:epd_ch420_time:epd_ch421_e:epd_ch421_time:epd_ch422_e:epd_ch422_time:epd_ch423_e:epd_ch423_time:epd_ch424_e:epd_ch424_time:epd_ch425_e:epd_ch425_time:epd_ch426_e:epd_ch426_time:epd_ch427_e:epd_ch427_time:epd_ch428_e:epd_ch428_time:epd_ch429_e:epd_ch429_time:epd_ch430_e:epd_ch430_time:epd_ch431_e:epd_ch431_time:epd_ch432_e:epd_ch432_time:epd_ch433_e:epd_ch433_time:epd_ch434_e:epd_ch434_time:epd_ch435_e:epd_ch435_time:epd_ch436_e:epd_ch436_time:epd_ch437_e:epd_ch437_time:epd_ch438_e:epd_ch438_time:epd_ch439_e:epd_ch439_time:epd_ch440_e:epd_ch440_time:epd_ch441_e:epd_ch441_time:epd_ch442_e:epd_ch442_time:epd_ch443_e:epd_ch443_time:epd_ch444_e:epd_ch444_time:epd_ch445_e:epd_ch445_time:epd_ch446_e:epd_ch446_time:epd_ch447_e:epd_ch447_time:epd_ch448_e:epd_ch448_time:epd_ch449_e:epd_ch449_time:epd_ch450_e:epd_ch450_time:epd_ch451_e:epd_ch451_time:epd_ch452_e:epd_ch452_time:epd_ch453_e:epd_ch453_time:epd_ch454_e:epd_ch454_time:epd_ch455_e:epd_ch455_time:epd_ch456_e:epd_ch456_time:epd_ch457_e:epd_ch457_time:epd_ch458_e:epd_ch458_time:epd_ch459_e:epd_ch459_time:epd_ch460_e:epd_ch460_time:epd_ch461_e:epd_ch461_time:epd_ch462_e:epd_ch462_time:epd_ch463_e:epd_ch463_time:epd_ch464_e:epd_ch464_time:epd_ch465_e:epd_ch465_time:epd_ch466_e:epd_ch466_time:epd_ch467_e:epd_ch467_time:epd_ch468_e:epd_ch468_time:epd_ch469_e:epd_ch469_time:epd_ch470_e:epd_ch470_time:epd_ch471_e:epd_ch471_time:epd_ch472_e:epd_ch472_time:epd_ch473_e:epd_ch473_time:epd_ch474_e:epd_ch474_time:epd_ch475_e:epd_ch475_time:epd_ch476_e:epd_ch476_time:epd_ch477_e:epd_ch477_time:epd_ch478_e:epd_ch478_time:epd_ch479_e:epd_ch479_time:epd_ch480_e:epd_ch480_time:epd_ch481_e:epd_ch481_time:epd_ch482_e:epd_ch482_time:epd_ch483_e:epd_ch483_time:epd_ch484_e:epd_ch484_time:epd_ch485_e:epd_ch485_time:epd_ch486_e:epd_ch486_time:epd_ch487_e:epd_ch487_time:epd_ch488_e:epd_ch488_time:epd_ch489_e:epd_ch489_time:epd_ch490_e:epd_ch490_time:epd_ch491_e:epd_ch491_time:epd_ch492_e:epd_ch492_time:epd_ch493_e:epd_ch493_time:epd_ch494_e:epd_ch494_time:epd_ch495_e:epd_ch495_time:epd_ch496_e:epd_ch496_time:epd_ch497_e:epd_ch497_time:epd_ch498_e:epd_ch498_time:epd_ch499_e:epd_ch499_time:epd_ch500_e:epd_ch500_time:epd_ch501_e:epd_ch501_time:epd_ch502_e:epd_ch502_time:epd_ch503_e:epd_ch503_time:epd_ch504_e:epd_ch504_time:epd_ch505_e:epd_ch505_time:epd_ch506_e:epd_ch506_time:epd_ch507_e:epd_ch507_time:epd_ch508_e:epd_ch508_time:epd_ch509_e:epd_ch509_time:epd_ch510_e:epd_ch510_time:epd_ch511_e:epd_ch511_time:epd_ch512_e:epd_ch512_time:epd_ch513_e:epd_ch513_time:epd_ch514_e:epd_ch514_time:epd_ch515_e:epd_ch515_time:epd_ch516_e:epd_ch516_time:epd_ch517_e:epd_ch517_time:epd_ch518_e:epd_ch518_time:epd_ch519_e:epd_ch519_time:epd_ch520_e:epd_ch520_time:epd_ch521_e:epd_ch521_time:epd_ch522_e:epd_ch522_time:epd_ch523_e:epd_ch523_time:epd_ch524_e:epd_ch524_time:epd_ch525_e:epd_ch525_time:epd_ch526_e:epd_ch526_time:epd_ch527_e:epd_ch527_time:epd_ch528_e:epd_ch528_time:epd_ch529_e:epd_ch529_time:epd_ch530_e:epd_ch530_time:epd_ch531_e:epd_ch531_time:epd_ch532_e:epd_ch532_time:epd_ch533_e:epd_ch533_time:epd_ch534_e:epd_ch534_time:epd_ch535_e:epd_ch535_time:epd_ch536_e:epd_ch536_time:epd_ch537_e:epd_ch537_time:epd_ch538_e:epd_ch538_time:epd_ch539_e:epd_ch539_time:epd_ch540_e:epd_ch540_time:epd_ch541_e:epd_ch541_time:epd_ch542_e:epd_ch542_time:epd_ch543_e:epd_ch543_time:epd_ch544_e:epd_ch544_time:epd_ch545_e:epd_ch545_time:epd_ch546_e:epd_ch546_time:epd_ch547_e:epd_ch547_time:epd_ch548_e:epd_ch548_time:epd_ch549_e:epd_ch549_time:epd_ch550_e:epd_ch550_time:epd_ch551_e:epd_ch551_time:epd_ch552_e:epd_ch552_time:epd_ch553_e:epd_ch553_time:epd_ch554_e:epd_ch554_time:epd_ch555_e:epd_ch555_time:epd_ch556_e:epd_ch556_time:epd_ch557_e:epd_ch557_time:epd_ch558_e:epd_ch558_time:epd_ch559_e:epd_ch559_time:epd_ch560_e:epd_ch560_time:epd_ch561_e:epd_ch561_time:epd_ch562_e:epd_ch562_time:epd_ch563_e:epd_ch563_time:epd_ch564_e:epd_ch564_time:epd_ch565_e:epd_ch565_time:epd_ch566_e:epd_ch566_time:epd_ch567_e:epd_ch567_time:epd_ch568_e:epd_ch568_time:epd_ch569_e:epd_ch569_time:epd_ch570_e:epd_ch570_time:epd_ch571_e:epd_ch571_time:epd_ch572_e:epd_ch572_time:epd_ch573_e:epd_ch573_time:epd_ch574_e:epd_ch574_time:epd_ch575_e:epd_ch575_time:epd_ch576_e:epd_ch576_time:epd_ch577_e:epd_ch577_time:epd_ch578_e:epd_ch578_time:epd_ch579_e:epd_ch579_time:epd_ch580_e:epd_ch580_time:epd_ch581_e:epd_ch581_time:epd_ch582_e:epd_ch582_time:epd_ch583_e:epd_ch583_time:epd_ch584_e:epd_ch584_time:epd_ch585_e:epd_ch585_time:epd_ch586_e:epd_ch586_time:epd_ch587_e:epd_ch587_time:epd_ch588_e:epd_ch588_time:epd_ch589_e:epd_ch589_time:epd_ch590_e:epd_ch590_time:epd_ch591_e:epd_ch591_time:epd_ch592_e:epd_ch592_time:epd_ch593_e:epd_ch593_time:epd_ch594_e:epd_ch594_time:epd_ch595_e:epd_ch595_time:epd_ch596_e:epd_ch596_time:epd_ch597_e:epd_ch597_time:epd_ch598_e:epd_ch598_time:epd_ch599_e:epd_ch599_time:epd_ch600_e:epd_ch600_time:epd_ch601_e:epd_ch601_time:epd_ch602_e:epd_ch602_time:epd_ch603_e:epd_ch603_time:epd_ch604_e:epd_ch604_time:epd_ch605_e:epd_ch605_time:epd_ch606_e:epd_ch606_time:epd_ch607_e:epd_ch607_time:epd_ch608_e:epd_ch608_time:epd_ch609_e:epd_ch609_time:epd_ch610_e:epd_ch610_time:epd_ch611_e:epd_ch611_time:epd_ch612_e:epd_ch612_time:epd_ch613_e:epd_ch613_time:epd_ch614_e:epd_ch614_time:epd_ch615_e:epd_ch615_time:epd_ch616_e:epd_ch616_time:epd_ch617_e:epd_ch617_time:epd_ch618_e:epd_ch618_time:epd_ch619_e:epd_ch619_time:epd_ch620_e:epd_ch620_time:epd_ch621_e:epd_ch621_time:epd_ch622_e:epd_ch622_time:epd_ch623_e:epd_ch623_time:epd_ch624_e:epd_ch624_time:epd_ch625_e:epd_ch625_time:epd_ch626_e:epd_ch626_time:epd_ch627_e:epd_ch627_time:epd_ch628_e:epd_ch628_time:epd_ch629_e:epd_ch629_time:epd_ch630_e:epd_ch630_time:epd_ch631_e:epd_ch631_time:epd_ch632_e:epd_ch632_time:epd_ch633_e:epd_ch633_time:epd_ch634_e:epd_ch634_time:epd_ch635_e:epd_ch635_time:epd_ch636_e:epd_ch636_time:epd_ch637_e:epd_ch637_time:epd_ch638_e:epd_ch638_time:epd_ch639_e:epd_ch639_time:epd_ch640_e:epd_ch640_time:epd_ch641_e:epd_ch641_time:epd_ch642_e:epd_ch642_time:epd_ch643_e:epd_ch643_time:epd_ch644_e:epd_ch644_time:epd_ch645_e:epd_ch645_time:epd_ch646_e:epd_ch646_time:epd_ch647_e:epd_ch647_time:epd_ch648_e:epd_ch648_time:epd_ch649_e:epd_ch649_time:epd_ch650_e:epd_ch650_time:epd_ch651_e:epd_ch651_time:epd_ch652_e:epd_ch652_time:epd_ch653_e:epd_ch653_time:epd_ch654_e:epd_ch654_time:epd_ch655_e:epd_ch655_time:epd_ch656_e:epd_ch656_time:epd_ch657_e:epd_ch657_time:epd_ch658_e:epd_ch658_time:epd_ch659_e:epd_ch659_time:epd_ch660_e:epd_ch660_time:epd_ch661_e:epd_ch661_time:epd_ch662_e:epd_ch662_time:epd_ch663_e:epd_ch663_time:epd_ch664_e:epd_ch664_time:epd_ch665_e:epd_ch665_time:epd_ch666_e:epd_ch666_time:epd_ch667_e:epd_ch667_time:epd_ch668_e:epd_ch668_time:epd_ch669_e:epd_ch669_time:epd_ch670_e:epd_ch670_time:epd_ch671_e:epd_ch671_time:epd_ch672_e:epd_ch672_time:epd_ch673_e:epd_ch673_time:epd_ch674_e:epd_ch674_time:epd_ch675_e:epd_ch675_time:epd_ch676_e:epd_ch676_time:epd_ch677_e:epd_ch677_time:epd_ch678_e:epd_ch678_time:epd_ch679_e:epd_ch679_time:epd_ch680_e:epd_ch680_time:epd_ch681_e:epd_ch681_time:epd_ch682_e:epd_ch682_time:epd_ch683_e:epd_ch683_time:epd_ch684_e:epd_ch684_time:epd_ch685_e:epd_ch685_time:epd_ch686_e:epd_ch686_time:epd_ch687_e:epd_ch687_time:epd_ch688_e:epd_ch688_time:epd_ch689_e:epd_ch689_time:epd_ch690_e:epd_ch690_time:epd_ch691_e:epd_ch691_time:epd_ch692_e:epd_ch692_time:epd_ch693_e:epd_ch693_time:epd_ch694_e:epd_ch694_time:epd_ch695_e:epd_ch695_time:epd_ch696_e:epd_ch696_time:epd_ch697_e:epd_ch697_time:epd_ch698_e:epd_ch698_time:epd_ch699_e:epd_ch699_time:epd_ch700_e:epd_ch700_time:epd_ch701_e:epd_ch701_time:epd_ch702_e:epd_ch702_time:epd_ch703_e:epd_ch703_time:epd_ch704_e:epd_ch704_time:epd_ch705_e:epd_ch705_time:epd_ch706_e:epd_ch706_time:epd_ch707_e:epd_ch707_time:epd_ch708_e:epd_ch708_time:epd_ch709_e:epd_ch709_time:epd_ch710_e:epd_ch710_time:epd_ch711_e:epd_ch711_time:epd_ch712_e:epd_ch712_time:epd_ch713_e:epd_ch713_time:epd_ch714_e:epd_ch714_time:epd_ch715_e:epd_ch715_time:epd_ch716_e:epd_ch716_time:epd_ch717_e:epd_ch717_time:epd_ch718_e:epd_ch718_time:epd_ch719_e:epd_ch719_time:epd_ch720_e:epd_ch720_time:epd_ch721_e:epd_ch721_time:epd_ch722_e:epd_ch722_time:epd_ch723_e:epd_ch723_time:epd_ch724_e:epd_ch724_time:epd_ch725_e:epd_ch725_time:epd_ch726_e:epd_ch726_time:epd_ch727_e:epd_ch727_time:epd_ch728_e:epd_ch728_time:epd_ch729_e:epd_ch729_time:epd_ch730_e:epd_ch730_time:epd_ch731_e:epd_ch731_time:epd_ch732_e:epd_ch732_time:epd_ch733_e:epd_ch733_time:epd_ch734_e:epd_ch734_time:epd_ch735_e:epd_ch735_time:epd_ch736_e:epd_ch736_time:epd_ch737_e:epd_ch737_time:epd_ch738_e:epd_ch738_time:epd_ch739_e:epd_ch739_time:epd_ch740_e:epd_ch740_time:epd_ch741_e:epd_ch741_time:epd_ch742_e:epd_ch742_time:epd_ch743_e:epd_ch743_time:epd_ch744_e:epd_ch744_time:epd_ch745_e:epd_ch745_time:epd_ch746_e:epd_ch746_time:epd_ch747_e:epd_ch747_time:epd_ch748_e:epd_ch748_time:epd_ch749_e:epd_ch749_time:epd_ch750_e:epd_ch750_time:epd_ch751_e:epd_ch751_time:epd_ch752_e:epd_ch752_time:epd_ch753_e:epd_ch753_time:epd_ch754_e:epd_ch754_time:epd_ch755_e:epd_ch755_time:epd_ch756_e:epd_ch756_time:epd_ch757_e:epd_ch757_time:epd_ch758_e:epd_ch758_time:epd_ch759_e:epd_ch759_time:epd_ch760_e:epd_ch760_time:epd_ch761_e:epd_ch761_time:epd_ch762_e:epd_ch762_time:epd_ch763_e:epd_ch763_time:epd_ch764_e:epd_ch764_time:epd_ch765_e:epd_ch765_time:epd_ch766_e:epd_ch766_time:epd_ch767_e:epd_ch767_time");
    
    hepdtime = new TH1D("hepdtime", "hepdtime", 13, -0.5, 12.5);

    mbd = new TNtuple("mbd","mbd", "mbd_ch0_e:mbd_ch0_time:mbd_ch1_e:mbd_ch1_time:mbd_ch2_e:mbd_ch2_time:mbd_ch3_e:mbd_ch3_time:mbd_ch4_e:mbd_ch4_time:mbd_ch5_e:mbd_ch5_time:mbd_ch6_e:mbd_ch6_time:mbd_ch7_e:mbd_ch7_time:mbd_ch8_e:mbd_ch8_time:mbd_ch9_e:mbd_ch9_time:mbd_ch10_e:mbd_ch10_time:mbd_ch11_e:mbd_ch11_time:mbd_ch12_e:mbd_ch12_time:mbd_ch13_e:mbd_ch13_time:mbd_ch14_e:mbd_ch14_time:mbd_ch15_e:mbd_ch15_time:mbd_ch16_e:mbd_ch16_time:mbd_ch17_e:mbd_ch17_time:mbd_ch18_e:mbd_ch18_time:mbd_ch19_e:mbd_ch19_time:mbd_ch20_e:mbd_ch20_time:mbd_ch21_e:mbd_ch21_time:mbd_ch22_e:mbd_ch22_time:mbd_ch23_e:mbd_ch23_time:mbd_ch24_e:mbd_ch24_time:mbd_ch25_e:mbd_ch25_time:mbd_ch26_e:mbd_ch26_time:mbd_ch27_e:mbd_ch27_time:mbd_ch28_e:mbd_ch28_time:mbd_ch29_e:mbd_ch29_time:mbd_ch30_e:mbd_ch30_time:mbd_ch31_e:mbd_ch31_time:mbd_ch32_e:mbd_ch32_time:mbd_ch33_e:mbd_ch33_time:mbd_ch34_e:mbd_ch34_time:mbd_ch35_e:mbd_ch35_time:mbd_ch36_e:mbd_ch36_time:mbd_ch37_e:mbd_ch37_time:mbd_ch38_e:mbd_ch38_time:mbd_ch39_e:mbd_ch39_time:mbd_ch40_e:mbd_ch40_time:mbd_ch41_e:mbd_ch41_time:mbd_ch42_e:mbd_ch42_time:mbd_ch43_e:mbd_ch43_time:mbd_ch44_e:mbd_ch44_time:mbd_ch45_e:mbd_ch45_time:mbd_ch46_e:mbd_ch46_time:mbd_ch47_e:mbd_ch47_time:mbd_ch48_e:mbd_ch48_time:mbd_ch49_e:mbd_ch49_time:mbd_ch50_e:mbd_ch50_time:mbd_ch51_e:mbd_ch51_time:mbd_ch52_e:mbd_ch52_time:mbd_ch53_e:mbd_ch53_time:mbd_ch54_e:mbd_ch54_time:mbd_ch55_e:mbd_ch55_time:mbd_ch56_e:mbd_ch56_time:mbd_ch57_e:mbd_ch57_time:mbd_ch58_e:mbd_ch58_time:mbd_ch59_e:mbd_ch59_time:mbd_ch60_e:mbd_ch60_time:mbd_ch61_e:mbd_ch61_time:mbd_ch62_e:mbd_ch62_time:mbd_ch63_e:mbd_ch63_time:mbd_ch64_e:mbd_ch64_time:mbd_ch65_e:mbd_ch65_time:mbd_ch66_e:mbd_ch66_time:mbd_ch67_e:mbd_ch67_time:mbd_ch68_e:mbd_ch68_time:mbd_ch69_e:mbd_ch69_time:mbd_ch70_e:mbd_ch70_time:mbd_ch71_e:mbd_ch71_time:mbd_ch72_e:mbd_ch72_time:mbd_ch73_e:mbd_ch73_time:mbd_ch74_e:mbd_ch74_time:mbd_ch75_e:mbd_ch75_time:mbd_ch76_e:mbd_ch76_time:mbd_ch77_e:mbd_ch77_time:mbd_ch78_e:mbd_ch78_time:mbd_ch79_e:mbd_ch79_time:mbd_ch80_e:mbd_ch80_time:mbd_ch81_e:mbd_ch81_time:mbd_ch82_e:mbd_ch82_time:mbd_ch83_e:mbd_ch83_time:mbd_ch84_e:mbd_ch84_time:mbd_ch85_e:mbd_ch85_time:mbd_ch86_e:mbd_ch86_time:mbd_ch87_e:mbd_ch87_time:mbd_ch88_e:mbd_ch88_time:mbd_ch89_e:mbd_ch89_time:mbd_ch90_e:mbd_ch90_time:mbd_ch91_e:mbd_ch91_time:mbd_ch92_e:mbd_ch92_time:mbd_ch93_e:mbd_ch93_time:mbd_ch94_e:mbd_ch94_time:mbd_ch95_e:mbd_ch95_time:mbd_ch96_e:mbd_ch96_time:mbd_ch97_e:mbd_ch97_time:mbd_ch98_e:mbd_ch98_time:mbd_ch99_e:mbd_ch99_time:mbd_ch100_e:mbd_ch100_time:mbd_ch101_e:mbd_ch101_time:mbd_ch102_e:mbd_ch102_time:mbd_ch103_e:mbd_ch103_time:mbd_ch104_e:mbd_ch104_time:mbd_ch105_e:mbd_ch105_time:mbd_ch106_e:mbd_ch106_time:mbd_ch107_e:mbd_ch107_time:mbd_ch108_e:mbd_ch108_time:mbd_ch109_e:mbd_ch109_time:mbd_ch110_e:mbd_ch110_time:mbd_ch111_e:mbd_ch111_time:mbd_ch112_e:mbd_ch112_time:mbd_ch113_e:mbd_ch113_time:mbd_ch114_e:mbd_ch114_time:mbd_ch115_e:mbd_ch115_time:mbd_ch116_e:mbd_ch116_time:mbd_ch117_e:mbd_ch117_time:mbd_ch118_e:mbd_ch118_time:mbd_ch119_e:mbd_ch119_time:mbd_ch120_e:mbd_ch120_time:mbd_ch121_e:mbd_ch121_time:mbd_ch122_e:mbd_ch122_time:mbd_ch123_e:mbd_ch123_time:mbd_ch124_e:mbd_ch124_time:mbd_ch125_e:mbd_ch125_time:mbd_ch126_e:mbd_ch126_time:mbd_ch127_e:mbd_ch127_time");

    hzvertex = new TH1D("hzvertex", "hzvertex", 500, -99.5, 99.5);
     
    coor = new TNtuple("coor", "coor", "epd_total_e:zdc_total_e:mbdq_ns:mbdq_n:mbdq_s");
    
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Detinfo::InitRun(PHCompositeNode *topNode)
{
  std::string sEPDchannelmapdir = "sEPD_mappingv1.root";
  if(!sEPDchannelmapdir.empty())
  {
    cdbttree = new CDBTTree(sEPDchannelmapdir);
  }
  else
  {
   std::cout<<" No mapping file found " << std::endl;
    exit(1);
  } 
   return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Detinfo::process_event(PHCompositeNode *topNode)
{

    _event++;
    
    
    TowerInfoContainer* towerinfosZDC;
    TowerInfoContainer* towerinfosEPD;
   
    
    GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
    if (!vertexmap)
    {
      std::cout << PHWHERE << " Fatal Error - GlobalVertexMap node is missing" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
    
    float thisvertex = -999.;

    if (vertexmap->empty())
    {
       thisvertex = 999.0;  // track events with empty vertices
    }
    else
    {
      GlobalVertex *vtx = vertexmap->begin()->second;
      if (vtx)
      {
          thisvertex = vtx->get_z();
      }
    }
   
 
    hzvertex->Fill(thisvertex);
  
    towerinfosZDC = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_ZDC");
    if(!towerinfosZDC)
    {
        std::cout << PHWHERE << ":: No TOWERS_ZDC!" << std::endl; exit(1);
    }
      
    towerinfosEPD = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_SEPD");
   
    if(!towerinfosEPD)
    {
        std::cout << PHWHERE << ":: No TOWERS_EPD!" << std::endl; exit(1);
    }

     MbdGeom *mbdgeom = findNode::getClass<MbdGeom>(topNode, "MbdGeom");
    if (!mbdgeom)
    {
      std::cout << PHWHERE << "::ERROR - cannot find MbdGeom" << std::endl;
      exit(-1);
    }

    MbdPmtContainer *mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
    if (!mbdpmts)
    {
        std::cout << PHWHERE << "::ERROR - cannot find MbdPmtContainer" << std::endl;
        exit(-1);
    }


    float totalepd = 0.;
    float totalzdc = 0.;


    _f.clear();
    
    int nchannels_zdc = towerinfosZDC->size();
    
    float zdc_e = 0.; float zdc_t = 0.;
      
    for (int channel = 0; channel < nchannels_zdc;channel++)
    {
       zdc_e = towerinfosZDC->get_tower_at_channel(channel)->get_energy();
       zdc_t = towerinfosZDC->get_tower_at_channel(channel)->get_time_float();
       hzdctime->Fill(zdc_t); //all channels

      _f.push_back(zdc_e);
      _f.push_back(zdc_t);
/*
       unsigned int key = TowerInfoDefs::encode_zdc(channel);
       if(TowerInfoDefs::isZDC(channel)) //get only zdc channels
       {
         if(TowerInfoDefs::get_zdc_side(key) == 0) //get zdc south channels
         if(TowerInfoDefs::get_zdc_side(key) == 1) //get zdc north channels
       }

      if(TowerInfoDefs::isSMD(channel)) //get only smd channels
       {
         if(TowerInfoDefs::get_smd_side(key) == 0) //get smd south channels
         if(TowerInfoDefs::get_smd_side(key) == 1) //get smd north channels
       }

       if(TowerInfoDefs::isVeto(channel)) //get only veto channels
       {
         if(TowerInfoDefs::get_veto_side(key) == 0) //get veto south channels
         if(TowerInfoDefs::get_veto_side(key) == 1) //get veto north channels
       }
       
     */
     
        
        if(channel == 0 || channel == 2 || channel == 4 || channel == 8 || channel == 10 || channel == 12)
        {
          if( (zdc_t > 4) &&  (zdc_t < 7))
          {
            totalzdc += zdc_e;
          }
        }
            

   }
    
    StoreZDCInfo(_f);
    _f.clear();

    _g.clear();

   unsigned int key = 999;
   std::vector < unsigned int> v;
   v.clear();

   //ok we are only doing this to allow for flexibility 
   //between 768 tile DST and 744 tile DST
   //Production makes 744 tile DST so mask channels with keymap 999
   // you don't have to use a vector, you can just print out the 
   //unsigned int keymap into a textfile and read it in if you wish instead 
   for(int i = 0; i < 768; i++)
   {
      int keymap = cdbttree->GetIntValue(i, "epd_channel_map");
      if(keymap == 999)
      {
        continue; //take out masked channels
      }
   
      key = TowerInfoDefs::encode_epd(keymap);
      v.push_back(key);
   }

 
    int nchannels_epd = towerinfosEPD->size();
    float epd_e = 0.; float epd_t = 0.;
    for (int channel = 0; channel < nchannels_epd;channel++)
    {
     //using epd mapping file 
      int arm = TowerInfoDefs::get_epd_arm(v[channel]); 
      int rbin = TowerInfoDefs::get_epd_rbin(v[channel]);
      int sectorid = TowerInfoDefs::get_epd_sector(v[channel]);
      int tileid = EPDDefs::get_tileid(v[channel]);
     
      epd_e = towerinfosEPD->get_tower_at_channel(channel)->get_energy();
      epd_t = towerinfosEPD->get_tower_at_channel(channel)->get_time_float();
      hepdtime->Fill(epd_t); //all channels
       _g.push_back(epd_e);
       _g.push_back(epd_t);

         if((epd_t > 5) &&  (epd_t < 7))
          {
           if(epd_e > 50 && epd_e < 3000)
           {
             totalepd += epd_e;
           }
          }
     }
  
     StoreEPDInfo(_g);
    _g.clear();

    _b.clear();

   
      float mbd_e_south = 0.;
      float mbd_e_north = 0.;
      float mbdQ = 0.;
      float t = 0.0;
    
      for (int ipmt = 0; ipmt < mbdpmts->get_npmt(); ipmt++)
      {

        float mbd_q = mbdpmts->get_pmt(ipmt)->get_q();
   
        _b.push_back(mbd_q);
        _b.push_back(t);
         mbdQ += mbd_q;

        int arm = mbdgeom->get_arm(ipmt);
          
        if (arm == 0)
         {
           mbd_e_south += mbd_q;
         }
          else if (arm == 1)
         {
           mbd_e_north += mbd_q;
         }
      }


    StoreMBDInfo(_b);
     
    event_info[0] = totalepd;
    event_info[1] = totalzdc;
    event_info[2] = mbdQ;
    event_info[3] = mbd_e_north;
    event_info[4] = mbd_e_south;

    coor->Fill(event_info);


    _b.clear();



    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Detinfo::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Detinfo::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Detinfo::End(PHCompositeNode *topNode)
{
    PHTFileServer::get().cd(_outfile_name);
    PHTFileServer::get().write(_outfile_name);

    return Fun4AllReturnCodes::EVENT_OK;

}

//____________________________________________________________________________..
int Detinfo::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}


double Detinfo::StoreZDCInfo(std::vector < float > _z)
{

    for (int i = 0; i < 32; i++)
    {
        ztower_info[i] = _z[i];
    }
        
    zdc->Fill(ztower_info);

    return 0;

}

double Detinfo::StoreEPDInfo(std::vector < float > _e)
{

    for (int i = 0; i < 1536; i++)
    {
        etower_info[i] = _e[i];
    }
        
    sepd->Fill(etower_info);

    return 0;

}

double Detinfo::StoreMBDInfo(std::vector < float > _m)
{

    for (int i = 0; i < 256; i++)
    {
        mtower_info[i] = _m[i];
    }
        
    mbd->Fill(mtower_info);

    return 0;

}
