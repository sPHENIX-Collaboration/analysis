#include "FitUtils.h"

/**
 * Fits the histogram to a gaussian over the specified range.
 * @param hist the histogram that needs to be fitted.
 * @param ndf the number of degrees of freedom of the fit.
 * @param mu the mean from the fit.
 * @param mu_error the error on the mean of the fit.
 * @param chi2 the chi-squared of the fit.
 * @param pvalue the chi-squared probability of the fit.
 * @param sigma standard deviation parameter of the fit.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit Default value 0..
 * @return 0 if success. 1 to indicate fitting range is empty. Otherwise error code from fit is returned according to
 * https://root.cern.ch/doc/master/classTMinuit.html#ab48dd4b48800edc090396e35cb465fb9
 */
Int_t FitUtils::single_fit(TH1* hist,
                         UInt_t &ndf,
                         Double_t &mu,
                         Double_t &mu_error,
                         Double_t &chi2,
                         Double_t &pvalue,
                         Double_t &sigma,
                         Double_t x_min,
                         Double_t x_max) {
    // Default value is 1 to indicate error
    Int_t fitStatus = 1;

    ndf       = 0;
    mu        = 0;
    mu_error  = 0;
    sigma     = 0;
    chi2      = 0;
    pvalue    = 0;

    // check first that there is at least one entry within the fitting range provided to perform the fit.
    Int_t bin_min = hist->FindBin(x_min);
    Int_t bin_max = hist->FindBin(x_max);
    Int_t counts = hist->Integral(bin_min, bin_max);
    if(!counts) return fitStatus;

    TFitResultPtr fit;

    auto f1 = new TF1("f1","gaus",x_min,x_max);
    // fitting options being Used
    // S: The full result of the fit is returned in the TFitResultPtr. This is needed to get the covariance matrix of the fit. See TFitResult and the base class ROOT::Math::FitResult.
    // Q: Quiet mode (minimum printing)
    // L: Uses a log likelihood method (default is chi-square method). To be used when the histogram represents counts.
    // R: Fit using a fitting range specified in the function range with TF1::SetRange.
    fit = hist->Fit(f1,"SQLR");
    fitStatus = fit;

    // attempt to perform fit
    // check to make sure ndf is nonzero along with status of the fit
    // fitStatus = 0: the fit has been performed successfully (i.e no error occurred).
    // fitStatus != 0: there is an error not connected with the minimization procedure, for example when a wrong function is used.
    if(!fitStatus && fit->Ndf()) {
        gStyle->SetOptFit(1011);
        mu = fit->Parameter(1);
        mu_error = fit->ParError(1);
        sigma = fit->Parameter(2);
        chi2 = fit->Chi2();
        ndf = fit->Ndf();
        pvalue = fit->Prob();
    }
    return fitStatus;
}

/**
 * Fits the histogram to a gaussian over the specified range.
 * @param hist the histogram that needs to be fitted.
 * @param ndf the number of degrees of freedom of the fit.
 * @param mu the mean from the fit.
 * @param mu_error the error on the mean of the fit.
 * @param chi2 the chi-squared of the fit.
 * @param pvalue the chi-squared probability of the fit.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit Default value 0..
 * @return 0 if success. 1 to indicate fitting range is empty. Otherwise error code from fit is returned according to
 * https://root.cern.ch/doc/master/classTMinuit.html#ab48dd4b48800edc090396e35cb465fb9
 */
Int_t FitUtils::single_fit(TH1* hist,
                         UInt_t &ndf,
                         Double_t &mu,
                         Double_t &mu_error,
                         Double_t &chi2,
                         Double_t &pvalue,
                         Double_t x_min,
                         Double_t x_max) {
    Double_t sigma;

    return FitUtils::single_fit(hist, ndf, mu, mu_error, chi2, pvalue, sigma, x_min, x_max);
}

/**
 * Fits the histogram to a gaussian over the specified range.
 * @param hist the histogram that needs to be fitted.
 * @param ndf the number of degrees of freedom of the fit.
 * @param mu the mean from the fit.
 * @param mu_error the error on the mean of the fit.
 * @param chi2 the chi-squared of the fit.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit. Default value 0.
 * @return 0 if success. 1 to indicate fitting range is empty. Otherwise error code from fit is returned according to
 * https://root.cern.ch/doc/master/classTMinuit.html#ab48dd4b48800edc090396e35cb465fb9
 */
Int_t FitUtils::single_fit(TH1* hist,
                        UInt_t &ndf,
                        Double_t &mu,
                        Double_t &mu_error,
                        Double_t &chi2,
                        Double_t x_min,
                        Double_t x_max) {
    Double_t pvalue;
    Double_t sigma;

    return FitUtils::single_fit(hist, ndf, mu, mu_error, chi2, pvalue, sigma, x_min, x_max);
}

/**
 * Fits the histogram to a gaussian over the specified range.
 * @param hist the histogram that needs to be fitted.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit. Default value 0.
 * @return 0 if success. 1 to indicate fitting range is empty. Otherwise error code from fit is returned according to
 * https://root.cern.ch/doc/master/classTMinuit.html#ab48dd4b48800edc090396e35cb465fb9
 */
Int_t FitUtils::single_fit(TH1* hist,
                         Double_t x_min,
                         Double_t x_max) {
    UInt_t ndf;
    Double_t mu;
    Double_t mu_error;
    Double_t chi2;
    Double_t pvalue;
    Double_t sigma;

    return FitUtils::single_fit(hist, ndf, mu, mu_error, chi2, pvalue, sigma, x_min, x_max);
}

