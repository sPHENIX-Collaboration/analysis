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

class TGraphErrors;


class SpinDBQA
{
public:
    SpinDBQA();
    ~SpinDBQA() = default;

    void SetCNIPath(std::string cnipath = "/gpfs02/eic/cnipol/jet_run24/results"){_cnipathname = cnipath;};
    void SetMarkdownFilename(std::string markdownfile = "README.md"){_markdownfilename = markdownfile;};
    void SetHtmlFilename(std::string htmlfile = "runsummary.html"){_htmlfilename = htmlfile;};
    void SetRootFilename(std::string rootfile = "qa_results.root"){_rootfilename = rootfile;};

    void SetRunList(std::string runlist);
    void SetCrossingAngleStdThreshold(float threshold){_crossanglestdthreshold = threshold;};

    void DefaultQA(){b_defaultQA = true;};
    void SetQALevel(int level){b_defaultQA = false; qalevel = level;};

    void ReadSpinDBData();
    void doQA();

    void WriteHtml();
    void WriteMarkdown();
    void WriteRootFile();

    void WriteNewQALevel(int newqalevel = -999);
    //void WriteSpinDBData();
    //void PrintSpinDBData();

private:
    SpinDBContent spin_cont;
    SpinDBOutput spin_out;
    SpinDBInput spin_in;
    
    void LocalPolQA(std::string &stringMarkdown,std::string &stringHtml);
    void GL1pScalersQA(std::string &stringMarkdown,std::string &stringHtml);
    void CNIHjetQA(std::string &stringMarkdown,std::string &stringHtml);
    void CrossingAngleQA(std::string &stringMarkdown,std::string &stringHtml);

    void PrepareHtml();
    std::string HtmlContent();

    void PrepareUpdatedCrossingShifts();
    void PrepareUpdatedBeamPolarizations();
    void PrepareUpdatedSpinPatterns();

    std::vector<int> runlistvect;
    std::map<std::string, std::vector<int>> map_spindbqa_markdown;
    std::map<std::string, std::vector<int>> map_spindbqa_html;
    
    std::string _cnipathname = "/gpfs02/eic/cnipol/jet_run24/results";
    std::string _markdownfilename;
    std::string _htmlfilename;
    std::string _rootfilename;

    float _crossanglestdthreshold = 0.1;  // mrad, mark runs with crossing angle std above this threshold as bad. Not very elegant to have this hardcoded cut, Dylan's fault

    // local pol checks
    float bf_phase_mean = 0;
    float bf_phase_rmse = 0.507;
    float yf_phase_mean = 0.006;
    float yf_phase_rmse = 0.447;
    float bf_mean = -2.269;
    float bf_rmse = 2.249;
    float yf_mean = -2.527;
    float yf_rmse = 1.007;


    int runnumber;

    bool b_defaultQA = true;
    int qalevel;

    //std::map<int, int> map_defaultQA, 
    std::map<int, int> map_fillnumber, map_qa_level, map_crossingshift;
    std::map<int, double> map_bluepol, map_yellpol, map_bluepolerr, map_yellpolerr;
    std::map<int, double> map_bluepolbyfill, map_yellpolbyfill, map_bluepolbyfillerr, map_yellpolbyfillerr;
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


    // TGraphErrors for local polarimetry qa
    TGraphErrors *locpolbf;
    TGraphErrors *locpolbb;
    TGraphErrors *locpolyf;
    TGraphErrors *locpolyb;
    TGraphErrors *locpolphasebf;
    TGraphErrors *locpolphasebb;
    TGraphErrors *locpolphaseyf;
    TGraphErrors *locpolphaseyb;

};






#endif