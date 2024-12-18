#include "tssahistmaker/TSSAhistmaker.h"

R__LOAD_LIBRARY(libTSSAhistmaker.so)

int MakeHists(const std::string treepath, const int jobnum, const std::string outfilename = "TSSAhists.root") {
    TSSAhistmaker t(treepath, jobnum, outfilename);
    t.main();
    return 0;
}
