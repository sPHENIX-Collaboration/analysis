#ifndef CROSSINGCORRECT_H
#define CROSSINGCORRECT_H
//
// Crossing correction for pt-ordered (dy,dphi) maps (README_Crossing.md, sec 8).
// Self-contained (ROOT TH2D only) so corral (CalcRm::Calculate) and Finalize use the
// identical code. Copy or include this file verbatim.
//
// Requirements on the fill (Increment/PairInfo): pt-ordering flips dphi ONLY (no dy flip).
// Then the dphi-symmetrized map is the same with and without pt-ordering, and it is the
// only symmetrization pt-ordering requires. dy is never touched here.
//
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "TH2D.h"

//------------------------------------------------------------
// Which dphi side of the pt-ordered map holds the crossing loss:
// +1 = dphi>0, -1 = dphi<0, 0 = no crossing correction (a neutral leg).
// PairInfo's pt-ordering already folds in the charge order (pi+pi- is flipped into
// pi-pi+ orientation, LS pairs are ordered by pt), so for any charged-charged pair the
// dirty side depends only on the field sign. The dphi sign convention of the ordering is
// chosen (2026-09-25, as in STAR) so the damage lands just BELOW dphi=0 at field>0 and
// 0..180 is untouched. Empirical anchor: field=+1.4 -> LS and ULS damage on dphi<0
// (it was on dphi>0 with the first, complementary conditions; README_Crossing sec 8).
inline int CrossingDirtySide(double field, double chg1, double chg2){
	if (chg1==0. || chg2==0.) return  0;
	if (field==0.){ std::cout<<"CrossingDirtySide .. field is zero or not set! exit...."<<std::endl; exit(0); }
	return (field>0.) ? -1 : +1;
}

