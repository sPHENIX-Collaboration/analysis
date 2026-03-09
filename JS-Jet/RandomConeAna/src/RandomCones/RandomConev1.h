#ifndef RANDOMCONES_RANDOMCONEV1_H
#define RANDOMCONES_RANDOMCONEV1_H


//===========================================================
/// \file RandomConev1.h
/// \author Tanner Mengel
//===========================================================

#include "RandomCone.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include <jetbase/Jet.h>

class RandomConev1 : public RandomCone
{
  public:

    RandomConev1() {};
    ~RandomConev1() override {}

    void identify(std::ostream &os = std::cout) const override;
    void Reset() override; // new in v1
    int isValid() const override { return 1; }

    void add_constituent(Jet * particle) override;

    void set_eta(float eta) override { m_eta = eta; }
    void set_phi(float phi) override { m_phi = phi; }
    void set_R(float R) override { m_R = R; }

    void set_cone_type(RandomCone::ConeType cone_type) override { m_cone_type = cone_type; }

    float get_pt() const override { return m_pt; }
    float get_eta() const override { return m_eta; }
    float get_phi() const override { return m_phi; }

    float get_R() const override { return m_R; }
    float get_area() const override { return M_PI*m_R*m_R; }

    RandomCone::ConeType get_cone_type() const override { return m_cone_type; }

    int n_clustered() const override { return _comp_ids.size(); }
    std::vector<std::pair<Jet::SRC, unsigned int>> & get_comp_vec() { return _comp_ids; }

  private:

    RandomCone::ConeType m_cone_type{RandomCone::ConeType::VOID};
  
    float m_pt{0.0};
    float m_eta{NAN};
    float m_phi{NAN};
    float m_R{NAN};

    std::vector<std::pair<Jet::SRC, unsigned int>> _comp_ids {};



    ClassDefOverride(RandomConev1, 1);
};

#endif // RHOBASE_TOWERRHOV1_H
