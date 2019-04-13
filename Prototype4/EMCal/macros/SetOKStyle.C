
#ifndef SetOKStyle_C
#define SetOKStyle_C


void
SetOKStyle()
{
  TStyle* OKStyle = new TStyle("OKStyle", "OK Default Style");

  // Colors

  //set the background color to white
  OKStyle->SetFillColor(10);
  OKStyle->SetFrameFillColor(kWhite);
  OKStyle->SetFrameFillStyle(0);
  OKStyle->SetFillStyle(0);
  OKStyle->SetCanvasColor(kWhite);
  OKStyle->SetPadColor(kWhite);
  OKStyle->SetTitleFillColor(0);
  OKStyle->SetStatColor(kWhite);

  // Get rid of drop shadow on legends
  // This doesn't seem to work.  Call SetBorderSize(1) directly on your TLegends
  OKStyle->SetLegendBorderSize(1);

  //don't put a colored frame around the plots
  OKStyle->SetFrameBorderMode(0);
  OKStyle->SetCanvasBorderMode(0);
  OKStyle->SetPadBorderMode(0);

  //use the primary color palette
  OKStyle->SetPalette(1, 0);

  //set the default line color for a histogram to be black
  OKStyle->SetHistLineColor(kBlack);

  //set the default line color for a fit function to be red
  OKStyle->SetFuncColor(kBlue);

  //make the axis labels black
  OKStyle->SetLabelColor(kBlack, "xyz");

  //set the default title color to be black
  OKStyle->SetTitleColor(kBlack);

  //set the margins
  OKStyle->SetPadBottomMargin(0.15);
  OKStyle->SetPadLeftMargin(0.1);
  OKStyle->SetPadTopMargin(0.075);
  OKStyle->SetPadRightMargin(0.1);

  //set axis label and title text sizes
  OKStyle->SetLabelSize(0.035, "xyz");
  OKStyle->SetTitleSize(0.05, "xyz");
  OKStyle->SetTitleOffset(0.9, "xyz");
  OKStyle->SetStatFontSize(0.035);
  OKStyle->SetTextSize(0.05);
  OKStyle->SetTitleBorderSize(0);
  OKStyle->SetTitleStyle(0);

  OKStyle->SetLegendBorderSize(0);

  //set line widths
  OKStyle->SetHistLineWidth(1);
  OKStyle->SetFrameLineWidth(2);
  OKStyle->SetFuncWidth(2);

  // Misc

  //align the titles to be centered
  //OKStyle->SetTextAlign(22);

  //turn off xy grids
  OKStyle->SetPadGridX(1);
  OKStyle->SetPadGridY(1);

  //set the tick mark style
  OKStyle->SetPadTickX(1);
  OKStyle->SetPadTickY(1);

  //don't show the fit parameters in a box
  OKStyle->SetOptFit(0);

  //set the default stats shown
  OKStyle->SetOptStat(1);

  //marker settings
// 	OKStyle->SetMarkerStyle(8);
// 	OKStyle->SetMarkerSize(0.7);

  // Fonts
  OKStyle->SetStatFont(42);
  OKStyle->SetLabelFont(42, "xyz");
  OKStyle->SetTitleFont(42, "xyz");
  OKStyle->SetTextFont(42);

  // Set the paper size for output
  OKStyle->SetPaperSize(TStyle::kUSLetter);


//  // color palette - manually define 'kBird' palette only available in ROOT 6
//  Int_t alpha = 0;
//  Double_t stops[9] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000};
//  Double_t red[9]   = { 0.2082, 0.0592, 0.0780, 0.0232, 0.1802, 0.5301, 0.8186, 0.9956, 0.9764};
//  Double_t green[9] = { 0.1664, 0.3599, 0.5041, 0.6419, 0.7178, 0.7492, 0.7328, 0.7862, 0.9832};
//  Double_t blue[9]  = { 0.5293, 0.8684, 0.8385, 0.7914, 0.6425, 0.4662, 0.3499, 0.1968, 0.0539};
//  TColor::CreateGradientColorTable(9, stops, red, green, blue, 255, alpha);

  //done
  OKStyle->cd();

  cout << "Using Jin's Style" << endl;
}

#endif
