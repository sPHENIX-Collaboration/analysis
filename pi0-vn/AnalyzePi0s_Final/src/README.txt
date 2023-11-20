FillHists.cpp:
Currently, all you need to do is change the array of cuts you want to analyze. 

It outputs an individual root file per cut combination, with a very specific and automated naming scheme that is set up currently to be read into the analysis macro and gets decomposed for auto tracking of what cuts were used for that specific root file. 

So if using this macro, the only thing that should be editted is the array of cuts you want to look at, and the path of output where you want the root files to be uploaded, mine currently goes to a folder called histRootFiles on line 128:
"/Users/patsfan753/Desktop/AnalyzePi0s_Final/histRootFiles..."


INSTRUCTIONS ON USING AnalyzePi0.cpp:
The way I use this macro is as follows-
1) Load in the root file of cuts you want to analyze in the variable 'globalFileName' at the beginning of the macro.
--It is set up such that as you change the root file name generated from FillHists, the name of the root file gets decomposed into what cuts are used.
-----The macro then knows automatically what cuts to print out to the invariant mass histograms, the 4 additional plots of analysis (signal yield, yield error, gauss mean, and gauss sigma) as well as what cuts to print out to the global tracking of histograms in the CSV file and the global tracking of fit tuning parameters in the additional CSV file 

2)After globalFileName, there are 3 booleans that are set up to be used:
CreateSignalandGaussParPlots:
---This is for after analyzing 12 histograms from a root file for a set of cuts, you set this boolean to true and re run the code, which then outputs all 4 plots specified above from text files with information needed to output this. No other manual changes need to be made at this point since text files automatically are named after the set of cuts.

globalSetFitManual:
Looking in the function PerformFitting(), you will see:
// Set fitting parameters
    if (setFitManual) {
        totalFit->SetParameter(0, 45000);
        totalFit->SetParameter(1, 0.147);
        totalFit->SetParameter(2, 0.007);
        totalFit->SetParLimits(1, 0.13, 0.15);
        totalFit->SetParLimits(2, 0.01, 0.03);
    } else {
        int bin1 = hPi0Mass->GetXaxis()->FindBin(globalFindBin1Value);
        int bin2 = hPi0Mass->GetXaxis()->FindBin(globalFindBin2Value);
        int maxBin = bin1;
        double maxBinContent = hPi0Mass->GetBinContent(bin1);
        for (int i = bin1 + 1; i <= bin2; ++i) {
            if (hPi0Mass->GetBinContent(i) > maxBinContent) {
                maxBinContent = hPi0Mass->GetBinContent(i);
                maxBin = i;
            }
        }
        double maxBinCenter = hPi0Mass->GetXaxis()->GetBinCenter(maxBin);
        totalFit->SetParameter(0, maxBinContent);
        totalFit->SetParameter(1, maxBinCenter);
        double sigmaEstimate = globalSigmaEstimate;
        totalFit->SetParameter(2, sigmaEstimate);
        totalFit->SetParLimits(1, maxBinCenter - sigmaEstimate, maxBinCenter + sigmaEstimate);
        totalFit->SetParLimits(2, sigmaEstimate - globalSigmaParScale*sigmaEstimate, sigmaEstimate + globalSigmaParScale*sigmaEstimate);
    }


in which if setFitManual is true, you can set the parameters manually for whatever fit you are on. I avoid using this tho so I can refine the dynamic fitting procedure.


bool globalSetDynamicParsAuto:
This is set up such that when true, it will load in the CSV file that outputted with the dynamic fit parameters that remained tunable, such as sigma and fitEnd, and set up your current histogram index being analyzed to the values used in the previous root file.

---This works very well with incremental changes of cuts, these values essentially remain consistent, so it quickens the fitting procedure a lot. Especially since when cuts do change some of these parameters, you can set it easily to the parameters from before once, and then those changes propagate as you incremtnally vary the cuts further.

------So if is not working for the fit, you can set this to false, and manually set the values in the PeformFitting function (i always start with the values from the dynamic setting which print to the terminal as a starting point):
        fitEnd = 0.4;
        globalFitEnd = fitEnd;
        globalFindBin1Value = 0.1; // Value in FindBin for bin1
        globalFindBin2Value = 0.11; // Value in FindBin for bin2
        globalSigmaEstimate = 0.008; // sigmaEstimate value
        // Check if SetParLimits is used for sigma
        if (!setFitManual) {
            globalSigmaParScale = .5; // Scale factor used in SetParLimits
        } else {
            globalSigmaParScale = 0.0; // No SetParLimits used
        }


NOT global sigmaParScale sets the bounds of sigma dynamically with this line of code:
        totalFit->SetParLimits(2, sigmaEstimate - globalSigmaParScale*sigmaEstimate, sigmaEstimate + globalSigmaParScale*sigmaEstimate);

and if you want to comment out that line of code, it uploads the value as a 0 to the CSV file



ON a hist by hist basis, just change:
int histIndex = 9;

that is after these boolean statements to whatever histogram you want to look at.
Really the only other thing that manually is inputted is y axis range and height of vertical lines printed, but this is pretty easy to guess based on image from previous output and adding or subtracting some value, which gets quick to guess after doing this a bunch.





WHEN RUNNING THE CODE, user input of 'Y' means you are happy with the fit, and ready to upload the parameters to the csv files for global tracking, and for the text files for tracking on a root file by root file basis (which are used for outputting the summary plots at the end)


Other then this, there is not much manual settings of any components in the codes, the text files are named dynamically so the summary plots can output for root file by root file basis without getting confused of what cuts were used etc.
