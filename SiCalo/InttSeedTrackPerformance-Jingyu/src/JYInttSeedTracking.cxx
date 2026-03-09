// InttSeedTracking::InttSeedTracking
//   ├── HitMatching
//   │     ├── TempINTTIOMatching (INTT 内外层匹配)
//   │     ├── TempInttCalMatch (INTT 和 EMCal 匹配)
//   │     │     ├── TempCalcdPhidR (计算 dPhi/dR)
//   │     │     └── AddHCalE (加入 HCal 能量)
//   │     └── tracKuma 构造轨迹
//   ├── TrackPropertiesEstimation
//   │     ├── TrackPtEstimation (估算横向动量 pT)
//   │     │     ├── Set3PointsXY (设置 3 点坐标)
//   │     │     ├── RoughEstiSagittaCenter3Point (粗略估算 Sagitta 中心与半径)
//   │     │     ├── AccuratePtEstimation (精确估算动量 pT)
//   │     │     │     ├── SagittaRByCircleFit (弧线拟合，精确估算 Sagitta)
//   │     │     │     ├── AddMvtxHits (匹配 MVTX 命中点)
//   │     │     │     └── CalcSagittaPt (基于 Sagitta 半径计算横向动量)
//   │     │     └── FitFunctionPt (基于 dPhi 校正动量)
//   │     ├── EstimateRecoTheta (估算重建角度 θ)
//   │     │     └── 不同探测器组合加权计算 θ
//   │     └── CrossLineCircle (计算顶点位置)
//   └── 数据清理与存储
//         └── clear() 和 shrink_to_fit() 清理命中点数据

#include <TStyle.h>
#include <TCanvas.h>

#include "JYInttSeedTracking.h"
#include "SPHTracKuma.h"

