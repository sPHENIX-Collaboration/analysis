// InttSeedTracking
//   ├── InttSeedTracking::InttSeedTracking (构造函数)
//   │     ├── HitMatching (未使用，代码注释)
//   │     ├── RecoTracksInttSeed2 (主重建流程)
//   │     └── TrackPropertiesEstimation2 (轨迹属性估算)
//   ├── InttSeedTracking::~InttSeedTracking (析构函数)
//   │     ├── 清理数据
//   │     └── 收缩内存
//   ├── HitMatching (轨迹匹配流程)
//   ├── RecoTracksInttSeed2
//   │     ├── 遍历 oINTT 数据并进行匹配
//   │     └── 遍历 iINTT 数据并进行匹配
//   ├── InttSeedMatching (匹配算法核心)
//   │     ├── Phi 和 Theta 范围检查
//   │     ├── dR 和 Chi² 优化
//   │     └── 校验轨迹有效性
//   ├── TrackPropertiesEstimation2 (轨迹属性估算)
//   │     ├── 估算 pT (CalcSagittaPt)
//   │     ├── 重建角度 (EstimateRecoTheta)
//   │     ├── 顶点估算 (EstiVertex)
//   │     └── 粒子鉴别 (ParticleIdentify)
//   ├── 其他辅助功能
//   │     ├── TempINTTIOMatching (内外层匹配)
//   │     ├── TempCalcdPhidR (计算 ΔPhi/ΔR)
//   │     ├── AddMvtxHits (添加 MVTX 命中点)
//   │     ├── RoughEstiSagittaCenter3Point (粗略圆拟合)
//   │     ├── AccuratePtEstimation (精确动量估算)
//   │     ├── ReturnHitsRPhiVect (命中点极坐标返回)
//   │     └── ParticleIdentify (粒子属性)

#ifndef InttSeedTracking_cxx
#define InttSeedTracking_cxx


#include <TStyle.h>
#include <TCanvas.h>

#include "SPHTracKuma.h"

InttSeedTracking::InttSeedTracking(std::vector<tracKuma>& tracks,\
   std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits)
{
   // reco way1
   // HitMatching(tracks, vFMvtxHits, vSMvtxHits, vTMvtxHits, vIInttHits, vOInttHits,\
   //    vEmcalHits, vIHCalHits, vOHCalHits);
   // for(Int_t iTrk = 0; iTrk < tracks.size(); iTrk++){
   //    TrackPropertiesEstimation(tracks.at(iTrk), vFMvtxHits, vSMvtxHits, vTMvtxHits);
   // }

   // reco way2
   RecoTracksInttSeed2(tracks, vFMvtxHits, vSMvtxHits, vTMvtxHits,\
      vIInttHits, vOInttHits, vEmcalHits, vIHCalHits, vOHCalHits);
   for(Int_t iTrk = 0; iTrk < tracks.size(); iTrk++){
      TrackPropertiesEstimation2(tracks.at(iTrk));
   }

   // Vertex estimation using all tracks
   // VertexFinder(); // 

   // Re-finding tracks for single INTT hits or only mvtx using estimated vertex
   // ReFindTrack(); // 

}

InttSeedTracking::~InttSeedTracking()
{
   m_fMvtxHits.clear();
   m_sMvtxHits.clear();
   m_tMvtxHits.clear();
   m_iInttHits.clear();
   m_oInttHits.clear();
   m_emcalHits.clear();
   m_iHCalHits.clear();
   m_oHCalHits.clear();

   m_fMvtxHits.shrink_to_fit();
   m_sMvtxHits.shrink_to_fit();
   m_tMvtxHits.shrink_to_fit();
   m_iInttHits.shrink_to_fit();
   m_oInttHits.shrink_to_fit();
   m_emcalHits.shrink_to_fit();
   m_iHCalHits.shrink_to_fit();
   m_oHCalHits.shrink_to_fit();
}


void InttSeedTracking::HitMatching(std::vector<tracKuma>& tracks,\
   std::vector<hitStruct > vFMvtxHits, std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits)
{
    Int_t numOfInttClus = vOInttHits.size();
    if(numOfInttClus == 0) return;

    for(Int_t iOInttClus = 0; iOInttClus < numOfInttClus; iOInttClus++)
    {
        Double_t oInttPhi = vOInttHits.at(iOInttClus).phi;

        Int_t closetIINTTID = TempINTTIOMatching(oInttPhi, vIInttHits);
        if(closetIINTTID == 9999) continue;      
        Int_t matchEcalID = TempInttCalMatch(closetIINTTID, iOInttClus, vIInttHits, vOInttHits, vEmcalHits);
        if(matchEcalID == 9999) continue;

        tracKuma trk;
        Double_t tempTheta = 0.;

        trk.setHitIs(0, true);
        trk.setHitR(0, 0.);
        trk.setHitZ(0, 0.);
        trk.setHitPhi(0, 0.);
        trk.setHitTheta(0, 0.);

        trk.setHitIs(4, true);
        trk.setHitR(4, vIInttHits.at(closetIINTTID).r);
        trk.setHitZ(4, vIInttHits.at(closetIINTTID).z);
        trk.setHitPhi(4, vIInttHits.at(closetIINTTID).phi);
        tempTheta = 2*atan(std::exp(-vIInttHits.at(closetIINTTID).eta));
        trk.setHitTheta(4, tempTheta);

        trk.setHitIs(5, true);
        trk.setHitR(5, vOInttHits.at(iOInttClus).r);
        trk.setHitZ(5, vOInttHits.at(iOInttClus).z);
        trk.setHitPhi(5, vOInttHits.at(iOInttClus).phi);
        tempTheta = 2*atan(std::exp(-vOInttHits.at(iOInttClus).eta));
        trk.setHitTheta(5, tempTheta);

        trk.setHitIs(6, true);
        trk.setHitR(6, vEmcalHits.at(matchEcalID).r);
        trk.setHitZ(6, vEmcalHits.at(matchEcalID).z);
        trk.setHitPhi(6, vEmcalHits.at(matchEcalID).phi);
        tempTheta = 2*atan(std::exp(-vEmcalHits.at(matchEcalID).eta));
        trk.setHitTheta(6, tempTheta);

        Double_t calE = vEmcalHits.at(matchEcalID).energy;
        calE = AddHCalE(vEmcalHits.at(matchEcalID).phi, calE, vIHCalHits, vOHcalHits); // kuma???
        trk.setTrackE(calE);

        tracks.push_back(trk);
        // m_tracks.push_back(trk);
    }
}

