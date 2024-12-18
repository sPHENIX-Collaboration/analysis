#ifndef __INTTCLUSTERING_H_
#define __INTTCLUSTERING_H_

// #include "InttConversion.h"

struct clu_info {
    int column;
    // int chip_id;
    double avg_chan;
    int sum_adc;
    int sum_adc_conv;
    int size;

    double x;
    double y;
    double z;
    int layer;
    double phi;
    // vector<double> bco_diff_vec; // note : for the multi-hit cluster, more than one hit was included. so more than one bco_diff
    int raw_layer_id;
    int raw_ladder_id;
    int raw_Z_id;
    int raw_phi_size;
    int raw_Z_size;
};

struct clu_info_private {
    int column;
    // int chip_id;
    double avg_chan;
    int sum_adc;
    int sum_adc_conv;
    int size;

    double x;
    double y;
    double z;
    int layer;
    double phi;
    vector<double> bco_diff_vec; // note : for the multi-hit cluster, more than one hit was included. so more than one bco_diff
};

struct hit_info {
    int chip_id;
    int chan_id;
    int adc;
    int adc_conv;
    int bco_diff; // note : small_bco - bco_full
};

struct hit_pro_info {
    int chip_id;
    int chan_id;

    int column_id;
    int chan_id_conv;

    int adc;
    int adc_conv;

    double x;
    double y;
    double z;
    int layer;
    int bco_diff;
};

namespace InttClustering{
    
