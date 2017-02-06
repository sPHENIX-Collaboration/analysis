// $Id: $                                                                                             

/*!
 * \file TemperatureCorrection.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef TEMPERATURECORRECTION_H_
#define TEMPERATURECORRECTION_H_

class TemperatureCorrection
{
public:

  static
  double
  Apply(const double Amplitude, const double T)
  {
    //! Change of gain VS T
    // https://indico.bnl.gov/conferenceDisplay.py?confId=2293 : Joey Smiga
    static const double Slope = -.589 / 16.76; // m / b
    static const double T0 = 28.86; // reference temperature

    return Amplitude / (1. + Slope * (T - T0));
  }

};

#endif /* TEMPERATURECORRECTION_H_ */
