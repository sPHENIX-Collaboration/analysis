//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Nov 23 01:L:33 2024 by ROOT version 6.26/06
// from TTree tree/sPHENIX info.
// found on file: ana439_Electron_2GeV_00150.root
//////////////////////////////////////////////////////////

#ifndef InttSeedTracking_h
#define InttSeedTracking_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <TMath.h>
#include "Fit/Fitter.h"
#include <Math/Functor.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>

#include "TArc.h"

#include "SPHTracKuma.h"


// Header file for the classes stored in the TTree if any.
#include <vector>

class InttSeedTracking {
public :
   InttSeedTracking(std::vector<tracKuma>& tracks,\
      std::vector<hitStruct > vFMvtxHits,\
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
      std::vector<hitStruct > vEmcalHits,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);
   InttSeedTracking(){};
   virtual ~InttSeedTracking();

   void HitMatching(std::vector<tracKuma>& tracks, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);

   Double_t AddHCalE(Double_t emcalPhi, Double_t emcalE,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);

   void AddMvtxHits(tracKuma& trk, std::vector<hitStruct > vFMvtxHits, \
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits, Double_t dRThre, Int_t& seleFMvtxId, Int_t& seleSMvtxId, Int_t& seleTMvtxId);


   void TrackPropertiesEstimation(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);
   Double_t TrackPtEstimation(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);


   void RoughEstiSagittaCenter3Point(Double_t& sagittaR, \
      Double_t& sagittaCenterX, Double_t& sagittaCenterY, Double_t HitsXY[3][2]);

   Double_t AccuratePtEstimation(Double_t sagittaR, Double_t centerX, Double_t centerY, tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);

   bool RecoTracksInttSeed2(std::vector<tracKuma>& tracks, std::vector<hitStruct > vFMvtxHits,\
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
      std::vector<hitStruct > vEmcalHits,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);
   bool InttSeedMatching(std::vector<tracKuma>& tracks, Int_t inttId,\
      hitStruct baseInttHit,\
      std::vector<hitStruct >& vFMvtxHits,\
      std::vector<hitStruct >& vSMvtxHits, std::vector<hitStruct >& vTMvtxHits,\
      std::vector<hitStruct >& vIInttHits,\
      std::vector<hitStruct > vEmcalHits,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits);
   Double_t EstiChiTrkOnCircle(tracKuma trk, Double_t cX, Double_t cY, Double_t sagittaR);
   bool CheckTrkRequirements(tracKuma trk);
   void  RefindCalHit(tracKuma trk, std::vector<hitStruct > vEmcalHits,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);
   void CalESumAndCorrPosi(tracKuma trk, std::vector<hitStruct > vEmcalHits,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);
   void TrackPropertiesEstimation2(tracKuma& trk);

   Int_t TempINTTIOMatching(Double_t oINTTPhi, std::vector<hitStruct > vIInttHits); //????
   Double_t TempCalcdPhidR(Int_t iInttID, Int_t oInttID, \
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits); //????
   Int_t TempInttCalMatch(Int_t iInttID, Int_t oInttID,\
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
      std::vector<hitStruct > vEmcalHits); //????

   void SagittaRByCircleFit(Double_t& cernterX, Double_t& centerY, Double_t& sagittaR,
      std::vector<Double_t > r, std::vector<Double_t > phi , Double_t oInttPhi, Double_t emcalPhi);

   bool FindHitsOnCircle(Int_t& hitMatchId, Double_t& hitX, Double_t& hitY, Double_t sagittaR, \
      Double_t centerX, Double_t centerY, std::vector<hitStruct > vHits, Double_t dRThre);

   Double_t EstimateRecoTheta(tracKuma trk, Int_t type);
   void EstiVertex(tracKuma trk);

   void ParticleIdentify(tracKuma trk);

   bool ReturnHitsRPhiVect(std::vector<Double_t >& hitR, std::vector<Double_t >& hitPhi,\
   std::vector<Int_t > subDetSet, tracKuma trk);

   void SetHitParaInTrack(tracKuma& trk, Int_t detId, hitStruct hitSt);

   // == s == Simple math equations  ##############################################