InttSeedTracking::InttSeedTracking(std::vector<tracKuma>& tracks,\
   std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits)
{
    HitMatching(tracks, vFMvtxHits, vSMvtxHits, vTMvtxHits, vIInttHits, vOInttHits, vEmcalHits, vIHCalHits, vOHCalHits);
    for(Int_t iTrk = 0; iTrk < tracks.size(); iTrk++)
    {
        TrackPropertiesEstimation(tracks.at(iTrk), vFMvtxHits, vSMvtxHits, vTMvtxHits);
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


// Hit Matching to get which hit point from one track
void InttSeedTracking::HitMatching(std::vector<tracKuma>& tracks,\
   std::vector<hitStruct> vFMvtxHits,\
   std::vector<hitStruct> vSMvtxHits, std::vector<hitStruct> vTMvtxHits,\
   std::vector<hitStruct> vIInttHits, std::vector<hitStruct> vOInttHits,\
   std::vector<hitStruct> vEmcalHits,\
   std::vector<hitStruct> vIHCalHits, std::vector<hitStruct> vOHcalHits)
{
    Int_t numOfInttClus = vOInttHits.size();
    if(numOfInttClus == 0) return;

    for(Int_t iOInttClus = 0; iOInttClus < numOfInttClus; iOInttClus++)
    {
        Double_t oInttPhi = vOInttHits.at(iOInttClus).phi;
        // Double_t refPhi = oInttPhi;
        // refPhi = TMath::Pi()/2 - refPhi;

        // oInttPhi += refPhi;

        // Int_t closetIINTTID = TempINTTIOMatching(oInttPhi, refPhi, vIInttHits);
        // if(closetIINTTID == 9999) continue;
        // Int_t matchEcalID = TempInttCalMatch(closetIINTTID, iOInttClus, refPhi, vIInttHits, vOInttHits, vEmcalHits);
        Int_t closetIINTTID = TempINTTIOMatching(oInttPhi, vIInttHits);
        if(closetIINTTID == 9999) continue;      
        Int_t matchEcalID = TempInttCalMatch(closetIINTTID, iOInttClus, vIInttHits, vOInttHits, vEmcalHits);
        if(matchEcalID == 9999) continue;

        tracKuma trk;
        // trk.setRefPhi(refPhi);
        Double_t tempTheta = 0.;

        trk.setHitIs(0, true); // 0 vertex, 1-3 mvtx, 4-5 ioINTT, 6 emcal,
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
    }
}

// match the inner INTT cluster and outer INTT cluster, return closest iINTTCluID
Int_t InttSeedTracking::TempINTTIOMatching(Double_t oINTTPhi, std::vector<hitStruct> vIInttHits)
{
    Double_t minDeltaPhi = 9999.;
    Int_t closestIINTTCluID = 9999;
    Int_t numOfIInttClu = vIInttHits.size();

    if(numOfIInttClu == 0) return 9999;
    for(Int_t iINTTClu = 0; iINTTClu < numOfIInttClu; iINTTClu++)
    {
        Double_t iInttPhi =  vIInttHits.at(iINTTClu).phi;
        Double_t dPhi = std::abs(iInttPhi - oINTTPhi);
        if(std::abs(dPhi) > TMath::Pi()) dPhi += (-1) * (dPhi/std::abs(dPhi)) * 2*TMath::Pi(); // turn to -PI ~ PI

        // checkumaDAYO!!! need to optimize the matching range.
        if((minDeltaPhi > dPhi)&&(dPhi > m_InttMatchPhiMin)&&(dPhi < m_InttMatchPhiMax))
        {
            minDeltaPhi = dPhi;
            closestIINTTCluID = iINTTClu;
        }
    }
    return closestIINTTCluID;
}

// calculate dPhi_intt dR_intt, return dPhi/dr, applied to TempInttCalMatch
Double_t InttSeedTracking::TempCalcdPhidR(Int_t iInttID, Int_t oInttID, std::vector<hitStruct> vIInttHits, std::vector<hitStruct> vOInttHits)
{
    Double_t dPhidR = 0.;
    Double_t iInttPhi = vIInttHits.at(iInttID).phi;
    Double_t oInttPhi = vOInttHits.at(oInttID).phi;

    Double_t dPhi = oInttPhi - iInttPhi;
    if(std::abs(dPhi) > TMath::Pi()) dPhi += (-1) * (dPhi/std::abs(dPhi)) * 2*TMath::Pi();
    Double_t dR = vOInttHits.at(oInttID).r - vIInttHits.at(iInttID).r;

    dPhidR = dPhi/dR;

    return dPhidR;
}

// calculate the dphi_cal, return all satisify emcal cluster ID
Int_t InttSeedTracking::TempInttCalMatch(Int_t iInttID, Int_t oInttID, std::vector<hitStruct> vIInttHits, std::vector<hitStruct> vOInttHits, std::vector<hitStruct> vEmcalHits)
{
   Int_t matchiECalID = 9999;
   Double_t matchiECalE = 0.;
   Int_t numOfECalT = vEmcalHits.size();

   for(Int_t iECalT = 0; iECalT < numOfECalT ;iECalT++)
   {
        Double_t ecalE = vEmcalHits.at(iECalT).energy;
        // checkumaDAYO!!! need to optimize the calorimeter threshold energy.
        if(ecalE < m_EcalEThre) continue;

        Double_t dPhidR = TempCalcdPhidR(iInttID, oInttID, vIInttHits, vOInttHits);
        Double_t dRInttEmCal = m_ECalR - vOInttHits.at(oInttID).r;
        Double_t phiRangeMin = 0.;
        Double_t phiRangeMax = 0.;

        // checkumaDAYO!!! need to optimize the matching range. ?TMath::Pi()/2 should be Phi_oINTT ?dPhi_cal = dPhidR * dRInttEmCal
        if(dPhidR < 0)
        {
            phiRangeMin = vOInttHits.at(oInttID).phi + (dPhidR * dRInttEmCal - 0.177);
            phiRangeMax = vOInttHits.at(oInttID).phi + 0.087;
        }
        else
        {
            phiRangeMin = vOInttHits.at(oInttID).phi - 0.087;
            phiRangeMax = vOInttHits.at(oInttID).phi + (dPhidR * dRInttEmCal + 0.177);
        }

        Double_t emcalPhi = vEmcalHits.at(iECalT).phi;
        if((phiRangeMin < - TMath::Pi())&&(phiRangeMin > - TMath::Pi())&&(emcalPhi > 0)) emcalPhi -= 2*TMath::Pi();
        else if((phiRangeMin < TMath::Pi())&&(phiRangeMax > TMath::Pi())&&(emcalPhi < 0)) emcalPhi += 2*TMath::Pi();

        if((phiRangeMin < emcalPhi)&&(emcalPhi < phiRangeMax))
        {
            if(matchiECalE < ecalE)
            {
                matchiECalID = iECalT;
                matchiECalE = ecalE;
            }
        }
        // std::cout << "ecalE = " << ecalE << std::endl;
   }
   // std::cout << "matchiECalE = " << matchiECalE << std::endl;
   return matchiECalID;
}

// Add HCalE, return calE = emcalE + iHcalE + oHcalE
Double_t InttSeedTracking::AddHCalE(Double_t emcalPhi, Double_t emcalE, std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHCalHits)
{
    // inner HCal match
    Int_t matchIhcalId = 99999;
    Double_t closePhiIhcal = 99999;

    for(Int_t iIhcal = 0; iIhcal < vIHCalHits.size(); iIhcal++)
    {
        if(closePhiIhcal > std::abs(vIHCalHits.at(iIhcal).phi - emcalPhi))
        {
            closePhiIhcal = std::abs(vIHCalHits.at(iIhcal).phi - emcalPhi);
            matchIhcalId = iIhcal;
        }
    }

    // outer HCal match
    Int_t matchOhcalId = 99999;
    Double_t closePhiOhcal = 99999;

    for(Int_t iOhcal = 0; iOhcal < vOHCalHits.size(); iOhcal++)
    {
        if(closePhiOhcal > std::abs(vOHCalHits.at(iOhcal).phi - emcalPhi))
        {
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


// ├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
// ├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
// ├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
// ├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
// ├────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────


// TrackPropertiesEstimation, get the track pt
void InttSeedTracking::TrackPropertiesEstimation(tracKuma& trk, std::vector<hitStruct> vFMvtxHits, std::vector<hitStruct> vSMvtxHits, std::vector<hitStruct> vTMvtxHits)
{
    Double_t recoPt = TrackPtEstimation(trk, vFMvtxHits, vSMvtxHits, vTMvtxHits);
    trk.setTrackPt(recoPt);

    Double_t recoTheta = EstimateRecoTheta(trk, 1);
    trk.setTrackTheta(recoTheta);

    Double_t recoP = recoPt/sin(recoTheta);
    Double_t recoE = trk.getTrackE();
    Double_t EOverP = recoE/recoP;
    trk.setTrackP(recoP);

    // Double_t vtxX = 0.;
    // Double_t vtxY = 0.;
    EstiVertex(trk);
    ParticleIdentify(trk);

    // // EstiVertex(vtxX, vtxY, trk.getTrackSagR(), trk.getTrackSagX(), trk.getTrackSagY());   1227
    // CrossLineCircle(vtxX, vtxY, trk.getTrackSagX(), trk.getTrackSagY(), trk.getTrackSagR());

    // Double_t vtxR = std::sqrt(vtxX*vtxX + vtxY*vtxY);
    // Double_t vtxPhi = atan(vtxY/vtxX);
    // if((vtxPhi < 0)&&(vtxX < 0)) vtxPhi += TMath::Pi();
    // else if((vtxPhi > 0)&&(vtxY < 0)) vtxPhi -= TMath::Pi();

    // trk.setHitR(0, vtxR);
    // trk.setHitPhi(0, vtxPhi);

    // Double_t trkPhi = -1/std::tan(vtxPhi);
    // Double_t vtxZ = 0.;
    // if(trk.getHitIs(6))
    // {
    //     vtxZ = trk.getHitZ(6) - trk.getHitZ(6)/std::tan(recoTheta);
    // }
    // trk.setHitZ(0, vtxZ);
}

// Track Pt Estimation 
Double_t InttSeedTracking::TrackPtEstimation(tracKuma& trk, std::vector<hitStruct> vFMvtxHits, std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct> vTMvtxHits)
{
    Double_t sagittaR = 9999.;
    Double_t sagittaCenterX = 0.;
    Double_t sagittaCenterY = 0.;
    Double_t HitsXY[3][2];
    Set3PointsXY(HitsXY, trk, 2);
    RoughEstiSagittaCenter3Point(sagittaR, sagittaCenterX, sagittaCenterY, HitsXY);

    Double_t trackPt = AccuratePtEstimation(sagittaR, sagittaCenterX, sagittaCenterY, trk, vFMvtxHits, vSMvtxHits, vTMvtxHits);

    Double_t dPhiOInttEmcal = dPhiOInttEmcalEsti(trk);
    trackPt = FitFunctionPt(dPhiOInttEmcal);

    return trackPt;
}

// a RoughEstiSagittaCenter3Point to get initial value for fit
void InttSeedTracking::RoughEstiSagittaCenter3Point(Double_t& sagittaR, Double_t& sagittaCenterX, Double_t& sagittaCenterY, Double_t HitsXY[3][2])
{
    Double_t iSlope = 0.;
    Double_t iSection = 0.;
    CalcPerpBis(iSlope, iSection, HitsXY[0], HitsXY[2]);

    Double_t oSlope = 0.;
    Double_t oSection = 0.;
    CalcPerpBis(oSlope, oSection, HitsXY[1], HitsXY[2]);

    sagittaCenterX = - (oSection - iSection)/(oSlope - iSlope);
    sagittaCenterY = iSlope * sagittaCenterX + iSection;

    sagittaR = std::sqrt((HitsXY[0][0] - sagittaCenterX)*(HitsXY[0][0] - sagittaCenterX) + (HitsXY[0][1] - sagittaCenterY)*(HitsXY[0][1] - sagittaCenterY));
}

// AccuratePtEstimation, return track pt from sagitta_Mvtx_Intt_Emcal
Double_t InttSeedTracking::AccuratePtEstimation(Double_t sagittaR, Double_t centerX, Double_t centerY, tracKuma& trk, std::vector<hitStruct> vFMvtxHits, std::vector<hitStruct> vSMvtxHits, std::vector<hitStruct> vTMvtxHits)
{
    Double_t sagittaVtxInttPt = 99999.;
    Double_t sagittaInttEmcalPt = 99999.;
    Double_t sagittaVtxInttEmcalPt = 99999.;
    Double_t sagittaMvtxInttEmcalPt = 99999.;
    Double_t sagittaVtxMvtxInttEmcalPt = 99999.;
    
    // checkumaDaYo!!!
    std::vector<Int_t> subDetIds_InttEmcal = {4, 5, 6};
    std::vector<Double_t> hitsR_InttEmcal = {};
    std::vector<Double_t> hitsPhi_InttEmcal = {};
    if(ReturnHitsRPhiVect(hitsR_InttEmcal, hitsPhi_InttEmcal, subDetIds_InttEmcal, trk))
    {
        Double_t tempHitOInttPhi = trk.getHitPhi(4);
        Double_t tempHitEmcalPhi = trk.getHitPhi(6);
        SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_InttEmcal, hitsPhi_InttEmcal, tempHitOInttPhi, tempHitEmcalPhi);

        sagittaMvtxInttEmcalPt = CalcSagittaPt(sagittaR);
    }

    Double_t dRThre_Mvtx = 0.01;
    AddMvtxHits(trk, vFMvtxHits, vSMvtxHits, vTMvtxHits, dRThre_Mvtx);
    
    std::vector<Int_t> subDetIds_MvtxInttEmcal = {1, 2, 3, 4, 5, 6};
    std::vector<Double_t> hitsR_MvtxInttEmcal = {};
    std::vector<Double_t> hitsPhi_MvtxInttEmcal = {};
    if(ReturnHitsRPhiVect(hitsR_MvtxInttEmcal, hitsPhi_MvtxInttEmcal, subDetIds_MvtxInttEmcal, trk))
    {
        Double_t tempHitOInttPhi = trk.getHitPhi(4);
        Double_t tempHitEmcalPhi = trk.getHitPhi(6);
        SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_MvtxInttEmcal, hitsPhi_MvtxInttEmcal, 
        tempHitOInttPhi, tempHitEmcalPhi);
        sagittaMvtxInttEmcalPt = CalcSagittaPt(sagittaR);
    }

    trk.setTrackSagR(sagittaR);
    trk.setTrackSagX(centerX);
    trk.setTrackSagY(centerY);

    return sagittaMvtxInttEmcalPt;
}

// calculate the SagittaR by CircleFit ???
void InttSeedTracking::SagittaRByCircleFit(Double_t& centerX, Double_t& centerY, Double_t& sagittaR, std::vector<Double_t > r, std::vector<Double_t > phi, Double_t oInttPhi, Double_t emcalPhi)
{ 
    Double_t basePhi = TMath::Pi()/4 - oInttPhi;
    bool bFlip = false;
    if((emcalPhi + basePhi)  > TMath::Pi()/4) bFlip = true;
    
    TH2D* hHitMap = new TH2D("hHitMap", "hHitMap; x [cm]; y [cm]", 2000, -100., 100., 2000, -100., 100.);

    Int_t numOfHits = r.size();
    for(Int_t iHit = 0; iHit < numOfHits; iHit++)
    {
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
    
    hHitMap->Fit("fCircle", "QM");
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

//  AddMvtxHits
void InttSeedTracking::AddMvtxHits(tracKuma& trk, std::vector<hitStruct> vFMvtxHits,\
   std::vector<hitStruct> vSMvtxHits, std::vector<hitStruct> vTMvtxHits, Double_t dRThre)
{
    Double_t sagiR = trk.getTrackSagR();
    Double_t sagiX = trk.getTrackSagX();
    Double_t sagiY = trk.getTrackSagY();

    // 逐层检查，匹配一层，就在trk里set一层
    for(Int_t iMvtxLay = 0; iMvtxLay < 3; iMvtxLay++) 
    {
        Int_t mvtxMatchId = 9999;
        Double_t mvtxClusX = 9999.;
        Double_t mvtxClusY = 9999.;        
        if(iMvtxLay == 0)
        {
            if(FindHitsOnCircle(mvtxMatchId, mvtxClusX, mvtxClusY, sagiR, sagiX, sagiY, vFMvtxHits, dRThre))
            {
                trk.setHitIs(1, true);
                trk.setHitR(1, vFMvtxHits.at(mvtxMatchId).r);
                trk.setHitPhi(1, vFMvtxHits.at(mvtxMatchId).phi);
                Double_t tempTheta = 2*atan(std::exp(-vFMvtxHits.at(mvtxMatchId).eta));
                trk.setHitTheta(1, tempTheta);
            }
        }
        else if(iMvtxLay == 1)
        {
            if(FindHitsOnCircle(mvtxMatchId, mvtxClusX, mvtxClusY, sagiR, sagiX, sagiY, vSMvtxHits, dRThre))
            {
                trk.setHitIs(2, true);
                trk.setHitR(2, vSMvtxHits.at(mvtxMatchId).r);
                trk.setHitPhi(2, vSMvtxHits.at(mvtxMatchId).phi);
                Double_t tempTheta = 2*atan(std::exp(-vSMvtxHits.at(mvtxMatchId).eta));
                trk.setHitTheta(2, tempTheta);
            }
        }
        else if(iMvtxLay == 2)
        {
            if(FindHitsOnCircle(mvtxMatchId, mvtxClusX, mvtxClusY, sagiR, sagiX, sagiY, vTMvtxHits, dRThre))
            {
                trk.setHitIs(3, true);
                trk.setHitR(3, vTMvtxHits.at(mvtxMatchId).r);
                trk.setHitPhi(3, vTMvtxHits.at(mvtxMatchId).phi);
                Double_t tempTheta = 2*atan(std::exp(-vTMvtxHits.at(mvtxMatchId).eta));
                trk.setHitTheta(3, tempTheta);
            }
        }
    }
}

// loop mvtx ilayer cluster,use delta_R = dR-sagittaR to check is match, get mvtxClusX and mvtxClusY ->FindHitsOnCircle
bool InttSeedTracking::FindHitsOnCircle(Int_t& hitMatchId,\
   Double_t& hitX, Double_t& hitY,\
   Double_t sagittaR, Double_t centerX, Double_t centerY, std::vector<hitStruct > vHits,\
   Double_t dRThre)
{
    Double_t minDeltaR = 99999.;
    
    if(vHits.size() == 0) return false;
    
    for(Int_t iHit=0; iHit< vHits.size(); iHit++)
    {
        Double_t tempHitX = vHits.at(iHit).r*std::cos(vHits.at(iHit).phi);
        Double_t tempHitY = vHits.at(iHit).r*std::sin(vHits.at(iHit).phi);

        Double_t dX = tempHitX - centerX;
        Double_t dY = tempHitY - centerY;
        Double_t dR = std::sqrt(dX*dX + dY*dY);

        Double_t deltaR = std::abs(dR - sagittaR);
        if(deltaR > dRThre) continue;
        if(minDeltaR > deltaR)
        {
            hitMatchId = iHit;
            minDeltaR = deltaR;
        }
    }

    if(hitMatchId == 9999) return false;  // no mvtxcluster to match
    hitX = vHits.at(hitMatchId).r * std::cos(vHits.at(hitMatchId).phi);
    hitY = vHits.at(hitMatchId).r * std::sin(vHits.at(hitMatchId).phi);
    
    return true;
}

// ReturnHitsRPhiVect,
bool InttSeedTracking::ReturnHitsRPhiVect(std::vector<Double_t>& hitR, std::vector<Double_t>& hitPhi, std::vector<Int_t> subDetSet, tracKuma trk)
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

// Track Theta(eta?) Estimation
Double_t InttSeedTracking::EstimateRecoTheta(tracKuma trk, Int_t type)
{
    // fitting weight for eta
    Double_t mvtxEtaW = 1.;
    Double_t inttEtaW = 1.;
    Double_t ecalEtaW = 1.;
    
    Double_t recoTheta = 9999.;
    if(type == 0) // intt + EMcal
    { 
        recoTheta = (inttEtaW*trk.getHitTheta(4)\
          + inttEtaW*trk.getHitTheta(5)\
          + ecalEtaW*trk.getHitTheta(6))/(2*inttEtaW+ecalEtaW);
    }
    else if(type == 1) // mvtx + intt + EMcal, 3 layers mvtx 1-3 bool getHitIs
    { 
        if((trk.getHitIs(1))&&(trk.getHitIs(2))&&(trk.getHitIs(3)))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(1) \
              + mvtxEtaW*trk.getHitTheta(2)\
              + mvtxEtaW*trk.getHitTheta(3)\
              + inttEtaW*trk.getHitTheta(4)\
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))\
              /(3*mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
        else if((trk.getHitIs(2))&&(trk.getHitIs(3)))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(2)\
              + mvtxEtaW*trk.getHitTheta(3)\
              + inttEtaW*trk.getHitTheta(4)\
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))/(2*mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
        else if((trk.getHitIs(1))&&(trk.getHitIs(3)))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(1)\
              + mvtxEtaW*trk.getHitTheta(3)\
              + inttEtaW*trk.getHitTheta(4)\
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))/(2*mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
        else if((trk.getHitIs(1))&&(trk.getHitIs(2)))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(1)\
              + mvtxEtaW*trk.getHitTheta(2)\
              + inttEtaW*trk.getHitTheta(4)\
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))/(2*mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
        else if(trk.getHitIs(1))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(1)
              + inttEtaW*trk.getHitTheta(4)\
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))/(mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
        else if(trk.getHitIs(2))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(2)\
              + inttEtaW*trk.getHitTheta(4)\
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))/(mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
        else if(trk.getHitIs(3))
        {
            recoTheta = (mvtxEtaW*trk.getHitTheta(3)\
              + inttEtaW*trk.getHitTheta(4)
              + inttEtaW*trk.getHitTheta(5)\
              + ecalEtaW*trk.getHitTheta(6))/(mvtxEtaW+2*inttEtaW+ecalEtaW);
        }
    }

    return recoTheta;
}

