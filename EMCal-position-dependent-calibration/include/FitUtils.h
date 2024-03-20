#ifndef FITUTILS_H_
#define FITUTILS_H_

#include <TH1.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TStyle.h>

using std::min;
using std::max;

class FitUtils {
    public:

    static Int_t single_fit(TH1* hist,
                            UInt_t &ndf,
                            Double_t &mu,
                            Double_t &mu_error,
                            Double_t &chi2,
                            Double_t &pvalue,
                            Double_t &sigma,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t single_fit(TH1* hist,
                            UInt_t &ndf,
                            Double_t &mu,
                            Double_t &mu_error,
                            Double_t &chi2,
                            Double_t &pvalue,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t single_fit(TH1* hist,
                            UInt_t &ndf,
                            Double_t &mu,
                            Double_t &mu_error,
                            Double_t &chi2,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t single_fit(TH1* hist,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t double_fit(TH1* hist,
                            UInt_t &ndf,
                            Double_t &mu,
                            Double_t &mu_error,
                            Double_t &chi2,
                            Double_t &pvalue,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t double_fit(TH1* hist,
                            UInt_t &ndf,
                            Double_t &mu,
                            Double_t &mu_error,
                            Double_t &chi2,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t double_fit(TH1* hist,
                            Double_t x_min,
                            Double_t x_max);

    static Int_t double_fitv2(TH1* hist,
                            UInt_t &ndf,
                            Double_t &mu,
                            Double_t &mu_error,
                            Double_t &chi2,
                            Double_t &pvalue,
                            Double_t x_min,
                            Double_t x_max);
};

#endif // FITUTILS_H_
