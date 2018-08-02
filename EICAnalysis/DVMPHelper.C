#include "DVMPHelper.h"

#include <cmath>
#include <iostream>
DVMPHelper::DVMPHelper(std::vector<float> reco_eta, 
		       std::vector<float> reco_phi,
		       std::vector<float> reco_ptotal, 
		       std::vector<int> reco_charge, 
		       std::vector<float> reco_cluster_e,
		       std::vector<bool> reco_is_scattered_lepton,
		       std::vector<float> true_eta, 
		       std::vector<float> true_phi,
		       std::vector<float> true_ptotal, 
		       std::vector<int> pid,
		       std::vector<bool> is_scattered_lepton)
{
  
  _size_reco = reco_eta.size();
  _size_truth = true_eta.size();

  rparticles = (DVMPHelper::particle_reco*)malloc(_size_reco * sizeof(DVMPHelper::particle_reco));
  tparticles = (DVMPHelper::particle_truth*)malloc(_size_truth * sizeof(DVMPHelper::particle_truth));
  
  for(int i = 0 ; i < _size_reco ; i++)
    {
      rparticles[i]=GetParticleReco(reco_eta.at(i),reco_phi.at(i),reco_ptotal.at(i),reco_charge.at(i),reco_cluster_e.at(i),reco_is_scattered_lepton.at(i));
    }
  for(int j = 0 ; j <_size_truth ; j++)
    {
      tparticles[j]=GetParticleTruth(true_eta.at(j),true_phi.at(j),true_ptotal.at(j),pid.at(j),is_scattered_lepton.at(j));
    }
}

DVMPHelper::particle_reco 
DVMPHelper::GetParticleReco(float eta, float phi, float ptotal, int charge,float e,bool is_scattered_lepton)
{
  DVMPHelper::particle_reco particle = {eta,phi,ptotal,charge,e,is_scattered_lepton};
  return particle;
}

DVMPHelper::particle_truth 
DVMPHelper::GetParticleTruth(float eta, float phi, float ptotal, int pid, bool is_scattered_lepton)
{
  DVMPHelper::particle_truth particle = {eta,phi,ptotal,pid,is_scattered_lepton};
  return particle;
}

float 
DVMPHelper::get_invariant_mass(DVMPHelper::particle_reco p1,
			       DVMPHelper::particle_reco p2)
{
  float pt1 = DVMPHelper::get_pt(p1.eta,p1.ptotal);
  float pt2 = DVMPHelper::get_pt(p2.eta,p2.ptotal);
  
  float eta1= p1.eta;
  float eta2= p2.eta;
  
  float phi1= p1.phi;
  float phi2= p2.phi;

  float Msquared = 2*pt1*pt2*(cosh(eta1-eta2)-cos(phi1-phi2));
  return sqrt(Msquared);
}

float 
DVMPHelper::get_invariant_mass(DVMPHelper::particle_truth p1,
			       DVMPHelper::particle_truth p2)
{
  float pt1 = DVMPHelper::get_pt(p1.eta,p1.ptotal);
  float pt2 = DVMPHelper::get_pt(p2.eta,p2.ptotal);
  
  float eta1= p1.eta;
  float eta2= p2.eta;
  
  float phi1= p1.phi;
  float phi2= p2.phi;

  float Msquared = 2*pt1*pt2*(cosh(eta1-eta2)-cos(phi1-phi2));
  return sqrt(Msquared);
}

float
DVMPHelper::get_pt(float eta, float ptotal)
{
  float theta = 2*atan(exp(-1*eta));
  float pt = ptotal * sin(theta);
  return pt;
}

bool
DVMPHelper::find_positron()
{
  for(int i = 0 ; i < _size_reco ; i++)
    {
      if(rparticles[i].charge==1)
	return true;
    }
  return false;
}

bool 
DVMPHelper::pass_cut(int index)
{
  const float ep_cut = 0.7;
  if( rparticles[index].e / rparticles[index].ptotal > ep_cut )
    return true;
  else
    return false;
}

std::vector<float>
DVMPHelper::calculateInvariantMass_1()
{
  std::vector<float> inv_mass;

  // Assert number of particles in event is greater than 1
  if(_size_reco<=1)
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  // Assert a positron exists
  if(!DVMPHelper::find_positron())
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  // Record index of positron and electrons
  int idx_positron=-1;
  std::vector<int> idx_electron;
  for(int i = 0 ; i < _size_reco ; i++)
    {
      if( rparticles[i].charge == 1 )
	idx_positron=i;
      else if( rparticles[i].charge == -1 )
	idx_electron.push_back(i);
    }
  // Ensure positron and electron(s) pass ep_cut
  if(!(pass_cut(idx_positron)))
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  for(unsigned i = 0 ; i < idx_electron.size() ; i++)
    {
      if( !pass_cut(idx_electron.at(i)) )
	idx_electron.erase(idx_electron.begin()+i);
    }

  if(idx_electron.size()==0) // all found electrons fail cut
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  // Calculate invariant mass for all electron and positron pairs
  for(unsigned i = 0; i < idx_electron.size() ; i++)
    {
      inv_mass.push_back(DVMPHelper::get_invariant_mass( rparticles[idx_electron.at(i)], rparticles[idx_positron]));
    }
  return inv_mass;
}