// == s == new tracking algorithm =====================
bool InttSeedTracking::RecoTracksInttSeed2(std::vector<tracKuma>& tracks,\
   std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits){
   
   std::vector<hitStruct > vRemainFMvtxHits;
   std::vector<hitStruct > vRemainSMvtxHits;
   std::vector<hitStruct > vRemainTMvtxHits;
   std::vector<hitStruct > vRemainIInttHits;
   std::copy(vFMvtxHits.begin(), vFMvtxHits.end(), std::back_inserter(vRemainFMvtxHits));
   std::copy(vSMvtxHits.begin(), vSMvtxHits.end(), std::back_inserter(vRemainSMvtxHits));
   std::copy(vTMvtxHits.begin(), vTMvtxHits.end(), std::back_inserter(vRemainTMvtxHits));
   std::copy(vIInttHits.begin(), vIInttHits.end(), std::back_inserter(vRemainIInttHits));
   
   for(Int_t iOInttClus = 0; iOInttClus < vOInttHits.size(); iOInttClus++){
      InttSeedMatching(tracks, 5, vOInttHits.at(iOInttClus),\
         vRemainFMvtxHits, vRemainSMvtxHits, vRemainTMvtxHits, vRemainIInttHits,\
            vEmcalHits, vIHCalHits, vOHCalHits);
   }// == e == oIntt Loop
   
   // ChecKumaDaYo!!!! I cannot use this function because it makes too many tracks.
   // This function is to recover tracks without oINTT 
   // I think to use it, you need to remove used MVTX hits for upper algorithm.
   // On the other hand, it will use long time...
   for(Int_t iIInttClus = 0; iIInttClus < vRemainIInttHits.size(); iIInttClus++){
      InttSeedMatching(tracks, 4, vRemainIInttHits.at(iIInttClus),\
         vRemainFMvtxHits, vRemainSMvtxHits, vRemainTMvtxHits,\
            vRemainIInttHits, vEmcalHits, vIHCalHits, vOHCalHits);
   }

   return true;
}

