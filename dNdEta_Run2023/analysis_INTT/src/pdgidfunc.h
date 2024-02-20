#ifndef PDGIDFUNC_H
#define PDGIDFUNC_H

// Direct translate from Reference: https://github.com/scikit-hep/particle/blob/master/src/particle/pdgid/functions.py

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <stdio.h>

enum class Location 
{
    Nj = 1,
    Nq3 = 2,
    Nq2 = 3,
    Nq1 = 4,
    Nl = 5,
    Nr = 6,
    N = 7,
    N8 = 8,
    N9 = 9,
    N10 = 10
};

int abspid(int pdgid) {
    return std::abs(int(pdgid));
}

int _digit(int pdgid, int loc) 
{
    // Provides a convenient index into the PDGID number, whose format is in base 10.
    // Returns the digit at position 'loc' given that the right-most digit is at position 1.
    std::string sid = std::to_string(abspid(pdgid));
    return (loc <= sid.length()) ? std::stoi(sid.substr(sid.length() - loc, 1)) : 0;
}

int _extra_bits(int pdgid) 
{
    return abspid(pdgid) / 10000000;
}

int _fundamental_id(int pdgid) 
{
    if (_extra_bits(pdgid) > 0) 
    {
        return 0;
    }
    int aid = abspid(pdgid);
    if (aid <= 100) 
    {
        return aid;
    }
    if (_digit(pdgid, (int) Location::Nq2) == _digit(pdgid, (int) Location::Nq1) == 0) 
    {
        return aid % 10000;
    }
    return 0;
}

bool is_quark(int pdgid) 
{
    return (abspid(pdgid) >= 1 && abspid(pdgid) <= 8) ? true:false;
}

bool is_meson(int pdgid) 
{
    if (_extra_bits(pdgid) > 0) 
    {
        return false;
    }
    int aid = abspid(pdgid);
    if (aid <= 100) 
    {
        return false;
    }
    if (0 < _fundamental_id(pdgid) && _fundamental_id(pdgid) <= 100) 
    {
        return false;
    }
    // Special IDs - K(L)0, ???, K(S)0
    if (aid == 130 || aid == 210 || aid == 310) 
    {
        return true;
    }
    // Special IDs - B(L)0, B(sL)0, B(H)0, B(sH)0
    if (aid == 150 || aid == 350 || aid == 510 || aid == 530) 
    {
        return true;
    }
    // Special particles - reggeon, pomeron, odderon
    if (pdgid == 110 || pdgid == 990 || pdgid == 9990) 
    {
        return true;
    }
    // Generator-specific "particles" for GEANT tracking purposes
    if (aid == 998 || aid == 999) 
    {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nj) > 0 && _digit(pdgid, (int) Location::Nq3) > 0 && _digit(pdgid, (int) Location::Nq2) > 0 && _digit(pdgid, (int) Location::Nq1) == 0) 
    {
        // check for illegal antiparticles
        return (_digit(pdgid, (int) Location::Nq3) != _digit(pdgid, (int) Location::Nq2) || pdgid >= 0);
    }
    return false;
}

bool is_baryon(int pdgid) 
{
    int aid = abspid(pdgid);
    if (aid <= 100) 
    {
        return false;
    }
    // Special case of proton and neutron:
    // needs to be checked first since _extra_bits(pdgid) > 0 for nuclei
    if (aid == 1000000010 || aid == 1000010010) 
    {
        return true;
    }

    if (_extra_bits(pdgid) > 0) 
    {
        return false;
    }

    if (0 < _fundamental_id(pdgid) && _fundamental_id(pdgid) <= 100) 
    {
        return false;
    }

    // Old codes for diffractive p and n (MC usage)
    if (aid == 2110 || aid == 2210) 
    {
        return true;
    }

    if (aid == 9221132 || aid == 9331122) 
    {
        return false;
    }

    return (
        _digit(pdgid, (int) Location::Nj) > 0
        && _digit(pdgid, (int) Location::Nq3) > 0
        && _digit(pdgid, (int) Location::Nq2) > 0
        && _digit(pdgid, (int) Location::Nq1) > 0
    );
}

bool is_SUSY(int pdgid) {
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::N) != 1 && _digit(pdgid, (int) Location::N) != 2) 
    {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nr) != 0) 
    {
        return false;
    }
    return _fundamental_id(pdgid) != 0;
}

