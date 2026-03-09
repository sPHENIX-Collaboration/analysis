#include "JetIndicesMatcher.h"
#include "cmath"
using namespace std;

JetIndicesMatcher::JetIndicesMatcher(float R, float min_T, float min_R) 
  : R2 {R*R}, min_pT_truth{min_T}, min_pT_reco{min_R}
{}

void JetIndicesMatcher::reset()
{
    eta_truth.clear();
    phi_truth.clear();
    pt_truth.clear();
    index_truth.clear();

    eta_reco.clear();
    phi_reco.clear();
    pt_reco.clear();
    index_reco.clear();

    indices_fake.clear();
    indices_miss.clear();
    indices_matched.clear();
}

/* void JetIndicesMatcher::add_truth(float eta, float phi, float pt) { */
/*     /1* if (fabs(eta)>0.6) return; *1/ */
/*     eta_truth.push_back(eta); */
/*     phi_truth.push_back(phi); */
/*     pt_truth.push_back(pt); */
/* } */

/* void JetIndicesMatcher::add_reco(float eta, float phi, float pt) { */
/*     /1* if (fabs(eta)>0.6) return; *1/ */
/*     eta_reco.push_back(eta); */
/*     phi_reco.push_back(phi); */
/*     pt_reco.push_back(pt); */
/* } */

void JetIndicesMatcher::add_truth(vector<float>& eta, vector<float>& phi, vector<float>& pt) {
    vector<array<float,4>> jets;
    for (unsigned int i=0; i<eta.size(); ++i) {
        jets.push_back({pt[i],eta[i],phi[i], (float) i});
    }
    std::sort(jets.rbegin(), jets.rend());
    for (auto jet : jets) {
      pt_truth  .push_back(jet[0]);
      eta_truth .push_back(jet[1]);
      phi_truth .push_back(jet[2]);
      index_truth .push_back((int)jet[3]);
    }
}
void JetIndicesMatcher::add_reco(vector<float>& eta, vector<float>& phi, vector<float>& pt) {
    vector<array<float,4>> jets;
    for (unsigned int i=0; i<eta.size(); ++i) {
        jets.push_back({pt[i],eta[i],phi[i], (float)i});
    }
    std::sort(jets.rbegin(), jets.rend());
    for (auto jet : jets) {
      pt_reco  .push_back(jet[0]);
      eta_reco .push_back(jet[1]);
      phi_reco .push_back(jet[2]);
      index_reco .push_back((int)jet[3]);
    }
}
array<unsigned int,3> JetIndicesMatcher::do_matching() {
    indices_fake.clear();
    indices_miss.clear();
    indices_matched.clear();

    vector<bool> is_matched (eta_reco.size(),false);

    //FIXME
          /* cout << " Matching Truth Jets " << endl; */
          /* for (int i = 0; i< pt_truth.size(); ++i) { */
          /*   cout << Form("  Tjet [%2i] pt:phi:eta [%5.2f,%5.2f,%5.2f]", */
          /*       i, pt_truth[i],phi_truth[i],eta_truth[i]) <<endl; */
          /* } */
          /* cout << " In matcher : Reco Jets " << endl; */
          /* for (int i = 0; i< pt_reco.size(); ++i) { */
          /*   cout << Form("  Mjet [%2i] pt:phi:eta [%5.2f,%5.2f,%5.2f]", */
          /*       i, pt_reco[i],phi_reco[i],eta_reco[i]) <<endl; */
          /* } */


    for (unsigned int T=0;T<eta_truth.size();++T) {
      if (pt_truth[T]<min_pT_truth) continue;
        /* if (fabs(eta_truth[T])>0.6) continue; */
        bool found_match { false };
        for (unsigned int R=0;R<eta_reco.size();++R) {
          if (pt_reco[R]<min_pT_reco) continue;
            /* if (fabs(eta_reco[R])>0.6) continue; */
            if (is_matched[R]) continue;
            float dphi = fabs(phi_truth[T]-phi_reco[R]);
            while (dphi>M_PI) dphi = fabs(dphi - 2*M_PI);
            const float deta = eta_truth[T]-eta_reco[R];
            const float R2_comp = deta*deta + dphi*dphi;
            if (R2_comp<=R2) {
                found_match = true;
                is_matched[R] = true;
                indices_matched.push_back({index_truth[T],index_reco[R]});

                /* cout << " ADDED MATCH( " << T << ":"<<R<<")" << endl; */
                break;
            }
        }
        if (!found_match) indices_miss.push_back(index_truth[T]);
    }
    for (unsigned int R=0;R<eta_reco.size();++R) {
      /* cout << " min_pT_reco: " << min_pT_reco << " and["<<R<<"] " << pt_reco[R] << endl; */
        if (pt_reco[R]<min_pT_reco) continue;
        if (!is_matched[R]) {
          indices_fake.push_back(index_reco[R]);
          /* cout << " indices fake: "; for (auto I : indices_fake) cout << " " << I; cout << endl; */
        }
    }
    return {static_cast<unsigned int>(indices_matched.size()),
            static_cast<unsigned int>(indices_miss.size()),
            static_cast<unsigned int>(indices_fake.size())};
}