bool InttSeedTracking::InttSeedMatching(std::vector<tracKuma>& tracks, Int_t inttId,\
   hitStruct baseInttHit,\
   std::vector<hitStruct >& vFMvtxHits,\
   std::vector<hitStruct >& vSMvtxHits, std::vector<hitStruct >& vTMvtxHits,\
   std::vector<hitStruct >& vIInttHits, std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits)
{
    Double_t inttTheta = 2*atan(std::exp(-baseInttHit.eta));
    Double_t inttPhi = baseInttHit.phi;
    Double_t inttR = baseInttHit.r;
    
    std::vector<tracKuma > v_tempTracks;
    tracKuma bestTrk;
    Int_t matchFMvtxId = 99999;
    Int_t matchSMvtxId = 99999;
    Int_t matchTMvtxId = 99999;
    Int_t matchIInttId = 99999;
    Double_t smallestChi = 9999.;
    Int_t matchiECalID = 9999;
    for(Int_t iECalT = 0; iECalT < vEmcalHits.size(); iECalT++)
    {
        Double_t ecalE = vEmcalHits.at(iECalT).energy;
        if(ecalE < m_EcalEThre) continue; // 能量阈值的筛选，太低能量的hit不要

        Double_t ecalTheta = 2*atan(std::exp(-vEmcalHits.at(iECalT).eta));
        // ChecKumaDaYo!!! you need to optimize the search range
        if((inttTheta - TMath::Pi()/10 < ecalTheta)&&(ecalTheta < inttTheta + TMath::Pi()/10)) // emcal eta match
        {
            Int_t minChiSqrtEMCalTrkId = 9999;
            Double_t minChiSqrt = 9999.;
            Double_t ecalPhi = vEmcalHits.at(iECalT).phi;
            // ChecKumaDaYo!!! you need to optimize the search range
            if((inttPhi - TMath::Pi()/10 < ecalPhi)&&(ecalPhi < inttPhi + TMath::Pi()/10)) // emcal phi match
            {
                tracKuma tempTrack;

                tempTrack.setHitIs(0, true); // vertex set
                tempTrack.setHitR(0, 0.);
                tempTrack.setHitZ(0, 0.);
                tempTrack.setHitPhi(0, 0.);
                tempTrack.setHitTheta(0, 0.);

                SetHitParaInTrack(tempTrack, inttId, baseInttHit); // set INTT parameters, inttid 4-iintt, 5-oIntt
                SetHitParaInTrack(tempTrack, 6, vEmcalHits.at(iECalT)); // set ECal parameters

                Double_t HitsXY[3][2];
                Set3PointsXY(HitsXY, tempTrack, 2); // convert r, phi -> x, y for the three points.
                Double_t sagittaR = 9999.;
                Double_t cX = 0.;
                Double_t cY = 0.;
                // estimate the circle using three points roughly
                RoughEstiSagittaCenter3Point(sagittaR, cX, cY, HitsXY);
                tempTrack.setTrackSagR(sagittaR);
                tempTrack.setTrackSagX(cX);
                tempTrack.setTrackSagY(cY);

                // 先match emcal，再match iintt吗？
                Int_t iInttMatchId = 9999;
                if(inttId == 5)
                {
                    Double_t iInttClusX = 9999.;
                    Double_t iInttClusY = 9999.;
                    Double_t iInttDRThre = 1.0;
                    // searching for the best iINTT hit satisfying dR threshold
                    // ChecKumaDaYo!!! you should optimize the dR threshold iInttDRThre
                    if(FindHitsOnCircle(iInttMatchId, iInttClusX, iInttClusY, sagittaR, cX, cY, vIInttHits, iInttDRThre))
                    {         
                        SetHitParaInTrack(tempTrack, 4, vIInttHits.at(iInttMatchId));
                    }      
                }

                // searching for the best MVTX hits satisfying dR threshold
                // ChecKumaDaYo!!! you should optimize the dR threshold dRThre_Mvtx
                Int_t seleFMvtxId = 99999;
                Int_t seleSMvtxId = 99999;
                Int_t seleTMvtxId = 99999;
                Double_t dRThre_Mvtx = 5.;
                AddMvtxHits(tempTrack, vFMvtxHits, vSMvtxHits, vTMvtxHits, dRThre_Mvtx, seleFMvtxId, seleSMvtxId, seleTMvtxId);
                Double_t dRChi = EstiChiTrkOnCircle(tempTrack, cX, cY, sagittaR);
                if(minChiSqrt > dRChi)
                {
                    minChiSqrt = dRChi;
                    bestTrk = tempTrack;
                    matchFMvtxId = seleFMvtxId;
                    matchSMvtxId = seleSMvtxId;
                    matchTMvtxId = seleTMvtxId;
                    matchIInttId = iInttMatchId;
                }
            }// == e == Ecal Phi range if
        }// == e == Ecal Theta range if
    }// == e == Ecal Loop
    
    // Judge the reconstructed track is available or not.
    // The requirements: 
    // (1) It has both iINTT and oINTT hits
    // (2) Single INTT and two MVTX hits
    if(!CheckTrkRequirements(bestTrk)) return false;
    RefindCalHit(bestTrk, vEmcalHits, vIHCalHits, vOHCalHits);
    tracks.push_back(bestTrk);
    if(matchFMvtxId!=99999) vFMvtxHits.erase(vFMvtxHits.begin() + matchFMvtxId);
    if(matchSMvtxId!=99999) vSMvtxHits.erase(vSMvtxHits.begin() + matchSMvtxId);
    if(matchTMvtxId!=99999) vTMvtxHits.erase(vTMvtxHits.begin() + matchTMvtxId);
    if((inttId == 5)&&(matchIInttId!=9999)) vIInttHits.erase(vIInttHits.begin() + matchIInttId);

    return true;
}


Double_t InttSeedTracking::EstiChiTrkOnCircle(tracKuma trk,\
   Double_t cX, Double_t cY, Double_t sagittaR){
   Double_t chi = 0.;
   Double_t numOfHits = 0.;
   for(Int_t iHit = 0; iHit < 7; iHit++){
      Double_t hitX = trk.getHitR(iHit)*cos(trk.getHitPhi(iHit));
      Double_t hitY = trk.getHitR(iHit)*sin(trk.getHitPhi(iHit));

      if(!trk.getHitIs(iHit)) continue;
      chi += std::sqrt((hitX - cX)*(hitX - cX) + (hitY - cY)*(hitY - cY)) - sagittaR;
      numOfHits++;
   }
   chi /= numOfHits;
   
   return chi;
}

// ointt + emcal + iintt/mvtx 3points
bool InttSeedTracking::CheckTrkRequirements(tracKuma trk)
{
    if(trk.getHitIs(4)) return true; // iintt
    Int_t numMvtx = 0;
    for(Int_t iMvtx = 1; iMvtx < 4; iMvtx++) if(trk.getHitIs(iMvtx)) numMvtx += 1; // mvtx

    if(numMvtx > 1) return true;
    else return false; 
}

