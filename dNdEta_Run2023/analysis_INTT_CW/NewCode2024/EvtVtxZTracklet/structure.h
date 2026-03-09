#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <TObject.h>

// struct pair_str {
//     double delta_phi;
//     double delta_eta;
//     double pair_eta_num;
//     double pair_eta_fit;

//     int inner_index;
//     int inner_phi_id;
//     int inner_layer_id;
//     int inner_zid;
//     int inner_phi_size;
//     double inner_adc;
//     double inner_x;
//     double inner_y;
//     double inner_z;
//     double inner_phi;
//     double inner_eta;
    
//     int outer_index;
//     int outer_phi_id;
//     int outer_layer_id;
//     int outer_zid;
//     int outer_phi_size;
//     double outer_adc;
//     double outer_x;
//     double outer_y;
//     double outer_z;
//     double outer_phi;
//     double outer_eta;

//     ClassDef(pair_str, 1) // ROOT macro for dictionary generation
// };

struct pair_str {
    double delta_phi;
    double delta_eta;
    double pair_eta_num;
    double pair_eta_fit;

    int inner_index;
    int inner_zid;
    int inner_phi_size;
    double inner_adc;
    double inner_x;
    double inner_y;
    
    int outer_index;
    int outer_zid;
    int outer_phi_size;
    double outer_adc;
    double outer_x;
    double outer_y;

    ClassDef(pair_str, 1) // ROOT macro for dictionary generation
};


#endif // STRUCTURE_H
