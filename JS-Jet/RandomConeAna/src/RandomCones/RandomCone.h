#ifndef RANDOMCONES_RANDOMCONE_H
#define RANDOMCONES_RANDOMCONE_H

//===========================================================
/// \file RandomCone.h
/// \brief PHObject to store rho and sigma for calorimeter towers on an event-by-event basis
/// \author Tanner Mengel
//===========================================================

#include <phool/PHObject.h>

#include <iostream>
#include <vector>
#include <utility>


#include <jetbase/Jet.h>

/// \class RandomCone
///
/// \brief PHObject to store cone constituents and properties
///

class RandomCone : public PHObject
{
  public:

    enum ConeType
    {
      VOID = 0,
      BASIC = 1,
      RANDOMIZE_INPUT_ETAPHI = 2,
      AVOID_LEAD_JET = 3,
    };

    ~RandomCone() override{};

    void identify(std::ostream &os = std::cout) const override { os << "RandomCone base class" << std::endl; };
    int isValid() const override { return 0; }

    // setters
    virtual void add_constituent(Jet * /*jet*/) { return; }
    virtual void set_eta(float /*eta*/) { return; }
    virtual void set_phi(float /*phi*/) { return; }
    
    virtual void set_R(float /*R*/) { return; }

    virtual void set_cone_type(ConeType /*cone_type*/) { return; }

    // getters
    virtual float get_pt() const { return 0; }
    
    virtual float get_eta() const { return 0; }
    virtual float get_phi() const { return 0; }

    virtual float get_R() const { return 0; }
    virtual float get_area() const { return 0; }

    virtual ConeType get_cone_type() const { return ConeType::VOID; }

    virtual int n_clustered() const { return 0; }
      

  protected:
    
    RandomCone() {} // ctor

  private:
    
    ClassDefOverride(RandomCone, 1);
};

#endif // RANDOMCONES_RANDOMCONE_H
