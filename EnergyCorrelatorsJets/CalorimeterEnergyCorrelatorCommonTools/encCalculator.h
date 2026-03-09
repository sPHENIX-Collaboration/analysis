#ifndef __ENCCALCULATOR_H__
#define __ENCCALCULATOR_H__

class encCalculator
{
	public : 
		enum EnergyCorrelatorType{ //allow for setting of Energy Correlator type
			Energy,
			Momentum,
			Transverse,
			TransverseMomentum
			Track,
			SpinDependent,
			EventObservable
		};
		struct skimmedDataPoints{
			float threshold=0.1; 
			std::vector<skimmedDataPoint> data_to_analyze;
		}
		struct skimmedDataPoints{
			float phi=0.;
			float eta=0.;
			float r=0.;
			float value=0.;
		}

		encCaluclator();
		~CalculateENC();
		void SkimDataSet(PHCompositeNode* RawTopNode, skimmedDataPoints* outputdata, float threshold=0.1 ); //take input format, apply a filter threshold, output paired down version
		void setENCParameters(float order1=2, EnergyCorrelatorType enctype=EnergyCorrelatorType::Energy, bool full=false){
		
		this->order=order1;
		this->typeofenc=enctype;
		this->fullshape=full;
	}
	private : 
		int order=2;
		bool fullshape=false;
		int typeofenc=EnergyCorrelatorType::Energy;
		std::pair<float, float> CalcuateTwoPoint(); 
};
#endif
