#ifndef pAu_bins__h
#define pAu_bins__h

#include "noiBinVec.h"

static const int _trigEt   { 0 };
static const int _geantid  { 0 };
static const int _bbc      { 1 };
static const int _zdcx     { 2 };
static const int _pt       { 3 };
static const int _dphi     { 4 }; // used for jets relative to trigger
static const int _eta      { 4 };
static const int _dphi     { 4 }; // used for jets relative to trigger
static const int _dca      { 5 };
static const int _ptmatch  { 5 }; // used for Aj jet pairs
static const int _nhitfit  { 6 };
static const int _Aj       { 6 }; // used for Aj jet pairs
static const int _nhitrat  { 7 };
static const int _ptreco   { 8 };

// used for Aj and acoplanarity 
/* static const int _dphiAj   { 6 }; */

static const int _etaAu  { 0 }; // use like (*data)[_etaAu][_PUfit_m]
static const int _etaMid { 1 };
static const int _etaPP  { 2 };
static const int _etaAll { 3 };

static const int _phi_trig   { 1 };
static const int _phi_trans  { 2 };
static const int _phi_recoil { 3 };

static const int _PUfit_b { 0 };
static const int _PUfit_m { 1 };

// bin boundaries for 6 bins of ZDCx (the 7th is for all ZDCx)
static const array<int, 9> _0_zdcx_3kHz { 1, 4, 7, 10, 12, 15, 18, 21,  1 }; // for binning in 3kHz increments 4-28 kHz
static const array<int, 9> _1_zdcx_3kHz { 3, 6, 9, 12, 14, 17, 20, 23, 23 };
array<int,9> _i0_zdcx { 5, 8,  11, 14, 17, 20, 23, 26,  5 }; // binning by 3 for zdcx
array<int,9> _i1_zdcx { 7, 10, 13, 16, 19, 22, 25, 28,  28};
// bin kHz for the various bins (bin 1 at 4 kHz, etc...)
static const double kHz_ZDCx (int bin) { return (float)bin+3; };


#ifdef ioClass__h
    static ioBinVec bin_id      {{ -0.5,  -0.5,   6,      5.5     }};
    static ioBinVec bin_bbc     {{ 0.,  2802.38, 5460.32, 8420.66, 11722, 15410.1, 19584.9, 24371, 30105.3, 37666.3, 64000}};
    static ioBinVec bin_bbc3070 {{ 0.,  8420.66,  24371, 64000}};
    static ioBinVec bin_zdcx    {{ 0., 0.,  30,     30000.  }};
    static ioBinVec bin_pt   {{ // track pT bins                                               
         0.0,  0.1,  0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.1,  1.2,  1.3,  1.4,
     1.5,  1.6,  1.7,  1.8,  1.9,  2.0,  2.1,  2.2,  2.3,  2.4,  2.5,  2.6,  2.7,  2.8,  2.9,
     3.0,  3.1,  3.2,  3.3,  3.4,  3.5,  3.6,  3.7,  3.8,  3.9,  4.0,  4.1,  4.2,  4.3,  4.4,
     4.5,  4.7,  4.9,  5.1,  5.3,  5.5,  5.7,  5.9,  6.1,  6.3,  6.5,  6.7,  6.9,  7.2,  7.5,
     7.8,  8.1,  8.4,  8.7,  9.1,  9.5,  9.9, 10.4, 10.9, 11.4, 12.0, 12.6, 13.3, 15.0
    }};
    static ioBinVec bin_trigEt  {{ 0.,    4.,     8.,    12.,  30. }};
    static ioBinVec bin_dca     {{ 0.,    0.,     15,     3.       }};
    static ioBinVec bin_hitsfit {{ 0.5,   0.5,    47.,    47.5     }};
    static ioBinVec bin_fitrat  {{ 0.02,  0.02,   10,     1.02     }};
    static ioBinVec bin_eta     {{ -1.,  -0.3,   0.3,     1.       }};
    static ioBinVec bin_dummy   {{ 0., 1. }}; // a dummy bin for non-used axes
#endif