void InttSeedTracking::RefindCalHit(tracKuma trk, std::vector<hitStruct> vEmcalHits, std::vector<hitStruct> vIHCalHits, std::vector<hitStruct> vOHcalHits)
{
    std::vector<Int_t> subDetIds = {1, 2, 3, 4, 5};
    std::vector<Double_t> vHitR = {};
    std::vector<Double_t> vHitsPhi = {};
    if(!ReturnHitsRPhiVect(vHitR, vHitsPhi, subDetIds, trk)) return;
    Double_t cX = 0.;
    Double_t cY = 0.;
    Double_t sagittaR = 0.;
    Double_t tempHitIInttPhi = trk.getHitPhi(4);
    Double_t tempHitEmcalPhi = trk.getHitPhi(6);
    // ChecKumaDaYo!!! it does not work in only sPHENIX server?????
    // SagittaRByCircleFit(cX, cY, sagittaR, vHitR, vHitsPhi, trk.getHitPhi(4), trk.getHitPhi(6));

    Double_t targetCalX = 0.;
    Double_t targetCalY = 0.;
    CrossCircleCircle(targetCalX, targetCalY, cX, cY, sagittaR, trk.getHitPhi(4));
    Double_t calHighestE = 0.;
    Int_t highestECalId = 99999;
    for(Int_t iECalT = 0; iECalT < vEmcalHits.size(); iECalT++)
    {
        Double_t ecalE = vEmcalHits.at(iECalT).energy;
        if(ecalE < m_EcalEThre) continue;
        if(ecalE < calHighestE) continue;

        Double_t tempEcalPhi = vEmcalHits.at(iECalT).phi;
        Double_t targetECalPhi = std::tan(targetCalY/targetCalX);
        if((targetECalPhi < 0)&&(cX < 0)) targetECalPhi += TMath::Pi();
        else if((targetECalPhi > 0)&&(cX < 0)) targetECalPhi -= TMath::Pi();
        if((targetECalPhi - 0.05 < tempEcalPhi)&&(tempEcalPhi < targetECalPhi + 0.05))
        {
            calHighestE = ecalE;
            highestECalId = iECalT;
        }
    }
    
    if(highestECalId == 99999) return;
    trk.setHitIs(6, true);
    trk.setHitR(6, vEmcalHits.at(highestECalId).r);
    trk.setHitZ(6, vEmcalHits.at(highestECalId).z);
    trk.setHitPhi(6, vEmcalHits.at(highestECalId).phi);
    Double_t ecalTheta = 2*atan(std::exp(-vEmcalHits.at(highestECalId).eta));
    trk.setHitTheta(6, ecalTheta);
    
    // ChecKumaDaYo!!!
    CalESumAndCorrPosi(trk, vEmcalHits, vIHCalHits, vOHcalHits);

    // Double_t calE = vEmcalHits.at(highestECalId).energy;
    // // ChecKumaDaYo!!!
    // calE = AddHCalE(vEmcalHits.at(highestECalId).phi, calE, vIHCalHits, vOHcalHits);
    // trk.setTrackE(calE);

}

// 对emcalhit取加权平均(w-energy)，energy加上ihcal和ohcal
void InttSeedTracking::CalESumAndCorrPosi(tracKuma trk, std::vector<hitStruct> vEmcalHits,\
    std::vector<hitStruct> vIHCalHits, std::vector<hitStruct> vOHCalHits)
{
    Double_t refCalPhi = trk.getHitPhi(6);
    Double_t refCalTheta = trk.getHitTheta(6);
    Double_t TotEMCalE = 0.;
    Double_t ModifEMCalPhi = 0.;
    Double_t ModifEMCalTheta = 0.;

    // 加权平均算phi，theta
    for(Int_t iEmcal = 0; iEmcal < vEmcalHits.size(); iEmcal++)
    {
        Double_t hitTheta = 2*atan(std::exp(-vEmcalHits.at(iEmcal).eta));
        if((hitTheta < refCalTheta - TMath::Pi()/20)&&(refCalTheta + TMath::Pi()/20 < hitTheta)) continue;
        if((vEmcalHits.at(iEmcal).phi < refCalPhi - TMath::Pi()/20)&&(refCalPhi + TMath::Pi()/20 < vEmcalHits.at(iEmcal).phi)) continue;
        
        TotEMCalE += vEmcalHits.at(iEmcal).energy;
        ModifEMCalPhi += vEmcalHits.at(iEmcal).energy*vEmcalHits.at(iEmcal).phi;
        ModifEMCalTheta += vEmcalHits.at(iEmcal).energy*hitTheta;
    }
    ModifEMCalPhi /= TotEMCalE;
    ModifEMCalTheta /= TotEMCalE;

    refCalPhi = ModifEMCalPhi;
    for(Int_t iIHcal = 0; iIHcal < vIHCalHits.size(); iIHcal++)
    {
        Double_t hitTheta = 2*atan(std::exp(-vIHCalHits.at(iIHcal).eta));
        if((hitTheta < refCalTheta - TMath::Pi()/20)&&(refCalTheta + TMath::Pi()/20 < hitTheta)) continue;
        if((vIHCalHits.at(iIHcal).phi < refCalPhi - TMath::Pi()/20)&&(refCalPhi + TMath::Pi()/20 < vIHCalHits.at(iIHcal).phi)) continue;
        
        TotEMCalE += vIHCalHits.at(iIHcal).energy;
    }

    for(Int_t iOHcal = 0; iOHcal < vOHCalHits.size(); iOHcal++)
    {
        Double_t hitTheta = 2*atan(std::exp(-vOHCalHits.at(iOHcal).eta));
        if((hitTheta < refCalTheta - TMath::Pi()/20)&&(refCalTheta + TMath::Pi()/20 < hitTheta)) continue;
        if((vOHCalHits.at(iOHcal).phi < refCalPhi - TMath::Pi()/20)&&(refCalPhi + TMath::Pi()/20 < vIHCalHits.at(iOHcal).phi)) continue;
        TotEMCalE += vIHCalHits.at(iOHcal).energy;
    }
    trk.setHitPhi(6, ModifEMCalPhi);
    trk.setHitTheta(6, ModifEMCalTheta);
    trk.setTrackE(TotEMCalE);
}

void InttSeedTracking::TrackPropertiesEstimation2(tracKuma& trk){
   Double_t dPhiOInttEmcal = dPhiOInttEmcalEsti(trk);
   Double_t recoPt = CalcSagittaPt(trk.getTrackSagR());
   // Double_t recoPt = FitFunctionPt(dPhiOInttEmcal);
   trk.setTrackPt(recoPt);
   
   Double_t recoTheta = EstimateRecoTheta(trk, 1);
   trk.setTrackTheta(recoTheta);

   Double_t recoP = recoPt/sin(recoTheta);
   trk.setTrackP(recoP);

   EstiVertex(trk);

   ParticleIdentify(trk);
}
// == e == new tracking algorithm =====================


