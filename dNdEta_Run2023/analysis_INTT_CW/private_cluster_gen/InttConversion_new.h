#ifndef __INTTCONVERSION_H_
#define __INTTCONVERSION_H_

// #include <Eigen/Dense>
// #include <Eigen/Geometry>
// #include <Eigen/LU>
// #include <Eigen/SVD>
// #include <Math/Transform3D.h>

// #include <intt/InttMapping.h>
// R__LOAD_LIBRARY(libintt.so)

struct full_ladder_info {
    int FC;
    TString Port;
    int ROC;
    int Direction; // note : 0 : south, 1 : north 
    TString Ladder;
};
struct pos_str{
    
    double x;
    double y;
    double z;
    int layer;
    double phi;
};

struct pos_3D{
    float x;
    float y;
    float z;
};

class InttConversion {
    public :
        InttConversion(); // note : constructor
        InttConversion(string conversion_mode, double peek); // note : constructor
        pos_str Get_XY_all(string server, int module, int chip_id, int chan_id);
        string GetGeoMode();
        string GetLadderName(string server_module);

    private :

        string conversion_mode;
        double peek;
        pos_3D all_ch_pos[8][14][26][128];

        // TTree * survey_tree = new TTree();
        // Intt::RawData_s trial_channel; // note : chip seems to be from 0 to 25