   inline Double_t CalcSagittaPt(Double_t sagittaR){
      Double_t pT = 0.3*1.4*sagittaR*0.01; //0.3 * 1.4 T *(R [cm -> m])
      return pT;
   }

   inline Double_t dPhiOInttEmcalEsti(tracKuma trk){
      Double_t xIIntt = trk.getHitR(4)*std::cos(trk.getHitPhi(4));
      Double_t yIIntt = trk.getHitR(4)*std::sin(trk.getHitPhi(4));
      Double_t xOIntt = trk.getHitR(5)*std::cos(trk.getHitPhi(5));
      Double_t yOIntt = trk.getHitR(5)*std::sin(trk.getHitPhi(5));
      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiIInttOIntt = std::atan((yOIntt-yIIntt)/(xOIntt-xIIntt));
      Double_t phiOInttEmcal = std::atan((yEmcal-yOIntt)/(xEmcal-xOIntt));

      Double_t dPhiOInttEmcal = phiOInttEmcal - phiIInttOIntt;
      
      return dPhiOInttEmcal;
   }

   inline Double_t dPhiVtxIInttEmcalEsti(tracKuma trk){
      Double_t xVtx = trk.getHitR(0)*std::cos(trk.getHitPhi(0));
      Double_t yVtx = trk.getHitR(0)*std::sin(trk.getHitPhi(0));
      Double_t xIIntt = trk.getHitR(4)*std::cos(trk.getHitPhi(4));
      Double_t yIIntt = trk.getHitR(4)*std::sin(trk.getHitPhi(4));
      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiVtxIIntt = std::atan((yIIntt-yVtx)/(xIIntt-xVtx));
      Double_t phiIInttEmcal = std::atan((yEmcal-yIIntt)/(xEmcal-xIIntt));

      Double_t dPhiIInttEmcal = phiIInttEmcal - phiVtxIIntt;
      
      return dPhiIInttEmcal;
   }

   inline Double_t dPhiVtxOInttEmcalEsti(tracKuma trk){
      Double_t xVtx = trk.getHitR(0)*std::cos(trk.getHitPhi(0));
      Double_t yVtx = trk.getHitR(0)*std::sin(trk.getHitPhi(0));
      Double_t xOIntt = trk.getHitR(5)*std::cos(trk.getHitPhi(5));
      Double_t yOIntt = trk.getHitR(5)*std::sin(trk.getHitPhi(5));
      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiVtxOIntt = std::atan((yOIntt-yVtx)/(xOIntt-xVtx));
      Double_t phiOInttEmcal = std::atan((yEmcal-yOIntt)/(xEmcal-xOIntt));

      Double_t dPhiOInttEmcal = phiOInttEmcal - phiVtxOIntt;
      
      return dPhiOInttEmcal;
   }

   inline Double_t dPhiVtxInttEmcalEsti(tracKuma trk){
      Double_t xVtx = trk.getHitR(0)*std::cos(trk.getHitPhi(0));
      Double_t yVtx = trk.getHitR(0)*std::sin(trk.getHitPhi(0));
      Double_t xIIntt = trk.getHitR(4)*std::cos(trk.getHitPhi(4));
      Double_t yIIntt = trk.getHitR(4)*std::sin(trk.getHitPhi(4));
      Double_t xOIntt = trk.getHitR(5)*std::cos(trk.getHitPhi(5));
      Double_t yOIntt = trk.getHitR(5)*std::sin(trk.getHitPhi(5));
      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiVtxIIntt = std::atan((yIIntt-yVtx)/(xIIntt-xVtx));
      Double_t phiVtxOIntt = std::atan((yOIntt-yVtx)/(xOIntt-xVtx));
      Double_t phiVtxIntt = (phiVtxOIntt - phiVtxIIntt)/2; 
      Double_t phiInttEmcal = std::atan((yEmcal-yOIntt)/(xEmcal-xOIntt));

      Double_t dPhiInttEmcal = phiInttEmcal - phiVtxIntt;
      
      return dPhiInttEmcal;
   }


