
// light weight code for MBD info
// \author Ejiro Umaka
#include "MBDinfo.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <mbd/MbdGeom.h>
#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

#include <TFile.h>
#include <TH3F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TNtuple.h>


#include <cassert>
#include <fstream>
#include <TGraph.h>
#include <vector>
#include <TComplex.h>


//____________________________________________________________________________..
MBDinfo::MBDinfo(const std::string &name):
 SubsysReco(name)
{
  _event = 0;
  _outfile_name = "MBDinfo.root";

}

//____________________________________________________________________________..
MBDinfo::~MBDinfo()
{
  
}

//____________________________________________________________________________..
int MBDinfo::Init(PHCompositeNode *topNode)
{
    
    std::cout << PHWHERE << " Opening file " << _outfile_name << std::endl;

    PHTFileServer::get().open(_outfile_name, "RECREATE");
    PHTFileServer::get().cd(_outfile_name);
    mbd = new TNtuple("mbd","mbd", "mbd_total_charge:mbd_south_total_charge:mbd_north_total_charge:mbd_south_raw_psi:mbd_north_raw_psi");
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MBDinfo::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MBDinfo::process_event(PHCompositeNode *topNode)
{

    _event++;
    
    MbdPmtContainer *mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
     if (!mbdpmts)
     {
         std::cout << PHWHERE << "::ERROR - cannot find MbdPmtContainer" << std::endl;
         exit(-1);
     }
     
     MbdGeom *mbdgeom = findNode::getClass<MbdGeom>(topNode, "MbdGeom");
     if (!mbdgeom)
     {
         std::cout << PHWHERE << "::ERROR - cannot find MbdGeom" << std::endl;
         exit(-1);
     }


    _f.clear();
   
    float mbd_e_south = 0.;
    float mbd_e_north = 0.;
    float mbdQ = 0.;
    TComplex Q2_north(0.0,0.0);
    TComplex Q2_south(0.0,0.0);
       
    //for (int ipmt = 0; ipmt < mbdpmts->get_npmt(); ipmt++) //issue with container size returning 0
    for (int ipmt = 0; ipmt < 128; ipmt++)
    {
        float mbd_q = mbdpmts->get_pmt(ipmt)->get_q();
        float phi = mbdgeom->get_phi(ipmt);
        int arm = mbdgeom->get_arm(ipmt);
        mbdQ += mbd_q;

        if (arm == 0)
        {
            mbd_e_south += mbd_q;
            Q2_south += TComplex(mbd_q*cos(2*phi),mbd_q*sin(2*phi));
        }
        
        else if (arm == 1)
        {
            mbd_e_north += mbd_q;
            Q2_north += TComplex(mbd_q*cos(2*phi),mbd_q*sin(2*phi));

        }
    }

    if((mbd_e_south > 0.0) && (mbd_e_north > 0.0))
    {    
     _f.push_back(mbdQ);
     _f.push_back(mbd_e_south);
     _f.push_back(mbd_e_north);
     _f.push_back(atan2(Q2_south.Im(),Q2_south.Re())/2.0);
     _f.push_back(atan2(Q2_north.Im(),Q2_north.Re())/2.0);
      StoreMBDInfo(_f);
    } 

    std::cout<<_event<<std::endl;
    _f.clear();

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MBDinfo::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MBDinfo::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MBDinfo::End(PHCompositeNode *topNode)
{
    PHTFileServer::get().cd(_outfile_name);
    PHTFileServer::get().write(_outfile_name);

    return Fun4AllReturnCodes::EVENT_OK;

}

//____________________________________________________________________________..
int MBDinfo::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}


double MBDinfo::StoreMBDInfo(std::vector < float > _m)
{
     
    for (int i = 0; i < 5; i++)
    {
        mtower_info[i] = _m[i];
    }
 
    
    mbd->Fill(mtower_info);

    return 0;

}
