#ifndef __GETET_H__
#define __GETET_H__

#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <CompareTransverseEnergy.h>
#include <vector>
#include <map>
#include <string>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4allraw/SinglePrdfInput.h>
#include <fun4all/SubsysReco.h>
//#include "sPhenixStyle.h"
//#include "sPhenixStyle.C"
R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libfun4allraw.so);
R__LOAD_LIBRARY(libffarawmodules.so);

//class CompareLEDRuns
//{
  //  public:
    //private variables
        //struct
            struct towerinfo {
                bool inner_outer; //false for inner, true for outer
                bool north_south; //false for North, true for south
                int sector; 	//Sector 0-31
                int channel;	//Channels 0-23
                int packet; 	//packet is shared between 4 sectors
                int etabin;	//pseudorapidity bin
                int phibin;	//phi bin
                float eta;	//psedorapidity value
                float phi;	//phi value
                std::string label;	//label for tower to quick parse
			};
		    void BuildTowerMap();
		    static std::map < std::pair< int, int > , LEDRunData::towerinfo > towermaper; //look up table for towers


//    public: 
    //public variables, get and set methods etc..

//};

#endif
