int
plot_track()
{

  /* Open iput file with trajectories from GEANT4 */
  TFile *fin = new TFile("data/G4EICIR.root_DSTReader.root");

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

  /* Create frame histogram for plot */
  TH1F *h1 = new TH1F("h1","",10,0,15000);
  h1->GetXaxis()->SetRangeUser(0,12000);
  h1->GetYaxis()->SetRangeUser(-50,200);
  h1->GetXaxis()->SetTitle("Z(cm)");
  h1->GetYaxis()->SetTitle("X(cm)");

  /* Plot frame histogram */
  TCanvas *c1 = new TCanvas();
  h1->Draw("AXIS");

  /* Read IR configuration file- this needs to go somewhere else using parameters and a .root file to store them */
  string irfile = "data/updated-magnets-2017.dat";
  ifstream irstream(irfile.c_str());

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
    outer_radius = 30.0; // cm

    //flip sign of dipole field component- positive y axis in Geant4 is defined as 'up',
    // positive z axis  as the hadron-going direction
    // in a right-handed coordinate system x,y,z
    B *= -1;

    // convert angle from millirad to degrees
    angle = (angle / 1000.) * (180./TMath::Pi());

    // Place IR component
    cout << "New IR component: " << name << " at z = " << center_z << endl;

    string volname = "IRMAGNET_";
    volname.append(name);

    /* Draw box for magnet position on canvas */
    /* TODO: Use angle! */
    TBox *b1 = new TBox(center_z - length/2,center_x-outer_radius,center_z+length/2,center_x-aperture_radius); //lower box
    TBox *b2 = new TBox(center_z - length/2,center_x+aperture_radius,center_z+length/2,center_x+outer_radius);//upper box

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
    b1->Draw("same");
    b2->Draw("same");

  }

  /* draw particle trajectory */
  g1->Draw("LPsame");

  return 0;
}
