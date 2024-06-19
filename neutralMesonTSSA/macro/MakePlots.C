#include "tssaplotter/TSSAplotter.h"

R__LOAD_LIBRARY(libTSSAplotter.so)

int MakePlots(std::string infilename) {
    TSSAplotter t;
    t.main(infilename);
    return 0;
}