std::vector<float>
DVMPHelper::calculateInvariantMass_2()
{
  std::vector<float> inv_mass;

  // Record index of positron and electrons
  int idx_positron=-1;
  std::vector<int> idx_electron;
  for(int i = 0 ; i < _size_truth ; i++)
    {
      if( tparticles[i].pid == -11 )
	idx_positron=i;
      else if( tparticles[i].pid == 11 )
	idx_electron.push_back(i);
    }

  // Calculate invariant mass for all electron positron pairs
  for(unsigned i = 0; i < idx_electron.size() ; i++)
    {
      inv_mass.push_back(DVMPHelper::get_invariant_mass( tparticles[idx_electron.at(i)], tparticles[idx_positron]));
    }
  return inv_mass;
}

std::vector<float>
DVMPHelper::calculateInvariantMass_3()
{
  std::vector<float> inv_mass;

  // Assert number of particles in event is greater than 1
  if(_size_reco<=1)
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  // Assert a positron exists
  if(!DVMPHelper::find_positron())
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  // Record index of positron and electrons
  int idx_positron=-1;
  std::vector<int> idx_electron;
  for(int i = 0 ; i < _size_reco ; i++)
    {
      if( rparticles[i].charge == 1 )
	idx_positron=i;
      else if( rparticles[i].charge == -1 && rparticles[i].is_scattered_lepton==false )
	idx_electron.push_back(i);
    }
  // Ensure positron and electron(s) pass ep_cut
  if(!(pass_cut(idx_positron)))
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  for(unsigned i = 0 ; i < idx_electron.size() ; i++)
    {
      if( !pass_cut(idx_electron.at(i)) )
	idx_electron.erase(idx_electron.begin()+i);
    }

  if(idx_electron.size()==0) // all found electrons fail cut
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  // Calculate invariant mass for all electron and positron pairs
  for(unsigned i = 0; i < idx_electron.size() ; i++)
    {
      inv_mass.push_back(DVMPHelper::get_invariant_mass( rparticles[idx_electron.at(i)], rparticles[idx_positron]));
    }
  return inv_mass;
}

std::vector<float>
DVMPHelper::calculateInvariantMass_4()
{
  std::vector<float> inv_mass;

  // Assert number of particles in event is greater than 1
  if(_size_reco<=1)
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  // Assert a positron exists
  if(!DVMPHelper::find_positron())
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  // Record index of positron and electrons
  int idx_positron=-1;
  std::vector<int> idx_electron;
  for(int i = 0 ; i < _size_reco ; i++)
    {
      if( rparticles[i].charge == 1 )
	idx_positron=i;
      else if( rparticles[i].charge == -1 && rparticles[i].is_scattered_lepton==true )
	idx_electron.push_back(i);
    }
  // Ensure positron and electron(s) pass ep_cut
  if(!(pass_cut(idx_positron)))
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  
  for(unsigned i = 0 ; i < idx_electron.size() ; i++)
    {
      if( !pass_cut(idx_electron.at(i)) )
	idx_electron.erase(idx_electron.begin()+i);
    }

  if(idx_electron.size()==0) // all found electrons fail cut
    {
      inv_mass.push_back(NAN);
      return inv_mass;
    }
  // Calculate invariant mass for all electron and positron pairs
  for(unsigned i = 0; i < idx_electron.size() ; i++)
    {
      inv_mass.push_back(DVMPHelper::get_invariant_mass( rparticles[idx_electron.at(i)], rparticles[idx_positron]));
    }
  return inv_mass;
}

std::vector<float>
DVMPHelper::calculateInvariantMass_5()
{
  std::vector<float> inv_mass;
  
  // Record index of positron and electrons
  int idx_positron=-1;
  std::vector<int> idx_electron;
  for(int i = 0 ; i < _size_truth ; i++)
    {
      if( tparticles[i].pid == -11 )
	idx_positron=i;
      else if( tparticles[i].pid == 11 && tparticles[i].is_scattered_lepton==false)
	idx_electron.push_back(i);
    }
  
  // Calculate invariant mass for all electron positron pairs
  for(unsigned i = 0; i < idx_electron.size() ; i++)
    {
      inv_mass.push_back(DVMPHelper::get_invariant_mass( tparticles[idx_electron.at(i)], tparticles[idx_positron]));
    }
  
  return inv_mass;
}

std::vector<float>
DVMPHelper::calculateInvariantMass_6()
{
  std::vector<float> inv_mass;
  // Record index of positron and electrons
  int idx_positron=-1;
  std::vector<int> idx_electron;
  for(int i = 0 ; i < _size_truth ; i++)
    {
      if( tparticles[i].pid == -11 )
	idx_positron=i;
      else if( tparticles[i].pid == 11 && tparticles[i].is_scattered_lepton==true)
	idx_electron.push_back(i);
    }

  // Calculate invariant mass for all electron positron pairs
  for(unsigned i = 0; i < idx_electron.size() ; i++)
    {
      inv_mass.push_back(DVMPHelper::get_invariant_mass( tparticles[idx_electron.at(i)], tparticles[idx_positron]));
    }
  return inv_mass;
}

