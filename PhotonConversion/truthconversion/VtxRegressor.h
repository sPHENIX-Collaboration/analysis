/**
 * @VtxRegressor.h
 * @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
 * @version 1.0
 *
 * @brief A wrapper class to interface with TMVA for regression of the VtxRegressor vtx
 */
#ifndef VtxRegressor_H__
#define VtxRegressor_H__

#include <GenFit/GFRaveVertex.h>
#include <TMVA/Reader.h>

class SvtxTrack;

class VtxRegressor
{
	public:
		VtxRegressor(std::string, std::string);
		~VtxRegressor();
		float regress(SvtxTrack*,SvtxTrack*,genfit::GFRaveVertex*);
		
	private:
		TMVA::Reader* reader=NULL;
    std::string _method;
		float pt1,pt2,phi,dphi,eta,deta,rin;
};
#endif //VtxRegressor_H__
