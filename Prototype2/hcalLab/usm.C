
#include <usmData.h>

  // Generic tile plots
TH2 * tcs = new TH2F("tcs","Clear side", 11, 0.5, 11.5, 9, 2., 20.);  
TH2 * tfs = new TH2F("tfs","Fiber side", 11, 0.5, 11.5, 9, 2., 20.);
TH2 * fcr = new TH2F("fcr","Ratio Fiber-to-Clear", 11, 0.5, 11.5, 9, 2., 20.);
TH1 * AtoB = new TH1F("AtoB","Reproducibility of measurements", 20, -0.5, 0.5);
TH1 * AtoB1cs = new TH1F("AtoB1cs","Ratio of Before-After mesurements in the last(away from SiPM) row (Clear)", 20, 0.5, 1.5);
TH1 * AtoB2cs = new TH1F("AtoB2cs","Ratio of Before-After mesurements in the first(close to SiPM) row (Clear)", 20, 0.5, 1.5);
TH1 * AtoBtcs = new TH1F("AtoBtcs","Ratio of Before-After mesurements in the first&last rows (Clear)", 20, 0.5, 1.5);
TH1 * AtoB1fs = new TH1F("AtoB1fs","Ratio of Before-After mesurements in the last(away from SiPM) row) (Fiber)", 20, 0.5, 1.5);
TH1 * AtoB2fs = new TH1F("AtoB2fs","Ratio of Before-After mesurements in the first(close to SiPM) row (Fiber)", 20, 0.5, 1.5);
TH1 * AtoBtfs = new TH1F("AtoBtfs","Ratio of Before-After mesurements in the first&last rows (Fiber)", 20, 0.5, 1.5);


TH2 * tcsn = new TH2F("tcsn","Normalized A: LED on Clear side", 11, 0.5, 11.5, 9, 2., 20.);  
//tcsn -> SetMaximum(5.);
TH2 * tfsn = new TH2F("tfsn","Normalized A: LED on Fiber side", 11, 0.5, 11.5, 9, 2., 20.);
//tfsn -> SetMaximum(5.);
TH2 * fcrn = new TH2F("fcrn","Ratio Fiber-to-Clear (both normalized)", 11, 0.5, 11.5, 9, 2., 20.);
//fcrn -> SetMaximum(5.);
//  Amplitudes and ratios away and over the fiber
TH1 * cs_all = new TH1F("cs_all","Normalized A: LED on Clear side",  40, 0., 3.);
TH1 * cs_off = new TH1F("cs_off","Off-fiber A: clear(black) & fiber(red) sides", 40, 0., 3.);
TH1 * cs_onf = new TH1F("cs_onf","Norm. amp. on fiber, clear side",  40, 0., 3.);
TH1 * fs_off = new TH1F("fs_off","Norm. amp. off fiber, fiber side", 40, 0., 3.);
TH1 * fs_onf = new TH1F("fs_onf","Norm. amp. on fiber, fiber side",  40, 0., 3.);
TH1 * rfc_off = new TH1F("rfc_off","Norm. amp. Ratio: off fiber", 40, 0., 3.);
TH1 * rfc_onf = new TH1F("rfc_onf","Norm. amp. Ratio: on fiber",  40, 0., 3.);

 
TProfile * tavsdnfcs = new TProfile("tavsdnfcs","Amp vs Dist from exit - no fiber - clear side", 20,0.,20.,0.,2000.,"");
TProfile * tavsdfcs  = new TProfile("tavsdfcs", "Amp vs Dist from exit - fiber - clear side", 20,0.,20.,0.,2000.,"");
TProfile * tavsdnffs = new TProfile("tavsdnffs","Amp vs Dist from exit - no fiber - fiber side", 20,0.,20.,0.,2000.,"");
TProfile * tavsdffs  = new TProfile("tavsdffs", "Amp vs Dist from exit - fiber - fiber side", 20,0.,20.,0.,2000.,"");

  // DATA
  // Clear side: lines 1-11
  // line 12 - Y-positions
  // Fiber side : lines 13-23
  // col 0 - R(Y=3) =  meas(A)/mes(B)
  // col 1 - meas(B)
  // col 2 - X (1-11, step 1)
  // col 3 - meas(Y=3)
  // col 4 - meas(Y=5)
  // col .....
  // col 11- meas(Y=19) - A
  // col 12- meas(Y=19) - B
  // col 13- R(Y=19) =  meas(A)/mes(B)
  // col 14-16 Comments

