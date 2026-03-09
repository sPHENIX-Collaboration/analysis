// David Stewart, Dec 1 2022
// A way to pass 
// used in conveinece for plotting
#ifndef noi_fnc__h
#define noi_fnc__h

#include "noiDict.h"
#include "noi_fmt.h"

string voi_stem(const string& path, const string& ext="") {
    // copied from https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
    std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
    std::string::size_type const p(base_filename.find_last_of('.'));
    std::string file_without_extension = base_filename.substr(0, p) + ext;
    return file_without_extension;
}


const char* noiUniqueName(int i=0,const char* prefix="__pause_") {
    while (gDirectory->FindObjectAny(Form("%s__%i",prefix,i))!=nullptr) ++i;
    return Form("%s__%i",prefix,i);
};
void noiPause(int i=0)  {
    TCanvas* c = new TCanvas( noiUniqueName(i+100), noiUniqueName(i+100), 100,100);
    c->SetFillColor(kCyan);
    c->SetFillStyle(1001);
    c->Draw();
    c->WaitPrimitive();
};

void noiDrawTLatex(const char* msg, double x, double y, 
                            noiDict options={}, noiDict dict= {{
                            "TextColor",kBlack, "TextColorAlpha",kBlack, 1., "TextSize",22, "TextFont",43,
                            "TextAlign",12, "TextAngle",0., }})
{
    dict += options;
    TLatex tlatex;
    tlatex.SetTextColorAlpha(dict("TextColorAlpha",1),dict("TextColorAlpha",2));
    tlatex.SetTextAlign(dict("TextAlign"));
    tlatex.SetTextSize (dict("TextSize"));
    tlatex.SetTextFont (dict("TextFont"));
    tlatex.SetTextAngle (dict("TextAngle"));
    tlatex.DrawLatex(x, y, msg);
};
TLine* noiDrawTLine(double x0, double y0, double x1, double y1, noiDict options, noiDict dict={{
   "LineColorAlpha",kBlack,1.0, "LineStyle",1, "LineWidth",1 }})
{
    dict += options;
    TLine* line = new TLine(x0,y0,x1,y1);
    noi_fmt(line, options);
    line->Draw();
    return line;
};
string noiStripEnd(string word, string sub) {
    auto i0  = sub.length();
    auto i1  = word.length();
    if (i1 < i0) return word;
    return (word.substr(i1-i0,i0)==sub)
        ? word.substr(0,i1-i0)
        : word;
}
string noiStripEnds(string word, vector<string> endings) {
    for (auto sub : endings) {
        word = noiStripEnd(word,sub);
    }
    return word;
};
string noiStripStart(string word, string sub) {
    auto i0  = sub.length();
    auto i1  = word.length();
    if (i0>i1) return word;
    word = (word.substr(0,i0) == sub) 
        ? word.substr(i0,i1-i0)
        : word;
    return word;
};
string noiStripStarts(string word, vector<string> startings) {
    for (auto pre : startings) {
        word = noiStripStart(word,pre);
    }
    return word;
};
const char* noi_geant05_ascii(int geantid) {
    switch (geantid) {
        case 8: return "pi";
        case 9: return "antipi";
        case 11: return "K";
        case 12: return "antiK";
        case 14: return "p";
        case 15: return "pbar";

        case 0: return "pi";
        case 1: return "antipi";
        case 2: return "K";
        case 3: return "antiK";
        case 4: return "p";
        case 5: return "pbar";
    }
    return "none";
};
int noi_geant05(int geantid) {
    switch (geantid) {
        case 8: return 0;
        case 9: return 1;
        case 11: return 2;
        case 12: return 3;
        case 14: return 4;
        case 15: return 5;
    }
    return -1;
};
const char* noi_geant05_greek(int geantid) {
    switch (geantid) {
        case 8: return "#pi";
        case 9: return "#pi^{-}";
        case 11: return "K";
        case 12: return "K^{-}";
        case 14: return "p";
        case 15: return "#bar{p}";

        case 0: return "#pi";
        case 1: return "#pi^{-}";
        case 2: return "K";
        case 3: return "K^{-}";
        case 4: return "p";
        case 5: return "#bar{p}";
    }
    return "none";
};

TH1* noiDivide(TH1* num, TH1* den, noiDict opt={}, noiDict dict={}) {
    dict += opt;

    double norm_num {1};
    double norm_den {1};

    if (dict["norm"]) {
        norm_num = num->Integral();
        norm_den = den->Integral();
    };
    if (dict["print"]) cout << " norms: " << norm_num << " and " << norm_den << endl;

    TH1D* ret;
    if (dict["style-den"]) {
        ret = (TH1D*) den->Clone(noiUniqueName());
    } else {
        ret = (TH1D*) num->Clone(noiUniqueName());
    }

    for (int j=1;j<den->GetNbinsX()+1;++j){
        double n = num->GetBinContent(j) / norm_num;
        double d = den->GetBinContent(j) / norm_den;
        if (n == 0 || d == 0) {
            ret->SetBinContent(j,0);
            ret->SetBinError(j,0);
            /* if (i==0) cout << "Jin: " << j << " val: " << den->GetBinContent(j) << endl; */
            continue;
        }
        double n_err = num->GetBinError(j) / norm_num;
        double d_err = den->GetBinError(j) / norm_den;
        double val = n / d;
        double err = val * pow( pow(n_err/n,2)+pow(d_err/d,2),0.5);
        ret->SetBinContent(j,val);
        ret->SetBinError(j,err);
    }
    return ret;
};

string noiStripExtension(const char* in) {
    string word = in;
    if (word.find(".",1) != string::npos) word = word.substr(0,word.find(".",1));
    return word;
};

TGraph* noiMakeTGraph(vector<double>& x, vector<double>& y) {
    if (x.size() != y.size()) 
        throw std::runtime_error("noiMakeTGraph(vec, vec) required vectors of same length");
    const unsigned int n = x.size();
    double* xpts = new double[n];
    double* ypts = new double[n];
    for (unsigned int i{0}; i<n; ++i) {
        xpts[i] = x[i];
        ypts[i] = y[i];
    }
    return new TGraph(n,xpts,ypts);
};
#endif