   inline Double_t dPhiMvtxIInttEmcalEsti(tracKuma trk){
      Int_t numOfMvtx = 0;
      Double_t xMvtx = 0.;
      Double_t yMvtx = 0.;
      for(Int_t i = 1; i < 4; i++){
         if(trk.getHitIs(i)){
            xMvtx += trk.getHitR(i)*std::cos(trk.getHitPhi(i));
            yMvtx += trk.getHitR(i)*std::sin(trk.getHitPhi(i));
            numOfMvtx++;
         }
      }
      xMvtx /= numOfMvtx;
      yMvtx /= numOfMvtx;

      Double_t xIIntt = trk.getHitR(4)*std::cos(trk.getHitPhi(4));
      Double_t yIIntt = trk.getHitR(4)*std::sin(trk.getHitPhi(4));

      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiMvtxIntt = std::atan((yIIntt-yMvtx)/(xIIntt-xMvtx));
      Double_t phiInttEmcal = std::atan((yEmcal-yIIntt)/(xEmcal-xIIntt));

      Double_t dPhiIInttEmcal = phiInttEmcal - phiMvtxIntt;
      
      return dPhiIInttEmcal;
   }

   inline Double_t dPhiMvtxOInttEmcalEsti(tracKuma trk){
      Int_t numOfMvtx = 0;
      Double_t xMvtx = 0.;
      Double_t yMvtx = 0.;
      for(Int_t i = 1; i < 4; i++){
         if(trk.getHitIs(i)){
            xMvtx += trk.getHitR(i)*std::cos(trk.getHitPhi(i));
            yMvtx += trk.getHitR(i)*std::sin(trk.getHitPhi(i));
            numOfMvtx++;
         }
      }
      xMvtx /= numOfMvtx;
      yMvtx /= numOfMvtx;

      Double_t xOIntt = trk.getHitR(5)*std::cos(trk.getHitPhi(5));
      Double_t yOIntt = trk.getHitR(5)*std::sin(trk.getHitPhi(5));

      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiMvtxOIntt = std::atan((yOIntt-yMvtx)/(xOIntt-xMvtx));
      Double_t phiOInttEmcal = std::atan((yEmcal-yOIntt)/(xEmcal-xOIntt));

      Double_t dPhiOInttEmcal = phiOInttEmcal - phiMvtxOIntt;
      
      return dPhiOInttEmcal;
   }

   inline Double_t dPhiMvtxInttEmcalEsti(tracKuma trk){
      Int_t numOfMvtx = 0;
      Double_t xMvtx = 0.;
      Double_t yMvtx = 0.;
      for(Int_t i = 1; i < 4; i++){
         if(trk.getHitIs(i)){
            xMvtx += trk.getHitR(i)*std::cos(trk.getHitPhi(i));
            yMvtx += trk.getHitR(i)*std::sin(trk.getHitPhi(i));
            numOfMvtx++;
         }
      }
      xMvtx /= numOfMvtx;
      yMvtx /= numOfMvtx;

      Double_t xIIntt = trk.getHitR(4)*std::cos(trk.getHitPhi(4));
      Double_t yIIntt = trk.getHitR(4)*std::sin(trk.getHitPhi(4));
      Double_t xOIntt = trk.getHitR(5)*std::cos(trk.getHitPhi(5));
      Double_t yOIntt = trk.getHitR(5)*std::sin(trk.getHitPhi(5));
      Double_t xIntt = (xIIntt + xOIntt)/2;
      Double_t yIntt = (yIIntt + yOIntt)/2;

      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiMvtxIntt = std::atan((yIntt-yMvtx)/(xIntt-xMvtx));
      Double_t phiInttEmcal = std::atan((yEmcal-yIntt)/(xEmcal-xIntt));

      Double_t dPhiInttEmcal = phiInttEmcal - phiMvtxIntt;
      
      return dPhiInttEmcal;
   }


   inline Double_t FitFunctionPt(Double_t dPhi){
      Double_t pT = 0.0249291 + 0.232799/dPhi -0.000752825/(dPhi*dPhi);
      return pT;
   }

   inline Double_t FitFunctionPt_VtxIInttEmcal(Double_t dPhi){
      Double_t pT = -0.075123 + 0.241657 /dPhi + 7.81643e-05/(dPhi*dPhi);
      return pT;
   }
   