#ifdef noiBinVec__h
    static tuBinVec bin_id      {{ -0.5,  -0.5,   6,      5.5     }};
    static tuBinVec bin_bbc     {{ 0.,  2802.38, 5460.32, 8420.66, 11722, 15410.1, 19584.9, 24371, 30105.3, 37666.3, 64000}};
    static tuBinVec bin_bbc3070 {{ 0.,  8420.66,  24371, 64000}};
    static tuBinVec bin_zdcx    {{ 0., 0.,  30,     30000.  }};
    static tuBinVec bin_pt   {{ // track pT bins                                               
         0.0,  0.1,  0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.1,  1.2,  1.3,  1.4,
     1.5,  1.6,  1.7,  1.8,  1.9,  2.0,  2.1,  2.2,  2.3,  2.4,  2.5,  2.6,  2.7,  2.8,  2.9,
     3.0,  3.1,  3.2,  3.3,  3.4,  3.5,  3.6,  3.7,  3.8,  3.9,  4.0,  4.1,  4.2,  4.3,  4.4,
     4.5,  4.7,  4.9,  5.1,  5.3,  5.5,  5.7,  5.9,  6.1,  6.3,  6.5,  6.7,  6.9,  7.2,  7.5,
     7.8,  8.1,  8.4,  8.7,  9.1,  9.5,  9.9, 10.4, 10.9, 11.4, 12.0, 12.6, 13.3, 15.0
    }};
    static tuBinVec bin_trigEt  {{ 0.,    4.,     8.,    12.,  30. }};
    static tuBinVec bin_dca     {{ 0.,    0.,     15,     3.       }};
    static tuBinVec bin_hitsfit {{ 0.5,   0.5,    47.,    47.5     }};
    static tuBinVec bin_fitrat  {{ 0.02,  0.02,   10,     1.02     }};
    static tuBinVec bin_eta     {{ -1.,  -0.3,   0.3,     1.       }};
    static tuBinVec bin_dummy   {{ 0., 1. }}; // a dummy bin for non-used axes
#endif

    static map<string, array<double,4>> TsallisParams {
        {"Tsallis_pp_pi",       {{ 0.135000,  5.5037062019,  0.1277746601,  9.7595242212 }}},
        {"Tsallis_pp_antipi",   {{ 0.135000,  5.5865021613,  0.1270067077,  9.7325864609 }}},
        {"Tsallis_pp_K",        {{ 0.493677,  0.1386983308,  0.1929685766, 11.8247896746 }}},
        {"Tsallis_pp_antiK",    {{ 0.493677,  0.1386983308,  0.1929685766, 11.8247896746 }}},
        {"Tsallis_pp_p",        {{ 0.938272,  0.0749981986,  0.1758814595, 10.5452494593 }}},
        {"Tsallis_pp_pbar",     {{ 0.938272,  0.0642725426,  0.1691919677, 10.0635969124 }}},

        {"Tsallis_dAu_pi",      {{ 0.135000, 13.1377700117,  0.1525501142, 10.1869046869 }}},
        {"Tsallis_dAu_antipi",  {{ 0.135000, 13.6251153259,  0.1505198390, 10.0949606259 }}},
        {"Tsallis_dAu_K",       {{ 0.493677,  0.4615505225,  0.2215729988, 11.6137583852 }}},
        {"Tsallis_dAu_antiK",   {{ 0.493677,  0.4675391506,  0.2192846303, 11.4475846094 }}},
        {"Tsallis_dAu_p",       {{ 0.938272,  0.2873036847,  0.2114254603, 11.4410990835 }}},
        {"Tsallis_dAu_pbar",    {{ 0.938272,  0.2257531137,  0.2195209347, 13.0260305553 }}}
    };
                                              //
    static constexpr double _n10xPyth6[9] {
        3920155 , 2101168 , 1187058 , 1695141 ,
        4967075 , 1797387 , 260676 , 261926 , 262366
    };
    static constexpr double _XsecPyth6[9] {
        0.107509,   0.0190967,  0.00475202,
        0.00198812, 0.000361282, 9.65463E-06,
        4.71077E-07, 2.68464E-08, 1.38211E-09
    };                                             //
    static constexpr double _pthat9[9] {
        0.107509    / 3920155,
        0.0190967   / 2101168,
        0.00475202  / 1187058,
        0.00198812  / 1695141,
        0.000361282 / 4967075,
        9.65463E-06 / 1797387,
        4.71077E-07 / 260676,
        2.68464E-08 / 261926,
        1.38211E-09 / 262366
    };
                                              //
#endif // pAu_bins__h
       //

