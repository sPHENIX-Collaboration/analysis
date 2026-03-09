#ifdef __CLING__
#pragma cling optimize(0)
#endif
void Error_Viz()
{
//=========Macro generated from canvas: Error_Viz/Error_Viz
//=========  (Tue Sep 19 17:57:48 2023) by ROOT version 6.26/06
   TCanvas *Error_Viz = new TCanvas("Error_Viz", "Error_Viz",20,42,1248,598);
   gStyle->SetOptStat(0);
   Error_Viz->Range(0,0,1,1);
   Error_Viz->SetFillColor(0);
   Error_Viz->SetBorderMode(0);
   Error_Viz->SetBorderSize(2);
   Error_Viz->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Error_Viz_1
   TPad *Error_Viz_1 = new TPad("Error_Viz_1", "Error_Viz_1",0.01,0.01,0.49,0.99);
   Error_Viz_1->Draw();
   Error_Viz_1->cd();
   Error_Viz_1->Range(-1.875,-1.875,1.875,1.875);
   Error_Viz_1->SetFillColor(0);
   Error_Viz_1->SetBorderMode(0);
   Error_Viz_1->SetBorderSize(2);
   Error_Viz_1->SetFrameBorderMode(0);
   Error_Viz_1->SetFrameBorderMode(0);
   
   TH2D *dummy1__1 = new TH2D("dummy1__1","26540-Noise Std. Dev. in ADC (North Side)",100,-1.5,1.5,100,-1.5,1.5);
   dummy1__1->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   dummy1__1->SetLineColor(ci);
   dummy1__1->GetXaxis()->SetLabelFont(42);
   dummy1__1->GetXaxis()->SetTitleOffset(1);
   dummy1__1->GetXaxis()->SetTitleFont(42);
   dummy1__1->GetYaxis()->SetLabelFont(42);
   dummy1__1->GetYaxis()->SetTitleFont(42);
   dummy1__1->GetZaxis()->SetLabelFont(42);
   dummy1__1->GetZaxis()->SetTitleOffset(1);
   dummy1__1->GetZaxis()->SetTitleFont(42);
   dummy1__1->Draw("");
   Double_t yAxis1[5] = {0, 0.256, 0.504, 0.752, 1}; 
   
   TH2D *CSide__2 = new TH2D("CSide__2","ADC Counts South Side",12,-0.2617994,6.021386,4, yAxis1);
   CSide__2->SetBinContent(29,1.989332);
   CSide__2->SetBinContent(30,3.196764);
   CSide__2->SetBinContent(31,2.028693);
   CSide__2->SetBinContent(32,2.287374);
   CSide__2->SetBinContent(33,1.905044);
   CSide__2->SetBinContent(34,2.01782);
   CSide__2->SetBinContent(35,2.011409);
   CSide__2->SetBinContent(36,2.057789);
   CSide__2->SetBinContent(37,2.000231);
   CSide__2->SetBinContent(38,2.047215);
   CSide__2->SetBinContent(39,2.051875);
   CSide__2->SetBinContent(40,1.924342);
   CSide__2->SetBinContent(43,2.201327);
   CSide__2->SetBinContent(44,2.095087);
   CSide__2->SetBinContent(45,3.010436);
   CSide__2->SetBinContent(46,1.962912);
   CSide__2->SetBinContent(47,2.100744);
   CSide__2->SetBinContent(48,2.212984);
   CSide__2->SetBinContent(49,2.070949);
   CSide__2->SetBinContent(50,1.985125);
   CSide__2->SetBinContent(51,1.963863);
   CSide__2->SetBinContent(52,2.005603);
   CSide__2->SetBinContent(53,1.932216);
   CSide__2->SetBinContent(54,2.431431);
   CSide__2->SetBinContent(57,2.381369);
   CSide__2->SetBinContent(58,2.195927);
   CSide__2->SetBinContent(59,2.306524);
   CSide__2->SetBinContent(60,2.302729);
   CSide__2->SetBinContent(61,1.940289);
   CSide__2->SetBinContent(62,2.093168);
   CSide__2->SetBinContent(63,2.130398);
   CSide__2->SetBinContent(64,2.060633);
   CSide__2->SetBinContent(65,2.134717);
   CSide__2->SetBinContent(66,2.255702);
   CSide__2->SetBinContent(67,1.992586);
   CSide__2->SetBinContent(68,3.100811);
   CSide__2->SetBinError(29,1.989332);
   CSide__2->SetBinError(30,3.196764);
   CSide__2->SetBinError(31,2.028693);
   CSide__2->SetBinError(32,2.287374);
   CSide__2->SetBinError(33,1.905044);
   CSide__2->SetBinError(34,2.01782);
   CSide__2->SetBinError(35,2.011409);
   CSide__2->SetBinError(36,2.057789);
   CSide__2->SetBinError(37,2.000231);
   CSide__2->SetBinError(38,2.047215);
   CSide__2->SetBinError(39,2.051875);
   CSide__2->SetBinError(40,1.924342);
   CSide__2->SetBinError(43,2.201327);
   CSide__2->SetBinError(44,2.095087);
   CSide__2->SetBinError(45,3.010436);
   CSide__2->SetBinError(46,1.962912);
   CSide__2->SetBinError(47,2.100744);
   CSide__2->SetBinError(48,2.212984);
   CSide__2->SetBinError(49,2.070949);
   CSide__2->SetBinError(50,1.985125);
   CSide__2->SetBinError(51,1.963863);
   CSide__2->SetBinError(52,2.005603);
   CSide__2->SetBinError(53,1.932216);
   CSide__2->SetBinError(54,2.431431);
   CSide__2->SetBinError(57,2.381369);
   CSide__2->SetBinError(58,2.195927);
   CSide__2->SetBinError(59,2.306524);
   CSide__2->SetBinError(60,2.302729);
   CSide__2->SetBinError(61,1.940289);
   CSide__2->SetBinError(62,2.093168);
   CSide__2->SetBinError(63,2.130398);
   CSide__2->SetBinError(64,2.060633);
   CSide__2->SetBinError(65,2.134717);
   CSide__2->SetBinError(66,2.255702);
   CSide__2->SetBinError(67,1.992586);
   CSide__2->SetBinError(68,3.100811);
   CSide__2->SetMinimum(0);
   CSide__2->SetMaximum(4);
   CSide__2->SetEntries(36);
   CSide__2->SetStats(0);
   CSide__2->SetContour(20);
   CSide__2->SetContourLevel(0,0);
   CSide__2->SetContourLevel(1,0.2);
   CSide__2->SetContourLevel(2,0.4);
   CSide__2->SetContourLevel(3,0.6);
   CSide__2->SetContourLevel(4,0.8);
   CSide__2->SetContourLevel(5,1);
   CSide__2->SetContourLevel(6,1.2);
   CSide__2->SetContourLevel(7,1.4);
   CSide__2->SetContourLevel(8,1.6);
   CSide__2->SetContourLevel(9,1.8);
   CSide__2->SetContourLevel(10,2);
   CSide__2->SetContourLevel(11,2.2);
   CSide__2->SetContourLevel(12,2.4);
   CSide__2->SetContourLevel(13,2.6);
   CSide__2->SetContourLevel(14,2.8);
   CSide__2->SetContourLevel(15,3);
   CSide__2->SetContourLevel(16,3.2);
   CSide__2->SetContourLevel(17,3.4);
   CSide__2->SetContourLevel(18,3.6);
   CSide__2->SetContourLevel(19,3.8);
   
   TPaletteAxis *palette = new TPaletteAxis(1.51875,-1.5,1.6875,1.5,CSide__2);
   palette->SetLabelColor(1);
   palette->SetLabelFont(42);
   palette->SetLabelOffset(0.005);
   palette->SetLabelSize(0.035);
   palette->SetTitleOffset(1);
   palette->SetTitleSize(0.035);

   ci = TColor::GetColor("#f9f90e");
   palette->SetFillColor(ci);
   palette->SetFillStyle(1001);
   CSide__2->GetListOfFunctions()->Add(palette,"br");

   ci = TColor::GetColor("#000099");
   CSide__2->SetLineColor(ci);
   CSide__2->GetXaxis()->SetLabelFont(42);
   CSide__2->GetXaxis()->SetTitleOffset(1);
   CSide__2->GetXaxis()->SetTitleFont(42);
   CSide__2->GetYaxis()->SetLabelFont(42);
   CSide__2->GetYaxis()->SetTitleFont(42);
   CSide__2->GetZaxis()->SetLabelFont(42);
   CSide__2->GetZaxis()->SetTitleOffset(1);
   CSide__2->GetZaxis()->SetTitleFont(42);
   CSide__2->Draw("colpolzsame0");
   
   TPaveLabel *pl = new TPaveLabel(1.046586,-0.1938999,1.407997,0.2144871,"00","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.962076,0.4382608,1.323487,0.8466479,"01","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.4801947,0.8802139,0.8416056,1.288601,"02","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.1823921,1.011681,0.1790189,1.425662,"03","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.8449788,0.8690253,-0.4835679,1.288601,"04","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-1.30879,0.441058,-0.9473786,0.8550394,"05","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-1.411009,-0.2050886,-1.049598,0.2144871,"06","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-1.302585,-0.7757116,-0.9471979,-0.3561359,"07","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.8449788,-1.309971,-0.4835679,-0.8848013,"08","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.1823921,-1.426557,0.1790189,-1.006982,"09","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.4801947,-1.309076,0.8416056,-0.8839062,"10","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.9622567,-0.7785088,1.323668,-0.3533387,"11","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   TPaveText *pt = new TPaveText(0.15,0.934955,0.85,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   TText *pt_LaTex = pt->AddText("26540-Noise Std. Dev. in ADC (North Side)");
   pt->Draw();
   Error_Viz_1->Modified();
   Error_Viz->cd();
  
// ------------>Primitives in pad: Error_Viz_2
   TPad *Error_Viz_2 = new TPad("Error_Viz_2", "Error_Viz_2",0.51,0.01,0.99,0.99);
   Error_Viz_2->Draw();
   Error_Viz_2->cd();
   Error_Viz_2->Range(-1.875,-1.875,1.875,1.875);
   Error_Viz_2->SetFillColor(0);
   Error_Viz_2->SetBorderMode(0);
   Error_Viz_2->SetBorderSize(2);
   Error_Viz_2->SetFrameBorderMode(0);
   Error_Viz_2->SetFrameBorderMode(0);
   
   TH2D *dummy2__3 = new TH2D("dummy2__3","26540-Noise Std. Dev. in ADC (South Side)",100,-1.5,1.5,100,-1.5,1.5);
   dummy2__3->SetStats(0);

   ci = TColor::GetColor("#000099");
   dummy2__3->SetLineColor(ci);
   dummy2__3->GetXaxis()->SetLabelFont(42);
   dummy2__3->GetXaxis()->SetTitleOffset(1);
   dummy2__3->GetXaxis()->SetTitleFont(42);
   dummy2__3->GetYaxis()->SetLabelFont(42);
   dummy2__3->GetYaxis()->SetTitleFont(42);
   dummy2__3->GetZaxis()->SetLabelFont(42);
   dummy2__3->GetZaxis()->SetTitleOffset(1);
   dummy2__3->GetZaxis()->SetTitleFont(42);
   dummy2__3->Draw("");
   Double_t yAxis2[5] = {0, 0.256, 0.504, 0.752, 1}; 
   
   TH2D *ASide__4 = new TH2D("ASide__4","ADC Counts North Side",12,-0.2617994,6.021386,4, yAxis2);
   ASide__4->SetBinContent(29,1.904025);
   ASide__4->SetBinContent(30,1.900522);
   ASide__4->SetBinContent(31,2.552518);
   ASide__4->SetBinContent(32,2.130305);
   ASide__4->SetBinContent(33,1.938709);
   ASide__4->SetBinContent(34,2.136297);
   ASide__4->SetBinContent(35,1.997312);
   ASide__4->SetBinContent(36,2.025557);
   ASide__4->SetBinContent(37,1.924307);
   ASide__4->SetBinContent(38,2.142733);
   ASide__4->SetBinContent(39,1.985398);
   ASide__4->SetBinContent(40,2.036818);
   ASide__4->SetBinContent(43,2.07103);
   ASide__4->SetBinContent(44,2.148584);
   ASide__4->SetBinContent(45,2.679135);
   ASide__4->SetBinContent(46,2.137);
   ASide__4->SetBinContent(47,2.145508);
   ASide__4->SetBinContent(48,2.092765);
   ASide__4->SetBinContent(49,2.151562);
   ASide__4->SetBinContent(50,2.269632);
   ASide__4->SetBinContent(51,2.123742);
   ASide__4->SetBinContent(52,3.130458);
   ASide__4->SetBinContent(53,2.078224);
   ASide__4->SetBinContent(54,2.06422);
   ASide__4->SetBinContent(57,2.19219);
   ASide__4->SetBinContent(58,2.396421);
   ASide__4->SetBinContent(59,2.145928);
   ASide__4->SetBinContent(60,2.321178);
   ASide__4->SetBinContent(61,2.123741);
   ASide__4->SetBinContent(62,2.470285);
   ASide__4->SetBinContent(63,2.185126);
   ASide__4->SetBinContent(64,2.506654);
   ASide__4->SetBinContent(65,2.313562);
   ASide__4->SetBinContent(66,2.833683);
   ASide__4->SetBinContent(67,2.290022);
   ASide__4->SetBinContent(68,2.186331);
   ASide__4->SetBinError(29,1.904025);
   ASide__4->SetBinError(30,1.900522);
   ASide__4->SetBinError(31,2.552518);
   ASide__4->SetBinError(32,2.130305);
   ASide__4->SetBinError(33,1.938709);
   ASide__4->SetBinError(34,2.136297);
   ASide__4->SetBinError(35,1.997312);
   ASide__4->SetBinError(36,2.025557);
   ASide__4->SetBinError(37,1.924307);
   ASide__4->SetBinError(38,2.142733);
   ASide__4->SetBinError(39,1.985398);
   ASide__4->SetBinError(40,2.036818);
   ASide__4->SetBinError(43,2.07103);
   ASide__4->SetBinError(44,2.148584);
   ASide__4->SetBinError(45,2.679135);
   ASide__4->SetBinError(46,2.137);
   ASide__4->SetBinError(47,2.145508);
   ASide__4->SetBinError(48,2.092765);
   ASide__4->SetBinError(49,2.151562);
   ASide__4->SetBinError(50,2.269632);
   ASide__4->SetBinError(51,2.123742);
   ASide__4->SetBinError(52,3.130458);
   ASide__4->SetBinError(53,2.078224);
   ASide__4->SetBinError(54,2.06422);
   ASide__4->SetBinError(57,2.19219);
   ASide__4->SetBinError(58,2.396421);
   ASide__4->SetBinError(59,2.145928);
   ASide__4->SetBinError(60,2.321178);
   ASide__4->SetBinError(61,2.123741);
   ASide__4->SetBinError(62,2.470285);
   ASide__4->SetBinError(63,2.185126);
   ASide__4->SetBinError(64,2.506654);
   ASide__4->SetBinError(65,2.313562);
   ASide__4->SetBinError(66,2.833683);
   ASide__4->SetBinError(67,2.290022);
   ASide__4->SetBinError(68,2.186331);
   ASide__4->SetMinimum(0);
   ASide__4->SetMaximum(4);
   ASide__4->SetEntries(36);
   ASide__4->SetStats(0);
   ASide__4->SetContour(20);
   ASide__4->SetContourLevel(0,0);
   ASide__4->SetContourLevel(1,0.2);
   ASide__4->SetContourLevel(2,0.4);
   ASide__4->SetContourLevel(3,0.6);
   ASide__4->SetContourLevel(4,0.8);
   ASide__4->SetContourLevel(5,1);
   ASide__4->SetContourLevel(6,1.2);
   ASide__4->SetContourLevel(7,1.4);
   ASide__4->SetContourLevel(8,1.6);
   ASide__4->SetContourLevel(9,1.8);
   ASide__4->SetContourLevel(10,2);
   ASide__4->SetContourLevel(11,2.2);
   ASide__4->SetContourLevel(12,2.4);
   ASide__4->SetContourLevel(13,2.6);
   ASide__4->SetContourLevel(14,2.8);
   ASide__4->SetContourLevel(15,3);
   ASide__4->SetContourLevel(16,3.2);
   ASide__4->SetContourLevel(17,3.4);
   ASide__4->SetContourLevel(18,3.6);
   ASide__4->SetContourLevel(19,3.8);
   
   palette = new TPaletteAxis(1.51875,-1.5,1.6875,1.5,ASide__4);
   palette->SetLabelColor(1);
   palette->SetLabelFont(42);
   palette->SetLabelOffset(0.005);
   palette->SetLabelSize(0.035);
   palette->SetTitleOffset(1);
   palette->SetTitleSize(0.035);

   ci = TColor::GetColor("#f9f90e");
   palette->SetFillColor(ci);
   palette->SetFillStyle(1001);
   ASide__4->GetListOfFunctions()->Add(palette,"br");

   ci = TColor::GetColor("#000099");
   ASide__4->SetLineColor(ci);
   ASide__4->GetXaxis()->SetLabelFont(42);
   ASide__4->GetXaxis()->SetTitleOffset(1);
   ASide__4->GetXaxis()->SetTitleFont(42);
   ASide__4->GetYaxis()->SetLabelFont(42);
   ASide__4->GetYaxis()->SetTitleFont(42);
   ASide__4->GetZaxis()->SetLabelFont(42);
   ASide__4->GetZaxis()->SetTitleOffset(1);
   ASide__4->GetZaxis()->SetTitleFont(42);
   ASide__4->Draw("colpolzsame0");
   
   pl = new TPaveLabel(1.046586,-0.1938999,1.407997,0.2144871,"18","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.962076,0.4382608,1.323487,0.8466479,"17","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.4801947,0.8802139,0.8416056,1.288601,"16","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.1823921,1.011681,0.1790189,1.425662,"15","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.8449788,0.8690253,-0.4835679,1.288601,"14","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-1.30879,0.441058,-0.9473786,0.8550394,"13","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-1.411009,-0.2050886,-1.049598,0.2144871,"12","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-1.302585,-0.7757116,-0.9471979,-0.3561359,"23","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.8449788,-1.309971,-0.4835679,-0.8848013,"22","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(-0.1823921,-1.426557,0.1790189,-1.006982,"21","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.4801947,-1.309076,0.8416056,-0.8839062,"20","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pl = new TPaveLabel(0.9622567,-0.7785088,1.323668,-0.3533387,"19","br");
   pl->SetFillColor(0);
   pl->SetTextSize(0.99);
   pl->Draw();
   
   pt = new TPaveText(0.15,0.934955,0.85,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   pt_LaTex = pt->AddText("26540-Noise Std. Dev. in ADC (South Side)");
   pt->Draw();
   Error_Viz_2->Modified();
   Error_Viz->cd();
   Error_Viz->Modified();
   Error_Viz->cd();
   Error_Viz->SetSelected(Error_Viz);
}