Double_t fiber[11][9];
Int_t    tileId;
TString  token;  TString Item;
TString  Tokens[24][20];
Double_t value[24][20];
Double_t tData[2][11][9];    //   Measurements on both sides of the tile
Double_t cData[2][11][2];    //   Control data (repeated measurements) in the first and last Y-rows
Double_t xValue[2][11], yValue[9];
Double_t csNorm(0.), fsNorm(0.);  // normalization coefficients for the clear and fiber sides
Int_t    X, Y, loc, line, pos;

// length along the fiber  (from exit along decreasing Y then to small X then to large Y and then to the exit)
Double_t tPath(Int_t x, Int_t y){
  // computes travel path to exit point (9,9)
  Double_t dx = abs(x-exitX); Double_t dy = abs(y-exitY);
  return sqrt(dx*dx+dy*dy);
}

void init(){
  for(Y = 0; Y<9;  Y++) {for (X = 0; X<11; X++) fiber[X][Y] = 0.;}
  for(Y = 0; Y<20; Y++) {for (X = 0; X<24; X++) value[X][Y] = 0.;}
  for(int side =0; side<2; side++) {
    for(Y = 0; Y<9;  Y++) {for (X = 0; X<11; X++) tData[side][X][Y] = 0.;}
    for(Y = 0; Y<2;  Y++) {for (X = 0; X<11; X++) cData[side][X][Y] = 0.;}
    for(X = 0; X<11; X++) xValue[side][X] = 0.;
  }
  for (Y = 0; Y<9;  Y++) yValue[Y]    = 0.;
}



