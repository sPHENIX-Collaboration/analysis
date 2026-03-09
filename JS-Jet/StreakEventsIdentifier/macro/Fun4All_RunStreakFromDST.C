// Fun4All_RunStreakFromDST.C

#ifdef __CLING__
#pragma cling add_include_path("/sphenix/u/$USER/install/include")
#endif

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libStreakEventsIdentifier.so)

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include <TError.h>

#include "streakeventsidentifier/StreakEventsIdentifier.h"

// -------- helpers --------
static std::string tolower_copy(std::string s){
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return s;
}
static bool has_suffix_ci(const std::string& s, const char* suf){
  auto ls = tolower_copy(s);
  std::string ss = tolower_copy(suf);
  return ls.size() >= ss.size() && ls.compare(ls.size()-ss.size(), ss.size(), ss) == 0;
}
static void AddFilesSmart(Fun4AllDstInputManager* mgr, const char* spec)
{
  if (!spec || !spec[0]) return;
  std::string s(spec);

  if (!s.empty() && s[0]=='@') { mgr->AddListFile(s.substr(1)); return; }

  // *.list / *.lst / *.txt => treat as list file
  if (has_suffix_ci(s, ".list") || has_suffix_ci(s, ".lst") || has_suffix_ci(s, ".txt"))
  { mgr->AddListFile(s); return; }

  // allow comma-separated list of .root files
  if (s.find(',') != std::string::npos) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
      if (!item.empty()) mgr->AddFile(item);
    }
    return;
  }

  mgr->AddFile(s);
}
// -------------------------

void Fun4All_RunStreakFromDST(const char* dst_jetcalo,
                              const char* dst_jet,
                              int   nevents      = 2,
                              const char* outprefix = "streak_fromDST",
                              float etMin_GeV   = 5.0,
                              float wetaMin     = 0.5,
                              float absTimeCutNs= 10.0,
                              float rmsMinNs    = 5.0,
                              float timeWeightEthresh= 0.10,
                              const char* pngDir= "",
                              bool  excludeStreaks = false,
                              float mbdTimeMin = -10.0,
                              float mbdTimeMax = 10.0,
                              const char* triggerBits = "24,25,26,27,36,37,38",
                              bool requireValidTiming = true)  //  trigger bits

{
  Fun4AllServer* se = Fun4AllServer::instance();


  gErrorAbortLevel = kError; 
  
  // inputs
  auto in1 = new Fun4AllDstInputManager("DST1");
  AddFilesSmart(in1, dst_jetcalo);
  se->registerInputManager(in1);

  if (dst_jet && dst_jet[0] != '\0') {
    auto in2 = new Fun4AllDstInputManager("DST2");
    AddFilesSmart(in2, dst_jet);
    se->registerInputManager(in2);
  }

  // Create module
  auto* mod = new StreakEventsIdentifier("StreakID", outprefix);
  mod->SetEnergyThreshold(etMin_GeV);
  mod->SetWetaCut(wetaMin);
  mod->SetTimingCuts(absTimeCutNs, rmsMinNs);
  mod->SetTimeWeightEthresh(timeWeightEthresh);

  //require valid timing information
  //mod->SetRequireValidTiming(true);
    mod->SetRequireValidTiming(requireValidTiming);

  // Set the exclude streaks flag
  mod->SetExcludeStreaks(excludeStreaks);
  mod->SetMbdTimingCuts(mbdTimeMin, mbdTimeMax);

  //trigger selection
  if (triggerBits && triggerBits[0] != '\0') {
    std::vector<int> trigger_bit_list;
    std::string s(triggerBits);
    std::stringstream ss(s);
    std::string item;
    
    std::cout << "\n=== Configuring Trigger Selection ===" << std::endl;
    std::cout << "Parsing trigger bits: " << triggerBits << std::endl;
    
    while (std::getline(ss, item, ',')) {
      if (!item.empty()) {
        // Trim whitespace
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        try {
          int bit = std::stoi(item);
          trigger_bit_list.push_back(bit);
          std::cout << "  Added trigger bit: " << bit << std::endl;
        } catch (...) {
          std::cout << "  [WARNING] Could not parse trigger bit: " << item << std::endl;
        }
      }
    }
    
    if (!trigger_bit_list.empty()) {
      mod->set_using_trigger_bits(trigger_bit_list);
      std::cout << "Total trigger bits configured: " << trigger_bit_list.size() << std::endl;
      std::cout << "Logic: OR (event passes if ANY bit fires)" << std::endl;
    } else {
      std::cout << "[WARNING] No valid trigger bits parsed!" << std::endl;
    }
    std::cout << "======================================\n" << std::endl;
  } else {
    std::cout << "\n[INFO] No trigger bits specified - accepting all events\n" << std::endl;
  }
  // ===== END TRIGGER SELECTION =====

  if (pngDir && pngDir[0] != '\0') mod->SetPngDir(pngDir);

  mod->SetJetContainer("AntiKt_unsubtracted_r04");
  mod->Verbosity(0);
  
  se->registerSubsystem(mod);
  se->run(nevents);
  se->End();
}
