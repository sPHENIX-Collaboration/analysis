#if defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE || defined INCLUDE_VZ_N
    constexpr int nvz = 20;
#if defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE
    constexpr float vzmin = -10;
    constexpr float vzmax = 10;
#if defined INCLUDE_VZ_BINS
    float vzb[nvz + 1];
    for (int i = 0; i <= nvz; i++)
        vzb[i] = i * (vzmax - vzmin) / nvz + vzmin;
#endif /* defined INCLUDE_VZ_BINS */
#endif /* defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE */
#endif /* defined INCLUDE_VZ_BINS || defined INCLUDE_VZ_RANGE || defined INCLUDE_VZ_N */

#if defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE || defined INCLUDE_ETA_N
    constexpr int neta = 27;
#if defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE
    constexpr float etamin = -2.7;
    constexpr float etamax = 2.7;
#if defined INCLUDE_ETA_BINS
    float etab[neta + 1];
    for (int i = 0; i <= neta; i++)
        etab[i] = i * (etamax - etamin) / neta + etamin;
#endif /* defined INCLUDE_ETA_BINS */
#endif /* defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE */
#endif /* defined INCLUDE_ETA_BINS || defined INCLUDE_ETA_RANGE || defined INCLUDE_ETA_N */

#if defined INCLUDE_MULT_BINS || defined INCLUDE_MULT_N
    // constexpr int nmult = 10;
    constexpr int nmult = 1;
#if defined INCLUDE_MULT_BINS
    // constexpr float multb[nmult + 1] = {0, 30, 70, 150, 280, 500, 800, 1200, 1900, 2600, 4000};
    constexpr float multb[nmult + 1] = {0, 5000};

#endif /* defined (INCLUDE_MULT_BINS) */
#endif /* defined (INCLUDE_MULT_BINS) || defined (INCLUDE_MULT_N) */