        double layer_raduis[4] = {71.844000, 77.284000, 96.764000, 102.58400}; // note : unit : mm
        int N_ladder[4] = {12, 12, 16, 16};
        double angle_increment[4] = {30,30,22.5,22.5}; // note : ladder rotation
        double ladder_location_angle_ini_correction[4] = {255,270,258.75,270};
        double ladder_self_angle_ini_correction[4] = {345,360,348.75,360};
        double pos_ini_correction[4];
        double channel_width = 0.078; // note : unit : mm
        // note : It's different from the sPHENIX-INTT official conversion, but it's 
        double ideal_z_pos[13] = {
            221.75, 201.75, 181.75, 161.75, 141.75, // note : type B, column 1 to column 5
            121.55, 105.55, 89.55, 73.55, 57.55, 41.55, 25.55, 9.55 // note : type A, column 6 to column 13

            // 9.55, 25.55, 41.55, 57.55, 73.55, 89.55, 105.55, 121.55, // note : type A
            // 141.75, 161.75, 181.75, 201.75, 221.75 // note : type B
        };
        string ladder_index_string[16] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};

        vector<vector<pair<double,double>>> survey_ring_pos{
            {
                {-21.46096,-75.90736},
                {-55.11142,-56.18837},
                {-74.28391,-22.54561},
                {-74.27526,16.63011},
                {-55.28476,50.09862},
                {-21.29183,69.67282},
                {21.88624,70.05464},
                {55.62919,50.6206},
                {74.91651,16.92973},
                {75.05231,-21.87484},
                {55.65433,-55.67402},
                {22.03765,-75.04037}
            },


            {
                {-1.95208,-84.45033},
                {-42.53011,-73.45485},
                {-72.16084,-43.72407},
                {-82.95509,-3.14924},
                {-72.23989,37.60837},
                {-42.44876,67.26436},
                {2.51679,78.71859},
                {42.93029,67.67477},
                {72.5979,37.94146},
                {83.584,-2.404},
                {72.79182,-43.00581},
                {43.10364,-72.82611}
            },


            {
                {-21.31346,-101.64604},
                {-57.58322,-86.34445},
                {-85.04187,-58.59704},
                {-99.84728,-22.61833},
                {-99.77396,16.49293},
                {-84.90474,52.57002},
                {-57.61532,80.21115},
                {-21.37688,95.16837},
                {22.02317,95.28014},
                {58.19153,80.27542},
                {85.65315,52.7357},
                {100.52747,16.68818},
                {100.46573,-21.98187},
                {85.62499,-58.16364},
                {58.02216,-85.95094},
                {21.76647,-100.8559}
            },

            {
                {-1.73438,-109.56383},
                {-42.59149,-101.4651},
                {-77.22727,-78.31667},
                {-100.13177,-43.74884},
                {-108.23473,-3.11344},
                {-99.90249,37.60892},
                {-77.06972,72.28864},
                {-42.4645,95.04719},
                {2.69826,103.51905},
                {43.41251,95.69813},
                {77.88536,72.49284},
                {100.8459,38.06807},
                {108.91711,-2.60334},
                {100.82318,-43.04702},
                {78.07134,-77.57372},
                {43.22038,-100.8719}
            }
        };

        vector<vector<double>> survey_ladder_rotate{
            {
                // -0.26384,
                // -0.78815,
                // -1.30314,
                // 1.31438,
                // 0.80029,
                // 0.27082,
                // -0.25736,
                // -0.78393,
                // -1.30531,
                // 1.31406,
                // 0.79025,
                // 0.26877

                254.88308,
                224.84233,
                195.33558,
                165.30843,
                135.85324,
                105.51684,
                75.25436,
                45.08412,
                15.21125,
                -14.70991,
                -44.72201,
                -74.60061
            },

            {
                // -0.00218,
                // -0.51881,
                // -1.0441,
                // -1.5659,
                // 1.0518,
                // 0.52987,
                // -0.01209,
                // -0.52199,
                // -1.04294,
                // 1.56356, // note : remove the sign manually
                // 1.05152,
                // 0.52701

                269.87510,
                240.27438,
                210.17748,
                180.28054,
                150.26370,
                120.35931,
                89.30729,
                60.09218,
                30.24394,
                -0.41461,
                -29.75234,
                -59.80455
            },

            {
                // -0.19496,
                // -0.58817,
                // -0.98211,
                // -1.37107,
                // 1.37973,
                // 0.9788,
                // 0.59268,
                // 0.20298,
                // -0.18748,
                // -0.58648,
                // -0.97829,
                // -1.37224,
                // 1.37942,
                // 0.98354,
                // 0.59753,
                // 0.19973

                258.82961,
                236.30034,
                213.72924,
                191.44348,
                169.05271,
                146.08111,
                123.95806,
                101.62990,
                79.25819,
                56.39717,
                33.94811,
                11.37644,
                -10.96506,
                -33.64731,
                -55.76405,
                -78.55631
            },

            {
                // 0.01453,
                // -0.38568,
                // -0.78268,
                // -1.17174,
                // -1.57008,
                // 1.18513,
                // 0.79233,
                // 0.40179,
                // 0.01651,
                // -0.39829,
                // -0.78889,
                // -1.1741,
                // 1.56883,
                // 1.1828,
                // 0.79526,
                // 0.39805

                270.83251,
                247.90216,
                225.15574,
                202.86424,
                180.04104,
                157.90295,
                135.39716,
                113.02087,
                90.94595,
                67.17966,
                44.79993,
                22.72903,
                -0.11266,
                -22.23055,
                -44.43496,
                -67.19341
            }
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

        map<int,string>pid_to_server{
            {3001,"intt0"},
            {3002,"intt1"},
            {3003,"intt2"},
            {3004,"intt3"},
            {3005,"intt4"},
            {3006,"intt5"},
            {3007,"intt6"},
            {3008,"intt7"}
        };

        pair<double,double> Get_dummy_ladder_XY(int chip_id, int chan_id, int ladder_SN);
        pair<double,double> Get_self_rotation( pair<double,double> chan_pos, double angle);

        // map<TString,ladder_info> ladder_toinfo_map{    
        //     {"B1L101S", {0, "D2", 0, 0}}, // note : intt 0
        //     {"B0L101S", {1, "C1", 0, 0}},
        //     {"B1L001S", {2, "C2", 0, 0}},
        //     {"B1L000S", {3, "B3", 0, 0}},
        //     {"B1L100S", {4, "A2", 0, 0}},
        //     {"B0L000S", {5, "B1", 0, 0}},
        //     {"B0L100S", {6, "A1", 0, 0}},

        //     {"B1L103S", {7, "C2", 1, 0}},
        //     {"B0L002S", {8, "C1", 1, 0}},
        //     {"B0L001S", {9, "A1", 1, 0}},
        //     {"B1L002S", {10, "B3", 1, 0}},
        //     {"B1L102S", {11, "A2", 1, 0}},
        //     {"B0L102S", {12, "B1", 1, 0}},
        //     {"B1L003S", {13, "D2", 1, 0}},



        //     {"B1L105S", {0, "C2", 2, 0}},  // note : intt 1
        //     {"B0L104S", {1, "C1", 2, 0}},  
        //     {"B0L103S", {2, "A2", 2, 0}},  
        //     {"B1L004S", {3, "B3", 2, 0}},  
        //     {"B1L104S", {4, "A1", 2, 0}},  
        //     {"B0L003S", {5, "B1", 2, 0}},  
        //     {"B1L005S", {6, "D2", 2, 0}},  

        //     {"B1L107S", {7, "C2", 3, 0}},  
        //     {"B0L005S", {8, "C1", 3, 0}},  
        //     {"B0L004S", {9, "A1", 3, 0}},  
        //     {"B1L006S", {10, "B2", 3, 0}},  
        //     {"B1L106S", {11, "A2", 3, 0}},  
        //     {"B0L105S", {12, "B1", 3, 0}},  
        //     {"B1L007S", {13, "D1", 3, 0}},



        //     {"B0L106S", {0, "A1", 4, 0}},  // note : intt 2
        //     {"B0L006S", {1, "B1", 4, 0}},  
        //     {"B0L107S", {2, "C1", 4, 0}},  
        //     {"B1L108S", {3, "A2", 4, 0}},  
        //     {"B1L008S", {4, "B2", 4, 0}},  
        //     {"B1L109S", {5, "C2", 4, 0}},  
        //     {"B1L009S", {6, "D1", 4, 0}},  

        //     {"B0L007S", {7, "A1", 5, 0}},  
        //     {"B0L108S", {8, "B3", 5, 0}},  
        //     {"B0L008S", {9, "C1", 5, 0}},  
        //     {"B1L110S", {10, "A2", 5, 0}},  
        //     {"B1L010S", {11, "B2", 5, 0}},  
        //     {"B1L111S", {12, "C2", 5, 0}},  
        //     {"B1L011S", {13, "C3", 5, 0}},



        //     {"B0L109S", {0, "A1", 6, 0}},  // note : intt 3
        //     {"B0L009S", {1, "B1", 6, 0}},  
        //     {"B0L110S", {2, "C1", 6, 0}},  
        //     {"B1L112S", {3, "A2", 6, 0}},  
        //     {"B1L012S", {4, "B3", 6, 0}},  
        //     {"B1L113S", {5, "C2", 6, 0}},  
        //     {"B1L013S", {6, "D1", 6, 0}},  

        //     {"B0L010S", {7, "A1", 7, 0}},  
        //     {"B0L111S", {8, "B1", 7, 0}},  
        //     {"B0L011S", {9, "C1", 7, 0}},  
        //     {"B1L114S", {10, "A2", 7, 0}},  
        //     {"B1L014S", {11, "B3", 7, 0}},  
        //     {"B1L115S", {12, "C3", 7, 0}},  
        //     {"B1L015S", {13, "D2", 7, 0}},



        //     {"B1L101N", {0,	"B1", 0, 1}},	// note : intt 4
        //     {"B0L000N", {1,	"C2", 0, 1}},	
        //     {"B0L100N", {2,	"D1", 0, 1}},	
        //     {"B1L001N", {3,	"A2", 0, 1}},	
        //     {"B0L101N", {4,	"B2", 0, 1}},	
        //     {"B1L000N", {5,	"C3", 0, 1}},	
        //     {"B1L100N", {6,	"D2", 0, 1}},	 

        //     {"B0L002N", {7, "B1", 1, 1}},	 
        //     {"B0L102N", {8, "C2", 1, 1}},	
        //     {"B0L001N", {9, "D1", 1, 1}},	
        //     {"B1L003N", {10, "A2", 1, 1}},	 
        //     {"B1L103N", {11, "B2", 1, 1}},	
        //     {"B1L002N", {12, "C3", 1, 1}},	
        //     {"B1L102N", {13, "B3", 1, 1}},



        //     {"B0L003N", {0, "C1", 2, 1}},	// note : intt 5
        //     {"B0L104N", {1, "B1", 2, 1}},	
        //     {"B0L103N", {2, "D2", 2, 1}},	 
        //     {"B1L004N", {3, "D1", 2, 1}},	
        //     {"B1L005N", {4, "A2", 2, 1}},	 
        //     {"B1L104N", {5, "C2", 2, 1}},	
        //     {"B1L105N", {6, "A1", 2, 1}},	

        //     {"B1L107N", {7, "C3", 3, 1}},	
        //     {"B1L007N", {8, "B1", 3, 1}},	
        //     {"B1L006N", {9, "C1", 3, 1}},	
        //     {"B1L106N", {10, "D2", 3, 1}},	
        //     {"B0L005N", {11, "A1", 3, 1}},	
        //     {"B0L105N", {12, "C2", 3, 1}},	
        //     {"B0L004N", {13, "D1", 3, 1}},



        //     {"B0L106N", {0, "A1", 4, 1}},	// note : intt 6
        //     {"B0L006N", {1, "C3", 4, 1}},	
        //     {"B0L107N", {2, "B2", 4, 1}},	
        //     {"B1L108N", {3, "D1", 4, 1}},	
        //     {"B1L008N", {4, "C2", 4, 1}},	
        //     {"B1L109N", {5, "B3", 4, 1}},	
        //     {"B1L009N", {6, "A2", 4, 1}},	

        //     {"B0L007N", {7, "D1", 5, 1}},	
        //     {"B0L108N", {8, "C3", 5, 1}},	
        //     {"B0L008N", {9, "A2", 5, 1}},	
        //     {"B1L110N", {10, "D2", 5, 1}},	  
        //     {"B1L010N", {11, "C2", 5, 1}},	
        //     {"B1L111N", {12, "B3", 5, 1}},	
        //     {"B1L011N", {13, "A1", 5, 1}},  



        //     {"B0L109N", {0, "B3", 6, 1}},	// note : intt 7
        //     {"B0L009N", {1, "C1", 6, 1}},	
        //     {"B0L110N", {2, "B1", 6, 1}},	
        //     {"B1L112N", {3, "D1", 6, 1}},	
        //     {"B1L012N", {4, "C2", 6, 1}},	
        //     {"B1L113N", {5, "B2", 6, 1}},	
        //     {"B1L013N", {6, "A1", 6, 1}},	

        //     {"B0L010N", {7, "B2", 7, 1}},	
        //     {"B0L111N", {8, "C2", 7, 1}},	
        //     {"B0L011N", {9, "B1", 7, 1}},	  
        //     {"B1L114N", {10, "D2", 7, 1}},	  
        //     {"B1L014N", {11, "C3", 7, 1}},	
        //     {"B1L115N", {12, "B3", 7, 1}},	
        //     {"B1L015N", {13, "A1", 7, 1}}
        // };

};

