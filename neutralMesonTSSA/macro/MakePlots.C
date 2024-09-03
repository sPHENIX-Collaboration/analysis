#include "tssaplotter/TSSAplotter.h"

R__LOAD_LIBRARY(libTSSAplotter.so)

int MakePlots(std::string infilename, std::string outfilename = "NMhist.pdf") {
    TSSAplotter t;
    t.main(infilename, outfilename);
    return 0;
}