bool is_Rhadron(int pdgid) {
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::N) != 1) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nr) != 0) {
        return false;
    }
    if (is_SUSY(pdgid)) {
        return false;
    }
    // All R-hadrons have at least 3 core digits
    return (
        _digit(pdgid, (int) Location::Nq2) != 0
        && _digit(pdgid, (int) Location::Nq3) != 0
        && _digit(pdgid, (int) Location::Nj) != 0
    );
}

bool is_hadron(int pdgid) {
    // Special case of proton and neutron:
    // needs to be checked first since _extra_bits(pdgid) > 0 for nuclei
    if (abs(pdgid) == 1000000010 || abs(pdgid) == 1000010010) {
        return true;
    }
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    if (is_meson(pdgid)) {
        return true;
    }
    if (is_baryon(pdgid)) {
        return true;
    }
    // Irrelevant test since all pentaquarks are baryons!
    // if (is_pentaquark(pdgid)) return true;
    return is_Rhadron(pdgid);
}

int A(int pdgid) {
    /*
    Returns the atomic mass number A if the PDG ID corresponds to a nucleus, else it returns None.
    The (atomic) mass number is also known as the nucleon number, the total number of protons and neutrons.
    The number of neutrons is hence N = A - Z.
    */
    // A proton can be a Hydrogen nucleus
    // A neutron can be considered as a nucleus when given the PDG ID 1000000010,
    // hence consistency demands that A(neutron) = 1
    if (abspid(pdgid) == 2112 || abspid(pdgid) == 2212) 
    {
        return 1;
    }
    if (_digit(pdgid, (int) Location::N10) != 1 || _digit(pdgid, (int) Location::N9) != 0) 
    {
        return -1;
    }
    return (abspid(pdgid) / 10) % 1000;
}

int Z(int pdgid) {
    /*
    Returns the nuclear charge Z if the PDG ID corresponds to a nucleus, else it returns None.
    The nuclear charge number Z is also known as the atomic number.
    For ordinary nuclei Z is equal to the number of protons.
    */
    // A proton can be a Hydrogen nucleus
    if (abspid(pdgid) == 2212) 
    {
        return int(pdgid) / 2212;
    }
    // A neutron can be considered as a nucleus when given the PDG ID 1000000010,
    // hence consistency demands that Z(neutron) = 0
    if (abspid(pdgid) == 2112) 
    {
        return 0;
    }
    if (_digit(pdgid, (int) Location::N10) != 1 || _digit(pdgid, (int) Location::N9) != 0) 
    {
        return -1;
    }
    return ((abspid(pdgid) / 10000) % 1000) * (int(pdgid) / abs(int(pdgid)));
}


bool is_nucleus(int pdgid) {
    /*
    Does this PDG ID correspond to a nucleus?
    Ion numbers are +/- 10LZZZAAAI.
    AAA is A - total baryon number
    ZZZ is Z - total charge
    L is the total number of strange quarks.
    I is the isomer number, with I=0 corresponding to the ground state.
    */
    // A proton can be a Hydrogen nucleus
    // A neutron can be considered as a nucleus when given the PDG ID 1000000010,
    // hence consistency demands that is_nucleus(neutron) is True
    if (abspid(pdgid) == 2112 || abspid(pdgid) == 2212) {
        return true;
    }
    if (_digit(pdgid, (int) Location::N10) == 1 && _digit(pdgid, (int) Location::N9) == 0) {
        // Charge should always be less than or equal to the baryon number
        int A_pdgid = A(pdgid);
        int Z_pdgid = Z(pdgid);

        // At this point neither A_pdgid nor Z_pdgid can be None,
        // see the definitions of the A and Z functions
        if (A_pdgid >= abs(Z_pdgid)) {
            return true;
        }
    }
    return false;
}

bool is_Qball(int pdgid) {
    /*
    Does this PDG ID correspond to a Q-ball or any exotic particle with electric charge beyond the qqq scheme?
    Ad-hoc numbering for such particles is +/- 100XXXY0, where XXX.Y is the charge.
    */
    if (_extra_bits(pdgid) != 1) {
        return false;
    }
    if (_digit(pdgid, (int)  Location::N) != 0) {
        return false;
    }
    if (_digit(pdgid, (int)  Location::Nr) != 0) {
        return false;
    }
    if ((abspid(pdgid) / 10) % 10000 == 0) {
        return false;
    }
    return _digit(pdgid, (int) Location::Nj) == 0;
}

