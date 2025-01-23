#ifndef __SPINDB_QA__
#define __SPINDB_QA__

#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>
#include <uspin/SpinDBInput.h>

#include <vector>
#include <string>
#include <map>
#include <memory>

// Forward declarations
class SpinDBContent;
class SpinDBOutput;
class SpinDBInput;


class SpinDBQA
{
public:
    SpinDBQA();
    ~SpinDBQA() = default;

    void SetCNIPath(std::string cnipath = "/gpfs02/eic/cnipol/jet_run24/results"){_cnipathname = cnipath;};
    void SetMarkdownFilename(std::string markdownfile = "README.md"){_markdownfilename = markdownfile;};
    //void SetHtmlFilename(std::string htmlfile = "runsummary.html") = {htmlfilename = htmlfile};

    void SetRunList(std::string runlist);
    void ReadSpinDBData();
    void doQA();

    //void WriteHtml();
    void WriteMarkdown();

    //void WriteSpinDBData();
    //void PrintSpinDBData();

private:
    SpinDBContent spin_cont;
    SpinDBOutput spin_out;
    SpinDBInput spin_in;
    
    void LocalPolQA(std::string &stringMarkdown);
    void GL1pScalersQA(std::string &stringMarkdown);
    void CNIHjetQA(std::string &stringMarkdown);

    std::vector<int> runlistvect;
    std::map<std::string, std::vector<int>> map_spindbqa_markdown;
    std::map<std::string, std::vector<int>> map_spindbqa_html;
    
    std::string _cnipathname = "/gpfs02/eic/cnipol/jet_run24/results";
    std::string _markdownfilename;
    std::string _htmlfilename;

    int runnumber;

    std::map<int, int> map_defaultQA, map_fillnumber, map_qa_level, map_crossingshift;
    std::map<int, double> map_bluepol, map_yellpol, map_bluepolerr, map_yellpolerr;
    std::map<int, int> map_badrunqa;
    std::map<int, float> map_crossingangle, map_crossanglestd, map_crossanglemin, map_crossanglemax;
    std::map<int, float> map_asymbf, map_asymbb, map_asymyf, map_asymyb;
    std::map<int, float> map_asymerrbf, map_asymerrbb, map_asymerryf, map_asymerryb;
    std::map<int, float> map_phasebf, map_phasebb, map_phaseyf, map_phaseyb;
    std::map<int, float> map_phaseerrbf, map_phaseerrbb, map_phaseerryf, map_phaseerryb;

    std::map<int, std::vector<int>> map_bluespin;
    std::map<int, std::vector<int>> map_yellspin;
    std::map<int, std::vector<long long>> map_mbdns;
    std::map<int, std::vector<long long>> map_mbdvtx;
    std::map<int, std::vector<long long>> map_zdcns;

};






#endif