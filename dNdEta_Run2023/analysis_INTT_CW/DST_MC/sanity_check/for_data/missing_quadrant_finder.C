#include "sPhenixStyle.C"
#include "../../ReadF4ANtupleData/ReadF4ANtupleData.C"

vector<double> findSmallestAndRatio(double a, double b, double c, double d) {
    // Put the values in a vector
    std::vector<double> values = {a, b, c, d};

    // Find the minimum value and its index
    auto minElementIt = std::min_element(values.begin(), values.end());
    double minValue = *minElementIt;
    int minIndex = std::distance(values.begin(), minElementIt);

    // Calculate the sum of the values
    double sum = a + b + c + d;

    // Calculate the sum of the other three values by subtracting the minValue
    double sumOfOthers = sum - minValue;

    // Calculate the average of the other three values
    double avgOfOthers = sumOfOthers / 3.0;

    // Calculate the ratio of the min value to the average of the other three
    double ratio = minValue / avgOfOthers;

    return {double(minIndex), ratio};
}

void missing_quadrant_finder()
{
    string input_file_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root";
    string input_tree_name = "EventTree";
    string output_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/sanity_check/with_cut";

    map<string,int> offline_quadrant_map = {
        {"3_1",1 - 1},
        {"3_2",1 - 1},
        {"3_3",1 - 1},

        {"4_1",1 - 1},
        {"4_2",1 - 1},
        {"4_3",1 - 1},

        {"5_1",1 - 1},
        {"5_2",1 - 1},
        {"5_3",1 - 1},
        {"5_4",1 - 1},

        {"6_1",1 - 1},
        {"6_2",1 - 1},
        {"6_3",1 - 1},
        {"6_4",1 - 1},



        {"3_4",2 - 1},
        {"3_5",2 - 1},
        {"3_6",2 - 1},

        {"4_4",2 - 1},
        {"4_5",2 - 1},
        {"4_6",2 - 1},

        {"5_5",2 - 1},
        {"5_6",2 - 1},
        {"5_7",2 - 1},
        {"5_8",2 - 1},

        {"6_5",2 - 1},
        {"6_6",2 - 1},
        {"6_7",2 - 1},
        {"6_8",2 - 1},



        {"3_7",3 - 1},
        {"3_8",3 - 1},
        {"3_9",3 - 1},

        {"4_7",3 - 1},
        {"4_8",3 - 1},
        {"4_9",3 - 1},

        {"5_9",3 - 1},
        {"5_10",3 - 1},
        {"5_11",3 - 1},
        {"5_12",3 - 1},

        {"6_9",3 - 1},
        {"6_10",3 - 1},
        {"6_11",3 - 1},
        {"6_12",3 - 1},



        {"3_0",4 - 1},
        {"3_10",4 - 1},
        {"3_11",4 - 1},

        {"4_0",4 - 1},
        {"4_10",4 - 1},
        {"4_11",4 - 1},

        {"5_0",4 - 1},
        {"5_13",4 - 1},
        {"5_14",4 - 1},
        {"5_15",4 - 1},

        {"6_0",4 - 1},
        {"6_13",4 - 1},
        {"6_14",4 - 1},
        {"6_15",4 - 1}
    };

    TFile * file_in = TFile::Open(input_file_directory.c_str());
    TTree * tree_in = (TTree*)file_in->Get(input_tree_name.c_str());
    ReadF4ANtupleData * inttDSTData = new ReadF4ANtupleData(tree_in);

    int evt_NClus_Q0 = 0;
    int evt_NClus_Q1 = 0;
    int evt_NClus_Q2 = 0;
    int evt_NClus_Q3 = 0;

    TH2F * quadrant_check_0 = new TH2F("","quadrant_check_0;X axis [cm];Y axis [cm]",300,-15,15,300,-15,15);
    TH2F * quadrant_check_1 = new TH2F("","quadrant_check_1;X axis [cm];Y axis [cm]",300,-15,15,300,-15,15);
    TH2F * quadrant_check_2 = new TH2F("","quadrant_check_2;X axis [cm];Y axis [cm]",300,-15,15,300,-15,15);
    TH2F * quadrant_check_3 = new TH2F("","quadrant_check_3;X axis [cm];Y axis [cm]",300,-15,15,300,-15,15);

    TH1F * less_quadrant_dist = new TH1F("","less_quadrant_dist;quadrant;Entry",4,0,4);
    TH1F * less_ratio_dist = new TH1F("","less_ratio_dist;Ratio;Entry",100,0,1);

    TH2F * NClus_less_ratio_2D_dist = new TH2F("","NClus_less_ratio_2D_dist;NClus;Ratio",200,0,8500,200,0,1);

    TH2F * NClus_less_ratio_2D_dist_0 = new TH2F("","NClus_less_ratio_2D_dist_0;NClus;Ratio",200,0,8500,200,0,1);
    TH2F * NClus_less_ratio_2D_dist_1 = new TH2F("","NClus_less_ratio_2D_dist_1;NClus;Ratio",200,0,8500,200,0,1);
    TH2F * NClus_less_ratio_2D_dist_2 = new TH2F("","NClus_less_ratio_2D_dist_2;NClus;Ratio",200,0,8500,200,0,1);
    TH2F * NClus_less_ratio_2D_dist_3 = new TH2F("","NClus_less_ratio_2D_dist_3;NClus;Ratio",200,0,8500,200,0,1);

    for (int i = 0; i < 100000/*tree_in -> GetEntries()*/; i++)
    {
        inttDSTData -> LoadTree(i);
        inttDSTData -> GetEntry(i);

        if (i % 2000 == 0) {cout<<i<<endl;}

        if (inttDSTData -> is_min_bias_wozdc == 0) { continue; }
        if (inttDSTData -> MBD_centrality != inttDSTData -> MBD_centrality) { continue; }
        if (inttDSTData -> MBD_z_vtx != inttDSTData -> MBD_z_vtx) { continue; }
        if (inttDSTData -> MBD_z_vtx < -50 || inttDSTData -> MBD_z_vtx > 10 ) { continue; }

        for (int clu_i = 0; clu_i < inttDSTData->NClus; clu_i++)
        {
            int clus_id = offline_quadrant_map[Form("%i_%i",inttDSTData->ClusLayer->at(clu_i), inttDSTData->ClusLadderPhiId->at(clu_i))];
            double CluX = inttDSTData->ClusX->at(clu_i);
            double CluY = inttDSTData->ClusY->at(clu_i);


            if (clus_id == 0) {
                evt_NClus_Q0 += 1;
                quadrant_check_0 -> Fill(CluX, CluY);
            } 

            else if (clus_id == 1 ) {
                evt_NClus_Q1 += 1;
                quadrant_check_1 -> Fill(CluX, CluY);
            }

            else if (clus_id == 2 ) {
                evt_NClus_Q2 += 1;
                quadrant_check_2 -> Fill(CluX, CluY);
            }

            else if (clus_id == 3 ) {
                evt_NClus_Q3 += 1;
                quadrant_check_3 -> Fill(CluX, CluY);
            } 
        }

        vector<double> less_quadrant = findSmallestAndRatio(evt_NClus_Q0, evt_NClus_Q1, evt_NClus_Q2, evt_NClus_Q3);
        less_quadrant_dist -> Fill(less_quadrant[0]);
        less_ratio_dist -> Fill(less_quadrant[1]);
        NClus_less_ratio_2D_dist -> Fill(inttDSTData->NClus, less_quadrant[1]);

        if (less_quadrant[0] == 0) {
            NClus_less_ratio_2D_dist_0 -> Fill(inttDSTData->NClus, less_quadrant[1]);
        } else if (less_quadrant[0] == 1) {
            NClus_less_ratio_2D_dist_1 -> Fill(inttDSTData->NClus, less_quadrant[1]);
        } else if (less_quadrant[0] == 2) {
            NClus_less_ratio_2D_dist_2 -> Fill(inttDSTData->NClus, less_quadrant[1]);
        } else if (less_quadrant[0] == 3) {
            NClus_less_ratio_2D_dist_3 -> Fill(inttDSTData->NClus, less_quadrant[1]);
        }

        evt_NClus_Q0 = 0;
        evt_NClus_Q1 = 0;
        evt_NClus_Q2 = 0;
        evt_NClus_Q3 = 0;
    }   

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("","",950,800);

    c1 -> cd();
    quadrant_check_0 -> Draw("colz0");
    c1 -> Print(Form("%s/quadrant_check_0.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    quadrant_check_1 -> Draw("colz0");
    c1 -> Print(Form("%s/quadrant_check_1.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    quadrant_check_2 -> Draw("colz0");
    c1 -> Print(Form("%s/quadrant_check_2.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    quadrant_check_3 -> Draw("colz0");
    c1 -> Print(Form("%s/quadrant_check_3.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    less_quadrant_dist -> Draw("hist");
    less_quadrant_dist -> SetMinimum(0);
    c1 -> Print(Form("%s/less_quadrant_dist.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    c1 -> SetLogy(1);
    less_ratio_dist -> Draw("hist");
    // less_ratio_dist -> SetMinimum(0);
    c1 -> Print(Form("%s/less_ratio_dist.pdf",output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    c1 -> cd();
    NClus_less_ratio_2D_dist -> Draw("colz0");
    c1 -> Print(Form("%s/NClus_less_ratio_2D_dist.pdf",output_directory.c_str()));
    c1 -> Clear();


    c1 -> cd();
    NClus_less_ratio_2D_dist_0 -> Draw("colz0");
    c1 -> Print(Form("%s/NClus_less_ratio_2D_dist_0.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    NClus_less_ratio_2D_dist_1 -> Draw("colz0");
    c1 -> Print(Form("%s/NClus_less_ratio_2D_dist_1.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    NClus_less_ratio_2D_dist_2 -> Draw("colz0");
    c1 -> Print(Form("%s/NClus_less_ratio_2D_dist_2.pdf",output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    NClus_less_ratio_2D_dist_3 -> Draw("colz0");
    c1 -> Print(Form("%s/NClus_less_ratio_2D_dist_3.pdf",output_directory.c_str()));
    c1 -> Clear();



}