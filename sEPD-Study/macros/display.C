// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TFile.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TChain.h>
#include <TMath.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <TProfile.h>
#include <TPaveStats.h>

// -- sPHENIX Style
#include <sPhenixStyle.C>

// tower mapping
#include <calobase/TowerInfoDefs.h>

// utils
#include "myUtils.C"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::unordered_map;
using std::map;
using std::unordered_set;
namespace fs = std::filesystem;

namespace myAnalysis {
    void make_plots_psi(TH1* hPsi_x, TH1* hPsi_y, TLegend* leg, TCanvas* c, Int_t n, const string &title, const string &x_title);
    Bool_t make_plots_QA(const string &input, const string &outputDir, const string &qa_file_path);
    void make_plots_Q_vec(const string &input, const string &outputDir);
    void print_stats();

    struct QA_general {
        std::unique_ptr<TH1> hEvent;
        std::unique_ptr<TH1> hCentrality;
        std::unique_ptr<TH1> hVtxZ;
        std::unique_ptr<TH1> hVtxZ_MB;
        std::unique_ptr<TH2> h2SEPD_Charge;
        std::unique_ptr<TH3> h3SEPD_Total_Charge;
    };

    struct QA_Q_vec {
        std::unique_ptr<TH3> h3SEPD_Psi_2;
        std::unique_ptr<TH3> h3SEPD_Psi_3;
        std::unique_ptr<TH3> h3SEPD_EventPlaneInfo_Psi_2;
        std::unique_ptr<TH3> h3SEPD_EventPlaneInfo_Psi_3;
    };

    map<string,Int_t> m_ctr;

    Bool_t m_saveFig = true;
}

void myAnalysis::make_plots_psi(TH1 *hPsi_x, TH1 *hPsi_y, TLegend *leg, TCanvas *c, Int_t n, const string &title, const string &x_title)
{
  c->cd(n);
  gPad->SetTopMargin(0.1f);

  Double_t y_lim_high = std::max(hPsi_x->GetMaximum(), hPsi_y->GetMaximum()) * 1.1;

  hPsi_x->Draw();
  hPsi_x->SetTitle(title.c_str());
  hPsi_x->GetYaxis()->SetTitle("Events");
  hPsi_x->GetXaxis()->SetTitle(x_title.c_str());
  hPsi_x->GetYaxis()->SetRangeUser(0, y_lim_high);
  hPsi_x->GetYaxis()->SetTitleOffset(1.6f);
  hPsi_x->GetXaxis()->SetTitleOffset(1.f);
  gStyle->SetTitleY(.97f);

  hPsi_y->Draw("same");
  hPsi_y->SetLineColor(kRed);

  stringstream legA, legB;

  legA << "South";
  legB << "North";

  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hPsi_x, legA.str().c_str(), "l");
  leg->AddEntry(hPsi_y, legB.str().c_str(), "l");
  leg->Draw("same");
}

