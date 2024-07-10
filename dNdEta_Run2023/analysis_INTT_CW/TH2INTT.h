#ifndef TH2INTT_h
#define TH2INTT_h

#include "TH2Poly.h"
#include "TLine.h"
#include "vector"
#include "map"
#include "TLatex.h"


struct ladder_info {
    int FC;
    TString Port;
    int ROC;
    int Direction; // note : 0 : south, 1 : north 
    int bin_id;
};

struct full_ladder_info {
    int FC;
    TString Port;
    int ROC;
    int Direction; // note : 0 : south, 1 : north 
    TString Ladder;
};

struct ladder_pos {
    double x1; 
    double y1;
    
    double x2; 
    double y2;
    
    double x3; 
    double y3;
    
    double x4; 
    double y4;
};

map<TString,ladder_info> ladder_toinfo_map{    
    {"B1L101S", {0, "D2", 0, 0}}, // note : intt 0
    {"B0L101S", {1, "C1", 0, 0}},
    {"B1L001S", {2, "C2", 0, 0}},
    {"B1L000S", {3, "B3", 0, 0}},
    {"B1L100S", {4, "A2", 0, 0}},
    {"B0L000S", {5, "B1", 0, 0}},
    {"B0L100S", {6, "A1", 0, 0}},

    {"B1L103S", {7, "C2", 1, 0}},
    {"B0L002S", {8, "C1", 1, 0}},
    {"B0L001S", {9, "A1", 1, 0}},
    {"B1L002S", {10, "B3", 1, 0}},
    {"B1L102S", {11, "A2", 1, 0}},
    {"B0L102S", {12, "B1", 1, 0}},
    {"B1L003S", {13, "D2", 1, 0}},



    {"B1L105S", {0, "C2", 2, 0}},  // note : intt 1
    {"B0L104S", {1, "C1", 2, 0}},  
    {"B0L103S", {2, "A2", 2, 0}},  
    {"B1L004S", {3, "B3", 2, 0}},  
    {"B1L104S", {4, "A1", 2, 0}},  
    {"B0L003S", {5, "B1", 2, 0}},  
    {"B1L005S", {6, "D2", 2, 0}},  

    {"B1L107S", {7, "C2", 3, 0}},  
    {"B0L005S", {8, "C1", 3, 0}},  
    {"B0L004S", {9, "A1", 3, 0}},  
    {"B1L006S", {10, "B2", 3, 0}},  
    {"B1L106S", {11, "A2", 3, 0}},  
    {"B0L105S", {12, "B1", 3, 0}},  
    {"B1L007S", {13, "D1", 3, 0}},



    {"B0L106S", {0, "A1", 4, 0}},  // note : intt 2
    {"B0L006S", {1, "B1", 4, 0}},  
    {"B0L107S", {2, "C1", 4, 0}},  
    {"B1L108S", {3, "A2", 4, 0}},  
    {"B1L008S", {4, "B2", 4, 0}},  
    {"B1L109S", {5, "C2", 4, 0}},  
    {"B1L009S", {6, "D1", 4, 0}},  

    {"B0L007S", {7, "A1", 5, 0}},  
    {"B0L108S", {8, "B3", 5, 0}},  
    {"B0L008S", {9, "C1", 5, 0}},  
    {"B1L110S", {10, "A2", 5, 0}},  
    {"B1L010S", {11, "B2", 5, 0}},  
    {"B1L111S", {12, "C2", 5, 0}},  
    {"B1L011S", {13, "C3", 5, 0}},



    {"B0L109S", {0, "A1", 6, 0}},  // note : intt 3
    {"B0L009S", {1, "B1", 6, 0}},  
    {"B0L110S", {2, "C1", 6, 0}},  
    {"B1L112S", {3, "A2", 6, 0}},  
    {"B1L012S", {4, "B3", 6, 0}},  
    {"B1L113S", {5, "C2", 6, 0}},  
    {"B1L013S", {6, "D1", 6, 0}},  

    {"B0L010S", {7, "A1", 7, 0}},  
    {"B0L111S", {8, "B1", 7, 0}},  
    {"B0L011S", {9, "C1", 7, 0}},  
    {"B1L114S", {10, "A2", 7, 0}},  
    {"B1L014S", {11, "B3", 7, 0}},  
    {"B1L115S", {12, "C3", 7, 0}},  
    {"B1L015S", {13, "D2", 7, 0}},



    {"B1L101N", {0,	"B1", 0, 1}},	// note : intt 4
    {"B0L000N", {1,	"C2", 0, 1}},	
    {"B0L100N", {2,	"D1", 0, 1}},	
    {"B1L001N", {3,	"A2", 0, 1}},	
    {"B0L101N", {4,	"B2", 0, 1}},	
    {"B1L000N", {5,	"C3", 0, 1}},	
    {"B1L100N", {6,	"D2", 0, 1}},	 

    {"B0L002N", {7, "B1", 1, 1}},	 
    {"B0L102N", {8, "C2", 1, 1}},	
    {"B0L001N", {9, "D1", 1, 1}},	
    {"B1L003N", {10, "A2", 1, 1}},	 
    {"B1L103N", {11, "B2", 1, 1}},	
    {"B1L002N", {12, "C3", 1, 1}},	
    {"B1L102N", {13, "B3", 1, 1}},



    {"B0L003N", {0, "C1", 2, 1}},	// note : intt 5
    {"B0L104N", {1, "B1", 2, 1}},	
    {"B0L103N", {2, "D2", 2, 1}},	 
    {"B1L004N", {3, "D1", 2, 1}},	
    {"B1L005N", {4, "A2", 2, 1}},	 
    {"B1L104N", {5, "C2", 2, 1}},	
    {"B1L105N", {6, "A1", 2, 1}},	

    {"B1L107N", {7, "C3", 3, 1}},	
    {"B1L007N", {8, "B1", 3, 1}},	
    {"B1L006N", {9, "C1", 3, 1}},	
    {"B1L106N", {10, "D2", 3, 1}},	
    {"B0L005N", {11, "A1", 3, 1}},	
    {"B0L105N", {12, "C2", 3, 1}},	
    {"B0L004N", {13, "D1", 3, 1}},



    {"B0L106N", {0, "A1", 4, 1}},	// note : intt 6
    {"B0L006N", {1, "C3", 4, 1}},	
    {"B0L107N", {2, "B2", 4, 1}},	
    {"B1L108N", {3, "D1", 4, 1}},	
    {"B1L008N", {4, "C2", 4, 1}},	
    {"B1L109N", {5, "B3", 4, 1}},	
    {"B1L009N", {6, "A2", 4, 1}},	

    {"B0L007N", {7, "D1", 5, 1}},	
    {"B0L108N", {8, "C3", 5, 1}},	
    {"B0L008N", {9, "A2", 5, 1}},	
    {"B1L110N", {10, "D2", 5, 1}},	  
    {"B1L010N", {11, "C2", 5, 1}},	
    {"B1L111N", {12, "B3", 5, 1}},	
    {"B1L011N", {13, "A1", 5, 1}},  



    {"B0L109N", {0, "B3", 6, 1}},	// note : intt 7
    {"B0L009N", {1, "C1", 6, 1}},	
    {"B0L110N", {2, "B1", 6, 1}},	
    {"B1L112N", {3, "D1", 6, 1}},	
    {"B1L012N", {4, "C2", 6, 1}},	
    {"B1L113N", {5, "B2", 6, 1}},	
    {"B1L013N", {6, "A1", 6, 1}},	

    {"B0L010N", {7, "B2", 7, 1}},	
    {"B0L111N", {8, "C2", 7, 1}},	
    {"B0L011N", {9, "B1", 7, 1}},	  
    {"B1L114N", {10, "D2", 7, 1}},	  
    {"B1L014N", {11, "C3", 7, 1}},	
    {"B1L115N", {12, "B3", 7, 1}},	
    {"B1L015N", {13, "A1", 7, 1}}
};

