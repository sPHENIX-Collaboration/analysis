#include "tssaplotter/TSSAplotter.h"

R__LOAD_LIBRARY(libTSSAplotter.so)

int MakePlots(std::string infileTSSA, std::string infileNM = "", std::string outfileTSSA = "TSSAplots.pdf", std::string outfileNM = "NMplots.pdf") {
    TSSAplotter t;
    t.TSSAhistfname = infileTSSA;
    if (!infileNM.empty()) {
	t.do_NMhists = true;
	t.NMhistfname = infileNM;
    }
    t.main(outfileTSSA, outfileNM);
    return 0;
}
