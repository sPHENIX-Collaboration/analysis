int
plot_track()
{

  TFile *fin = new TFile("G4EICIR.root_DSTReader.root");

  TTree *tin = (TTree*)fin->Get("T");

  int nhits = 0;
  tin->SetBranchAddress("n_G4HIT_FWDDISC",&nhits);

  TH1F *h1 = new TH1F("h1","",10,0,15000);
  h1->GetXaxis()->SetRangeUser(0,12000);
  h1->GetYaxis()->SetRangeUser(-200,200);
  h1->GetXaxis()->SetTitle("Z(cm)");
  h1->GetYaxis()->SetTitle("X(cm)");

  /* create graph */
  tin->GetEntry(0);
  cout << "hits: " << nhits << endl;

  tin->Draw("G4HIT_FWDDISC.x:G4HIT_FWDDISC.z","Entry$==0","");

  TGraph* g1 = new TGraph(nhits*2, &(tin->GetV2()[0]), &(tin->GetV1()[0]));
  g1->SetMarkerStyle(7);
  g1->SetMarkerSize(1);

  TCanvas *c1 = new TCanvas();
  h1->Draw("AXIS");
  g1->Draw("Psame");

// Read IR configuration file- this needs to go somewhere else using parameters and a .root file to store them
  string irfile = "updated-magnets-2017.dat";
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

    //   TBox *b1 = new TBox(length/2,-0.3,center_z + length,-0.1);//both at same z, one positive and one negative
    TBox *b1 = new TBox(center_z - length/2,center_x-30,center_z+length/2,center_x-aperture_radius); //lower box
     TBox *b2 = new TBox(center_z - length/2,center_x+aperture_radius,center_z+length/2,center_x+30);//upper box


    if(B != 0 && gradient == 0.0){
      //dipole magnet
      b1->SetFillColor(kRed);
       b2->SetFillColor(kRed);
      }
    else if( B == 0 && gradient != 0.0){
      //quad magnet
      b1->SetFillColor(kGreen);
       b2->SetFillColor(kGreen);
    }
    else{
      //placeholder magnet
      b1->SetFillColor(kBlue);
      b2->SetFillColor(kBlue);
    }
    b1->Draw("same");
    b2->Draw("same");

    /* if ( B != 0 && gradient == 0.0 )
      {
	ir_magnet_i->set_string_param("magtype","dipole");
	ir_magnet_i->set_double_param("field_y",B);
      }
    else if ( B == 0 && gradient != 0.0 )
      {
	ir_magnet_i->set_string_param("magtype","quadrupole");
	ir_magnet_i->set_double_param("fieldgradient",gradient);
      } */  

  }

    // TBox *b = new TBox(493,-7.1,527,26.9);
    // b->Draw("same");

  return 0;
}
