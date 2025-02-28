// ----------------------------------------------------------------------------
// 'SBaseQAPlugin.h'
// Derek Anderson
// 11.01.2023
//
// Base class for subroutines ("plugins") to be run by the SCorrelatorQAMaker
// module.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SBASEQAPLUGIN_H
#define SCORRELATORQAMAKER_SBASEQAPLUGIN_H

// c++ utilities
#include <string>
// root libraries
#include <TFile.h>
#include <TSystem.h>
#include <TDirectory.h>

using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SBaseQAPlugin definition -------------------------------------------------

  template <class Config> class SBaseQAPlugin {

    public:

      // ctor/dtor
      SBaseQAPlugin()  {};
      ~SBaseQAPlugin() {};

      // setters
      void SetDebug(const bool debug)        {m_isDebugOn   = debug;}
      void SetOutFile(const string file)     {m_outFileName = file;}
      void SetOutDir(const string name)      {m_outDirName  = name;}
      void SetVerbosity(const uint16_t verb) {m_verbosity   = verb;}
      void SetConfig(const Config& cfg)      {m_config      = cfg;}

      // output getters
      TFile*      GetOutFile() {return m_outFile;}
      TDirectory* GetOutDir()  {return m_outDir;}

    protected:

      void InitOutput() {

        // check output file and create if needed
        //   FIXME doesn't work for files already opened yet!
        const bool doesFileExist = gSystem -> AccessPathName(m_outFileName.data());
        if (!doesFileExist) {
          m_outFile = new TFile(m_outFileName.data(), "recreate");
        } else {
          m_outFile = new TFile(m_outFileName.data(), "update");
        }

        // create output directory if needed
        const bool doesDirExist = m_outFile -> cd(m_outDirName.data());
        if (!doesDirExist) {
          m_outDir = (TDirectory*) m_outFile -> mkdir(m_outDirName.data());
        } else {
          m_outDir = (TDirectory*) m_outFile -> GetDirectory(m_outDirName.data());
        }
        return;
      };  // end 'InitOutput()'

      void CloseOutput() {

        // close output if still open
        //   FIXME needs care not to close files that should still be open!
        const bool isFileOpen = m_outFile -> IsOpen();
        if (isFileOpen) {
          m_outFile -> cd();
          m_outFile -> Close();
        }
        return;

      };  // end 'CloseOutput()'

      // io members
      TFile*      m_outFile = NULL;
      TDirectory* m_outDir  = NULL;

      // atomic members
      bool     m_isDebugOn   = false;
      string   m_outFileName = "";
      string   m_outDirName  = "";
      uint16_t m_verbosity   = 0;

      // routine configuration
      Config m_config;

  };  // end SBaseQAPlugin Definition

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