InttConversion::InttConversion()
{}

InttConversion::InttConversion(string conversion_mode, double peek)
:conversion_mode(conversion_mode), peek(peek)
{
    // todo : check the file you use
    TFile * file_in = TFile::Open(Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/geo_folder/survey_data_all_3.32.root"));
    TTree * tree = (TTree *)file_in->Get("t_ch_pos");
    int pid;
    int module;
    int chip;
    int chan;
    double ch_x;
    double ch_y;
    double ch_z;
    double ch_phi;

    tree -> SetBranchAddress("pid",&pid);
    tree -> SetBranchAddress("module",&module);
    tree -> SetBranchAddress("chip",&chip);
    tree -> SetBranchAddress("chan",&chan);
    tree -> SetBranchAddress("x",&ch_x);
    tree -> SetBranchAddress("y",&ch_y);
    tree -> SetBranchAddress("z",&ch_z);
    tree -> SetBranchAddress("phi",&ch_phi);

    // pos_3D all_ch_pos[8][14][26][128];
    
    for (int i = 0; i < tree -> GetEntries(); i++){
        tree -> GetEntry(i);
        all_ch_pos[pid][module][chip-1][chan] = {float(ch_x),float(ch_y),float(ch_z)};
    }

    file_in -> Close();
}


pair<double,double> InttConversion::Get_dummy_ladder_XY(int chip_id, int chan_id, int ladder_SN)
{
    // note : chip_id 1 ~ 13 -> 0 
    // note : chip_id 14 ~ 26 -> 1
    int row_index = (chip_id > 0 && chip_id < 14) ? 0 : 1;
    double dummy_X = 0; 
    double dummy_Y;
    
    // note : north
    if (ladder_SN == 1) {
        dummy_Y = (row_index == 0) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;
    }
    // note : south
    else if (ladder_SN == 0){
        dummy_Y = (row_index == 1) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;
    }
    else{
        cout<<" wrong ladder_SN ID "<<endl;
        exit(1);
    }

    // note : original, only works for north
    // // note : chip_id 1 ~ 13, 14 ~ 26
    // int row_index = (chip_id > 0 && chip_id < 14) ? 0 : 1;

    // double dummy_X = 0;
    // double dummy_Y = (row_index == 0) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;


    return {dummy_X,dummy_Y};
}

pair<double,double> InttConversion::Get_self_rotation( pair<double,double> chan_pos, double angle)
{
    return {chan_pos.second * cos(angle / (180/TMath::Pi())), chan_pos.second * sin(angle / (180/TMath::Pi()))};
}

pos_str InttConversion::Get_XY_all(/*string ladder_name*/string server, int module, int chip_id, int chan_id)
{

    vector<string> conversion_mode_DB = {"ideal","nominal","survey_1_XY","survey_1_XYAlpha","survey_1_XYAlpha_Peek","full_survey_3.32"};

    // string ladder_pos_str = ladder_name;
    string ladder_pos_str = string(serverFC_toinfo_map[Form("%s_%i",server.c_str(),module)].Ladder);
    
    int layer_index; 
    if      (ladder_pos_str.substr(0,4) == "B0L0") layer_index = 0;
    else if (ladder_pos_str.substr(0,4) == "B0L1") layer_index = 1;
    else if (ladder_pos_str.substr(0,4) == "B1L0") layer_index = 2;
    else if (ladder_pos_str.substr(0,4) == "B1L1") layer_index = 3;
    else 
    {
        cout<<"error happens, wrong name"<<endl;
        exit(1);
    }

    int ladder_index = stoi(ladder_pos_str.substr(4,2));

    // note : 0, 1, 2, 3 : south, id 0
    // note : 4, 5, 6, 7 : north, id 1
    int ladder_SN = ( stoi(server.substr(4,1)) < 4 ) ? 0 : 1;
    // cout<<"test : ladder_SN : "<<ladder_SN<<endl;

    
    double ladder_location_angle = ladder_location_angle_ini_correction[layer_index] -  (angle_increment[layer_index] * ladder_index);

    double ring_pos_x = layer_raduis[layer_index] * cos(ladder_location_angle / (180/TMath::Pi()));
    double ring_pos_y = layer_raduis[layer_index] * sin(ladder_location_angle / (180/TMath::Pi()));

    double ladder_self_pos_X;
    double ladder_self_pos_Y;
    double final_pos_X;
    double final_pos_Y;
    double final_pos_Z;

    if (conversion_mode == conversion_mode_DB[0] || conversion_mode == conversion_mode_DB[1]) // note : ideal, nominal
    {
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second;
        final_pos_X = ring_pos_x + ladder_self_pos_X;
        final_pos_Y = ring_pos_y + ladder_self_pos_Y;
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if ( conversion_mode == conversion_mode_DB[2] ) // note : survey_1_XY (based on survey data typeB south)
    {
        // note : ideal ladder rotation
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second;
        
        // note : survey data ladder center position
        final_pos_X = survey_ring_pos[layer_index][ladder_index].first + ladder_self_pos_X;
        final_pos_Y = survey_ring_pos[layer_index][ladder_index].second + ladder_self_pos_Y;
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if ( conversion_mode == conversion_mode_DB[3] ) // note : survey_1_XYAlpha (based on survey data typeB south)
    {
        // note : here is an attempt, try to import the survey data, but only the center position of ladders

        // note : 
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second;
        
        // note : survey data ladder center position
        final_pos_X = survey_ring_pos[layer_index][ladder_index].first + ladder_self_pos_X;
        final_pos_Y = survey_ring_pos[layer_index][ladder_index].second + ladder_self_pos_Y;    
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if ( conversion_mode == conversion_mode_DB[4] ) // note : survey_1_XYAlpha_Peek
    {
        // note : 
        ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).first;
        ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id,ladder_SN),( survey_ladder_rotate[layer_index][ladder_index] + 90. )).second;
        
        double peek_correction_x = peek * cos( ( survey_ladder_rotate[layer_index][ladder_index] + 90. + 90.) / (180./M_PI) );
        double peek_correction_y = peek * sin( ( survey_ladder_rotate[layer_index][ladder_index] + 90. + 90.) / (180./M_PI) );
        // cout<<" test : "<<sqrt(pow(peek_correction_x,2)+pow(peek_correction_y,2))<<endl;
        // if (chip_id == 1)
        // { 
        //     if (chan_id == 0)
        //     {
        //         cout<<"test : "<<ladder_pos_str<<" "<<survey_ladder_rotate[layer_index][ladder_index] + 180.<<" "<<peek_correction_x<<" "<<peek_correction_y<<endl;
        //     }
        // }
        

        // note : survey data ladder center position
        final_pos_X = survey_ring_pos[layer_index][ladder_index].first + ladder_self_pos_X + peek_correction_x;
        final_pos_Y = survey_ring_pos[layer_index][ladder_index].second + ladder_self_pos_Y + peek_correction_y;    
        final_pos_Z = ideal_z_pos[(chip_id - 1) % 13] * pow(-1, 1 - ladder_SN);
    }
    else if (conversion_mode == conversion_mode_DB[5]) // note : full survey data with the 3.32 mm correction inward. 2023_10_11
    {
        // trial_channel.felix_server = server[4] - '0'; // note : from 0 to 7
        // trial_channel.felix_channel = module;
        // trial_channel.chip = chip_id - 1; // note : from 0 to 25
        // trial_channel.channel = chan_id;   // note : chan should be from 0 to 127

        // Eigen::Affine3d sensor_trans = Intt::GetTransform(survey_tree, ToOffline(trial_channel));
        // Eigen::Vector4d local_trans = Intt::GetLocalPos(ToOffline(trial_channel));
        // final_pos_X = (sensor_trans * local_trans)[0];
        // final_pos_Y = (sensor_trans * local_trans)[1];
        // final_pos_Z = (sensor_trans * local_trans)[2];

        pos_3D single_ch_pos = all_ch_pos[(server[4] - '0')][module][chip_id - 1][chan_id];

        final_pos_X = single_ch_pos.x;
        final_pos_Y = single_ch_pos.y;
        final_pos_Z = single_ch_pos.z;
    }
    else 
    {
        cout<<"InttConversion_new.h wrong mode input"<<endl;
        cout<<"you have filled : "<<conversion_mode<<endl;
        exit(1);
    }    

    int    final_pos_layer = (layer_index == 0 || layer_index == 1) ? 0 : 1;
    double final_pos_phi = (final_pos_Y < 0) ? atan2(final_pos_Y,final_pos_X) * (180./TMath::Pi()) + 360 : atan2(final_pos_Y,final_pos_X) * (180./TMath::Pi());

    return {final_pos_X,final_pos_Y,final_pos_Z,final_pos_layer,final_pos_phi};
}

string InttConversion::GetGeoMode()
{
    return conversion_mode;
}

string InttConversion::GetLadderName(string server_module)
{
    return (serverFC_toinfo_map[server_module].Ladder).Data();
}

#endif



// class InttClutering
// {
//     public:
//         InttClutering();
//         pair<double,double> Get_XY_all(string ladder_name,/*string server, int module,*/ int chip_id, int chan_id);
    
//     private: 
//         double layer_raduis[4] = {71.844000, 77.284000, 96.764000, 102.58400}; // note : unit : mm
//         int N_ladder[4] = {12, 12, 16, 16};
//         double angle_increment[4] = {30,30,22.5,22.5}; // note : ladder rotation
//         double ladder_location_angle_ini_correction[4] = {255,270,258.75,270};
//         double ladder_self_angle_ini_correction[4] = {345,360,348.75,360};
//         double pos_ini_correction[4];
//         double channel_width = 0.078; // note : unit : mm
//         string ladder_index_string[16] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};

//         map<TString,full_ladder_info> serverFC_toinfo_map{    
//             {"intt0_0", {0, "D2", 0, 0, "B1L101S"}}, // note : intt 0
//             {"intt0_1", {1, "C1", 0, 0, "B0L101S"}},
//             {"intt0_2", {2, "C2", 0, 0, "B1L001S"}},
//             {"intt0_3", {3, "B3", 0, 0, "B1L000S"}},
//             {"intt0_4", {4, "A2", 0, 0, "B1L100S"}},
//             {"intt0_5", {5, "B1", 0, 0, "B0L000S"}},
//             {"intt0_6", {6, "A1", 0, 0, "B0L100S"}},

//             {"intt0_7", {7, "C2", 1, 0, "B1L103S"}},
//             {"intt0_8", {8, "C1", 1, 0, "B0L002S"}},
//             {"intt0_9", {9, "A1", 1, 0, "B0L001S"}},
//             {"intt0_10", {10, "B3", 1, 0, "B1L002S"}},
//             {"intt0_11", {11, "A2", 1, 0, "B1L102S"}},
//             {"intt0_12", {12, "B1", 1, 0, "B0L102S"}},
//             {"intt0_13", {13, "D2", 1, 0, "B1L003S"}},



//             {"intt1_0", {0, "C2", 2, 0, "B1L105S"}},  // note : intt 1
//             {"intt1_1", {1, "C1", 2, 0, "B0L104S"}},  
//             {"intt1_2", {2, "A2", 2, 0, "B0L103S"}},  
//             {"intt1_3", {3, "B3", 2, 0, "B1L004S"}},  
//             {"intt1_4", {4, "A1", 2, 0, "B1L104S"}},  
//             {"intt1_5", {5, "B1", 2, 0, "B0L003S"}},  
//             {"intt1_6", {6, "D2", 2, 0, "B1L005S"}},  

//             {"intt1_7", {7, "C2", 3, 0, "B1L107S"}},  
//             {"intt1_8", {8, "C1", 3, 0, "B0L005S"}},  
//             {"intt1_9", {9, "A1", 3, 0, "B0L004S"}},  
//             {"intt1_10", {10, "B2", 3, 0, "B1L006S"}},  
//             {"intt1_11", {11, "A2", 3, 0, "B1L106S"}},  
//             {"intt1_12", {12, "B1", 3, 0, "B0L105S"}},  
//             {"intt1_13", {13, "D1", 3, 0, "B1L007S"}},



//             {"intt2_0", {0, "A1", 4, 0, "B0L106S"}},  // note : intt 2
//             {"intt2_1", {1, "B1", 4, 0, "B0L006S"}},  
//             {"intt2_2", {2, "C1", 4, 0, "B0L107S"}},  
//             {"intt2_3", {3, "A2", 4, 0, "B1L108S"}},  
//             {"intt2_4", {4, "B2", 4, 0, "B1L008S"}},  
//             {"intt2_5", {5, "C2", 4, 0, "B1L109S"}},  
//             {"intt2_6", {6, "D1", 4, 0, "B1L009S"}},  

//             {"intt2_7", {7, "A1", 5, 0, "B0L007S"}},  
//             {"intt2_8", {8, "B3", 5, 0, "B0L108S"}},  
//             {"intt2_9", {9, "C1", 5, 0, "B0L008S"}},  
//             {"intt2_10", {10, "A2", 5, 0, "B1L110S"}},  
//             {"intt2_11", {11, "B2", 5, 0, "B1L010S"}},  
//             {"intt2_12", {12, "C2", 5, 0, "B1L111S"}},  
//             {"intt2_13", {13, "C3", 5, 0, "B1L011S"}},



//             {"intt3_0", {0, "A1", 6, 0, "B0L109S"}},  // note : intt 3
//             {"intt3_1", {1, "B1", 6, 0, "B0L009S"}},  
//             {"intt3_2", {2, "C1", 6, 0, "B0L110S"}},  
//             {"intt3_3", {3, "A2", 6, 0, "B1L112S"}},  
//             {"intt3_4", {4, "B3", 6, 0, "B1L012S"}},  
//             {"intt3_5", {5, "C2", 6, 0, "B1L113S"}},  
//             {"intt3_6", {6, "D1", 6, 0, "B1L013S"}},  

//             {"intt3_7", {7, "A1", 7, 0, "B0L010S"}},  
//             {"intt3_8", {8, "B1", 7, 0, "B0L111S"}},  
//             {"intt3_9", {9, "C1", 7, 0, "B0L011S"}},  
//             {"intt3_10", {10, "A2", 7, 0, "B1L114S"}},  
//             {"intt3_11", {11, "B3", 7, 0, "B1L014S"}},  
//             {"intt3_12", {12, "C3", 7, 0, "B1L115S"}},  
//             {"intt3_13", {13, "D2", 7, 0, "B1L015S"}},



//             {"intt4_0", {0,	"B1", 0, 1, "B1L101N"}},	// note : intt 4
//             {"intt4_1", {1,	"C2", 0, 1, "B0L000N"}},	
//             {"intt4_2", {2,	"D1", 0, 1, "B0L100N"}},	
//             {"intt4_3", {3,	"A2", 0, 1, "B1L001N"}},	
//             {"intt4_4", {4,	"B2", 0, 1, "B0L101N"}},	
//             {"intt4_5", {5,	"C3", 0, 1, "B1L000N"}},	
//             {"intt4_6", {6,	"D2", 0, 1, "B1L100N"}},	 

//             {"intt4_7", {7, "B1", 1, 1, "B0L002N"}},	 
//             {"intt4_8", {8, "C2", 1, 1, "B0L102N"}},	
//             {"intt4_9", {9, "D1", 1, 1, "B0L001N"}},	
//             {"intt4_10", {10, "A2", 1, 1, "B1L003N"}},	 
//             {"intt4_11", {11, "B2", 1, 1, "B1L103N"}},	
//             {"intt4_12", {12, "C3", 1, 1, "B1L002N"}},	
//             {"intt4_13", {13, "B3", 1, 1, "B1L102N"}},



//             {"intt5_0", {0, "C1", 2, 1, "B0L003N"}},	// note : intt 5
//             {"intt5_1", {1, "B1", 2, 1, "B0L104N"}},	
//             {"intt5_2", {2, "D2", 2, 1, "B0L103N"}},	 
//             {"intt5_3", {3, "D1", 2, 1, "B1L004N"}},	
//             {"intt5_4", {4, "A2", 2, 1, "B1L005N"}},	 
//             {"intt5_5", {5, "C2", 2, 1, "B1L104N"}},	
//             {"intt5_6", {6, "A1", 2, 1, "B1L105N"}},	

//             {"intt5_7", {7, "C3", 3, 1, "B1L107N"}},	
//             {"intt5_8", {8, "B1", 3, 1, "B1L007N"}},	
//             {"intt5_9", {9, "C1", 3, 1, "B1L006N"}},	
//             {"intt5_10", {10, "D2", 3, 1, "B1L106N"}},	
//             {"intt5_11", {11, "A1", 3, 1, "B0L005N"}},	
//             {"intt5_12", {12, "C2", 3, 1, "B0L105N"}},	
//             {"intt5_13", {13, "D1", 3, 1, "B0L004N"}},



//             {"intt6_0", {0, "A1", 4, 1, "B0L106N"}},	// note : intt 6
//             {"intt6_1", {1, "C3", 4, 1, "B0L006N"}},	
//             {"intt6_2", {2, "B2", 4, 1, "B0L107N"}},	
//             {"intt6_3", {3, "D1", 4, 1, "B1L108N"}},	
//             {"intt6_4", {4, "C2", 4, 1, "B1L008N"}},	
//             {"intt6_5", {5, "B3", 4, 1, "B1L109N"}},	
//             {"intt6_6", {6, "A2", 4, 1, "B1L009N"}},	

//             {"intt6_7", {7, "D1", 5, 1, "B0L007N"}},	
//             {"intt6_8", {8, "C3", 5, 1, "B0L108N"}},	
//             {"intt6_9", {9, "A2", 5, 1, "B0L008N"}},	
//             {"intt6_10", {10, "D2", 5, 1, "B1L110N"}},	  
//             {"intt6_11", {11, "C2", 5, 1, "B1L010N"}},	
//             {"intt6_12", {12, "B3", 5, 1, "B1L111N"}},	
//             {"intt6_13", {13, "A1", 5, 1, "B1L011N"}},  



//             {"intt7_0", {0, "B3", 6, 1, "B0L109N"}},	// note : intt 7
//             {"intt7_1", {1, "C1", 6, 1, "B0L009N"}},	
//             {"intt7_2", {2, "B1", 6, 1, "B0L110N"}},	
//             {"intt7_3", {3, "D1", 6, 1, "B1L112N"}},	
//             {"intt7_4", {4, "C2", 6, 1, "B1L012N"}},	
//             {"intt7_5", {5, "B2", 6, 1, "B1L113N"}},	
//             {"intt7_6", {6, "A1", 6, 1, "B1L013N"}},	

//             {"intt7_7", {7, "B2", 7, 1, "B0L010N"}},	
//             {"intt7_8", {8, "C2", 7, 1, "B0L111N"}},	
//             {"intt7_9", {9, "B1", 7, 1, "B0L011N"}},	  
//             {"intt7_10", {10, "D2", 7, 1, "B1L114N"}},	  
//             {"intt7_11", {11, "C3", 7, 1, "B1L014N"}},	
//             {"intt7_12", {12, "B3", 7, 1, "B1L115N"}},	
//             {"intt7_13", {13, "A1", 7, 1, "B1L015N"}}
//         };

//         pair<double,double> Get_dummy_ladder_XY(int chip_id, int chan_id);
//         pair<double,double> Get_self_rotation( pair<double,double> chan_pos, double angle );
        
// };

// InttClutering::InttClutering()
// {

// }

// pair<double,double> InttClutering::Get_XY_all(string ladder_name,/*string server, int module,*/ int chip_id, int chan_id)
// {
//     string ladder_pos_str = ladder_name;
//     // string ladder_pos_str = string(serverFC_toinfo_map[Form("%s_%i",server.c_str(),module)].Ladder);
    
//     int layer_index; 
//     if      (ladder_pos_str.substr(0,4) == "B0L0") layer_index = 0;
//     else if (ladder_pos_str.substr(0,4) == "B0L1") layer_index = 1;
//     else if (ladder_pos_str.substr(0,4) == "B1L0") layer_index = 2;
//     else if (ladder_pos_str.substr(0,4) == "B1L1") layer_index = 3;

//     int ladder_index = stoi(ladder_pos_str.substr(4,2));
    
    
//     double ladder_location_angle = ladder_location_angle_ini_correction[layer_index] -  (angle_increment[layer_index] * ladder_index);

//     double ring_pos_x = layer_raduis[layer_index] * cos(ladder_location_angle / (180/TMath::Pi()));
//     double ring_pos_y = layer_raduis[layer_index] * sin(ladder_location_angle / (180/TMath::Pi()));

//     double ladder_self_pos_X = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).first;
//     double ladder_self_pos_Y = ( fabs(Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second) < 0.0000001) ? 0 : Get_self_rotation(Get_dummy_ladder_XY(chip_id,chan_id),( ladder_self_angle_ini_correction[layer_index] - (angle_increment[layer_index] * ladder_index) )).second;

//     return {ring_pos_x +  ladder_self_pos_X, ring_pos_y + ladder_self_pos_Y};
// }

// pair<double,double> InttClutering::Get_dummy_ladder_XY(int chip_id, int chan_id)
// {
//     // note : chip_id 1 ~ 13, 14 ~ 26
//     int row_index = (chip_id > 0 && chip_id < 14) ? 0 : 1;

//     double dummy_X = 0;
//     double dummy_Y = (row_index == 0) ? -0.039 - (127 - chan_id) * 0.078 : 0.039 + (127 - chan_id) * 0.078;

//     return {dummy_X,dummy_Y};
// }

// pair<double,double> InttClutering::Get_self_rotation( pair<double,double> chan_pos, double angle )
// {
//     return {chan_pos.second * cos(angle / (180/TMath::Pi())), chan_pos.second * sin(angle / (180/TMath::Pi()))};
// }