void myAnalysis::make_plots_Q_vec(const string &input, const string &outputDir) {
    fs::path p(input);
    string stem = p.stem().string();
    string run = myUtils::split(stem,'-')[1];

    cout << "Processing: " << run << endl;
    std::unique_ptr<TFile> tfile(TFile::Open(input.c_str()));
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return;
    }

    QA_Q_vec qa =
    {
      .h3SEPD_Psi_2 = std::unique_ptr<TH3>(static_cast<TH3*>(tfile->Get("h3SEPD_Psi_2"))),
      .h3SEPD_Psi_3 = std::unique_ptr<TH3>(static_cast<TH3*>(tfile->Get("h3SEPD_Psi_3"))),
      .h3SEPD_EventPlaneInfo_Psi_2 = std::unique_ptr<TH3>(static_cast<TH3*>(tfile->Get("h3SEPD_EventPlaneInfo_Psi_2"))),
      .h3SEPD_EventPlaneInfo_Psi_3 = std::unique_ptr<TH3>(static_cast<TH3*>(tfile->Get("h3SEPD_EventPlaneInfo_Psi_3")))
    };

    std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1200, 1000);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08f);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    c1->Divide(2, 2, 0.00025f, 0.00025f);

    string output = outputDir + "/pdf/plots-" + run + ".pdf";

    Double_t xshift = 0.5;
    Double_t yshift = 0;

    std::unique_ptr<TH1> h3SEPD_Psi_2_x(qa.h3SEPD_Psi_2->Project3D("x"));
    std::unique_ptr<TH1> h3SEPD_Psi_2_y(qa.h3SEPD_Psi_2->Project3D("y"));
    std::unique_ptr<TH1> h3SEPD_Psi_3_x(qa.h3SEPD_Psi_3->Project3D("x"));
    std::unique_ptr<TH1> h3SEPD_Psi_3_y(qa.h3SEPD_Psi_3->Project3D("y"));

    std::unique_ptr<TH1> h3SEPD_EventPlaneInfo_Psi_2_x(qa.h3SEPD_EventPlaneInfo_Psi_2->Project3D("x"));
    std::unique_ptr<TH1> h3SEPD_EventPlaneInfo_Psi_2_y(qa.h3SEPD_EventPlaneInfo_Psi_2->Project3D("y"));
    std::unique_ptr<TH1> h3SEPD_EventPlaneInfo_Psi_3_x(qa.h3SEPD_EventPlaneInfo_Psi_3->Project3D("x"));
    std::unique_ptr<TH1> h3SEPD_EventPlaneInfo_Psi_3_y(qa.h3SEPD_EventPlaneInfo_Psi_3->Project3D("y"));

    std::unique_ptr<TLegend> leg  = std::make_unique<TLegend>(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    std::unique_ptr<TLegend> leg2 = std::make_unique<TLegend>(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    std::unique_ptr<TLegend> leg3 = std::make_unique<TLegend>(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    std::unique_ptr<TLegend> leg4 = std::make_unique<TLegend>(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);

    make_plots_psi(h3SEPD_Psi_2_x.get(), h3SEPD_Psi_2_y.get(), leg.get(), c1.get(), 1, "Run: " + run + " , |z| < 10 cm and MB", "2#Psi_{2}");
    make_plots_psi(h3SEPD_Psi_3_x.get(), h3SEPD_Psi_3_y.get(), leg2.get(), c1.get(), 2, "|z| < 10 cm and MB", "3#Psi_{3}");
    make_plots_psi(h3SEPD_EventPlaneInfo_Psi_2_x.get(), h3SEPD_EventPlaneInfo_Psi_2_y.get(), leg3.get(), c1.get(), 3, "EventPlaneInfo , |z| < 10 cm and MB", "2#Psi_{2}");
    make_plots_psi(h3SEPD_EventPlaneInfo_Psi_3_x.get(), h3SEPD_EventPlaneInfo_Psi_3_y.get(), leg4.get(), c1.get(), 4, "EventPlaneInfo , |z| < 10 cm and MB", "3#Psi_{3}");

    c1->Update();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/QA-Q-vec-" + run + ".png").c_str());
}

Bool_t myAnalysis::make_plots_QA(const string &input, const string &outputDir, const string &qa_file_path) {
    fs::path p(input);
    string stem = p.stem().string();
    string run = myUtils::split(stem,'-')[1];

    cout << "Processing: " << run << endl;
    std::unique_ptr<TFile> tfile(TFile::Open(input.c_str()));
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return false;
    }

    QA_general qa =
    {
      .hEvent = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hEvent"))),
      .hCentrality = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hCentrality"))),
      .hVtxZ = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hVtxZ"))),
      .hVtxZ_MB = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hVtxZ_MB"))),
      .h2SEPD_Charge = std::unique_ptr<TH2>(static_cast<TH2*>(tfile->Get("h2SEPD_Charge"))),
      .h3SEPD_Total_Charge = std::unique_ptr<TH3>(static_cast<TH3*>(tfile->Get("h3SEPD_Total_Charge")))
    };

    std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1200, 1000);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08f);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    c1->Divide(2, 2, 0.00025f, 0.00025f);

    string output = outputDir + "/pdf/plots-" + run + ".pdf";

    Bool_t is_good = true;
    Bool_t no_mb_events = false;
    Bool_t high_avg_zvtx = false;

    ++m_ctr["run_ctr"];
    Int_t events = static_cast<Int_t>(qa.hEvent->GetBinContent(3));
    m_ctr["total_mb_events_all"] += events;

    cout << "Run: " << run;
    cout << ", Events: " << events << endl;

    if(events == 0) {
        no_mb_events = true;
        is_good = false;
        ++m_ctr["no_mb_events"];
    }

    c1->cd(1);
    gPad->SetTopMargin(0.1f);

    stringstream title;
    title << "Run: " << run;

    qa.hEvent->Draw();
    qa.hEvent->GetXaxis()->SetTitleOffset(1.f);
    qa.hEvent->GetYaxis()->SetTitleOffset(1.6f);
    qa.hEvent->GetXaxis()->SetLabelSize(0.05f);
    qa.hEvent->GetYaxis()->SetRangeUser(0, qa.hEvent->GetMaximum()*1.1);
    qa.hEvent->SetTitle(title.str().c_str());
    gStyle->SetTitleY(.97f);

    c1->cd(2);
    gPad->SetTopMargin(0.1f);

    qa.hCentrality->Draw();
    qa.hCentrality->GetXaxis()->SetTitleOffset(1.f);
    qa.hCentrality->GetYaxis()->SetTitleOffset(1.6f);

    c1->Update();
    c1->cd(3);
    gPad->SetTopMargin(0.1f);

    Int_t vtx_z_rebin_factor = 4;

    qa.hVtxZ->Draw();
    qa.hVtxZ->Rebin(vtx_z_rebin_factor);
    qa.hVtxZ->GetXaxis()->SetTitleOffset(1.f);
    qa.hVtxZ->GetYaxis()->SetTitleOffset(1.6f);
    qa.hVtxZ->GetYaxis()->SetRangeUser(0, qa.hVtxZ->GetMaximum()*1.1);
    gStyle->SetTitleY(1.f);

    qa.hVtxZ_MB->Draw("same");
    qa.hVtxZ_MB->Rebin(vtx_z_rebin_factor);
    qa.hVtxZ_MB->SetLineColor(kRed);

    Double_t z_vtx_mb_avg = qa.hVtxZ_MB->GetBinCenter(qa.hVtxZ_MB->GetMaximumBin());

    if(std::fabs(z_vtx_mb_avg) >= 10) {
        high_avg_zvtx = true;
        is_good = false;
        ++m_ctr["high_avg_zvtx"];
    }

    std::unique_ptr<TLine> line = std::make_unique<TLine>(z_vtx_mb_avg, 0, z_vtx_mb_avg, qa.hVtxZ->GetMaximum()*1);
    line->Draw("same");
    line->SetLineColor(kBlue);

    stringstream legA, legB;

    legA << "All";
    legB << "MB";

    Double_t xshift = 0.5;
    Double_t yshift = 0;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.06f);
    leg->AddEntry(qa.hVtxZ.get(),legA.str().c_str(),"l");
    leg->AddEntry(qa.hVtxZ_MB.get(),legB.str().c_str(),"l");
    leg->Draw("same");

    c1->cd(4);
    gPad->SetTopMargin(0.1f);
    gPad->SetRightMargin(0.15f);
    gPad->SetLogz();
    gStyle->SetTitleY(1.f);

    std::unique_ptr<TH2> h3SEPD_Total_Charge_yx(static_cast<TH2*>(qa.h3SEPD_Total_Charge->Project3D("yx")));
    h3SEPD_Total_Charge_yx->Draw("COLZ1");
    h3SEPD_Total_Charge_yx->SetTitle("sEPD Total Charge: |z| < 10 cm and MB");
    h3SEPD_Total_Charge_yx->GetXaxis()->SetLabelSize(0.035f);
    h3SEPD_Total_Charge_yx->GetYaxis()->SetTitleOffset(1.6f);
    h3SEPD_Total_Charge_yx->GetXaxis()->SetTitleOffset(1.f);

    c1->Update();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/QA-" + run + ".png").c_str());

    // Open the CSV file for writing
    std::ofstream qa_file(qa_file_path, std::ios::app);

    // Check if the file opened successfully
    if (!qa_file.is_open()) {
        cout << "Error: Could not open output.csv for writing!" << endl;
        return false;
    }

    qa_file << run << ","
            << no_mb_events << ","
            << high_avg_zvtx << ","
            << is_good << endl;

    if(is_good) {
        m_ctr["total_mb_events_isGood"] += events;
    }

    return is_good;
}