bool is_dyon(int pdgid) 
{
    /*
    Does this PDG ID correspond to a Dyon, a magnetic monopole?
    Magnetic monopoles and Dyons are assumed to have one unit of Dirac monopole charge
    and a variable integer number xyz units of electric charge,
    where xyz stands for Nq1 Nq2 Nq3.
    Codes 411xyz0 are used when the magnetic and electrical charge sign agree and 412xyz0 when they disagree,
    with the overall sign of the particle set by the magnetic charge.
    For now, no spin information is provided.
    */
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::N) != 4) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nr) != 1) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nl) != 1 && _digit(pdgid, (int) Location::Nl) != 2) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nq3) == 0) {
        return false;
    }
    return _digit(pdgid, (int) Location::Nj) == 0;
}

bool is_diquark(int pdgid) {
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    if (abs(abspid(pdgid)) <= 100) {
        return false;
    }
    if (0 < _fundamental_id(pdgid) && _fundamental_id(pdgid) <= 100) {
        return false;
    }
    return (
        _digit(pdgid, (int) Location::Nj) > 0
        && _digit(pdgid, (int) Location::Nq3) == 0
        && _digit(pdgid, (int) Location::Nq2) > 0
        && _digit(pdgid, (int) Location::Nq1) > 0
    );
}

bool is_generator_specific(int pdgid) {
    int aid = abs(abspid(pdgid));
    if (81 <= aid && aid <= 100) {
        return true;
    }
    if (901 <= aid && aid <= 930) {
        return true;
    }
    if (1901 <= aid && aid <= 1930) {
        return true;
    }
    if (2901 <= aid && aid <= 2930) {
        return true;
    }
    if (3901 <= aid && aid <= 3930) {
        return true;
    }
    if (aid == 998 || aid == 999) {
        return true;
    }
    if (aid == 20022 || aid == 480000000) {  // Special cases of opticalphoton and geantino
        return true;
    }
    return false;
}

bool is_technicolor(int pdgid) 
{
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    return _digit(pdgid, (int) Location::N) == 3;
}

bool is_excited_quark_or_lepton(int pdgid) 
{
    if (_extra_bits(pdgid) > 0) 
    {
        return false;
    }
    if (_fundamental_id(pdgid) == 0) 
    {
        return false;
    }

    return _digit(pdgid, (int) Location::N) == 4 && _digit(pdgid, (int) Location::Nr) == 0;
}

bool is_gauge_boson_or_higgs(int pdgid) {
    /*
    Does this PDG ID correspond to a gauge boson or a Higgs?
    Codes 21-30 are reserved for the Standard Model gauge bosons and the Higgs.
    The graviton and the boson content of a two-Higgs-doublet scenario
    and of additional SU(2)xU(1) groups are found in the range 31-40.
    */
    return (abs(pdgid) >= 21 && abs(pdgid) <= 40) ? true:false ;
}

bool is_pentaquark(int pdgid) {
    if (_extra_bits(pdgid) > 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::N) != 9) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nr) == 9 || _digit(pdgid, (int) Location::Nr) == 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nj) == 9 || _digit(pdgid, (int) Location::Nl) == 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nq1) == 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nq2) == 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nq3) == 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nj) == 0) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nq2) > _digit(pdgid, (int) Location::Nq1)) {
        return false;
    }
    if (_digit(pdgid, (int) Location::Nq1) > _digit(pdgid, (int) Location::Nl)) {
        return false;
    }
    return _digit(pdgid, (int) Location::Nl) <= _digit(pdgid, (int) Location::Nr);
}

bool is_valid(int pdgid) {
    /*Is it a valid PDG ID?*/
    if (is_gauge_boson_or_higgs(pdgid)) { // test first since quickest check
        return true;
    }
    if (_fundamental_id(pdgid) != 0) { // function always returns a number >= 0
        return true;
    }
    if (is_meson(pdgid)) {
        return true;
    }
    if (is_baryon(pdgid)) {
        return true;
    }
    if (is_pentaquark(pdgid)) {
        return true;
    }
    if (is_SUSY(pdgid)) {
        return true;
    }
    if (is_Rhadron(pdgid)) {
        return true;
    }
    if (is_dyon(pdgid)) {
        return true;
    }
    if (is_diquark(pdgid)) {
        return true;
    }
    if (is_generator_specific(pdgid)) {
        return true;
    }
    if (is_technicolor(pdgid)) {
        return true;
    }
    if (is_excited_quark_or_lepton(pdgid)) {
        return true;
    }
    if (_extra_bits(pdgid) > 0) {
        return is_Qball(pdgid) || is_nucleus(pdgid);
    }
    return false;
}