void tile(TString tDcsv){
  init();
  //  Data are structured into lines with carriage returns ending those lines
  //  Each line is 20 positions full with few last missing in some (comments)
  Int_t   from = 0; Int_t nTok = 0; Int_t nCom = 0; Int_t lCom = 0; Int_t lItem = 0;
  while ((lCom=tDcsv.Index(",",lItem))>=0||(nCom==0)) {
    Int_t cr = tDcsv.Index("\n",lItem);
    if(cr>=0&&cr<lCom) lCom = cr;
    Item = tDcsv(lItem, lCom-lItem);
    //  if item length is nonzero - check for carriage return (EOL)
    //    Int_t cr = Item.First("\n");
    //    if(cr!=TString::kNPOS) Item.Remove(cr);
    line = nCom/20;
    pos  = nCom%20;
    Tokens[line][pos] = Item;
    // if Item is numeric - convert it to Double_t
    if(Tokens[line][pos].IsFloat()) value[line][pos] = Tokens[line][pos].Atof();
    //    std::cout<<"Item  "<<nCom<<"  lItem="<<lItem<<"  lCom="<<lCom<<"  line="<<line<<"  pos="<<pos<<"   Item="<<Item<<"  Value="<<value[line][pos]<<std::endl;
    if(cr==lCom)   {
      //      cout<<"CR found nCom = "<<nCom<<endl;
      nCom = ((nCom/20)+1)*20; 
    } else     nCom++;
    lItem = lCom+1;
  }
  tileId =  Tokens[0][0].Atoi();
  cout<<"tileID = "<<tileId<<endl;
  if(tileId>1) for(loc=0; loc<99; loc++) dam[loc] = 0;
  // Rearranging tile data
  int nEntCs(0), nEntFs(0);
  for(Y = 0; Y < 9; Y++) {
    pos = Y + 3; 
    yValue[Y] = value[12][pos];
    for (X = 0; X < 11; X++) {
      //  clear side
      line = 11-X;
      loc  = Y*11 + X;
      tData[0][X][Y] = value[line][pos];
      if(Y==0) 	{
	xValue[0][X] = value[line][2];
	// repeated (control)  measurements 
	cData[0][X][0] = value[line][1];
	cData[0][X][1] = value[line][12];
	// normalization for clear side
	if(!fib[loc]&&tData[0][X][0]>0.) {nEntCs++; csNorm += tData[0][X][0];}
      }
      //  fiber side
      line = 12+X+1;
      loc  = Y*11 + X;
      tData[1][X][Y] = value[line][pos];
      if(Y==0)  {
	xValue[1][X]   = value[line][2];
	cData[1][X][0] = value[line][1];
	cData[1][X][1] = value[line][12];
	if(!fib[loc]&&tData[1][X][0]>0.) {nEntFs++; fsNorm += tData[1][X][0];}
      }
    }
  }
  cout<<"Norm cs "<<nEntCs<<"/"<<csNorm<<"  Norm fs "<<nEntFs<<"/"<<fsNorm<<endl;
  if(nEntCs) csNorm /= Double_t(nEntCs);
  csNorm *= tScale[tileId-1];
  if(nEntFs) fsNorm /= Double_t(nEntFs);
  fsNorm *= tScale[tileId-1];
  cout<<"Norm cs "<<nEntCs<<"/"<<csNorm<<"  Norm fs "<<nEntFs<<"/"<<fsNorm<<endl;

  // Presentation
  Double_t xExit = value[3][2];        //  91mm
  Double_t yExit = value[12][11];      // 196mm
  Double_t dx, dy, d;
  //  cout<<"xExit = "<<xExit<<"  yExit = "<<yExit<<endl;
  for(Y = 0; Y < 9; Y++) {
    // distance to exit
    pos = Y + 3; 
    // clear side
    for (X = 0; X < 11; X++) {
      line = 11-X;
      loc  = Y*11 + X;
      dx   = xValue[0][X] - xExit;  dy = yValue[Y]- yExit;
      Double_t d  = sqrt(dx*dx+dy*dy);
      //      cout<<X<<"  "<<Y<<"  "<<xValue[0][X]<<"  "<< yValue[Y]<<" tData "<< tData[0][X][Y]<<endl;
      //      std::cout<<"X/Y/loc/dam/fib/line/pos/V/tData  "<<X<<"/"<<xValue[0][X]<<"  "<<Y<<"/"<<yValue[Y]<<"  "<<loc<<" D/F "<<dam[loc]<<"/"<<fib[loc]<<"  "<<line<<"  "<<pos<<"  "<<value[line][pos]<<" tD  "<<tData[0][X][Y]<<std::endl;
      if(!dam[loc]) tcs -> Fill(xValue[0][X], yValue[Y], tData[0][X][Y]);
      if(!dam[loc]&&csNorm>0.) tcsn -> Fill(xValue[0][X], yValue[Y], tData[0][X][Y]/csNorm);
      if(!dam[loc]&&!fib[loc])	tavsdnfcs -> Fill(d, tData[0][X][Y]);
      if(!dam[loc]&&fib[loc])   tavsdfcs  -> Fill(d, tData[0][X][Y]); 
    }
    //  fiber side
    for (X = 0; X<11; X++) {
      line = 12+X+1;
      loc  = Y*11 + X;
      dx   = xValue[1][X] - xExit;  dy = yValue[Y]- yExit;
      Double_t d  = sqrt(dx*dx+dy*dy);
      //      std::cout<<"X/Y/loc/dam/fib/line/pos/V  "<<X<<" "<<Y<<" "<<loc<<" D/F "<<dam[loc]<<"/"<<fib[loc]<<"  "<<line<<"  "<<pos<<"  "<<value[line][pos]<<std::endl;
      if(/*!dam[loc]*/) tfs -> Fill(xValue[1][X],yValue[Y], tData[1][X][Y]);
      if(/*!dam[loc]&&*/fsNorm>0.) tfsn -> Fill(xValue[1][X],yValue[Y], tData[1][X][Y]/fsNorm);
      if(!dam[loc]&&!fib[loc])     tavsdnffs -> Fill(d, tData[1][X][Y]); 
      if(!dam[loc]&&fib[loc])      tavsdffs  -> Fill(d, tData[1][X][Y]); 
    }

    for (X = 0; X<11; X++) {
      //  clear to fiber raw ratio
      loc  = Y*11 + X;
      if(tData[0][X][Y]>0.&&tData[1][X][Y]>0.) {
	Double_t fcrat  = tData[1][X][Y]/tData[0][X][Y];
	if(!dam[loc])   fcr -> Fill(xValue[1][X],yValue[Y], fcrat);
	if(!dam[loc]&&csNorm>0.&&fsNorm>0.) fcrn -> Fill(xValue[1][X],yValue[Y], fcrat/(fsNorm/csNorm));
      }
      Double_t repr(0);
      if(Y==0) {
	//  reproducibility of measurements
	if(cData[0][X][0]>0.&&tData[0][X][0]>0.) {
	  repr   = 2.*(tData[0][X][0]-cData[0][X][0])/(tData[0][X][0]+cData[0][X][0]);
	  AtoB    -> Fill(repr);
	  AtoB1cs -> Fill(tData[0][X][0]/cData[0][X][0]);
	  AtoBtcs -> Fill(tData[0][X][0]/cData[0][X][0]);
	}
	if(cData[0][X][1]>0.&&tData[0][X][8]>0.) {
	  repr   = 2.*(tData[0][X][8]-cData[0][X][1])/(tData[0][X][8]+cData[0][X][1]);
	  AtoB    -> Fill(repr);
	  AtoB2cs -> Fill(tData[0][X][8]/cData[0][X][1]);
	  AtoBtcs -> Fill(tData[0][X][8]/cData[0][X][1]);
	}
	if(cData[1][X][0]>0.&&tData[1][X][0]>0.) {
	  repr   = 2.*(tData[1][X][0]-cData[1][X][0])/(tData[1][X][0]+cData[1][X][0]);
	  AtoB    -> Fill(repr);
	  AtoB1fs -> Fill(tData[1][X][0]/cData[1][X][0]);
	  AtoBtfs -> Fill(tData[1][X][0]/cData[1][X][0]);
	}
	if(cData[1][X][1]>0.&&tData[1][X][8]>0.) {        
	  repr   = 2.*(tData[1][X][8]-cData[1][X][1])/(tData[1][X][8]+cData[1][X][1]);
	  AtoB    -> Fill(repr);
	  AtoB    -> Fill(tData[1][X][8]/cData[1][X][1]);
	  AtoB2fs -> Fill(tData[1][X][8]/cData[1][X][1]);
	  AtoBtfs -> Fill(tData[1][X][8]/cData[1][X][1]);
	}
      }
    }
  }
  for (X = 0; X < 11; X++) {
    for (Y = 0; Y < 9;  Y++) {
      loc  = Y*11 + X;
      Double_t vacsn = tcsn -> GetBinContent(X, Y);
      Double_t vafsn = tfsn -> GetBinContent(X, Y);
      Double_t vfcrn = fcrn -> GetBinContent(X, Y);
      if(vacsn>0.) cs_all ->Fill(vacsn);
      if(fib[loc]) {
	if(vacsn>0.) cs_onf ->Fill(vacsn);
	if(vafsn>0.) fs_onf ->Fill(vafsn);
	if(vfcrn>0.) rfc_onf->Fill(vfcrn);
      } else {
	if(vacsn>0.) cs_off ->Fill(vacsn);
	if(vafsn>0.) fs_off ->Fill(vafsn);
	if(vfcrn>0.) rfc_off->Fill(vfcrn);
      }

    }
  }

}

