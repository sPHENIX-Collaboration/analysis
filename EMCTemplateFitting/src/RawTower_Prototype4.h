// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PROTOTYPE4_RAWTOWERPROTOTYPE4_H
#define PROTOTYPE4_RAWTOWERPROTOTYPE4_H

#include "PROTOTYPE4_FEM.h"

#include <calobase/RawTower.h>
#include <calobase/RawTowerDefs.h>

#include <iostream>  // for cout, ostream

class RawTower_Prototype4 : public RawTower
{
 public:
  RawTower_Prototype4();
  RawTower_Prototype4(const RawTower &tower);
  RawTower_Prototype4(RawTowerDefs::keytype id);
  RawTower_Prototype4(const unsigned int icol, const unsigned int irow);
  RawTower_Prototype4(const RawTowerDefs::CalorimeterId caloid,
                      const unsigned int ieta, const unsigned int iphi);
  virtual ~RawTower_Prototype4() {}

  void Reset() override;
  int isValid() const override;
  void identify(std::ostream &os = std::cout) const override;

  void set_id(RawTowerDefs::keytype id) override { towerid = id; }
  RawTowerDefs::keytype get_id() const override { return towerid; }
  int get_bineta() const override { return RawTowerDefs::decode_index1(towerid); }
  int get_binphi() const override { return RawTowerDefs::decode_index2(towerid); }
  int get_column() const override { return RawTowerDefs::decode_index1(towerid); }
  int get_row() const override { return RawTowerDefs::decode_index2(towerid); }
  double get_energy() const override { return energy; }
  void set_energy(const double e) override { energy = e; }
  float get_time() const override { return time; }
  void set_time(const float t) override { time = t; }
  //---Raw data
  //access------------------------------------------------------------

  enum
  {
    NSAMPLES = PROTOTYPE4_FEM::NSAMPLES
  };
  typedef float signal_type;

  void set_signal_samples(int i, signal_type sig);
  signal_type get_signal_samples(int i) const;
  void set_HBD_channel_number(int i) { HBD_channel = i; }
  int get_HBD_channel_number() const { return HBD_channel; }

  //---Fits------------------------------------------------------------

  double get_energy_peak_sample(int verbosity = 0);
  double get_energy_power_law_exp(int verbosity = 0);
  double get_energy_power_law_double_exp(int verbosity = 0);

 protected:
  RawTowerDefs::keytype towerid;

  //! energy assigned to the tower. Depending on stage of process and DST node
  //! name, it could be energy deposition, light yield or calibrated energies
  double energy;
  //! Time stamp assigned to the tower. Depending on the tower maker, it could
  //! be rise time or peak time.
  float time;

  // Signal samples from DATA
  signal_type signal_samples[NSAMPLES];  // Low Gain
  int HBD_channel;

  ClassDefOverride(RawTower_Prototype4, 3)
};

#endif
