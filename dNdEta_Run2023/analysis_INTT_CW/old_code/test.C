#include "/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/InttConversion_new.h"
#include "/sphenix/user/ChengWei/INTT/INTT_commissioning/INTT_CW/INTT_commissioning/DAC_Scan/InttClustering.h"

// struct hit_info {
//     int chip_id;
//     int chan_id;
//     int adc;
//     int adc_conv;
//     int bco_diff; // note : small_bco - bco_full
// };

void test()
{
    // note : the initiator for the channel position
    InttConversion * ch_pos_DB = new InttConversion("full_survey_3.32", 3.32);

    // InttClustering::test_func(ch_pos_DB);
    pos_str aaa = InttClustering::test_func(ch_pos_DB);
    cout<<aaa.x<<" "<<aaa.y<<" "<<aaa.z<<endl;

    // vector<clu_info> clu_vec; 
    // // vector<vector<vector<hit_info>>> single_event_hit_vec(N_server, single_event_hit_ladder_vec); // note : [server][ladder]
    // vector<hit_info> single_event_hit_vec;
    // single_event_hit_vec.push_back({1,0,0,22,10});
    // single_event_hit_vec.push_back({1,1,1,45,10});
    // clu_vec = InttClustering::clustering(Form("intt0"), 2, single_event_hit_vec, ch_pos_DB);
}



// #include <Eigen/Dense>
// #include <Eigen/Geometry>
// #include <Eigen/LU>
// #include <Eigen/SVD>
// #include <Math/Transform3D.h>

// #include <intt/InttMapping.h>
// R__LOAD_LIBRARY(libintt.so)



// namespace InttConversion
// {
//     Intt::RawData_s trial_channel; // note : chip seems to be from 0 to 25
    
//     TTree* ReadTTree()
//     {
//         TFile * file_in = new TFile(Form("/sphenix/user/ChengWei/INTT/test_build/intt_transforms-1.root"),"read");
//         TTree * tree = (TTree *)file_in->Get("intt_transforms");

//         ROOT::Math::Transform3D * transform_branch = new ROOT::Math::Transform3D(); 
//         tree -> SetBranchAddress("transform",&transform_branch);

//         return tree;
//     }
    
//     void func_1 ()
//     {
//         trial_channel.felix_server = 0;
//         trial_channel.felix_channel = 0;
//         trial_channel.chip = 0;
//         trial_channel.channel = 127; // note : chan should be from 0 to 127

//         Eigen::Affine3d sensor_trans = Intt::GetTransform(ReadTTree(), ToOffline(trial_channel));
//         Eigen::Vector4d local_trans = Intt::GetLocalPos(ToOffline(trial_channel));
        

        
        
//     //     // std::cout<<sensor_trans<<std::endl;
//     //     // std::cout<<"----------- ----------- -----------"<<std::endl;
//     //     // std::cout<<local_trans<<std::endl;
//     //     // std::cout<<sensor_trans * local_trans<<std::endl;
//     //     // std::cout<<"----------- ----------- -----------"<<std::endl;
//     //     double test_x = (sensor_trans * local_trans)[0];
//     //     std::cout<<test_x<<std::endl;
//     //     // std::cout<<(sensor_trans * local_trans)[1]<<std::endl;
//     //     // std::cout<<(sensor_trans * local_trans)[2]<<std::endl;
//     //     return;
//     }





// }