// track curve vertex Estimation, get vX and vY, what method be used?
void InttSeedTracking::EstiVertex(tracKuma trk)
{
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
    if(trk.getHitIs(6))
    {
        vtxZ = trk.getHitZ(6) - trk.getHitZ(6)/std::tan(trk.getTrackTheta());
    }
    trk.setHitZ(0, vtxZ);
}

void InttSeedTracking::ParticleIdentify(tracKuma trk)
{
    Double_t tempInttPhi = 0.;
    if(trk.getHitIs(4)) tempInttPhi = trk.getHitPhi(4);
    else tempInttPhi = trk.getHitPhi(5);
    Double_t tempCalPhi = trk.getHitPhi(6);
    
    if(tempInttPhi < 0) tempInttPhi += 2*TMath::Pi();
    if(tempCalPhi < 0)  tempCalPhi += 2*TMath::Pi();
    Int_t charge = (tempCalPhi - tempCalPhi)/std::abs(tempCalPhi - tempCalPhi);
    trk.setTrackCharge(charge);

    bool tempElectronIs = false;
    if(trk.getTrackE()/trk.getTrackP() > m_EOverPElectron) tempElectronIs = true;
    trk.setTrackElectronIs(tempElectronIs);
}

void InttSeedTracking::CrossLineCircle(Double_t& xv, Double_t& yv, Double_t xc, Double_t yc, Double_t R)
{
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




















































