#ifndef TRASHINFO_H
#define TRASHINFO_H

#include <phool/PHObject.h>

class TrashInfo : public PHObject
{
 public:
  ~TrashInfo() override{};

  void identify(std::ostream &os = std::cout) const override { os << "TrashInfo base class" << std::endl; };
  void Reset() override {}
  int isValid() const override { return 0; }
  virtual void setIsTrash(bool) { return; }
  virtual bool isTrash() const { return false; }

  virtual void setR1(int) { return; }
  virtual int getR1() const { return 0; }

  virtual void setEMCALorHCAL(int) { return; }
  virtual int getEMCALorHCAL() const { return 0; }

  virtual void setSpread(float) { return; }
  virtual float getSpread() const { return 0; }

  virtual void setEMCALEnergy(float) { return; }
  virtual float getEMCALEnergy() const { return 0; }

  virtual void setEnergy(float) { return; }
  virtual float getEnergy() const { return 0; }

 protected:
  TrashInfo() {}

 private:
  ClassDefOverride(TrashInfo, 1);
};

#endif  // TRIGGER_MINIMUMBIASINFO_H
