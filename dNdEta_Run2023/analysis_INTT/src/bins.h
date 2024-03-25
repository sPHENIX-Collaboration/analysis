#if defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE || defined INCLUDE_VZ_N
constexpr int nvz = 30;
#if defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE
constexpr float vzmin = -15;
constexpr float vzmax = 15;
#if defined INCLUDE_VZ_BINS
float vzb[nvz + 1];
for (int i = 0; i <= nvz; i++)
    vzb[i] = i * (vzmax - vzmin) / nvz + vzmin;
#endif /* defined INCLUDE_VZ_BINS */
#endif /* defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE */
#endif /* defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE || defined INCLUDE_VZ_N */

#if defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE || defined INCLUDE_ETA_N
constexpr int neta = 36;
#if defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE
constexpr float etamin = -3.6;
constexpr float etamax = 3.6;
#if defined INCLUDE_ETA_BINS
float etab[neta + 1];
for (int i = 0; i <= neta; i++)
    etab[i] = i * (etamax - etamin) / neta + etamin;
#endif /* defined INCLUDE_ETA_BINS */
#endif /* defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE */
#endif /* defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE || defined INCLUDE_ETA_N */

#if defined INCLUDE_MULT_BINS || defined INCLUDE_MULT_N
constexpr int nmult = 16;
#if defined INCLUDE_MULT_BINS
// constexpr float multb[nmult + 1] = {
//   0,   20, 50,  100,  200,
//      320,  540,  800, 1200, 1750,
//     2400, 3200, 4200, 5400, 7000,
//        12800
// };
constexpr float multb[nmult + 1] = {0, 30, 70, 150, 280, 500, 800, 1200, 1900, 2600, 3600, 4900, 6500, 8300, 10800, 15400, 20000};

#endif /* defined (INCLUDE_MULT_BINS) */
#endif /* defined (INCLUDE_MULT_BINS) || defined (INCLUDE_MULT_N) */