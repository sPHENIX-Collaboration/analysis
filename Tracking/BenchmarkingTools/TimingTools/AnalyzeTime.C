#include "../CommonTools.h"

void initializeModules(std::map<std::string,std::vector<double>>& modules);
void collectData(std::map<std::string,std::vector<double>>& modules, 
		 std::string& filename);
void plotTimes(std::map<std::string, std::vector<double>>& modules, std::string& filename);



/*
 * This root macro analyzes the output from parseTimers.csh
 * The input is a filename to the text file created from parseTimers.csh, e.g.
 * root -l -b -q AnalyzeTime.C'("time.txt")'
 * The output is a root file which contains the time of each tracking module
 * The output also produces a .png with each module's individual contribution
 * to the total time. The output rootfile has a total time graph which
 * sums the contributions
 */
void AnalyzeTime(std::string infile, std::string outfile)
{

  std::map<std::string,std::vector<double>> modules;
  initializeModules(modules);
 
  collectData(modules,infile);
    
  plotTimes(modules, outfile);
}

void plotTimes(std::map<std::string, std::vector<double>>& modules,
	       std::string& filename)
{
  ostringstream canname;
  int nbins = 200;
  double bins[nbins+1];
  double stepsize = 1.07;
  bins[0] = 1.;

  for(int i=1;i <nbins+1; i++)
    {
      bins[i]=bins[i-1]*stepsize;
    }


  std::vector<double> totalTime;
  const int totentries = modules.at("MvtxClusterizer").size();
 
  for(int i=0; i<totentries; i++)
    { totalTime.push_back(0); }

  TFile *outfile = new TFile(filename.c_str(),"recreate");
  for(const auto& [moduleName, values] : modules)
    {
      canname.str("");
      canname<<"can_"<<moduleName.c_str();
      TCanvas *can = new TCanvas(canname.str().c_str(),canname.str().c_str(),
				 200,200,800,600);
      //gStyle->SetOptStat(0);
      TH1F *histo = new TH1F(moduleName.c_str(),moduleName.c_str(),
			     nbins,bins);
      
      for(int i=0; i<values.size(); i++)
	{
	  histo->Fill(values.at(i));
	  if(i<totentries)
	    totalTime.at(i) += values.at(i);
	}

      histo->GetXaxis()->SetTitle("time [ms]");
      can->cd();
 
      gPad->SetLogx();
      
      histo->Draw();
      outfile->cd();
      histo->Write();
      myText(0.2,0.96,kBlack,moduleName.c_str());
      canname<<".png";
      //can->Print(canname.str().c_str());
    }

  TH1F *totalTimes = new TH1F("totalTime",";time [ms]",nbins,bins);
  for(const auto& val : totalTime)
    {  totalTimes->Fill(val); }

  totalTimes->Write();
  outfile->Write();
  outfile->Close();

}

void collectData(std::map<std::string,std::vector<double>>& modules, 
		 std::string& filename)
{

  ifstream file;
  file.open(filename.c_str());
  std::string line;

  if(file.is_open()) {
    while(getline(file,line)) {
      for(auto& [module, values] : modules) {
	if(line.find(module) != std::string::npos) {
	  if(line.find("TOP") != std::string::npos) {
	    const auto pos = line.find("(ms):  ");
	    const auto val = line.substr(pos+7,11);
	    values.push_back(atof(val.c_str()));
	  }
	  else if(line.find("Acts seed time") != std::string::npos) {
	    const auto apos = line.find("time ");
	    const auto aval = line.substr(apos+5,6);
	    values.push_back(atof(aval.c_str()));
	  }
	}
      }
    }
  }  
}


void initializeModules(std::map<std::string,std::vector<double>>& modules)
{
  std::vector<double> mvtx, intt, tpc, tpclean, silseed, caseed, preprop, kfprop, circlefit, clusterMover, tpcresiduals, secondtrkfit, trackmatch, trackfit, vtxfinder, vertexprop, actsseed, mm, mmmatch, dzcor;
  modules.insert(std::make_pair("MvtxClusterizer",mvtx));
  modules.insert(std::make_pair("InttClusterizer",intt));
  modules.insert(std::make_pair("TpcClusterizer",tpc));
  modules.insert(std::make_pair("MicromegasClusterizer",mm));
  modules.insert(std::make_pair("PHMicromegasTpcTrackMatching",mmmatch));
  modules.insert(std::make_pair("TpcClusterCleaner",tpclean));
  modules.insert(std::make_pair("PHActsSiliconSeeding",silseed));
  modules.insert(std::make_pair("Acts seed time",actsseed));
  modules.insert(std::make_pair("PHCASeeding",caseed));
  modules.insert(std::make_pair("PrePropagatorPHTpcTrackSeedCircleFit",preprop));
  modules.insert(std::make_pair("PHSimpleKFProp",kfprop));
  modules.insert(std::make_pair("PHTpcTrackSeedCircleFit",circlefit));
  modules.insert(std::make_pair("PHSiliconTpcTrackMatching",trackmatch));
  modules.insert(std::make_pair("PHActsFirstTrkFitter",trackfit));
  modules.insert(std::make_pair("PHSimpleVertexFinder",vtxfinder));
  modules.insert(std::make_pair("PHActsVertexPropagator",vertexprop));
  modules.insert(std::make_pair("PHTpcClusterMover",clusterMover));
  modules.insert(std::make_pair("PHTpcResiduals",tpcresiduals));
  modules.insert(std::make_pair("PHActsSecondTrkFitter",secondtrkfit));
  modules.insert(std::make_pair("PHTpcDeltaZCorrection",dzcor));
}