Int_t InttSeedTracking::TempINTTIOMatching(Double_t oINTTPhi, std::vector<hitStruct > vIInttHits){
   Double_t minDeltaPhi = 9999.;
   Int_t closestIINTTCluID = 9999;
   Int_t numOfIInttClu = vIInttHits.size();

   if(numOfIInttClu == 0) return 9999;
   for(Int_t iINTTClu = 0; iINTTClu < numOfIInttClu; iINTTClu++){
      Double_t iInttPhi =  vIInttHits.at(iINTTClu).phi;
      Double_t dPhi = std::abs(iInttPhi - oINTTPhi);
      if(std::abs(dPhi) > TMath::Pi()) dPhi += (-1) * (dPhi/std::abs(dPhi)) * 2*TMath::Pi();

      // checkumaDAYO!!! need to optimize the matching range.
      if((minDeltaPhi > dPhi)&&(dPhi > m_InttMatchPhiMin)&&(dPhi < m_InttMatchPhiMax)){
         minDeltaPhi = dPhi;
         closestIINTTCluID = iINTTClu;
      }
   }
   return closestIINTTCluID;
}


Double_t InttSeedTracking::TempCalcdPhidR(Int_t iInttID, Int_t oInttID,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits){
   Double_t dPhidR = 0.;
   Double_t iInttPhi = vIInttHits.at(iInttID).phi;
   Double_t oInttPhi = vOInttHits.at(oInttID).phi;

   Double_t dPhi = oInttPhi - iInttPhi;
   if(std::abs(dPhi) > TMath::Pi()) dPhi += (-1) * (dPhi/std::abs(dPhi)) * 2*TMath::Pi();
   Double_t dR = vOInttHits.at(oInttID).r - vIInttHits.at(iInttID).r;

   dPhidR = dPhi/dR;

   return dPhidR;
}


Int_t InttSeedTracking::TempInttCalMatch(Int_t iInttID, Int_t oInttID,
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits){
   Int_t matchiECalID = 9999;
   Double_t matchiECalE = 0.;
   Int_t numOfECalT = vEmcalHits.size();
   for(Int_t iECalT = 0; iECalT < numOfECalT ;iECalT++){
      Double_t ecalE = vEmcalHits.at(iECalT).energy;
      // checkumaDAYO!!! need to optimize the calorimeter threshold energy.
      if(ecalE < m_EcalEThre) continue;

      Double_t dPhidR = TempCalcdPhidR(iInttID, oInttID, vIInttHits, vOInttHits);
      Double_t dRInttEmCal = m_ECalR - vOInttHits.at(oInttID).r;
      Double_t phiRangeMin = 0.;
      Double_t phiRangeMax = 0.;

      // checkumaDAYO!!! need to optimize the matching range.
      if(dPhidR < 0){
         phiRangeMin = vOInttHits.at(oInttID).phi + (dPhidR * dRInttEmCal - 0.177);
         phiRangeMax = vOInttHits.at(oInttID).phi + 0.087;
      }else{
         phiRangeMin = vOInttHits.at(oInttID).phi - 0.087;
         phiRangeMax = vOInttHits.at(oInttID).phi + (dPhidR * dRInttEmCal + 0.177);
      }
      
      Double_t emcalPhi = vEmcalHits.at(iECalT).phi;
      if((phiRangeMin < - TMath::Pi())&&(phiRangeMin > - TMath::Pi())&&(emcalPhi > 0)) emcalPhi -= 2*TMath::Pi();
      else if((phiRangeMin < TMath::Pi())&&(phiRangeMax > TMath::Pi())&&(emcalPhi < 0)) emcalPhi += 2*TMath::Pi();

      if((phiRangeMin < emcalPhi)&&(emcalPhi < phiRangeMax)){
         if(matchiECalE < ecalE){
            matchiECalID = iECalT;
            matchiECalE = ecalE;
         }
      }
      // std::cout << "ecalE = " << ecalE << std::endl;
   }
   // std::cout << "matchiECalE = " << matchiECalE << std::endl;
   return matchiECalID;

}
 

Double_t InttSeedTracking::AddHCalE(Double_t emcalPhi, Double_t emcalE,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits){
   Int_t matchIhcalId = 99999;
   Double_t closePhiIhcal = 99999;
   for(Int_t iIhcal = 0; iIhcal < vIHCalHits.size(); iIhcal++){
      if(closePhiIhcal > std::abs(vIHCalHits.at(iIhcal).phi - emcalPhi)){
         closePhiIhcal = std::abs(vIHCalHits.at(iIhcal).phi - emcalPhi);
         matchIhcalId = iIhcal;
      }
   }
   Int_t matchOhcalId = 99999;
   Double_t closePhiOhcal = 99999;
   for(Int_t iOhcal = 0; iOhcal < vOHCalHits.size(); iOhcal++){
      if(closePhiOhcal > std::abs(vOHCalHits.at(iOhcal).phi - emcalPhi)){
         closePhiOhcal = std::abs(vOHCalHits.at(iOhcal).phi - emcalPhi);
         matchOhcalId = iOhcal;
      }
   }
   
   Double_t iHcalE = 0.;
   Double_t oHcalE = 0.;
   if(matchIhcalId != 99999) iHcalE = vIHCalHits.at(matchIhcalId).energy;
   if(matchOhcalId != 99999) oHcalE = vOHCalHits.at(matchOhcalId).energy;

   Double_t calE = emcalE + iHcalE + oHcalE;

   return calE;
}