    vector<clu_info_private> clustering(string server_name, int FC_id, vector<hit_info> single_event, InttConversion * ch_pos_DB)
    {

        // note : [0] : nominal_chip_id, 
        // note : [1] : nominal_chan_id,
        // note : [2] : adc,
        // note : [3] : adc_conv
        // note : [4] : bco_diff
        vector<int> nominal_vec = {-1,-1,-1,-1,-1};
        vector<int> hit_seat[13][256]; 
        for (int i = 0; i < 13; i++){
            for (int i1 = 0; i1 < 256; i1++){
                hit_seat[i][i1] = nominal_vec;
            }
        }

        vector<hit_pro_info>hit_pro_vec; hit_pro_vec.clear(); // note : have the position information
        vector<clu_info_private> output_vec; output_vec.clear();

        int num_hit = 0;
        double chan_truck = 0;
        double chan_truck_denominator = 0;
        double pos_x_truck = 0;
        double pos_x_truck_denominator = 0;
        double pos_y_truck = 0;
        double pos_y_truck_denominator = 0;
        int standard_channel;
        int distance_meter = 0;
        int sum_adc = 0;
        int sum_adc_conv = 0;
        vector<double> bco_diff_vec; bco_diff_vec.clear();

        // cout<<"test 3"<<endl;
        
        for (int i = 0; i < single_event.size(); i++) // note : number of hits in this event, for this half-ladder
        {
            // note : U1 ~ U13, 0 ~ 127. // U14 ~ U26, -> 128 ~ 255
            int chip_conv = ( single_event[i].chip_id > 13 ) ? single_event[i].chip_id - 13  : single_event[i].chip_id;
            int chan_conv = ( single_event[i].chip_id > 13 ) ? 255 - single_event[i].chan_id : single_event[i].chan_id;

            if (hit_seat[chip_conv - 1][chan_conv] == nominal_vec)
            {
                hit_seat[chip_conv - 1][chan_conv] = {single_event[i].chip_id, single_event[i].chan_id, single_event[i].adc, single_event[i].adc_conv, single_event[i].bco_diff};
            }
            else // note : clone hit, but do nothing for now
            {
                // cout<<"chip_id : "<<chip_conv<<" chan_id : "<<chan_conv<<" fired more than once ! N_hit in this HL : "<<single_event.size()<<" nominal_chipid : "<<single_event[i].chip_id<<" chan_id : "<<single_event[i].chan_id<<endl;
                // note : take the latest one.
                hit_seat[chip_conv - 1][chan_conv] = {single_event[i].chip_id, single_event[i].chan_id, single_event[i].adc, single_event[i].adc_conv, single_event[i].bco_diff};    
            }

            // cout<<"test 3-5 "<<single_event[i].chip_id<<" "<<single_event[i].chan_id<<endl;    
        }
        // cout<<"test 4"<<endl;
        
        // for (int col = 0; col < 13; col++) // note : column
        // {
        //     // cout<<" "<<endl;
        //     for (int ch = 0; ch < 256; ch++) // note : channel, remove those empty seat
        //     {
        //         cout<<"---------------- "<<col<<" "<<ch<<" "<<hit_seat[col][ch][0]<<" "<<hit_seat[col][ch][1]<<" "<<hit_seat[col][ch][2]<<" "<<hit_seat[col][ch][3]<<" "<<hit_seat[col][ch][4]<<" size : "<<hit_seat[col][ch].size()<<endl;
        //     }
        // }

        for (int col = 0; col < 13; col++) // note : column
        {
            // cout<<" "<<endl;
            for (int ch = 0; ch < 256; ch++) // note : channel, remove those empty seat
            {
                if (hit_seat[col][ch][0] != -1 && hit_seat[col][ch][1] != -1 && hit_seat[col][ch][2] != -1 && hit_seat[col][ch][3] != -1 && hit_seat[col][ch][4] != -1 && hit_seat[col][ch].size() == 5)
                {
                    // cout<<"test 4-5 "<<col<<" "<<ch<<endl;
                    // cout<<"test 4-5 "<<server_name<<" "<<FC_id<<" "<<hit_seat[col][ch][0]<<" "<<hit_seat[col][ch][1]<<endl;
                    pos_str hit_pos = ch_pos_DB -> Get_XY_all(server_name, FC_id, hit_seat[col][ch][0], hit_seat[col][ch][1]);
                    // cout<<"test 5"<<endl;
                    // cout<<" "<<endl;

                    // cout<<"!!!!!!!!!!!!!!!!!! test in clustering func "<<hit_pos.x<<" "<<hit_pos.y<<endl;
                    // cout<<"("<<hit_pos.x<<","<<hit_pos.y<<"),";

                    hit_pro_vec.push_back({
                        hit_seat[col][ch][0], // note : original chip_id
                        hit_seat[col][ch][1], // note : original chan_id
                        col + 1,              // note : column
                        ch,                   // note : conv_ch_id
                        hit_seat[col][ch][2], // note : adc
                        hit_seat[col][ch][3], // note : conv_adc
                        
                        hit_pos.x,            // note : nominal strip pos in sPHENIX coordinate 
                        hit_pos.y,            // note : nominal strip pos in sPHENIX coordinate 
                        hit_pos.z,            // note : nominal strip pos in sPHENIX coordinate
                        hit_pos.layer,        // note : N layer, 0 or 1.
                        
                        hit_seat[col][ch][4]  // note : bco_diff
                    });
                    // if (hit_seat[col][ch][3] > 1000){
                    //     cout<<"before clustering, hit info :\t"<<hit_pos.x<<" "<<hit_pos.y<<" "<<hit_pos.z<<"\toriginal chip/chan "<<hit_seat[col][ch][0]<<" "<<hit_seat[col][ch][1]<<"\tconverted chip/chan "<<col+1<<" "<<ch<<"\tadc & adc_conv "<<hit_seat[col][ch][2]<<" "<<hit_seat[col][ch][3]<<endl;
                    // }
                    
                }
            } // note : end 256 chan check

            for (int hit_i = 0; hit_i < hit_pro_vec.size(); hit_i++)
            {
                standard_channel         = hit_pro_vec[hit_i].chan_id_conv;

                chan_truck              += hit_pro_vec[hit_i].chan_id_conv;
                chan_truck_denominator  += 1;

                pos_x_truck             += hit_pro_vec[hit_i].x * hit_pro_vec[hit_i].adc_conv;
                pos_x_truck_denominator += hit_pro_vec[hit_i].adc_conv;
                
                pos_y_truck             += hit_pro_vec[hit_i].y * hit_pro_vec[hit_i].adc_conv;
                pos_y_truck_denominator += hit_pro_vec[hit_i].adc_conv;
                
                num_hit                 += 1; 
                sum_adc                 += hit_pro_vec[hit_i].adc;
                sum_adc_conv            += hit_pro_vec[hit_i].adc_conv;

                bco_diff_vec.push_back(hit_pro_vec[hit_i].bco_diff);

                if (hit_pro_vec.size() - hit_i == 1) // note : for the case that only one hit in this column, and the "last single hit" in the column 
                {
                    output_vec.push_back({
                        col + 1,  // note : column
                        chan_truck / chan_truck_denominator,
                        sum_adc,
                        sum_adc_conv,
                        num_hit,

                        pos_x_truck / pos_x_truck_denominator,
                        pos_y_truck / pos_y_truck_denominator,
                        hit_pro_vec[0].z,     // note : sPH-INTT nominal position
                        hit_pro_vec[0].layer, // note : layer
                        ((pos_y_truck / pos_y_truck_denominator) < 0) ? atan2((pos_y_truck / pos_y_truck_denominator),(pos_x_truck / pos_x_truck_denominator)) * (180./TMath::Pi()) + 360 : atan2((pos_y_truck / pos_y_truck_denominator),(pos_x_truck / pos_x_truck_denominator)) * (180./TMath::Pi()),
                        bco_diff_vec
                    });
                    // if (sum_adc_conv > 1000){
                    //     cout<<"~~~~ bug check 1, column : "<<col + 1<<" avg_chan : "<<chan_truck / chan_truck_denominator<<" sum_adc : "<<sum_adc <<" sum_adc_conv : "<<sum_adc_conv <<" size : "<<num_hit <<" avg_posX : "<<pos_x_truck / pos_x_truck_denominator <<" avg_posY : "<<pos_y_truck / pos_y_truck_denominator <<" Z : "<<hit_pro_vec[0].z<<" layer : "<<hit_pro_vec[0].layer<<endl;
                    // }
                    // cout<<"~~~~~~ test clustering func, "<<pos_x_truck / pos_x_truck_denominator<<" "<<pos_y_truck / pos_y_truck_denominator<<endl;
                    // cout<<"("<<pos_x_truck/pos_x_truck_denominator<<","<<pos_y_truck/pos_y_truck_denominator<<"),";
                }

                // note : this for loop will be skipped if it's single hit
                for (int hit_i1 = 0; hit_i1 < hit_pro_vec.size() - (hit_i + 1); hit_i1++)
                {
                    if ( fabs(standard_channel - hit_pro_vec[hit_i + hit_i1 + 1].chan_id_conv) == hit_i1 + 1 )
                    {
                        chan_truck              += hit_pro_vec[hit_i + hit_i1 + 1].chan_id_conv;
                        chan_truck_denominator  += 1;

                        pos_x_truck             += hit_pro_vec[hit_i + hit_i1 + 1].x * hit_pro_vec[hit_i + hit_i1 + 1].adc_conv;
                        pos_x_truck_denominator += hit_pro_vec[hit_i + hit_i1 + 1].adc_conv;
                        
                        pos_y_truck             += hit_pro_vec[hit_i + hit_i1 + 1].y * hit_pro_vec[hit_i + hit_i1 + 1].adc_conv;
                        pos_y_truck_denominator += hit_pro_vec[hit_i + hit_i1 + 1].adc_conv;
                        
                        num_hit                 += 1; 
                        sum_adc                 += hit_pro_vec[hit_i + hit_i1 + 1].adc;
                        sum_adc_conv            += hit_pro_vec[hit_i + hit_i1 + 1].adc_conv;

                        bco_diff_vec.push_back(hit_pro_vec[hit_i].bco_diff);

                        distance_meter += 1;

                        if ((hit_pro_vec.size() - (hit_i + 1) - hit_i1) == 1) // note : the last non single-hit cluster
                        {
                            output_vec.push_back({
                                col + 1,  // note : column
                                chan_truck / chan_truck_denominator,
                                sum_adc,
                                sum_adc_conv,
                                num_hit,

                                pos_x_truck / pos_x_truck_denominator,
                                pos_y_truck / pos_y_truck_denominator,
                                hit_pro_vec[0].z,
                                hit_pro_vec[0].layer, // note : layer
                                ((pos_y_truck / pos_y_truck_denominator) < 0) ? atan2((pos_y_truck / pos_y_truck_denominator),(pos_x_truck / pos_x_truck_denominator)) * (180./TMath::Pi()) + 360 : atan2((pos_y_truck / pos_y_truck_denominator),(pos_x_truck / pos_x_truck_denominator)) * (180./TMath::Pi()),
                                bco_diff_vec
                            });
                            // if (sum_adc_conv > 1000){
                            //     cout<<"~~~~ bug check 2, column : "<<col + 1<<" avg_chan : "<<chan_truck / chan_truck_denominator<<" sum_adc : "<<sum_adc <<" sum_adc_conv : "<<sum_adc_conv <<" size : "<<num_hit <<" avg_posX : "<<pos_x_truck / pos_x_truck_denominator <<" avg_posY : "<<pos_y_truck / pos_y_truck_denominator <<" Z : "<<hit_pro_vec[0].z<<" layer : "<<hit_pro_vec[0].layer<<endl;
                            // }

                            // cout<<"~~~~~~ test clustering func, "<<pos_x_truck / pos_x_truck_denominator<<" "<<pos_y_truck / pos_y_truck_denominator<<endl;
                            // cout<<"("<<pos_x_truck/pos_x_truck_denominator<<","<<pos_y_truck/pos_y_truck_denominator<<"),";
                        }

                    }
                    else 
                    {
                        output_vec.push_back({
                            col + 1,  // note : column
                            chan_truck / chan_truck_denominator,
                            sum_adc,
                            sum_adc_conv,
                            num_hit,

                            pos_x_truck / pos_x_truck_denominator,
                            pos_y_truck / pos_y_truck_denominator,
                            hit_pro_vec[0].z,
                            hit_pro_vec[0].layer, // note : layer
                            ((pos_y_truck / pos_y_truck_denominator) < 0) ? atan2((pos_y_truck / pos_y_truck_denominator),(pos_x_truck / pos_x_truck_denominator)) * (180./TMath::Pi()) + 360 : atan2((pos_y_truck / pos_y_truck_denominator),(pos_x_truck / pos_x_truck_denominator)) * (180./TMath::Pi()),
                            bco_diff_vec
                        });
                        // if (sum_adc_conv > 1000){
                        //     cout<<"~~~~ bug check 3, column : "<<col + 1<<" avg_chan : "<<chan_truck / chan_truck_denominator<<" sum_adc : "<<sum_adc <<" sum_adc_conv : "<<sum_adc_conv <<" size : "<<num_hit <<" avg_posX : "<<pos_x_truck / pos_x_truck_denominator <<" avg_posY : "<<pos_y_truck / pos_y_truck_denominator <<" Z : "<<hit_pro_vec[0].z<<" layer : "<<hit_pro_vec[0].layer<<endl;
                        // }
                        // cout<<"~~~~~~ test clustering func, "<<pos_x_truck / pos_x_truck_denominator<<" "<<pos_y_truck / pos_y_truck_denominator<<endl;
                        // cout<<"("<<pos_x_truck/pos_x_truck_denominator<<","<<pos_y_truck/pos_y_truck_denominator<<"),";

                        break;
                    }

                } // note : end : n_hit i1

                hit_i += distance_meter;
                distance_meter = 0;
                chan_truck = 0;
                chan_truck_denominator = 0;

                pos_x_truck = 0;             
                pos_x_truck_denominator = 0; 
                
                pos_y_truck = 0;             
                pos_y_truck_denominator = 0; 
                
                num_hit = 0;                 
                sum_adc = 0;                 
                sum_adc_conv = 0;    

                bco_diff_vec.clear();        

            } // note : end n_hit in single column

            hit_pro_vec.clear();

        } // note : end column

        return output_vec;
    }

    pos_str test_func(InttConversion * ch_pos_DB){
        // cout<<"In InttClustering.h, test_func : "<< ch_pos_DB -> GetGeoMode()<<endl;

        return ch_pos_DB -> Get_XY_all("intt0", 0, 1, 127);
    }

};

#endif