void myAnalysis::print_stats() {
    cout << "=====================" << endl;
    cout << "Stats" << endl;
    cout << "Runs: " << m_ctr["run_ctr"] << endl;
    cout << "No MB Events: " << m_ctr["no_mb_events"]
         << ", " <<  m_ctr["no_mb_events"] * 100. / m_ctr["run_ctr"] << " %" << endl;
    cout << "Avg |z| >= 10 cm: " << m_ctr["high_avg_zvtx"]
         << ", " <<  m_ctr["high_avg_zvtx"] * 100. / m_ctr["run_ctr"] << " %" << endl;
    cout << "=====================" << endl;
    cout << "Total Events for |z| < 10 cm and MB: " << m_ctr["total_mb_events_all"] << endl;
    cout << "Total Events for |z| < 10 cm and MB and isGood: " << m_ctr["total_mb_events_isGood"] << ", " << m_ctr["total_mb_events_isGood"]*100. / m_ctr["total_mb_events_all"] << " %" << endl;
    cout << "=====================" << endl;
}

void display(const string &input, const string &outputDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: " << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "save figs: " << myAnalysis::m_saveFig << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // create output directories
    fs::create_directories(outputDir+"/QA/images");
    fs::create_directories(outputDir+"/QA/pdf");
    fs::create_directories(outputDir+"/QA-Q-vec/images");
    fs::create_directories(outputDir+"/QA-Q-vec/pdf");

    // TH1::AddDirectory(kFALSE);

    std::ifstream input_file(input);

    if (!input_file.is_open()) {
        cout << "Error: [" << input << "] Could not open file." << endl;
        return;
    }

    string line;

    string qa_file_path = outputDir+"/qa-runs.csv";
    std::ofstream qa_file(qa_file_path);
    qa_file << "run,no_mb_events,high_avg_zvtx,is_good" << endl;
    qa_file.close();

    while (std::getline(input_file, line)) {
        bool is_good = myAnalysis::make_plots_QA(line, outputDir+"/QA", qa_file_path);

        if(is_good) {
            myAnalysis::make_plots_Q_vec(line, outputDir+"/QA-Q-vec");
        }
    }

    myAnalysis::print_stats();
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./" << argv[0] << " input [outputDir]" << endl;
        cout << "input: list of root files" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 3) {
        outputDir = argv[2];
    }

    display(argv[1], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