map<TString,full_ladder_info> serverFC_toinfo_map{    
    {"intt0_0", {0, "D2", 0, 0, "B1L101S"}}, // note : intt 0
    {"intt0_1", {1, "C1", 0, 0, "B0L101S"}},
    {"intt0_2", {2, "C2", 0, 0, "B1L001S"}},
    {"intt0_3", {3, "B3", 0, 0, "B1L000S"}},
    {"intt0_4", {4, "A2", 0, 0, "B1L100S"}},
    {"intt0_5", {5, "B1", 0, 0, "B0L000S"}},
    {"intt0_6", {6, "A1", 0, 0, "B0L100S"}},

    {"intt0_7", {7, "C2", 1, 0, "B1L103S"}},
    {"intt0_8", {8, "C1", 1, 0, "B0L002S"}},
    {"intt0_9", {9, "A1", 1, 0, "B0L001S"}},
    {"intt0_10", {10, "B3", 1, 0, "B1L002S"}},
    {"intt0_11", {11, "A2", 1, 0, "B1L102S"}},
    {"intt0_12", {12, "B1", 1, 0, "B0L102S"}},
    {"intt0_13", {13, "D2", 1, 0, "B1L003S"}},



    {"intt1_0", {0, "C2", 2, 0, "B1L105S"}},  // note : intt 1
    {"intt1_1", {1, "C1", 2, 0, "B0L104S"}},  
    {"intt1_2", {2, "A2", 2, 0, "B0L103S"}},  
    {"intt1_3", {3, "B3", 2, 0, "B1L004S"}},  
    {"intt1_4", {4, "A1", 2, 0, "B1L104S"}},  
    {"intt1_5", {5, "B1", 2, 0, "B0L003S"}},  
    {"intt1_6", {6, "D2", 2, 0, "B1L005S"}},  

    {"intt1_7", {7, "C2", 3, 0, "B1L107S"}},  
    {"intt1_8", {8, "C1", 3, 0, "B0L005S"}},  
    {"intt1_9", {9, "A1", 3, 0, "B0L004S"}},  
    {"intt1_10", {10, "B2", 3, 0, "B1L006S"}},  
    {"intt1_11", {11, "A2", 3, 0, "B1L106S"}},  
    {"intt1_12", {12, "B1", 3, 0, "B0L105S"}},  
    {"intt1_13", {13, "D1", 3, 0, "B1L007S"}},



    {"intt2_0", {0, "A1", 4, 0, "B0L106S"}},  // note : intt 2
    {"intt2_1", {1, "B1", 4, 0, "B0L006S"}},  
    {"intt2_2", {2, "C1", 4, 0, "B0L107S"}},  
    {"intt2_3", {3, "A2", 4, 0, "B1L108S"}},  
    {"intt2_4", {4, "B2", 4, 0, "B1L008S"}},  
    {"intt2_5", {5, "C2", 4, 0, "B1L109S"}},  
    {"intt2_6", {6, "D1", 4, 0, "B1L009S"}},  

    {"intt2_7", {7, "A1", 5, 0, "B0L007S"}},  
    {"intt2_8", {8, "B3", 5, 0, "B0L108S"}},  
    {"intt2_9", {9, "C1", 5, 0, "B0L008S"}},  
    {"intt2_10", {10, "A2", 5, 0, "B1L110S"}},  
    {"intt2_11", {11, "B2", 5, 0, "B1L010S"}},  
    {"intt2_12", {12, "C2", 5, 0, "B1L111S"}},  
    {"intt2_13", {13, "C3", 5, 0, "B1L011S"}},



    {"intt3_0", {0, "A1", 6, 0, "B0L109S"}},  // note : intt 3
    {"intt3_1", {1, "B1", 6, 0, "B0L009S"}},  
    {"intt3_2", {2, "C1", 6, 0, "B0L110S"}},  
    {"intt3_3", {3, "A2", 6, 0, "B1L112S"}},  
    {"intt3_4", {4, "B3", 6, 0, "B1L012S"}},  
    {"intt3_5", {5, "C2", 6, 0, "B1L113S"}},  
    {"intt3_6", {6, "D1", 6, 0, "B1L013S"}},  

    {"intt3_7", {7, "A1", 7, 0, "B0L010S"}},  
    {"intt3_8", {8, "B1", 7, 0, "B0L111S"}},  
    {"intt3_9", {9, "C1", 7, 0, "B0L011S"}},  
    {"intt3_10", {10, "A2", 7, 0, "B1L114S"}},  
    {"intt3_11", {11, "B3", 7, 0, "B1L014S"}},  
    {"intt3_12", {12, "C3", 7, 0, "B1L115S"}},  
    {"intt3_13", {13, "D2", 7, 0, "B1L015S"}},



    {"intt4_0", {0,	"B1", 0, 1, "B1L101N"}},	// note : intt 4
    {"intt4_1", {1,	"C2", 0, 1, "B0L000N"}},	
    {"intt4_2", {2,	"D1", 0, 1, "B0L100N"}},	
    {"intt4_3", {3,	"A2", 0, 1, "B1L001N"}},	
    {"intt4_4", {4,	"B2", 0, 1, "B0L101N"}},	
    {"intt4_5", {5,	"C3", 0, 1, "B1L000N"}},	
    {"intt4_6", {6,	"D2", 0, 1, "B1L100N"}},	 

    {"intt4_7", {7, "B1", 1, 1, "B0L002N"}},	 
    {"intt4_8", {8, "C2", 1, 1, "B0L102N"}},	
    {"intt4_9", {9, "D1", 1, 1, "B0L001N"}},	
    {"intt4_10", {10, "A2", 1, 1, "B1L003N"}},	 
    {"intt4_11", {11, "B2", 1, 1, "B1L103N"}},	
    {"intt4_12", {12, "C3", 1, 1, "B1L002N"}},	
    {"intt4_13", {13, "B3", 1, 1, "B1L102N"}},



    {"intt5_0", {0, "C1", 2, 1, "B0L003N"}},	// note : intt 5
    {"intt5_1", {1, "B1", 2, 1, "B0L104N"}},	
    {"intt5_2", {2, "D2", 2, 1, "B0L103N"}},	 
    {"intt5_3", {3, "D1", 2, 1, "B1L004N"}},	
    {"intt5_4", {4, "A2", 2, 1, "B1L005N"}},	 
    {"intt5_5", {5, "C2", 2, 1, "B1L104N"}},	
    {"intt5_6", {6, "A1", 2, 1, "B1L105N"}},	

    {"intt5_7", {7, "C3", 3, 1, "B1L107N"}},	
    {"intt5_8", {8, "B1", 3, 1, "B1L007N"}},	
    {"intt5_9", {9, "C1", 3, 1, "B1L006N"}},	
    {"intt5_10", {10, "D2", 3, 1, "B1L106N"}},	
    {"intt5_11", {11, "A1", 3, 1, "B0L005N"}},	
    {"intt5_12", {12, "C2", 3, 1, "B0L105N"}},	
    {"intt5_13", {13, "D1", 3, 1, "B0L004N"}},



    {"intt6_0", {0, "A1", 4, 1, "B0L106N"}},	// note : intt 6
    {"intt6_1", {1, "C3", 4, 1, "B0L006N"}},	
    {"intt6_2", {2, "B2", 4, 1, "B0L107N"}},	
    {"intt6_3", {3, "D1", 4, 1, "B1L108N"}},	
    {"intt6_4", {4, "C2", 4, 1, "B1L008N"}},	
    {"intt6_5", {5, "B3", 4, 1, "B1L109N"}},	
    {"intt6_6", {6, "A2", 4, 1, "B1L009N"}},	

    {"intt6_7", {7, "D1", 5, 1, "B0L007N"}},	
    {"intt6_8", {8, "C3", 5, 1, "B0L108N"}},	
    {"intt6_9", {9, "A2", 5, 1, "B0L008N"}},	
    {"intt6_10", {10, "D2", 5, 1, "B1L110N"}},	  
    {"intt6_11", {11, "C2", 5, 1, "B1L010N"}},	
    {"intt6_12", {12, "B3", 5, 1, "B1L111N"}},	
    {"intt6_13", {13, "A1", 5, 1, "B1L011N"}},  



    {"intt7_0", {0, "B3", 6, 1, "B0L109N"}},	// note : intt 7
    {"intt7_1", {1, "C1", 6, 1, "B0L009N"}},	
    {"intt7_2", {2, "B1", 6, 1, "B0L110N"}},	
    {"intt7_3", {3, "D1", 6, 1, "B1L112N"}},	
    {"intt7_4", {4, "C2", 6, 1, "B1L012N"}},	
    {"intt7_5", {5, "B2", 6, 1, "B1L113N"}},	
    {"intt7_6", {6, "A1", 6, 1, "B1L013N"}},	

    {"intt7_7", {7, "B2", 7, 1, "B0L010N"}},	
    {"intt7_8", {8, "C2", 7, 1, "B0L111N"}},	
    {"intt7_9", {9, "B1", 7, 1, "B0L011N"}},	  
    {"intt7_10", {10, "D2", 7, 1, "B1L114N"}},	  
    {"intt7_11", {11, "C3", 7, 1, "B1L014N"}},	
    {"intt7_12", {12, "B3", 7, 1, "B1L115N"}},	
    {"intt7_13", {13, "A1", 7, 1, "B1L015N"}}
};

