TPolyLine* TraceBox( float angle, float center_z, float center_x, float length, float aperture_radius, float outer_radius )
{
  /* center of magnet box */
  TVector3* vcenter = new TVector3( center_z,
				    center_x,
				    0.0 );

  /* fill corner positions in 3D vector TVector objects for easier rotation */
  const unsigned npts = 5;
  TObjArray a;

  /* corner points in unrotated coordinate system */
  a.AddLast( new TVector3( 0.0 - ( length / 2. ),
			   0.0 + outer_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 + ( length / 2. ),
			   0.0 + outer_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 + ( length / 2. ),
			   0.0 + aperture_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 - ( length / 2. ),
			   0.0 + aperture_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 - ( length / 2. ),
			   0.0 + outer_radius,
			   0.0 ) );

  /* loop over array and rotate points coordinate system and move to real center */
  for ( int i = 0; i < npts; i++ )
    {
      /* calculated rotated x, y */
      float rotated_x = ( (TVector3*)a[i] )->X() * cos( angle ) - ( (TVector3*)a[i] )->Y() * sin( angle );
      float rotated_y = ( (TVector3*)a[i] )->X() * sin( angle ) + ( (TVector3*)a[i] )->Y() * cos( angle );

      /* add center offset */
      rotated_x += center_z;
      rotated_y += center_x;

      /* set vector coordinates to new x, y */
      ( (TVector3*)a[i] )->SetX( rotated_x );
      ( (TVector3*)a[i] )->SetY( rotated_y );
    }

  /* extract two 1-D arrays from array of vectors with corner points coordinates */
  float xarr[ npts ];
  float yarr[ npts ];

  for ( int i = 0; i < npts; i++ )
    {
      xarr[i] = ( (TVector3*)a[i] )->X();
      yarr[i] = ( (TVector3*)a[i] )->Y();
    }

  /* create and return TPolyLine object */
  return new TPolyLine(npts, xarr, yarr);
}

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
  g1->Draw("LPsame");

  return 0;
}