void InttSeedTracking::AddMvtxHits(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits, Double_t dRThre,\
   Int_t& seleFMvtxId, Int_t& seleSMvtxId, Int_t& seleTMvtxId){
   Double_t sagiR = trk.getTrackSagR();
   Double_t sagiX = trk.getTrackSagX();
   Double_t sagiY = trk.getTrackSagY();

   for(Int_t iMvtxLay = 0; iMvtxLay < 3; iMvtxLay++){
      Int_t mvtxMatchId = 9999;
      Double_t mvtxClusX = 9999.;
      Double_t mvtxClusY = 9999.;
      if(iMvtxLay == 0){
         if(FindHitsOnCircle(mvtxMatchId, mvtxClusX, mvtxClusY, sagiR, sagiX, sagiY, \
            vFMvtxHits, dRThre)){
            SetHitParaInTrack(trk, 1, vFMvtxHits.at(mvtxMatchId));
            seleFMvtxId = mvtxMatchId;
         }
      }else if(iMvtxLay == 1){
         if(FindHitsOnCircle(mvtxMatchId, mvtxClusX, mvtxClusY, sagiR, sagiX, sagiY,\
            vSMvtxHits, dRThre)){
            SetHitParaInTrack(trk, 2, vSMvtxHits.at(mvtxMatchId));
            seleSMvtxId = mvtxMatchId;
         }
      }else if(iMvtxLay == 2){
         if(FindHitsOnCircle(mvtxMatchId, mvtxClusX, mvtxClusY, sagiR, sagiX, sagiY,\
            vTMvtxHits, dRThre)){
            SetHitParaInTrack(trk, 3, vTMvtxHits.at(mvtxMatchId));
            seleTMvtxId = mvtxMatchId;
         }
      }
   }
}

void InttSeedTracking::TrackPropertiesEstimation(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits){
   Double_t recoPt = TrackPtEstimation(trk, vFMvtxHits, vSMvtxHits, vTMvtxHits);
   trk.setTrackPt(recoPt);
   
   Double_t recoTheta = EstimateRecoTheta(trk, 1);
   trk.setTrackTheta(recoTheta);

   Double_t recoP = recoPt/sin(recoTheta);
   trk.setTrackP(recoP);
   // Double_t recoE = trk.getTrackE();
   // Double_t EOverP = recoE/recoP;

   EstiVertex(trk);

   ParticleIdentify(trk);
}

Double_t InttSeedTracking::TrackPtEstimation(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits){
   Double_t sagittaR = 9999.;
   Double_t sagittaCenterX = 0.;
   Double_t sagittaCenterY = 0.;
   Double_t HitsXY[3][2];
   Set3PointsXY(HitsXY, trk, 2);
   RoughEstiSagittaCenter3Point(sagittaR, sagittaCenterX, sagittaCenterY, HitsXY);

   Double_t trackPt = AccuratePtEstimation(sagittaR, sagittaCenterX, sagittaCenterY, trk, \
   vFMvtxHits, vSMvtxHits, vTMvtxHits);

   Double_t dPhiOInttEmcal = dPhiOInttEmcalEsti(trk);
   trackPt = FitFunctionPt(dPhiOInttEmcal);

   return trackPt;
}


void InttSeedTracking::RoughEstiSagittaCenter3Point(Double_t& sagittaR,\
   Double_t& sagittaCenterX, Double_t& sagittaCenterY, Double_t HitsXY[3][2]){
   Double_t iSlope = 0.;
   Double_t iSection = 0.;
   CalcPerpBis(iSlope, iSection, HitsXY[0], HitsXY[2]);

   Double_t oSlope = 0.;
   Double_t oSection = 0.;
   CalcPerpBis(oSlope, oSection, HitsXY[1], HitsXY[2]);

   sagittaCenterX = - (oSection - iSection)/(oSlope - iSlope);
   sagittaCenterY = iSlope * sagittaCenterX + iSection;

   sagittaR = std::sqrt((HitsXY[0][0] - sagittaCenterX)*(HitsXY[0][0] - sagittaCenterX) \
      + (HitsXY[0][1] - sagittaCenterY)*(HitsXY[0][1] - sagittaCenterY));

}

Double_t InttSeedTracking::AccuratePtEstimation(\
   Double_t sagittaR, Double_t centerX, Double_t centerY, tracKuma& trk,
   std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits){

   Double_t sagittaVtxInttPt = 99999.;
   Double_t sagittaInttEmcalPt = 99999.;
   Double_t sagittaVtxInttEmcalPt = 99999.;
   Double_t sagittaMvtxInttEmcalPt = 99999.;
   Double_t sagittaVtxMvtxInttEmcalPt = 99999.;
   
   // checkumaDaYo!!!
   std::vector<Int_t > subDetIds_InttEmcal = {4, 5, 6};
   std::vector<Double_t > hitsR_InttEmcal = {};
   std::vector<Double_t > hitsPhi_InttEmcal = {};
   if(ReturnHitsRPhiVect(hitsR_InttEmcal, hitsPhi_InttEmcal, subDetIds_InttEmcal, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_InttEmcal, hitsPhi_InttEmcal, tempHitOInttPhi, tempHitEmcalPhi);
      
      sagittaMvtxInttEmcalPt = CalcSagittaPt(sagittaR);
   }

   Int_t seleFMvtxId = 99999;
   Int_t seleSMvtxId = 99999;
   Int_t seleTMvtxId = 99999;
   Double_t dRThre_Mvtx = 5.;
   AddMvtxHits(trk, vFMvtxHits, vSMvtxHits, vTMvtxHits, dRThre_Mvtx,\
      seleFMvtxId, seleSMvtxId, seleTMvtxId);
   
   std::vector<Int_t > subDetIds_MvtxInttEmcal = {1, 2, 3, 4, 5, 6};
   std::vector<Double_t > hitsR_MvtxInttEmcal = {};
   std::vector<Double_t > hitsPhi_MvtxInttEmcal = {};
   if(ReturnHitsRPhiVect(hitsR_MvtxInttEmcal, hitsPhi_MvtxInttEmcal, subDetIds_MvtxInttEmcal, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_MvtxInttEmcal, hitsPhi_MvtxInttEmcal,\
      tempHitOInttPhi, tempHitEmcalPhi);
      sagittaMvtxInttEmcalPt = CalcSagittaPt(sagittaR);
   }

   trk.setTrackSagR(sagittaR);
   trk.setTrackSagX(centerX);
   trk.setTrackSagY(centerY);

   return sagittaMvtxInttEmcalPt;
}

