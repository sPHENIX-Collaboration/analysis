#ifndef JetInidicesMatcher__h
#define JetInidicesMatcher__h

#include <vector>
#include <array>
using std::vector;
using std::array;
using std::pair;

class JetIndicesMatcher {
    // a simple class that matches jets based on their pt,
    // This is done by matching the first jets (feed in presumable
    // by pt) with phi-eta distance less than R
    const double R2; // distance in phi-eta squared
    vector<float> eta_truth{};
    vector<float> phi_truth{};
    vector<float> pt_truth{};
    vector<int>   index_truth{};

    vector<float> eta_reco{};
    vector<float> phi_reco{};
    vector<float> pt_reco{};
    vector<int>   index_reco{};

    float min_pT_truth ;
    float min_pT_reco  ;

public:
    vector<unsigned int> indices_fake{};
    vector<unsigned int> indices_miss{};
    vector<pair < unsigned int, unsigned int> > indices_matched{};

    vector<unsigned int> &f{indices_fake};
    vector<unsigned int> &m{indices_miss};
    vector<pair<unsigned int,unsigned int>> &match{indices_matched};

    void reset();

    /* void add_truth(float eta, float phi, float pt); */
    /* void add_reco(float eta, float phi, float pt); */

    void add_truth(vector<float> &eta, vector<float> &phi, vector<float> &pt);

    void add_reco(vector<float> &eta, vector<float> &phi, vector<float> &pt);

    array<unsigned int, 3> do_matching(); // returns n-matched, n-miss, n-fake
    JetIndicesMatcher(float R, float min_pT_truth=-1000., float min_pT_reco=-1000.);
};
#endif
