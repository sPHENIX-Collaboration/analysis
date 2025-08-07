#include "RooFitHM.h"

// Fun4All includes
#include <fun4all/Fun4AllReturnCodes.h>
#include <qautils/QAHistManagerDef.h>
#include <phool/phool.h>  // for PHWHERE, optional

#include <fun4all/Fun4AllHistoManager.h>

#include <TF1.h>


// RooFit includes
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooLandau.h>
#include <RooGaussian.h>
#include <RooFFTConvPdf.h>

// standard library
#include <cassert>
#include <iostream>

//______________________________________________________________
RooFitHM::RooFitHM(const std::string &name)
  : SubsysReco(name)
{

}

//______________________________________________________________
int RooFitHM::Init(PHCompositeNode*)
{

    using namespace RooFit;

    RooRealVar x("x","x",0,1000);
    RooRealVar mean_landau("mean_landau","Landau MPV",100,0,1000);
    RooRealVar sigma_landau("sigma_landau","Landau #sigma",10,0.1,50);
    RooLandau landau("landau","landau PDF", x, mean_landau, sigma_landau);

    RooRealVar mean_gauss("mean_gauss","Gauss mean",0,-50,50);
    RooRealVar sigma_gauss("sigma_gauss","Gauss #sigma",5,0.1,20);
    RooGaussian gauss("gauss","gauss PDF",x,mean_gauss,sigma_gauss);

    x.setBins(1000,"cache");


    m_pdf = new RooFFTConvPdf("pdf","Landau (x) Gauss", x, landau, gauss);

    m_func = m_pdf->asTF(RooArgList(x), RooArgList());



    auto hm = QAHistManagerDef::getHistoManager();
    assert(hm);


    hm->registerHisto(m_func);

    //check if pdf is registered

    //int is_registered = hm->isHistoRegistered("pdf");

    //std::cout << "is registered = " << is_registered << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;
}

//______________________________________________________________
int RooFitHM::End(PHCompositeNode*)
{
    std::cout << "RooFitHM::End() finished.\n";
    return Fun4AllReturnCodes::EVENT_OK;
}
