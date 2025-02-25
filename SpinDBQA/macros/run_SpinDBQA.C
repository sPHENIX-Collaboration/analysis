#include <SpinDBQA.h>

R__LOAD_LIBRARY(libSpinDBQA.so)

//First run in spin db: 41935
//First official spin run: 45236
//Final spin (pp) run: 53880
void run_SpinDBQA()
{
    std::string runfile = "runnumbers.dat";
    std::string CNIpath = "/gpfs02/eic/cnipol/jet_run24/results";
    std::string mdfile = "README.md";
    std::string htmlfile = "runsummary.html";
    std::string rootfile = "qa_results.root";

    SpinDBQA *qa = new SpinDBQA();
    
    qa->SetRunList(runfile);
    qa->SetCNIPath(CNIpath);

    // ==== Set QA level (can either do default qa level or specific choice of qa level) ==== //
    qa->DefaultQA();
    //int qa_level = 0xFFFF; qa->SetQALevel(qa_level);
    //int qa_level = 0; qa->SetQALevel(qa_level);
    // ====================== //

    qa->ReadSpinDBData();
    qa->doQA();

    qa->SetMarkdownFilename(mdfile);
    qa->WriteMarkdown();

    qa->SetHtmlFilename(htmlfile);
    qa->WriteHtml();
    
    // qa->SetRootFilename(rootfile);
    // qa->WriteRootFile();


    // ======== Write new QA level (WARNING: EXPERT ONLY!)========== //
    // int newqalevel = 0; 
    // qa->WriteNewQALevel(newqalevel);

}
