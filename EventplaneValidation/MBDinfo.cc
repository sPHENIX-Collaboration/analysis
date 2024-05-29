//Author Ejiro Umaka
//sample code for extracting MBD eventplane info

#include "MBDinfo.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <eventplaneinfo/Eventplaneinfo.h>
#include <eventplaneinfo/EventplaneinfoMap.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <centrality/CentralityInfo.h>
#include <centrality/CentralityInfov1.h>


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
    mbd = new TNtuple("mbd","mbd", "vertex:cent:tQ:sQ:nQ:spsi1:spsi1_cor:spsi2:spsi2_cor:spsi3:spsi3_cor:npsi1:npsi1_cor:npsi2:npsi2_cor:npsi3:npsi3_cor");
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
    _f.clear();
  
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
  

    GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
    if (!vertexmap)
    {
      std::cout << PHWHERE << " Fatal Error - GlobalVertexMap node is missing" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
     
   if (!(vertexmap->empty()))
    {
        GlobalVertex *vtx = vertexmap->begin()->second;
        if (vtx)
        {
           thisvertex = vtx->get_z();
        }
    }
   
  _f.push_back(thisvertex);


if(_dosim){
      CentralityInfov1 *cent = findNode::getClass<CentralityInfov1>(topNode, "CentralityInfo");
      if (!cent)
      {
          std::cout << "can't find CentralityInfo node " << std::endl;
          exit(1);

      }
  
    float cent_index = cent->get_centile(CentralityInfo::PROP::bimp);
    _f.push_back(cent_index);
}
else
{
  _f.push_back(-990.0);
}  

   
    float mbd_e_south = 0.;
    float mbd_e_north = 0.;
    float mbdQ = 0.;
 
       
    for (int ipmt = 0; ipmt < 128; ipmt++)
    {
        float mbd_q = mbdpmts->get_pmt(ipmt)->get_q();
        int arm = mbdgeom->get_arm(ipmt);
        mbdQ += mbd_q;

        if (arm == 0)
        {
            mbd_e_south += mbd_q;
        }
        
        else if (arm == 1)
        {
            mbd_e_north += mbd_q;

        }
    }

        
   
    double sep1 = NAN;
    double nep1 = NAN;
    double sep_shift1 = NAN;
    double nep_shift1 = NAN;
    
    
    double sep2 = NAN;
    double nep2 = NAN;
    double sep_shift2 = NAN;
    double nep_shift2 = NAN;
    
        
    double sep3 = NAN;
    double nep3 = NAN;
    double sep_shift3 = NAN;
    double nep_shift3 = NAN;
    
    
    EventplaneinfoMap *epmap = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap");
    if (!epmap)
    {
       std::cout << PHWHERE << "::ERROR - cannot find EventplaneinfoMap" << std::endl;
         exit(-1);
    }
    
    if (epmap->empty())
    {
//      std::cout << "Empty epmap - this event falls outside abs(60cm) or has no determined vertex" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
   
    auto mbds = epmap->get(EventplaneinfoMap::MBDS);
    
    sep1 = mbds->get_psi(1);
    sep2 = mbds->get_psi(2);
    sep3 = mbds->get_psi(3);
    sep_shift1 = mbds->get_shifted_psi(1);
    sep_shift2 = mbds->get_shifted_psi(2);
    sep_shift3 = mbds->get_shifted_psi(3);
    
    auto mbdn = epmap->get(EventplaneinfoMap::MBDN);
    
    nep1 = mbdn->get_psi(1);
    nep2 = mbdn->get_psi(2);
    nep3 = mbdn->get_psi(3);
    nep_shift1 = mbdn->get_shifted_psi(1);
    nep_shift2 = mbdn->get_shifted_psi(2);
    nep_shift3 = mbdn->get_shifted_psi(3);

     _f.push_back(mbdQ);
     _f.push_back(mbd_e_south);
     _f.push_back(mbd_e_north);
 
    _f.push_back(sep1);
    _f.push_back(sep_shift1);

    _f.push_back(sep2);
    _f.push_back(sep_shift2);

    _f.push_back(sep3);
    _f.push_back(sep_shift3);
    
    
    _f.push_back(nep1);
    _f.push_back(nep_shift1);

    _f.push_back(nep2);
    _f.push_back(nep_shift2);

    _f.push_back(nep3);
    _f.push_back(nep_shift3);
      
    StoreMBDInfo(_f);
 
    std::cout<<_event<<std::endl;
    
     //QVector info
    
    //std::pair<double, double> Qsouth;
    //Qsouth = mbds->get_qvector(2);// second order qvector in south
    
//    std::pair<double, double> Qnorth;
//    Qnorth = mbdn->get_qvector(2);// second order qvector in north
    
    //float qxsouth = Qsouth.first;
    //float qysouth = Qsouth.second;

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
     
    for (int i = 0; i < 17; i++)
    {
        mtower_info[i] = _m[i];
    }
 
    
    mbd->Fill(mtower_info);

    return 0;

}