map<int,TString> layer_map = {
    {0,"B0L0"},
    {1,"B0L1"},
    {2,"B1L0"},
    {3,"B1L1"}
};

class TH2INTT : public TH2Poly
{
    public :
        TH2INTT() : TH2Poly(){
            TH2Poly::Initialize(-23, 23., -10., 10., 25, 25);
            TH2Poly::SetStats(0);
            TH2INTT::fill_ladder_pos_map();
            TH2INTT::fill_ladder_line();
            TH2INTT::fill_ladder_toinfo_map_bin();

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B0L0S"].size(); i++) { 
                px={ladder_pos_map["B0L0S"][i].x1, ladder_pos_map["B0L0S"][i].x2, ladder_pos_map["B0L0S"][i].x3, ladder_pos_map["B0L0S"][i].x4};
                py={ladder_pos_map["B0L0S"][i].y1, ladder_pos_map["B0L0S"][i].y2, ladder_pos_map["B0L0S"][i].y3, ladder_pos_map["B0L0S"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B0L1S"].size(); i++) { 
                px={ladder_pos_map["B0L1S"][i].x1, ladder_pos_map["B0L1S"][i].x2, ladder_pos_map["B0L1S"][i].x3, ladder_pos_map["B0L1S"][i].x4};
                py={ladder_pos_map["B0L1S"][i].y1, ladder_pos_map["B0L1S"][i].y2, ladder_pos_map["B0L1S"][i].y3, ladder_pos_map["B0L1S"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B1L0S"].size(); i++) { 
                px={ladder_pos_map["B1L0S"][i].x1, ladder_pos_map["B1L0S"][i].x2, ladder_pos_map["B1L0S"][i].x3, ladder_pos_map["B1L0S"][i].x4};
                py={ladder_pos_map["B1L0S"][i].y1, ladder_pos_map["B1L0S"][i].y2, ladder_pos_map["B1L0S"][i].y3, ladder_pos_map["B1L0S"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B1L1S"].size(); i++) { 
                px={ladder_pos_map["B1L1S"][i].x1, ladder_pos_map["B1L1S"][i].x2, ladder_pos_map["B1L1S"][i].x3, ladder_pos_map["B1L1S"][i].x4};
                py={ladder_pos_map["B1L1S"][i].y1, ladder_pos_map["B1L1S"][i].y2, ladder_pos_map["B1L1S"][i].y3, ladder_pos_map["B1L1S"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }



            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B0L0N"].size(); i++) { 
                px={ladder_pos_map["B0L0N"][i].x1, ladder_pos_map["B0L0N"][i].x2, ladder_pos_map["B0L0N"][i].x3, ladder_pos_map["B0L0N"][i].x4};
                py={ladder_pos_map["B0L0N"][i].y1, ladder_pos_map["B0L0N"][i].y2, ladder_pos_map["B0L0N"][i].y3, ladder_pos_map["B0L0N"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B0L1N"].size(); i++) { 
                px={ladder_pos_map["B0L1N"][i].x1, ladder_pos_map["B0L1N"][i].x2, ladder_pos_map["B0L1N"][i].x3, ladder_pos_map["B0L1N"][i].x4};
                py={ladder_pos_map["B0L1N"][i].y1, ladder_pos_map["B0L1N"][i].y2, ladder_pos_map["B0L1N"][i].y3, ladder_pos_map["B0L1N"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B1L0N"].size(); i++) { 
                px={ladder_pos_map["B1L0N"][i].x1, ladder_pos_map["B1L0N"][i].x2, ladder_pos_map["B1L0N"][i].x3, ladder_pos_map["B1L0N"][i].x4};
                py={ladder_pos_map["B1L0N"][i].y1, ladder_pos_map["B1L0N"][i].y2, ladder_pos_map["B1L0N"][i].y3, ladder_pos_map["B1L0N"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }

            // note : set the bin shape
            for (int i = 0; i < ladder_pos_map["B1L1N"].size(); i++) { 
                px={ladder_pos_map["B1L1N"][i].x1, ladder_pos_map["B1L1N"][i].x2, ladder_pos_map["B1L1N"][i].x3, ladder_pos_map["B1L1N"][i].x4};
                py={ladder_pos_map["B1L1N"][i].y1, ladder_pos_map["B1L1N"][i].y2, ladder_pos_map["B1L1N"][i].y3, ladder_pos_map["B1L1N"][i].y4};
                TH2Poly::AddBin(4, &px[0], &py[0]); 
            }
            


        };
        
        void SetSerFCSContent(TString server_FC, double content); // note : TString
        double GetSerFCSContent(TString server_FC);
        void SetSerFCIContent(int server_id, int FC_id, double content); // note : int, server_id, FC_id
        double GetSerFCIContent(int server_id, int FC_id); // note : int, server_id, FC_id
        
        void SetLadderSContent(TString ladder_name, double content); // note : TString
        double GetLadderSContent(TString ladder_name);
        void SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content); // note : int, barrel_id, layer_id, ladder_id
        double GetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side); // note : int, barrel_id, layer_id, ladder_id

        virtual void Draw(Option_t* option = "") {
            TH2Poly::Draw(option); // note : Call the base class Draw() function

            TLatex *side_text = new TLatex();
            // side_text -> SetNDC();
            side_text -> SetTextSize(0.06);
            side_text -> SetTextAlign(21);

            double text_offset = 2.5;

            side_text -> DrawLatex(-10, 8, "South" );
            side_text -> DrawLatex(10, 8, "North" );

            TArrow *arx = new TArrow(-1.5,-8,1.5,-8,0.015,"|>");
            arx->SetAngle(40);
            arx->SetLineWidth(2);
            arx->Draw("");

            TArrow *ary = new TArrow(-1.5,-8,-1.5,-5,0.015,"|>");
            ary->SetAngle(40);
            ary->SetLineWidth(2);
            ary->Draw("");

            TLatex *coord_text = new TLatex();
            coord_text -> SetTextSize(0.05);
            coord_text -> SetTextAlign(21);
            coord_text -> DrawLatex(2, -8.5, "X");
            coord_text -> DrawLatex(-1.5, -4.5, "Y");
            coord_text -> DrawLatex(1, -6, "#odot Z");

            TLatex *note_text = new TLatex();
            note_text -> SetTextSize(0.035);
            note_text -> SetTextAlign(32);
            note_text -> DrawLatex(22, -9, "View from North to South");

            // side_text -> DrawLatex(0.285, 0.83, "South" );
            // side_text -> DrawLatex(0.64, 0.83, "North" );

            // note : Draw the line
            for (int i = 0; i < ladder_line.size(); i++)
            {
                ladder_line[i] -> Draw("lsame");
            }
        }


    private : 
        
        vector<TLine *> ladder_line;
        void fill_ladder_line();
        void fill_ladder_pos_map();
        void fill_ladder_toinfo_map_bin();

        vector<double> px;
        vector<double> py;
        
        TString index_word[16] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};

        // note : B0L0S, B0L1S, B1L0S, B1L1S
        // note : B0L0N, B0L1N, B1L0N, B1L1N
        map<TString,vector<ladder_pos>> ladder_pos_map;

        double south_x_offset = -10;
        double north_x_offset =  10;

        double B0L0_12_r = 10.2798/2.;
        double B0L0_point1_initial = 81;
        double B0L0_point2_initial = 69;

        double B0L0_34_r = 9.9152/2.;
        double B0L0_point3_initial = 64;
        double B0L0_point4_initial = 86;


        double B0L1_12_r = 11.028/2.;
        double B0L1_point1_initial = 174 + 90;
        double B0L1_point2_initial = 186 + 90;

        double B0L1_34_r = 10.6569/2.;
        double B0L1_point3_initial = 190 + 90;
        double B0L1_point4_initial = 170 + 90;



        double B1L0_correction = 22.5 + 3.75;
        double B1L0_12_r = 13.6355/2.;
        double B1L0_point1_initial = 160 + 90 + B1L0_correction;
        double B1L0_point2_initial = 170 + 90 + B1L0_correction;

        double B1L0_34_r = 13.2395/2.;
        double B1L0_point3_initial = 173 + 90 + B1L0_correction;
        double B1L0_point4_initial = 157 + 90 + B1L0_correction;


        double B1L1_correction = 3.5;
        double B1L1_12_r = 14.4022 /2.;
        double B1L1_point1_initial = 172 + 90 + B1L1_correction;
        double B1L1_point2_initial = 181 + 90 + B1L1_correction;

        double B1L1_34_r = 14.007 /2.;
        double B1L1_point3_initial = 184 + 90 + B1L1_correction;
        double B1L1_point4_initial = 168 + 90 + B1L1_correction;

};

void TH2INTT::fill_ladder_pos_map(){

    vector<ladder_pos> temp_vec; temp_vec.clear();
    
    // note : B0L0S
    for (int i = 0; i <12; i++)
    {
        temp_vec.push_back({
            B0L0_12_r * cos((B0L0_point1_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + south_x_offset, B0L0_12_r * sin((B0L0_point1_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())),
            B0L0_12_r * cos((B0L0_point2_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + south_x_offset, B0L0_12_r * sin((B0L0_point2_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())),
            B0L0_34_r * cos((B0L0_point3_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + south_x_offset, B0L0_34_r * sin((B0L0_point3_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())),
            B0L0_34_r * cos((B0L0_point4_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + south_x_offset, B0L0_34_r * sin((B0L0_point4_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B0L0S"] = temp_vec; temp_vec.clear();

    // note : B0L1S
    for (int i = 0; i <12; i++)
    {
        temp_vec.push_back({
            B0L1_12_r * cos((B0L1_point1_initial - i * 30) / (180./TMath::Pi())) + south_x_offset, B0L1_12_r * sin((B0L1_point1_initial - i * 30) / (180./TMath::Pi())),
            B0L1_12_r * cos((B0L1_point2_initial - i * 30) / (180./TMath::Pi())) + south_x_offset, B0L1_12_r * sin((B0L1_point2_initial - i * 30) / (180./TMath::Pi())),
            B0L1_34_r * cos((B0L1_point3_initial - i * 30) / (180./TMath::Pi())) + south_x_offset, B0L1_34_r * sin((B0L1_point3_initial - i * 30) / (180./TMath::Pi())),
            B0L1_34_r * cos((B0L1_point4_initial - i * 30) / (180./TMath::Pi())) + south_x_offset, B0L1_34_r * sin((B0L1_point4_initial - i * 30) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B0L1S"] = temp_vec; temp_vec.clear();

    // note : B1L0S
    for (int i = 0; i <16; i++)
    {
        temp_vec.push_back({
            B1L0_12_r * cos((B1L0_point1_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + south_x_offset, B1L0_12_r * sin((B1L0_point1_initial - i * 22.5 - 22.5) / (180./TMath::Pi())),
            B1L0_12_r * cos((B1L0_point2_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + south_x_offset, B1L0_12_r * sin((B1L0_point2_initial - i * 22.5 - 22.5) / (180./TMath::Pi())),
            B1L0_34_r * cos((B1L0_point3_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + south_x_offset, B1L0_34_r * sin((B1L0_point3_initial - i * 22.5 - 22.5) / (180./TMath::Pi())),
            B1L0_34_r * cos((B1L0_point4_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + south_x_offset, B1L0_34_r * sin((B1L0_point4_initial - i * 22.5 - 22.5) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B1L0S"] = temp_vec; temp_vec.clear();

    // note : B1L1S
    for (int i = 0; i <16; i++)
    {
        temp_vec.push_back({
            B1L1_12_r * cos((B1L1_point1_initial - i * 22.5) / (180./TMath::Pi())) + south_x_offset, B1L1_12_r * sin((B1L1_point1_initial - i * 22.5) / (180./TMath::Pi())),
            B1L1_12_r * cos((B1L1_point2_initial - i * 22.5) / (180./TMath::Pi())) + south_x_offset, B1L1_12_r * sin((B1L1_point2_initial - i * 22.5) / (180./TMath::Pi())),
            B1L1_34_r * cos((B1L1_point3_initial - i * 22.5) / (180./TMath::Pi())) + south_x_offset, B1L1_34_r * sin((B1L1_point3_initial - i * 22.5) / (180./TMath::Pi())),
            B1L1_34_r * cos((B1L1_point4_initial - i * 22.5) / (180./TMath::Pi())) + south_x_offset, B1L1_34_r * sin((B1L1_point4_initial - i * 22.5) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B1L1S"] = temp_vec; temp_vec.clear();

    



    // note : B0L0N
    for (int i = 0; i <12; i++)
    {
        temp_vec.push_back({
            B0L0_12_r * cos((B0L0_point1_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + north_x_offset, B0L0_12_r * sin((B0L0_point1_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())),
            B0L0_12_r * cos((B0L0_point2_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + north_x_offset, B0L0_12_r * sin((B0L0_point2_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())),
            B0L0_34_r * cos((B0L0_point3_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + north_x_offset, B0L0_34_r * sin((B0L0_point3_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())),
            B0L0_34_r * cos((B0L0_point4_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi())) + north_x_offset, B0L0_34_r * sin((B0L0_point4_initial - 5 * 30 - i * 30 - 30) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B0L0N"] = temp_vec; temp_vec.clear();

    // note : B0L1N
    for (int i = 0; i <12; i++)
    {
        temp_vec.push_back({
            B0L1_12_r * cos((B0L1_point1_initial - i * 30) / (180./TMath::Pi())) + north_x_offset, B0L1_12_r * sin((B0L1_point1_initial - i * 30) / (180./TMath::Pi())),
            B0L1_12_r * cos((B0L1_point2_initial - i * 30) / (180./TMath::Pi())) + north_x_offset, B0L1_12_r * sin((B0L1_point2_initial - i * 30) / (180./TMath::Pi())),
            B0L1_34_r * cos((B0L1_point3_initial - i * 30) / (180./TMath::Pi())) + north_x_offset, B0L1_34_r * sin((B0L1_point3_initial - i * 30) / (180./TMath::Pi())),
            B0L1_34_r * cos((B0L1_point4_initial - i * 30) / (180./TMath::Pi())) + north_x_offset, B0L1_34_r * sin((B0L1_point4_initial - i * 30) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B0L1N"] = temp_vec; temp_vec.clear();

    // note : B1L0N
    for (int i = 0; i <16; i++)
    {
        temp_vec.push_back({
            B1L0_12_r * cos((B1L0_point1_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + north_x_offset, B1L0_12_r * sin((B1L0_point1_initial - i * 22.5 - 22.5) / (180./TMath::Pi())),
            B1L0_12_r * cos((B1L0_point2_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + north_x_offset, B1L0_12_r * sin((B1L0_point2_initial - i * 22.5 - 22.5) / (180./TMath::Pi())),
            B1L0_34_r * cos((B1L0_point3_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + north_x_offset, B1L0_34_r * sin((B1L0_point3_initial - i * 22.5 - 22.5) / (180./TMath::Pi())),
            B1L0_34_r * cos((B1L0_point4_initial - i * 22.5 - 22.5) / (180./TMath::Pi())) + north_x_offset, B1L0_34_r * sin((B1L0_point4_initial - i * 22.5 - 22.5) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B1L0N"] = temp_vec; temp_vec.clear();

    // note : B1L1N
    for (int i = 0; i <16; i++)
    {
        temp_vec.push_back({
            B1L1_12_r * cos((B1L1_point1_initial - i * 22.5) / (180./TMath::Pi())) + north_x_offset, B1L1_12_r * sin((B1L1_point1_initial - i * 22.5) / (180./TMath::Pi())),
            B1L1_12_r * cos((B1L1_point2_initial - i * 22.5) / (180./TMath::Pi())) + north_x_offset, B1L1_12_r * sin((B1L1_point2_initial - i * 22.5) / (180./TMath::Pi())),
            B1L1_34_r * cos((B1L1_point3_initial - i * 22.5) / (180./TMath::Pi())) + north_x_offset, B1L1_34_r * sin((B1L1_point3_initial - i * 22.5) / (180./TMath::Pi())),
            B1L1_34_r * cos((B1L1_point4_initial - i * 22.5) / (180./TMath::Pi())) + north_x_offset, B1L1_34_r * sin((B1L1_point4_initial - i * 22.5) / (180./TMath::Pi()))
        });
    }
    ladder_pos_map["B1L1N"] = temp_vec; temp_vec.clear();
    
}

void TH2INTT::fill_ladder_line(){


    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B0L0S"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0S"][i].x1, ladder_pos_map["B0L0S"][i].y1, ladder_pos_map["B0L0S"][i].x2, ladder_pos_map["B0L0S"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0S"][i].x2, ladder_pos_map["B0L0S"][i].y2, ladder_pos_map["B0L0S"][i].x3, ladder_pos_map["B0L0S"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0S"][i].x3, ladder_pos_map["B0L0S"][i].y3, ladder_pos_map["B0L0S"][i].x4, ladder_pos_map["B0L0S"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0S"][i].x4, ladder_pos_map["B0L0S"][i].y4, ladder_pos_map["B0L0S"][i].x1, ladder_pos_map["B0L0S"][i].y1 ));
    }

    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B0L1S"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1S"][i].x1, ladder_pos_map["B0L1S"][i].y1, ladder_pos_map["B0L1S"][i].x2, ladder_pos_map["B0L1S"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1S"][i].x2, ladder_pos_map["B0L1S"][i].y2, ladder_pos_map["B0L1S"][i].x3, ladder_pos_map["B0L1S"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1S"][i].x3, ladder_pos_map["B0L1S"][i].y3, ladder_pos_map["B0L1S"][i].x4, ladder_pos_map["B0L1S"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1S"][i].x4, ladder_pos_map["B0L1S"][i].y4, ladder_pos_map["B0L1S"][i].x1, ladder_pos_map["B0L1S"][i].y1 )); 
    }

    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B1L0S"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0S"][i].x1, ladder_pos_map["B1L0S"][i].y1, ladder_pos_map["B1L0S"][i].x2, ladder_pos_map["B1L0S"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0S"][i].x2, ladder_pos_map["B1L0S"][i].y2, ladder_pos_map["B1L0S"][i].x3, ladder_pos_map["B1L0S"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0S"][i].x3, ladder_pos_map["B1L0S"][i].y3, ladder_pos_map["B1L0S"][i].x4, ladder_pos_map["B1L0S"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0S"][i].x4, ladder_pos_map["B1L0S"][i].y4, ladder_pos_map["B1L0S"][i].x1, ladder_pos_map["B1L0S"][i].y1 )); 
    }

    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B1L1S"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1S"][i].x1, ladder_pos_map["B1L1S"][i].y1, ladder_pos_map["B1L1S"][i].x2, ladder_pos_map["B1L1S"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1S"][i].x2, ladder_pos_map["B1L1S"][i].y2, ladder_pos_map["B1L1S"][i].x3, ladder_pos_map["B1L1S"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1S"][i].x3, ladder_pos_map["B1L1S"][i].y3, ladder_pos_map["B1L1S"][i].x4, ladder_pos_map["B1L1S"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1S"][i].x4, ladder_pos_map["B1L1S"][i].y4, ladder_pos_map["B1L1S"][i].x1, ladder_pos_map["B1L1S"][i].y1 )); 
    }



    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B0L0N"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0N"][i].x1, ladder_pos_map["B0L0N"][i].y1, ladder_pos_map["B0L0N"][i].x2, ladder_pos_map["B0L0N"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0N"][i].x2, ladder_pos_map["B0L0N"][i].y2, ladder_pos_map["B0L0N"][i].x3, ladder_pos_map["B0L0N"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0N"][i].x3, ladder_pos_map["B0L0N"][i].y3, ladder_pos_map["B0L0N"][i].x4, ladder_pos_map["B0L0N"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L0N"][i].x4, ladder_pos_map["B0L0N"][i].y4, ladder_pos_map["B0L0N"][i].x1, ladder_pos_map["B0L0N"][i].y1 )); 
    }

    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B0L1N"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1N"][i].x1, ladder_pos_map["B0L1N"][i].y1, ladder_pos_map["B0L1N"][i].x2, ladder_pos_map["B0L1N"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1N"][i].x2, ladder_pos_map["B0L1N"][i].y2, ladder_pos_map["B0L1N"][i].x3, ladder_pos_map["B0L1N"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1N"][i].x3, ladder_pos_map["B0L1N"][i].y3, ladder_pos_map["B0L1N"][i].x4, ladder_pos_map["B0L1N"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B0L1N"][i].x4, ladder_pos_map["B0L1N"][i].y4, ladder_pos_map["B0L1N"][i].x1, ladder_pos_map["B0L1N"][i].y1 ));  
    }

    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B1L0N"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0N"][i].x1, ladder_pos_map["B1L0N"][i].y1, ladder_pos_map["B1L0N"][i].x2, ladder_pos_map["B1L0N"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0N"][i].x2, ladder_pos_map["B1L0N"][i].y2, ladder_pos_map["B1L0N"][i].x3, ladder_pos_map["B1L0N"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0N"][i].x3, ladder_pos_map["B1L0N"][i].y3, ladder_pos_map["B1L0N"][i].x4, ladder_pos_map["B1L0N"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L0N"][i].x4, ladder_pos_map["B1L0N"][i].y4, ladder_pos_map["B1L0N"][i].x1, ladder_pos_map["B1L0N"][i].y1 )); 
    }

    // note : fill the bin line, to show the shape
    for (int i = 0; i < ladder_pos_map["B1L1N"].size(); i++) { 
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1N"][i].x1, ladder_pos_map["B1L1N"][i].y1, ladder_pos_map["B1L1N"][i].x2, ladder_pos_map["B1L1N"][i].y2 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1N"][i].x2, ladder_pos_map["B1L1N"][i].y2, ladder_pos_map["B1L1N"][i].x3, ladder_pos_map["B1L1N"][i].y3 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1N"][i].x3, ladder_pos_map["B1L1N"][i].y3, ladder_pos_map["B1L1N"][i].x4, ladder_pos_map["B1L1N"][i].y4 ));
        ladder_line.push_back(new TLine(ladder_pos_map["B1L1N"][i].x4, ladder_pos_map["B1L1N"][i].y4, ladder_pos_map["B1L1N"][i].x1, ladder_pos_map["B1L1N"][i].y1 )); 
    }


}

void TH2INTT::fill_ladder_toinfo_map_bin(){

    TString side_word;
    int bin_index = 1;

    for (int arm = 0; arm < 2; arm++) // note : arm, 0 for south, 1 for north
    {
        side_word = (arm == 0) ? "S" : "N";

        for (int layer = 0; layer < 4; layer++) // note : layer
        {   
            int n_ladder = (layer < 2) ? 12 : 16;

            for (int HL = 0; HL < n_ladder; HL++)
            {
                ladder_toinfo_map[ Form("%s%s%s",layer_map.at(layer).Data(),index_word[HL].Data(),side_word.Data()) ].bin_id = bin_index;

                // cout<<Form("%s%s%s",layer_map.at(layer).Data(),index_word[HL].Data(),side_word.Data())<<" "<<bin_index<<endl;

                bin_index += 1;
            }

            // cout<<" "<<endl;

        }
        // cout<<" "<<endl;
    }
    
}

void TH2INTT::SetLadderSContent(TString ladder_name, double content)
{
    TH2Poly::SetBinContent(ladder_toinfo_map.at(ladder_name).bin_id, content);   
}

void TH2INTT::SetSerFCSContent(TString server_FC, double content)
{
    TH2Poly::SetBinContent(ladder_toinfo_map.at( serverFC_toinfo_map.at(server_FC).Ladder ).bin_id, content);
}

void TH2INTT::SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content)
{
    if (side != 0 && side != 1) {
        cout<<"wrong side fill"<<endl;
        return;
    }

    TString side_word = (side == 0) ? "S" : "N";
    // cout<<Form("B%iL%i%s%s",barrel_id,ladder_id,index_word[ladder_id].Data(),side_word.Data())<<endl;
    TH2Poly::SetBinContent(ladder_toinfo_map.at(Form("B%iL%i%s%s",barrel_id,layer_id,index_word[ladder_id].Data(),side_word.Data())).bin_id, content);   
}

void TH2INTT::SetSerFCIContent(int server_id, int FC_id, double content)
{
    TH2Poly::SetBinContent(ladder_toinfo_map.at( serverFC_toinfo_map.at(Form("intt%i_%i",server_id,FC_id)).Ladder ).bin_id, content);
}



double TH2INTT::GetLadderSContent(TString ladder_name)
{
    return TH2Poly::GetBinContent(ladder_toinfo_map.at(ladder_name).bin_id);   
}

double TH2INTT::GetSerFCSContent(TString server_FC)
{
    return TH2Poly::GetBinContent(ladder_toinfo_map.at( serverFC_toinfo_map.at(server_FC).Ladder ).bin_id);
}

double TH2INTT::GetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side)
{
    if (side != 0 && side != 1) {
        cout<<"wrong side fill"<<endl;
        return 0;
    }

    TString side_word = (side == 0) ? "S" : "N";
    return TH2Poly::GetBinContent(ladder_toinfo_map.at(Form("B%iL%i%s%s",barrel_id,layer_id,index_word[ladder_id].Data(),side_word.Data())).bin_id);   
}

double TH2INTT::GetSerFCIContent(int server_id, int FC_id)
{
    return TH2Poly::GetBinContent(ladder_toinfo_map.at( serverFC_toinfo_map.at(Form("intt%i_%i",server_id,FC_id)).Ladder ).bin_id);
}

#endif