void InttSeedTracking::SagittaRByCircleFit(Double_t& centerX, Double_t& centerY, Double_t& sagittaR,
   std::vector<Double_t > r, std::vector<Double_t > phi, Double_t oInttPhi, Double_t emcalPhi){
   Double_t basePhi = TMath::Pi()/4 - oInttPhi;
   bool bFlip = false;
   if((emcalPhi + basePhi)  > TMath::Pi()/4) bFlip = true;
   
   TH2D* hHitMap = new TH2D("hHitMap", "hHitMap; x [cm]; y [cm]",\
      2000, -100., 100., 2000, -100., 100.);

   Int_t numOfHits = r.size();
   for(Int_t iHit = 0; iHit < numOfHits; iHit++){
      Double_t tempPhi = phi.at(iHit) + basePhi;
      if(bFlip) tempPhi = TMath::Pi()/2 - tempPhi;
      hHitMap->Fill(r.at(iHit)*cos(tempPhi), r.at(iHit)*sin(tempPhi));
   }

   Double_t tempCPhi = std::atan(centerY/centerX);
   if((tempCPhi < 0)&&(centerX < 0)) tempCPhi += TMath::Pi();
   else if((tempCPhi > 0)&&(centerX < 0)) tempCPhi -= TMath::Pi();
   tempCPhi = tempCPhi + basePhi;
   if(bFlip) tempCPhi = TMath::Pi()/2 - tempCPhi;
   Double_t tempCR = std::sqrt(centerX*centerX + centerY*centerY);
   Double_t tempCX = tempCR*std::cos(tempCPhi);
   Double_t tempCY = tempCR*std::sin(tempCPhi);
      
   // [0]: radius, [1]: x center, [2]: y center 
   auto fCircle = new TF1("fCircle", "std::sqrt([0]*[0]-(x-[1])*(x-[1]))+[2]", -300, 300);
   fCircle->SetParameter(0, sagittaR);
   fCircle->SetParameter(1, tempCX);
   fCircle->SetParameter(2, tempCY);
   
   hHitMap->Fit("fCircle", "Q");
   sagittaR = fCircle->GetParameter(0);
   centerX = fCircle->GetParameter(1);
   centerY = fCircle->GetParameter(2);

   delete fCircle;
   delete hHitMap;

   Double_t tempRetCPhi = std::atan(centerY/centerX);
   if((tempRetCPhi < 0)&&(centerX < 0)) tempRetCPhi += TMath::Pi();
   else if((tempRetCPhi > 0)&&(centerX < 0)) tempRetCPhi -= TMath::Pi();
   if(bFlip) tempRetCPhi = TMath::Pi()/2 - tempRetCPhi;
   tempRetCPhi = tempRetCPhi - basePhi;
   centerX = sagittaR*std::cos(tempRetCPhi);
   centerY = sagittaR*std::sin(tempRetCPhi);

   return;
}

bool InttSeedTracking::FindHitsOnCircle(Int_t& hitMatchId,\
   Double_t& hitX, Double_t& hitY,\
   Double_t sagittaR, Double_t centerX, Double_t centerY, std::vector<hitStruct > vHits,\
   Double_t dRThre){
   Double_t minDeltaR = 99999.;

   if(vHits.size() == 0) return false;
   for(Int_t iHit=0; iHit< vHits.size(); iHit++){
      Double_t tempHitX = vHits.at(iHit).r*std::cos(vHits.at(iHit).phi);
      Double_t tempHitY = vHits.at(iHit).r*std::sin(vHits.at(iHit).phi);

      Double_t dX = tempHitX - centerX;
      Double_t dY = tempHitY - centerY;
      Double_t dR = std::sqrt(dX*dX + dY*dY);
      
      Double_t deltaR = std::abs(dR - sagittaR);
      if(deltaR > dRThre) continue;
      if(minDeltaR > deltaR){
         hitMatchId = iHit;
         minDeltaR = deltaR;
      }
   }
   
   if(hitMatchId == 9999) return false;
   hitX = vHits.at(hitMatchId).r * std::cos(vHits.at(hitMatchId).phi);
   hitY = vHits.at(hitMatchId).r * std::sin(vHits.at(hitMatchId).phi);

   return true;
}


void InttSeedTracking::EstiVertex(tracKuma trk){
   Double_t vtxX = 0.;
   Double_t vtxY = 0.;

   CrossLineCircle(vtxX, vtxY, trk.getTrackSagX(), trk.getTrackSagY(), trk.getTrackSagR());
   Double_t vtxR = std::sqrt(vtxX*vtxX + vtxY*vtxY);
   Double_t vtxPhi = atan(vtxY/vtxX);
   if((vtxPhi < 0)&&(vtxX < 0)) vtxPhi += TMath::Pi();
   else if((vtxPhi > 0)&&(vtxY < 0)) vtxPhi -= TMath::Pi();
   trk.setHitR(0, vtxR);
   trk.setHitPhi(0, vtxPhi);

   Double_t trkPhi = -1/std::tan(vtxPhi);
   Double_t vtxZ = 0.;
   if(trk.getHitIs(6)){
      vtxZ = trk.getHitZ(6) - trk.getHitZ(6)/std::tan(trk.getTrackTheta());
   }
   trk.setHitZ(0, vtxZ);
}