//------------------------------------------------------------
// In place. x axis = dy (untouched), y axis = dphi in degrees, covering 360 deg with
// DPHINB%4==0 so that dphi=0 and +-90 are bin edges and every bin has a distinct mirror.
// Each dphi bin is paired with its mirror (dphi -> -dphi, mod 360; away side 180+x <-> 180-x),
// at the same dy.
//   near side (|dphi|<90), dirtySide=+-1: the dirty bin takes the clean bin's content AND error
//        (the two are then 100% correlated copies); the clean bin is unchanged.
//   away side (|dphi|>90): both bins get the mean, error sqrt(e1^2+e2^2)/2.
//   dirtySide=0 (a neutral leg, never pt-ordered): NOTHING is done -- we don't symmetrize what we
//        did not pt-order (user, 2026-09-25; README_Crossing sec 0).
// Valid bin <=> denominator hden (rho2(M) of the same slice, same binning) > 0. A bin with an
// empty denominator is invalid (its CF is undefined); rho2(S)=0 over a filled denominator is a
// VALID zero. Invalid bins are not averaged: if one partner is invalid both get the valid one;
// if both are invalid both keep their sentinels. On the near side the dirty bin copies the clean
// bin even if that is invalid (sentinel and all).
// The validity of the CORRECTED map follows the same rules; get it by correcting a 0/1 mask of
// hden>0 with itself as hden: CrossingCorrect(mask, dirtySide, mask).
inline void CrossingCorrect(TH2D* h, int dirtySide, const TH2D* hden){
	if (!h || !hden){ std::cout<<"CrossingCorrect .. null histogram! exit...."<<std::endl; exit(0); }
	if (hden->GetNbinsX()!=h->GetNbinsX() || hden->GetNbinsY()!=h->GetNbinsY()){
		std::cout<<"CrossingCorrect .. "<<h->GetName()<<": denominator binning differs. exit...."<<std::endl; exit(0);
	}
	TAxis* ax	= h->GetYaxis();
	int    nphi	= ax->GetNbins();
	double lo	= ax->GetXmin();
	double w	= ax->GetBinWidth(1);
	if (nphi%4!=0 || std::fabs(ax->GetXmax()-lo-360.)>1e-6 || ax->GetXbins()->GetSize()>0){
		std::cout<<"CrossingCorrect .. "<<h->GetName()<<": need uniform dphi binning over 360 deg with nbins%4==0, have "
			<<nphi<<" bins ["<<lo<<","<<ax->GetXmax()<<"). exit...."<<std::endl; exit(0);
	}
	if (dirtySide<-1 || dirtySide>1){
		std::cout<<"CrossingCorrect .. dirtySide must be -1, 0 or +1, have "<<dirtySide<<". exit...."<<std::endl; exit(0);
	}
	if (dirtySide==0) return;	// not pt-ordered -> not symmetrized
	for (int iy=1;iy<=nphi;iy++){
		double c	= ax->GetBinCenter(iy);
		double cw	= c;							// center wrapped to [-180,180)
		while (cw>= 180.) cw -= 360.;
		while (cw< -180.) cw += 360.;
		double cm	= -cw;							// mirror center, wrapped back into [lo,lo+360)
		while (cm<  lo      ) cm += 360.;
		while (cm>= lo+360. ) cm -= 360.;
		int jy		= ax->FindFixBin(cm);
		if (jy<1 || jy>nphi || std::fabs(ax->GetBinCenter(jy)-cm)>1e-6*w || jy==iy){
			std::cout<<"CrossingCorrect .. "<<h->GetName()<<": dphi bin "<<iy<<" has no distinct mirror bin. exit...."<<std::endl; exit(0);
		}
		if (jy<iy) continue;						// each pair once; read both before writing
		bool nearSide	= std::fabs(cw)<90.;
		for (int ix=1;ix<=h->GetNbinsX();ix++){
			double vi = h->GetBinContent(ix,iy), ei = h->GetBinError(ix,iy);
			double vj = h->GetBinContent(ix,jy), ej = h->GetBinError(ix,jy);
			bool   oki = hden->GetBinContent(ix,iy)>0., okj = hden->GetBinContent(ix,jy)>0.;
			if (nearSide){
				bool iDirty = (cw*dirtySide>0.);	// bin iy is on the dirty side, jy is its clean mirror
				if (iDirty){ h->SetBinContent(ix,iy,vj); h->SetBinError(ix,iy,ej); }
				else       { h->SetBinContent(ix,jy,vi); h->SetBinError(ix,jy,ei); }
			} else {
				double v,e;
				if      (oki && okj){ v = 0.5*(vi+vj); e = 0.5*std::sqrt(ei*ei+ej*ej); }
				else if (oki)       { v = vi;         e = ei; }
				else if (okj)       { v = vj;         e = ej; }
				else                { continue; }	// both invalid: keep sentinels
				h->SetBinContent(ix,iy,v); h->SetBinError(ix,iy,e);
				h->SetBinContent(ix,jy,v); h->SetBinError(ix,jy,e);
			}
		}
	}
}

//------------------------------------------------------------
// Check: number of in-range bins whose content or error differs from its dphi mirror at the
// same dy (0 for any map that has been through CrossingCorrect, and for sums/averages of such maps
// up to rounding; tol is relative). Assumes the binning CrossingCorrect accepted.
inline int CrossingAsymmetry(TH2D* h, double tol=1e-9){
	TAxis* ax	= h->GetYaxis();
	int    nphi	= ax->GetNbins();
	double lo	= ax->GetXmin();
	int    nasym= 0;
	for (int iy=1;iy<=nphi;iy++){
		double cm	= -ax->GetBinCenter(iy);
		while (cm<  lo      ) cm += 360.;
		while (cm>= lo+360. ) cm -= 360.;
		int jy		= ax->FindFixBin(cm);
		for (int ix=1;ix<=h->GetNbinsX();ix++){
			double vi = h->GetBinContent(ix,iy), vj = h->GetBinContent(ix,jy);
			double ei = h->GetBinError(ix,iy),   ej = h->GetBinError(ix,jy);
			if (std::fabs(vi-vj)>tol*(std::fabs(vi)+std::fabs(vj)) || std::fabs(ei-ej)>tol*(ei+ej)) ++nasym;
		}
	}
	return nasym;
}

#endif
