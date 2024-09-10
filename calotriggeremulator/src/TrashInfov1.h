#ifndef TRASHINFOV1_H
#define TRASHINFOV1_H

#include "TrashInfo.h"

#include <iostream>

class TrashInfov1 : public TrashInfo
{
 public:
  TrashInfov1() = default;
  virtual ~TrashInfov1() override = default;

  void identify(std::ostream &os = std::cout) const override;

  void Reset() override;

  int isValid() const override { return 1; }

  PHObject* CloneMe() const override { return new TrashInfov1(*this); }

  void setIsTrash(bool is_trash) override { _isTrash = is_trash; }
  bool isTrash() const override { return _isTrash; }

  void setR1(int r1) override { _r1 = r1; }
  int getR1() const override { return _r1; }

  void setEMCALorHCAL(int EMCALorHCAL) override { _EMCALorHCAL = EMCALorHCAL; }
  int getEMCALorHCAL() const override { return _EMCALorHCAL; }

  void setSpread(float spread) override { _spread = spread; }
  float getSpread() const override { return _spread; }

  void setEMCALEnergy(float emcal_energy) override { _emcal_energy = emcal_energy; }
  float getEMCALEnergy() const override { return _emcal_energy; }

  void setEnergy(float energy) override { _energy = energy; }
  float getEnergy() const override { return _energy; }

 private:
  bool _isTrash{false};
  int _r1{0};
  float _spread{0};
  float _emcal_energy{0};
  float _EMCALorHCAL{1};
  float _energy{0};
  ClassDefOverride(TrashInfov1, 1)
};

#endif