   inline Double_t FitFunctionPt_VtxOInttEmcal(Double_t dPhi){
      Double_t pT = 0.0592938 + 0.210859/dPhi - 0.00157569/(dPhi*dPhi);
      return pT;
   }

   inline Double_t FitFunctionPt_VtxInttEmcal(Double_t dPhi){
      Double_t pT = 0.089305 + 0.202456/dPhi + 0.0018411/(dPhi*dPhi);
      return pT;
   }

   inline Double_t FitFunctionPt_MVtxIInttEmcal(Double_t dPhi){
      Double_t pT = -0.125412 + 0.293345/dPhi - 0.00233657/(dPhi*dPhi);
      return pT;
   }

   inline Double_t FitFunctionPt_MVtxOInttEmcal(Double_t dPhi){
      Double_t pT = -0.0708943 + 0.269396/dPhi - 0.00158606/(dPhi*dPhi);
      return pT;
   }

   inline Double_t FitFunctionPt_MVtxInttEmcal(Double_t dPhi){
      Double_t pT = -0.0645267 + 0.268115/dPhi -0.00156531/(dPhi*dPhi);
      return pT;
   }


    inline void Set3PointsXY(Double_t (&HitXY)[3][2], tracKuma trk, Int_t type)
    {
        if(type==0)
        {
            HitXY[0][0] = 0.;
            HitXY[0][1] = 0.;
            HitXY[1][0] = trk.getHitR(4)*cos(trk.getHitPhi(4));
            HitXY[1][1] = trk.getHitR(4)*sin(trk.getHitPhi(4));
            HitXY[2][0] = trk.getHitR(5)*cos(trk.getHitPhi(5));
            HitXY[2][1] = trk.getHitR(5)*sin(trk.getHitPhi(5));
        }
        else if(type==1)
        {
            HitXY[0][0] = trk.getHitR(4)*cos(trk.getHitPhi(4));
            HitXY[0][1] = trk.getHitR(4)*sin(trk.getHitPhi(4));
            HitXY[1][0] = trk.getHitR(5)*cos(trk.getHitPhi(5));
            HitXY[1][1] = trk.getHitR(5)*sin(trk.getHitPhi(5));
            HitXY[2][0] = trk.getHitR(6)*cos(trk.getHitPhi(6));
            HitXY[2][1] = trk.getHitR(6)*sin(trk.getHitPhi(6));
        }
        else if(type==2)
        {
            HitXY[0][0] = 0.;
            HitXY[0][1] = 0.;
            HitXY[1][0] = trk.getHitR(5)*cos(trk.getHitPhi(5));
            HitXY[1][1] = trk.getHitR(5)*sin(trk.getHitPhi(5));
            HitXY[2][0] = trk.getHitR(6)*cos(trk.getHitPhi(6));
            HitXY[2][1] = trk.getHitR(6)*sin(trk.getHitPhi(6));
        }
    }

   inline void CalcPerpBis(Double_t& slope, Double_t& sector, 
      Double_t HitXY1[2], Double_t HitXY2[2]){

      Double_t tempSlope = (HitXY2[1] - HitXY1[1])/(HitXY2[0] - HitXY1[0]);
      Double_t tempSector = HitXY1[1] - tempSlope*HitXY1[0];

      Double_t centerX = (HitXY2[0] + HitXY1[0])/2;
      Double_t centerY = (HitXY2[1] + HitXY1[1])/2;

      slope = -1/tempSlope;
      sector = centerY - slope*centerX;

      return;
   }

   inline void CalcLineEq(Double_t& slope, Double_t& section,\
      Double_t x1, Double_t y1, Double_t x2, Double_t y2){
      slope = (y2 - y1)/(x2 - x1);
      section = y1 - slope*x1;
   }

   inline void CalcCrossPoint(Double_t& crossX, Double_t& crossY,\
      Double_t origSlope, Double_t origSection, Double_t nextPointX, Double_t nextPointY){
      Double_t invSlope = - (1/origSlope);      
      Double_t invSection = nextPointY - invSlope*nextPointX;
      crossX = - (invSection - origSection)/(invSlope - origSlope);
      crossY = origSlope*crossX + origSection;
   }