/**
 * Double Fits the histogram to a gaussian over the specified range. First a single fit over the range x_min to x_max and then another fit over the range
 * max(mu-sigma, x_min) to min(mu+sigma, x_max) where mu and sigma are obtained from the first fit.
 * @param hist the histogram that needs to be fitted.
 * @param ndf the number of degrees of freedom of the fit.
 * @param mu the mean from the fit.
 * @param mu_error the error on the mean of the fit.
 * @param chi2 the chi-squared of the fit.
 * @param pvalue the chi-squared probability of the fit.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit Default value 0..
 * @return
 * Error code:
 * 0: success
 * 1: single fit is success but double fit failed so the fit parameters returned are that of the single fit
 * 2: single fit failed
 */
Int_t FitUtils::double_fit(TH1* hist,
                         UInt_t &ndf,
                         Double_t &mu,
                         Double_t &mu_error,
                         Double_t &chi2,
                         Double_t &pvalue,
                         Double_t x_min,
                         Double_t x_max) {
    Double_t sigma;
    Int_t fitStatus = FitUtils::single_fit(hist, ndf, mu, mu_error, chi2, pvalue, sigma, x_min, x_max);

    if(!fitStatus) {
        UInt_t   ndf_temp;
        Double_t mu_temp;
        Double_t mu_error_temp;
        Double_t chi2_temp;
        Double_t pvalue_temp;

        Double_t x2_min = min(max(mu-sigma, x_min), x_max);
        Double_t x2_max = max(min(mu+sigma, x_max), x_min);

        fitStatus = FitUtils::single_fit(hist, ndf_temp, mu_temp, mu_error_temp, chi2_temp, pvalue_temp, x2_min, x2_max);

        if(!fitStatus) {
            ndf = ndf_temp;
            mu = mu_temp;
            mu_error = mu_error_temp;
            chi2 = chi2_temp;
            pvalue = pvalue_temp;
            return 0;
        }
        return 1;
    }
    return 2;
}

/**
 * Double Fits the histogram to a gaussian over the specified range. First a single fit over the range x_min to x_max and then another fit over the range
 * max(mu-sigma, x_min) to min(mu+sigma, x_max) where mu and sigma are obtained from the first fit.
 * @param hist the histogram that needs to be fitted.
 * @param ndf the number of degrees of freedom of the fit.
 * @param mu the mean from the fit.
 * @param mu_error the error on the mean of the fit.
 * @param chi2 the chi-squared of the fit.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit Default value 0..
 * @return
 * Error code:
 * 0: success
 * 1: single fit is success but double fit failed so the fit parameters returned are that of the single fit
 * 2: single fit failed
 */
Int_t FitUtils::double_fit(TH1* hist,
                         UInt_t &ndf,
                         Double_t &mu,
                         Double_t &mu_error,
                         Double_t &chi2,
                         Double_t x_min,
                         Double_t x_max) {
    Double_t pvalue;

    return FitUtils::double_fit(hist, ndf, mu, mu_error, chi2, pvalue, x_min, x_max);
}

/**
 * Double Fits the histogram to a gaussian over the specified range. First a single fit over the range x_min to x_max and then another fit over the range
 * max(mu-sigma, x_min) to min(mu+sigma, x_max) where mu and sigma are obtained from the first fit.
 * @param hist the histogram that needs to be fitted.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit Default value 0..
 * @return
 * Error code:
 * 0: success
 * 1: single fit is success but double fit failed so the fit parameters returned are that of the single fit
 * 2: single fit failed
 */
Int_t FitUtils::double_fit(TH1* hist,
                         Double_t x_min,
                         Double_t x_max) {
    UInt_t ndf;
    Double_t mu;
    Double_t mu_error;
    Double_t chi2;
    Double_t pvalue;

    return FitUtils::double_fit(hist, ndf, mu, mu_error, chi2, pvalue, x_min, x_max);
}

/**
 * Double Fits the histogram to a gaussian over the specified range. First a single fit over the range x_min to x_max and then another fit over the range
 * max(mu-sigma, x_min) to min(mu+sigma, x_max) where mu and sigma are obtained from the first fit.
 * @param hist the histogram that needs to be fitted.
 * @param ndf the number of degrees of freedom of the fit.
 * @param mu the mean from the fit.
 * @param mu_error the error on the mean of the fit.
 * @param chi2 the chi-squared of the fit.
 * @param pvalue the chi-squared probability of the fit.
 * @param x_min the lower edge from where to fit. Default value 0.
 * @param x_max the upper edge from where to fit Default value 0..
 * @return
 * Error code:
 * 0: success
 * 1: single fit is success but double fit failed so the fit parameters returned are that of the single fit
 * 2: single fit failed
 */
Int_t FitUtils::double_fitv2(TH1* hist,
                             UInt_t &ndf,
                             Double_t &mu,
                             Double_t &mu_error,
                             Double_t &chi2,
                             Double_t &pvalue,
                             Double_t x_min,
                             Double_t x_max) {
    Double_t sigma;
    Int_t fitStatus = FitUtils::single_fit(hist, ndf, mu, mu_error, chi2, pvalue, sigma, x_min, x_max);

    if(!fitStatus) {
        UInt_t   ndf_temp;
        Double_t mu_temp;
        Double_t mu_error_temp;
        Double_t chi2_temp;
        Double_t pvalue_temp;

        Double_t x2_min = min(max(mu-sigma, x_min), x_max);
        Double_t x2_max = x_max;

        fitStatus = FitUtils::single_fit(hist, ndf_temp, mu_temp, mu_error_temp, chi2_temp, pvalue_temp, x2_min, x2_max);

        if(!fitStatus) {
            ndf = ndf_temp;
            mu = mu_temp;
            mu_error = mu_error_temp;
            chi2 = chi2_temp;
            pvalue = pvalue_temp;
            return 0;
        }
        return 1;
    }
    return 2;
}