int three_charge(int pdgid) 
{
    if (!is_valid(pdgid)) 
    {
        // return NULL;
        return -999;
    }

    int aid = abspid(pdgid);
    // int charge = NULL; // pylint: disable=redefined-outer-name
    int charge = -999;
    int ch100[] = {
        -1, 2, -1, 2, -1, 2, -1, 2, 0, 0, -3, 0, -3, 0, -3, 0, -3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int q1 = _digit(pdgid, (int) Location::Nq1);
    int q2 = _digit(pdgid, (int) Location::Nq2);
    int q3 = _digit(pdgid, (int) Location::Nq3);
    int sid = _fundamental_id(pdgid);
    // std::cout << "sid=" << sid << std::endl;

    if (_extra_bits(pdgid) > 0) 
    {
        if (is_nucleus(pdgid)) 
        { // ion
            int Z_pdgid = Z(pdgid);
            // return (Z_pdgid == NULL) ? NULL : 3 * Z_pdgid;
            return 3 * Z_pdgid;
        }
        if (is_Qball(pdgid)) 
        { // Qball
            charge = 3 * ((aid / 10) % 10000);
        } 
        else 
        { // this should never be reached in the present numbering scheme
            // return NULL; // since extra bits exist only for Q-balls and nuclei
            return -999;
        }
    } 
    else if (is_dyon(pdgid)) 
    { // Dyon
        charge = 3 * ((aid / 10) % 1000);
        // this is half right
        // the charge sign will be changed below if pid < 0
        if (_digit(pdgid, (int) Location::Nl) == 2) 
        {
            charge = -charge;
        }
    } 
    else if (sid > 0 && sid <= 100) 
    { // use table
        charge = ch100[sid - 1];
        if (aid == 1000017 || aid == 1000018 || aid == 1000034 || aid == 1000052 || aid == 1000053 || aid == 1000054) 
        {
            charge = 0;
        }
        if (aid == 5100061 || aid == 5100062) 
        {
            charge = 6;
        }
    } 
    else if (_digit(pdgid, (int) Location::Nj) == 0) 
    { // KL, KS, or undefined
        return 0;
    } 
    else if (q1 == 0 || (is_Rhadron(pdgid) && q1 == 9)) 
    { // mesons
        if (q2 == 3 || q2 == 5) 
        {
            charge = ch100[q3 - 1] - ch100[q2 - 1];
        } 
        else 
        {
            charge = ch100[q2 - 1] - ch100[q3 - 1];
        }
    } 
    else if (q3 == 0) 
    { // diquarks
        charge = ch100[q2 - 1] + ch100[q1 - 1];
    } 
    else if (is_baryon(pdgid) || (is_Rhadron(pdgid) && _digit(pdgid, (int) Location::Nl) == 9)) 
    { // baryons
        charge = ch100[q3 - 1] + ch100[q2 - 1] + ch100[q1 - 1];
    }
    else if (is_pentaquark(pdgid))
    {
        if (aid == 9221132) 
        {
            charge = 3;
        }
        if (aid == 9331122) 
        {
            charge = -6;
        }
    }

    if (charge != -999 && (int) pdgid < 0) {
        charge = -charge;
    }
    return charge;
}

float charge(int pdgid) {
    float three_charge_pdgid = three_charge(pdgid);
    // if (three_charge_pdgid == NULL) 
    if (three_charge_pdgid == -999) 
    {
        // return NULL;
        return -999;
    }
    if (!is_Qball(pdgid)) 
    {
        return three_charge_pdgid / 3.0;
    }
    return three_charge_pdgid / 30.0;
}

bool is_chargedHadron(int pdgid)
{
    bool is_hadron = (is_meson(pdgid) || is_baryon(pdgid)) ? true : false;
    bool is_charged = (fabs(charge(pdgid)) > 0) ? true : false;
    return (is_hadron && is_charged) ? true : false;
} 


#endif