   inline Double_t CalcSection(Double_t x, Double_t y, Double_t slope){
      Double_t section = y - slope*x;
      return section;
   }

   inline void CrossLineCircle(Double_t& xv, Double_t& yv,\
      Double_t xc, Double_t yc, Double_t R){
      // line (0,0) -> (xc, yc): y = slope*x
      // circle: (y-yc)^2 = R^2 - (x-xc)^2
      // x^2 + 2bx + c = 0
      // -> x = -b pm sqrt(b^2 - c)
      Double_t slope = yc/xc;

      Double_t b = (xc - slope*yc)/(slope*slope + 1);
      Double_t c = (yc*yc - R*R + xc*xc)/(slope*slope + 1);

      Double_t xv1 = -b + std::sqrt(b*b - c);
      Double_t xv2 = -b - std::sqrt(b*b - c);
      Double_t yv1 = slope*xv1;
      Double_t yv2 = slope*xv2;

      if((xv1*xv1+yv1*yv1) < (xv2*xv2+yv2*yv2)) xv = xv1;
      else xv = xv2;

      yv = slope*xv;

      return;
   }

    inline Double_t CrossCircleCircle(Double_t& xcal, Double_t& ycal, Double_t xc, Double_t yc, Double_t R, Double_t phiIntt)
    {

        Double_t slope = -1*(xc/yc);
        Double_t section = (xc*xc+yc*yc-R*R)/(2*yc);

        Double_t coeff1 = (slope*section)/(slope*slope + 1);
        Double_t coeff2 = (section*section - m_ECalR*m_ECalR)/(slope*slope + 1);

        Double_t x1 = -1 * coeff1 + std::sqrt(coeff1*coeff1 - coeff2);
        Double_t x2 = -1 * coeff1 - std::sqrt(coeff1*coeff1 - coeff2);

        Double_t y1 = slope*x1 + section;
        Double_t y2 = slope*x2 + section;

        Double_t phi1 = std::atan(y1/x1);
        if((phi1 < 0)&&(x1 < 0)) phi1 += TMath::Pi();
        else if((phi1 > 0)&&(x1 < 0)) phi1 -= TMath::Pi();

        Double_t phi2 = std::atan(y2/x2);
        if((phi2 < 0)&&(x2 < 0)) phi2 += TMath::Pi();
        else if((phi2 > 0)&&(x2 < 0)) phi2 -= TMath::Pi();

        Double_t calPhi = 0.;
        if((std::abs(phi1-phiIntt)) < (std::abs(phi2-phiIntt)))
        {
            xcal = x1;
            ycal = y1;
            calPhi = phi1;
        }
        else
        {
            xcal = x2;
            ycal = y2;
            calPhi = phi2;
        }

        return calPhi;
    }

    // == e == Simple math equations  ##############################################

private:
   Double_t m_ECalR = 93.5; //surface
   // const Double_t m_ECalR = 106.; // center 106
   Double_t m_iHCalR = 127.502;
   Double_t m_oHCalR = 225.87;

   Double_t m_InttMatchPhiMin = -0.05;
   Double_t m_InttMatchPhiMax = 0.05;

   Double_t m_EcalEThre = 0.1; // GeV
   Double_t m_iHcalEThre = 5; // GeV
   Double_t m_oHcalEThre = 5; // GeV

   Double_t m_EcalSearchPhiMin = 0.;
   Double_t m_EcalSearchPhiMax = 0.;

   Double_t m_EOverPElectron = 0.2; // CheckumaDayo!! not be optimize

   std::vector<hitStruct > m_fMvtxHits;
   std::vector<hitStruct > m_sMvtxHits;
   std::vector<hitStruct > m_tMvtxHits;

   std::vector<hitStruct > m_iInttHits;
   std::vector<hitStruct > m_oInttHits;

   std::vector<hitStruct > m_emcalHits;
   std::vector<hitStruct > m_iHCalHits;
   std::vector<hitStruct > m_oHCalHits;

};



#endif