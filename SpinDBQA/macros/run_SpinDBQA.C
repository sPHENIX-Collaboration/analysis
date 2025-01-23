#include <SpinDBQA.h>

//First run in spin db: 41935
//First official spin run: 45236

// For now, limit run selection to 51730-52206, 52469-53880
R__LOAD_LIBRARY(libSpinDBQA.so)

void run_SpinDBQA()
{
    std::string runfile = "runnumbers.dat";
    std::string CNIpath = "/gpfs02/eic/cnipol/jet_run24/results";
    std::string mdfile = "README.md";

    SpinDBQA *qa = new SpinDBQA();
    
    qa->SetRunList(runfile);
    qa->SetCNIPath(CNIpath);
    qa->ReadSpinDBData();
    qa->doQA();

    qa->SetMarkdownFilename(mdfile);
    qa->WriteMarkdown();

}