Double_t InttSeedTracking::EstimateRecoTheta(tracKuma trk, Int_t type){
   // fitting weight for eta
   Double_t mvtxEtaW = 1.;
   Double_t inttEtaW = 1.;
   Double_t ecalEtaW = 1.;
   
   Double_t recoTheta = 9999.;
   if(type == 0){ // intt + EMcal
      recoTheta = (inttEtaW*trk.getHitTheta(4)\
         + inttEtaW*trk.getHitTheta(5)\
         + ecalEtaW*trk.getHitTheta(6))/(2*inttEtaW+ecalEtaW);
   }else if(type == 1){ // mvtx + intt + EMcal
      if((trk.getHitIs(1))&&(trk.getHitIs(2))\
      &&(trk.getHitIs(3))){
         recoTheta = (mvtxEtaW*trk.getHitTheta(1) \
            + mvtxEtaW*trk.getHitTheta(2)\
            + mvtxEtaW*trk.getHitTheta(3)\
            + inttEtaW*trk.getHitTheta(4)\
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))\
            /(3*mvtxEtaW+2*inttEtaW+ecalEtaW);
      }else if((trk.getHitIs(2))&&(trk.getHitIs(3))){
         recoTheta = (mvtxEtaW*trk.getHitTheta(2)\
            + mvtxEtaW*trk.getHitTheta(3)\
            + inttEtaW*trk.getHitTheta(4)\
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))/(2*mvtxEtaW+2*inttEtaW+ecalEtaW);
      }else if((trk.getHitIs(1))&&(trk.getHitIs(3))){
         recoTheta = (mvtxEtaW*trk.getHitTheta(1)\
            + mvtxEtaW*trk.getHitTheta(3)\
            + inttEtaW*trk.getHitTheta(4)\
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))/(2*mvtxEtaW+2*inttEtaW+ecalEtaW);
      }else if((trk.getHitIs(1))&&(trk.getHitIs(2))){
         recoTheta = (mvtxEtaW*trk.getHitTheta(1)\
            + mvtxEtaW*trk.getHitTheta(2)\
            + inttEtaW*trk.getHitTheta(4)\
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))/(2*mvtxEtaW+2*inttEtaW+ecalEtaW);
      }else if(trk.getHitIs(1)){
         recoTheta = (mvtxEtaW*trk.getHitTheta(1)
            + inttEtaW*trk.getHitTheta(4)\
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))/(mvtxEtaW+2*inttEtaW+ecalEtaW);
      }else if(trk.getHitIs(2)){
         recoTheta = (mvtxEtaW*trk.getHitTheta(2)\
            + inttEtaW*trk.getHitTheta(4)\
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))/(mvtxEtaW+2*inttEtaW+ecalEtaW);
      }else if(trk.getHitIs(3)){
         recoTheta = (mvtxEtaW*trk.getHitTheta(3)\
            + inttEtaW*trk.getHitTheta(4)
            + inttEtaW*trk.getHitTheta(5)\
            + ecalEtaW*trk.getHitTheta(6))/(mvtxEtaW+2*inttEtaW+ecalEtaW);
      }

   }

   return recoTheta;
}

void InttSeedTracking::ParticleIdentify(tracKuma trk){
   Double_t tempInttPhi = 0.;
   if(trk.getHitIs(4)) tempInttPhi = trk.getHitPhi(4);
   else tempInttPhi = trk.getHitPhi(5);
   Double_t tempCalPhi = trk.getHitPhi(6);
   
   if(tempInttPhi < 0) tempInttPhi += 2*TMath::Pi();
   if(tempCalPhi < 0)  tempCalPhi += 2*TMath::Pi();

   Int_t charge = (tempCalPhi - tempCalPhi)/std::abs(tempCalPhi - tempCalPhi);

   trk.setTrackCharge(charge);

   bool tempElectronIs = false;
   // ChecKumaDaYo!!! You need to optimize the threshold to identify electron
   if(trk.getTrackE()/trk.getTrackP() > m_EOverPElectron) tempElectronIs = true;
   trk.setTrackElectronIs(tempElectronIs);

}

// 获取track在各个子探测器的R,Phi，命中点极坐标返回，以及检查是否超过3个hit点
bool InttSeedTracking::ReturnHitsRPhiVect(std::vector<Double_t>& hitR, std::vector<Double_t>& hitPhi,\
    std::vector<Int_t> subDetSet, tracKuma trk)
{
    for(Int_t iDet = 0; iDet < subDetSet.size(); iDet++)
    {
        Int_t detID = subDetSet.at(iDet);
        if(!trk.getHitIs(detID)) continue;

        hitR.push_back(trk.getHitR(detID)); 
        hitPhi.push_back(trk.getHitPhi(detID));
    }
    if(hitR.size() < 3) return false;
    return true;
}


void InttSeedTracking::SetHitParaInTrack(tracKuma& trk, Int_t detId, hitStruct hitSt)
{
   trk.setHitIs(detId, true);
   trk.setHitR(detId, hitSt.r);
   trk.setHitZ(detId, hitSt.z);
   trk.setHitPhi(detId, hitSt.phi);
   Double_t hitTheta = 2*atan(std::exp(-hitSt.eta));
   trk.setHitTheta(detId, hitTheta);
}

#endif // #ifdef InttSeedTracking_cxx