TCanvas  *tMap(NULL), * tDQ(NULL);
void plot(){
  //  Tile Response Map
  if((tMap = (TCanvas *)(gROOT->FindObject("tMAP")))) delete tMap;
  tMap = new TCanvas("tMap","Tile Response Map",200, 10, 600, 500);
  //  Tile Data Quality (sequential measurements)
  tMap -> Divide(2,2,0.05,0.05,0);
  tMap -> cd(1);
  gStyle ->SetOptStat(0);
  tcsn -> Draw("lego2");
  tMap -> cd(2);
  tfsn -> Draw("colz");
  //tMap -> cd(3);
  //fcrn -> Draw("lego2");
  tMap -> cd(3);
  //cs_off -> SetLineColor(2);
  gStyle ->SetOptStat(111);
  cs_all -> SetMaximum(20);
  cs_all -> SetLineColor(1);
  cs_all -> SetLineWidth(2);
  cs_all -> Draw();
  //cs_onf -> Draw("same");
  tMap -> cd(4);
  gStyle ->SetOptStat(0);
  cs_off -> SetMaximum(20);
  cs_off -> SetLineColor(1);
  cs_off -> SetLineWidth(2);
  cs_off -> Draw();
  fs_off -> SetLineColor(2);
  fs_off -> SetLineWidth(2);
  fs_off -> Draw("same");
  //  tMap -> cd(6);
  //  rfc_off -> SetLineColor(2);
  //  rfc_off -> Draw();
  //  rfc_onf -> Draw("same");

  if((tDQ  = (TCanvas *)(gROOT->FindObject("tDQ"))))  delete tDQ;
  if(!tDQ)  tDQ = new TCanvas("tDQ","Tile Data Quality",  160, 10, 400, 300);
  gStyle -> SetOptStat(0);
  AtoB  -> SetLineWidth(2);
  AtoB  -> Draw();
  //  AtoBtcs -> Draw("same");

}
;
