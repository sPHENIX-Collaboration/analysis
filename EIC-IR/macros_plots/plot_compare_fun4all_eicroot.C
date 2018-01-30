#include "TraceBox.h"

int
plot_compare_fun4all_eicroot()
{
  /**** Chose input files ****/
  /* File with IR mangets configuration*/
  TString fname_irmag("example/proton-magnets-250GeV-opt2.dat");
  /* File with Fun4All output*/
  TString fname_fun4all("example/eRHIC_proton-magnets-250GeV-opt2_250GeV_0mrad.root");
  /* File with EICROOT output*/
  TString fname_eicroot("example/eicroot-track_proton-magnets-250GeV-opt2_250GeV_0mrad.txt");

  /* Open iput file with trajectories from GEANT4 */
  TFile *fin = new TFile(fname_fun4all);

  /* Get tree from file */
  TTree *tin = (TTree*)fin->Get("T");

  int nhits = 0;
  tin->SetBranchAddress("n_G4HIT_FWDDISC",&nhits);

  /* create graph of particle trajectory */
  /* Use only first event (for now) */
  tin->GetEntry(0);
  cout << "hits: " << nhits << endl;

  tin->Draw("G4HIT_FWDDISC.x:G4HIT_FWDDISC.z","Entry$==0","");

  TGraph* g1 = new TGraph(nhits*2, &(tin->GetV2()[0]), &(tin->GetV1()[0]));
  g1->SetMarkerStyle(7);
  g1->SetMarkerSize(1);
  g1->SetMarkerColor(kRed);

  /* Get tree from file */
  TTree *tin2 = new TTree();
  tin2->ReadFile(fname_eicroot,"x/F:y:z");

  int nhits = tin2->GetEntries();
  tin2->Draw("x:z","","");

  TGraph* g2 = new TGraph(nhits, &(tin2->GetV2()[0]), &(tin2->GetV1()[0]));
  g2->SetMarkerStyle(7);
  g2->SetMarkerSize(1);
  g2->SetMarkerColor(kGreen+1);

  /* Create frame histogram for plot */
  TH1F *h1 = new TH1F("h1","",10,0,15000);
  h1->GetXaxis()->SetRangeUser(0,5000);
  h1->GetYaxis()->SetRangeUser(-50,70);
  h1->GetXaxis()->SetTitle("Z(cm)");
  h1->GetYaxis()->SetTitle("X(cm)");

  /* Plot frame histogram */
  TCanvas *c1 = new TCanvas();
  h1->Draw("AXIS");

  /* Read IR configuration file- this needs to go somewhere else using parameters and a .root file to store them */
  ifstream irstream(fname_irmag);

  if(!irstream.is_open())
    {
      cout << "ERROR: Could not open IR configuration file " << fname_irmag << endl;
      return -1;
    }

  while(!irstream.eof()){
    string str;
    getline(irstream, str);
    if(str[0] == '#') continue; //for comments
    stringstream ss(str);

    string name;
    double center_z, center_x, center_y, aperture_radius, length, angle, B, gradient;

    ss >> name >> center_z >> center_x >> center_y >> aperture_radius >> length >> angle >> B >> gradient;

    if ( name == "" ) continue; //for empty lines

    // convert units from m to cm
    center_x *= 100;
    center_y *= 100;
    center_z *= 100;
    aperture_radius *= 100;
    length *= 100;

    // define magnet outer radius
    float outer_radius = 30.0; // cm

    //flip sign of dipole field component- positive y axis in Geant4 is defined as 'up',
    // positive z axis  as the hadron-going direction
    // in a right-handed coordinate system x,y,z
    B *= -1;

    // convert angle from millirad to rad
    angle = (angle / 1000.);

    // Place IR component
    cout << "New IR component: " << name << " at z = " << center_z << endl;

    string volname = "IRMAGNET_";
    volname.append(name);

    /* Draw box for magnet position on canvas */
    TPolyLine *b1 = TraceBox( angle, center_z, center_x, length, aperture_radius, outer_radius ); //upper box
    TPolyLine *b2 = TraceBox( angle, center_z, center_x, length, -1 * aperture_radius, -1 * outer_radius ); //lower box

    if(B != 0 && gradient == 0.0){
      //dipole magnet
      b1->SetFillColor(kOrange+1);
      b2->SetFillColor(kOrange+1);
    }
    else if( B == 0 && gradient != 0.0){
      //quad magnet
      b1->SetFillColor(kBlue+1);
      b2->SetFillColor(kBlue+1);
    }
    else{
      //placeholder magnet
      b1->SetFillColor(kGray+1);
      b2->SetFillColor(kGray+1);
    }
    b1->Draw("Fsame");
    b2->Draw("Fsame");
  }

  /* draw particle trajectory */
  g2->Draw("LPsame");
  g1->Draw("LPsame");

  return 0;
}
