
#ifndef MISALIGNMENT_H
#define MISALIGNMENT_H

#include <cmath>
#include <map>
#include <vector>

#include <TMath.h>

vector<float> misalignment(int c)
{
    float shift_1 = 0.001, shift_2 = 0.005, shift_3 = 0.01;

    std::map<int, std::vector<float>> Map_MisliagnCase = {{0, {0, 0, 0}},
                                                          {1, {shift_1, 0, 0}}, // 10 micron
                                                          {2, {-1 * shift_1, 0, 0}},
                                                          {3, {shift_2, 0, 0}}, // 50 micron
                                                          {4, {-1 * shift_2, 0, 0}},
                                                          {5, {shift_3, 0, 0}}, // 100 micron
                                                          {6, {-1 * shift_3, 0, 0}},
                                                          {7, {0, shift_1, 0}},                 // angle = 0
                                                          {8, {0, shift_1, TMath::Pi() / 6.}},  // angle = 30
                                                          {9, {0, shift_1, TMath::PiOver4()}},  // angle = 45
                                                          {10, {0, shift_1, TMath::Pi() / 3.}}, // angle = 60
                                                          {11, {0, shift_1, TMath::Pi() / 2.}}, // angle = 90
                                                          {12, {0, shift_1, 3 * TMath::PiOver4()}},
                                                          {13, {0, shift_1, 5 * TMath::PiOver4()}},
                                                          {14, {0, shift_1, 7 * TMath::PiOver4()}},
                                                          {15, {0, shift_2, 0}},                // angle = 0
                                                          {16, {0, shift_2, TMath::Pi() / 6.}}, // angle = 30
                                                          {17, {0, shift_2, TMath::PiOver4()}}, // angle = 45
                                                          {18, {0, shift_2, TMath::Pi() / 3.}}, // angle = 60
                                                          {19, {0, shift_2, TMath::Pi() / 2.}}, // angle = 90
                                                          {20, {0, shift_2, 3 * TMath::PiOver4()}},
                                                          {21, {0, shift_2, 5 * TMath::PiOver4()}},
                                                          {22, {0, shift_2, 7 * TMath::PiOver4()}},
                                                          {23, {0, shift_3, 0}},                // angle = 0
                                                          {24, {0, shift_3, TMath::Pi() / 6.}}, // angle = 30
                                                          {25, {0, shift_3, TMath::PiOver4()}}, // angle = 45
                                                          {26, {0, shift_3, TMath::Pi() / 3.}}, // angle = 60
                                                          {27, {0, shift_3, TMath::Pi() / 2.}}, // angle = 90
                                                          {28, {0, shift_3, 3 * TMath::PiOver4()}},
                                                          {29, {0, shift_3, 5 * TMath::PiOver4()}},
                                                          {30, {0, shift_3, 7 * TMath::PiOver4()}},
                                                          {31, {shift_1, shift_1, TMath::PiOver4()}},
                                                          {32, {shift_1, shift_2, TMath::PiOver4()}},
                                                          {33, {shift_1, shift_3, TMath::PiOver4()}},
                                                          {34, {shift_2, shift_1, TMath::PiOver4()}},
                                                          {35, {shift_2, shift_2, TMath::PiOver4()}},
                                                          {36, {shift_2, shift_3, TMath::PiOver4()}},
                                                          {37, {shift_3, shift_1, TMath::PiOver4()}},
                                                          {38, {shift_3, shift_2, TMath::PiOver4()}},
                                                          {39, {shift_3, shift_3, TMath::PiOver4()}},
                                                          // misalignment case for the MVTX position as of April 21 2023: south end gap 1.5mm and north end gap 2.9mm, so the two halfs have an angle
                                                          {100, {float(TMath::ATan(0.7 / 604.59))}},
                                                          {101, {float(TMath::ATan(1.0 / 604.59))}}};

    return Map_MisliagnCase[c];
}

// For gap in the two halves of the MVTX
// s is the distance from the center of the clip to the beamline
// g1 is the asymmetric gap for the top half of the stave
// See [TODO: add link to the presentation]
// Rotate, then shift based on the asymmetric gap between the two halves
void UpdatePos_GapTwoHalves(vector<float> &hitpos, float gap_northend, float s, float g1)
{
    float NorthEnd_to_StaveMid = 177.5;                                        // mm
    float StaveSouthEnd_to_StaveMid = 154.;                                    // mm
    float Clips_to_StaveSouthEnd = 273.09;                                     // mm
    float zshift = (Clips_to_StaveSouthEnd + StaveSouthEnd_to_StaveMid) / 10.; // cm
    float gap_atClips = 1.5;                                                   // mm
    float g2 = gap_northend - g1;                                              // mm
    float dalpha1_num = g1 - (gap_atClips / 2.);                               // mm, for the top half (x0 > 0)
    float dalpha2_num = g2 - (gap_atClips / 2.);                               // mm, for the bottom half (x0 < 0)

    if (gap_northend != gap_atClips)
    {
        // Rotate
        float z0 = hitpos[2] + zshift;
        float x0 = hitpos[0];
        float l = z0;
        float alpha0 = TMath::ATan(x0 / z0);               // In radian
        float dalpha1 = TMath::ATan(dalpha1_num / 604.59); // upper half (x0 > 0), in radian
        float dalpha2 = TMath::ATan(dalpha2_num / 604.59); // lower half (x0 < 0), in radian
        // hitpos[2] = (x0 > 0) ? l * TMath::Cos(alpha0 + dalpha1) - zshift : l * TMath::Cos(alpha0 - dalpha2) - zshift;
        // hitpos[0] = (x0 > 0) ? l * TMath::Sin(alpha0 + dalpha1) : l * TMath::Sin(alpha0 - dalpha2);
        hitpos[2] = (x0 > 0) ? l * TMath::Cos(dalpha1) - zshift : l * TMath::Cos(dalpha2) - zshift;
        hitpos[0] = (x0 > 0) ? x0 + l * TMath::Sin(dalpha1) : x0 + l * TMath::Sin(-1 * dalpha2);
        // Shift
        hitpos[0] = hitpos[0] + s;
    }
}

#endif