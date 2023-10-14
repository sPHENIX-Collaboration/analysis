#include <iostream>
#include <fstream>
#include <TLorentzVector.h>
#include <TF1.h>
#include <TRandom3.h>

using namespace std;

const double E_MASS = 0.000510998950;  // electron mass [Gev]

TRandom3 *rand3;

// Return the relative resolution dp/p, for a particle with momentum p
// The momentum resolution was taken from sPHENIX simulation
double get_dpp( double p )
{
  // Parametrization of the relative momentum resolution (dp/p)
  static TF1 *f_dppfit = new TF1("f_dppfit","[0]*x + [1]/x + [2]",0.3,50.0);
  f_dppfit->SetParameter(0,0.0010557);
  f_dppfit->SetParameter(1,0.00262218);
  f_dppfit->SetParameter(2,0.0090893);
  
  // Return dp/p for a particle of momentum p 
  double dpp = f_dppfit->Eval( p );
  return dpp;
}

// Return the smeared inv mass, for two truth particles v1 and v2
Double_t get_invmass_smeared(TLorentzVector v1, TLorentzVector v2)
{
  // Get total momentum of particles 1 and 2
  double p1 = v1.P();
  double p2 = v2.P();

  // Get the sigma of the momentum resolution for particle 1 (dp1)
  double sigma_p1 = get_dpp( p1 ) * p1;

  // Now get random value for momentum deviation (mis-measured momentum)
  double dp1 = rand3->Gaus( 0, sigma_p1 );

  // now modify the true momentum to the mis-measured momentum
  double dpp1 = 1.0 + dp1/p1;
  double px1 = v1.Px()*dpp1;
  double py1 = v1.Py()*dpp1;
  double pz1 = v1.Pz()*dpp1;

  // Use mis-measured momentum to get mis-measured E, and generate lorentz 4-vector
  double E1 = sqrt( E_MASS*E_MASS + px1*px1 + py1*py1 + pz1*pz1 );
  v1.SetPxPyPzE( px1, py1, pz1, E1 );

  // Now get momentum smearing of 2nd particle, exactly in the same way as the 1st particle
  double sigma_p2 = get_dpp( p2 ) * p2;
  double dp2 = rand3->Gaus( 0, sigma_p2 );
  double dpp2 = 1.0 + dp2/p2;

  double px2 = v2.Px()*dpp2;
  double py2 = v2.Py()*dpp2;
  double pz2 = v2.Pz()*dpp2;
  double E2 = sqrt( E_MASS*E_MASS + px2*px2 + py2*py2 + pz2*pz2 );
  v2.SetPxPyPzE( px2, py2, pz2, E2 );

  // Once we have both tracks with momentum resolution effects included, we sum them to get
  // measured parent particle 4-vector and return the mass
  TLorentzVector sum = v1 + v2;

  return sum.M();
}

void ana_starlight()
{ 
    gStyle->SetOptStat(0);
    rand3 = new TRandom3(0);

    const double integ_lumi = 4.5e9;   // in inverse barns

    // Get cross-section of process
    TString xsection_txt = gSystem->GetFromPipe("tail -1 output.txt | awk '{print $9}'");
    TString xsection_units_txt = gSystem->GetFromPipe("tail -1 output.txt | awk '{print $10}'");
    Double_t xsection = xsection_txt.Atof();    // xsection is in barns
    if ( xsection_units_txt == "mb." )
    {
      xsection *= 1e-3;
    }
    else if ( xsection_units_txt == "microbarn." )
    {
      xsection *= 1e-6;
    }
    cout << "xsection (b) and integ lumi (1/b) = " << xsection << "\t" << integ_lumi << endl;

    // open ASCII file.
    ifstream in;
    //in.open(Form("output.txt"));
    in.open("slight.out");

    // Check if file is open

    if (in.is_open())
    {
        cout << "File found\n";
        //in.close();
    }
    else
    {
        cout << "Error opening file";
    }

    // kinematics variables

    const Int_t kMaxTrack = 1000;
    Int_t ntrack;

    Float_t px1;  // momentum of first daughter in the x-direction 
    Float_t py1;  // momentum of first daughter in the y-direction
    Float_t pz1;  // momentum of first daughter in the z-direction
    Float_t E1;   // energy of the first daughter
    Float_t p1;   // momentum of the first daughter
    Float_t pt1;  // momentum of the first daughter
    Float_t me1;  // mass of the first daughter
    Float_t rap1; // rapidity of the first daughter
    Float_t eta1; // pseudorapidity of the first daughter

    Float_t px2;  // kinematic variables for the 2nd daughter...
    Float_t py2;
    Float_t pz2;
    Float_t E2;
    Float_t p2;
    Float_t pt2;
    Float_t me2;
    Float_t rap2;
    Float_t eta2;

    Float_t pxt;      // x-direction momentum of the parent particle
    Float_t pyt;      // y-direction momentum  of the parent particle
    Float_t pzt;      // z -direction momentum of the parent atom
    Float_t ptt;      // transverse momentum of the parent atom
    Float_t ptott;    // momentum of the parent atom
    Float_t E;        // total energy of the parent atom
    Float_t rapt;     // rapidity
    Float_t etat;     // pseudorapidity
    Float_t InvMass;  // invariant mass

    Float_t theta;    // polar angle

    Int_t num_of_events;

    // Create tfile to save results
    TFile *savefile = new TFile("upc_starlight.root","RECREATE");

    //// Create a TTree and branches of the events
    TTree * tree = new TTree("t", "Analyze.tree");
    tree->Branch("px",&pxt,"px/F"); 
    tree->Branch("py",&pyt,"py/F"); 
    tree->Branch("pz",&pzt,"pz/F"); 
    tree->Branch("p",&ptott,"p/F"); 
    tree->Branch("pt",&ptt,"pt/F");
    tree->Branch("y",&rapt,"y/F"); 
    tree->Branch("eta",&etat,"eta/F"); 
    tree->Branch("m", &InvMass, "m/F");
    tree->Branch("p1", &p1, "p1/F");
    tree->Branch("pt1", &pt1, "pt1/F");
    tree->Branch("eta1", &eta1, "eta1/F");
    tree->Branch("p2", &p2, "p2/F");
    tree->Branch("pt2", &pt2, "pt2/F");
    tree->Branch("eta2", &eta2, "eta2/F");


    //tree->Print();

    
    const double MAX_RAP = 4;
    TH1F *h_rapt = new TH1F("h_rapt", "Rapidity J/#Psi#rightarrow e^{+}e^{-}", 200, -MAX_RAP, MAX_RAP);
    TH1F *h_rapt_sphenix = new TH1F("h_rapt_sphenix", "Rapidity J/#Psi#rightarrow e^{+}e^{-} in sPHENIX Acceptance", 200, -MAX_RAP, MAX_RAP);
    TH1F *h_rap1 = new TH1F("h_rap1", "Rapidity Track 1", 200, -MAX_RAP, MAX_RAP);
    TH1F *h_rap2 = new TH1F("h_rap2", "Rapidity Track 2", 200, -MAX_RAP, MAX_RAP);

    const double MAX_INVMASS =6;
    TH1F *h_InvMass = new TH1F("h_InvMass", "Invariant Mass", 200, 0, 6.0);
    TH1F *h_InvMass_smeared = new TH1F("h_InvMass_smeared", "Invariant Mass", 200, 0, 6.0);
    TH1F *h_InvMass_sphenix = new TH1F("h_InvMass_sphenix", "Invariant Mass of J/#Psi in sPHENIX Acceptance",200, 0, 6.0);
    TH1F *h_InvMass_smeared_sphenix = new TH1F("h_InvMass_smeared_sphenix", "Invariant Mass of J/#Psi in sPHENIX Acceptance",200, 0, 6.0);

    TH1F *h_pt1 = new TH1F("h_pt1", "Transverse Momentum track 1", 200, 0, 2.);
    TH1F *h_pt2 = new TH1F("h_pt2", "Transverse Momentum track 2", 200, 0, 2.);
    TH1F *h_pt = new TH1F("h_pt","Transverse Momentum of J/Psi",200, 0, 2.0);
    TH1F *h_pt_sphenix = new TH1F("h_pt_sphenix", "Transverse Momentum of UPC in sPHENIX Acceptance", 300, 0.0, 2.0);
    TH1F *h_pz = new TH1F("h_pz", "Z-Axis Momentum of J/Psi", 100,-2,6);

    const double MAX_ETA = 6;
    TH1F *h_eta1 = new TH1F("h_eta1", "PseudoRapidity Track 1", 200, -MAX_ETA, MAX_ETA);
    TH1F *h_eta2 = new TH1F("h_eta2", "PseudoRapidity Track 2", 200, -MAX_ETA, MAX_ETA);

    // Read in one event from starlight output
    string label;
    int nev, ntr, stopv; //nev is the event number; ntr is the number of tracks within the vertex; stopv is the vertex number where the track ends
    Float_t v0[4]; // 0-2 = x,y,z 3=t
    int nv, nproc, nparent, ndaughter;//
    cout << label << "\t" << ndaughter << endl;
    const int MAXDAUGHTERS = 10; //changing this number does not change the energy or value outputted at the end of the function
    int gpid[MAXDAUGHTERS], pdgpid[MAXDAUGHTERS]; //gpid is the monte carlo particle id code
    Float_t mom[MAXDAUGHTERS][3];
    int  nevtrk, stopvtrk, junk[9];//number of event tracks, number of stopping tracks
    /* for (int idtr=0; idtr<ndaughter; idtr++)
       {
       in >> label >> gpid[idtr] >> mom[idtr][0] >> mom[idtr][1] >> mom[idtr][2]
       >> nevtrk >> ntr >> stopvtrk >> pdgpid[idtr];
       }
       */
    TLorentzVector vp; //parent particle JPsi
    TLorentzVector v1; //daughter particle 1 electron
    TLorentzVector v2; //daughter particle 2 electron

    /*	Double_t pxt= vp.pxt(); //truthfully idek if i need this double because I already floated pxt-E and when I run it with this commented out it works.
        Double_t pyt= vp.pyt(); //but I'm not getting multiple graphs.
        Double_t pzt= vp.pzt();
        Double_t E= vp.E(); 


        vp.SetPxPyPzE(vp->GetPx(),vp->GetPy(),vp->GetPz(),vp->GetE());
        v1.SetPxPyPzE(v1->GetPx(),v1->GetPy(),v1->GetPz(),v1->GetE());
        v2.SetPxPyPzE(v2->GetPx(),v2->GetPy(),v2->GetPz(),v2->GetE()); */


    Double_t mass = vp.M();// mass of parent

    Double_t pt = vp.Pt();//parent pT

    unsigned int nevents = 0;   // number of events in file

    while ( in.good() )
    {

        // read the event line
        in >> label >> nev >> ntr >> stopv;
        if ( in.eof() ) break;    // quit reading if we reached eend of file

        // read the vertex line
        in >> label >> v0[0] >> v0[1] >> v0[2] >> v0[3] >> nv >> nproc >> nparent >> ndaughter;

        // read in daughters
        in >> label >>gpid[0] >> mom[0][0] >> mom[0][1] >> mom[0][2]
            >> nevtrk >> ntr >> stopvtrk >> pdgpid[0];

        in >> label >>gpid[1] >> mom[1][0] >> mom[1][1] >> mom[1][2]
            >> nevtrk >> ntr >> stopvtrk >> pdgpid[1];

        //	cout << "\n GPID=" << gpid[idtr] << "\n Momentum 0=" << mom[idtr][0] << "\n Momentum 1=" << mom[idtr][1] << "\n Momentum 2=" << mom[idtr][2] 
        //	<< "\n # of EventTracks=" << nevtrk << "\n # of Tracks w/in Vertex=" << ntr << "\n Vertex # where Track Stops=" << stopvtrk << endl;

        //	Calculate pT
        // set the values in each track

        // get momentum from 1st daughter particle
        px1 = mom[0][0];
        py1 = mom[0][1];
        pz1 = mom[0][2];
        pt1 = TMath::Sqrt( px1*px1 + py1*py1 );
        p1 = TMath::Sqrt( px1*px1 + py1*py1 + pz1*pz1 );

        // get momentum from 2nd daughter particle
        px2 = mom[1][0];
        py2 = mom[1][1];
        pz2 = mom[1][2];
        pt2 = TMath::Sqrt( px2*px2 + py2*py2 );
        p2 = TMath::Sqrt( px2*px2 + py2*py2 + pz2*pz2 );

        // total momentum of the daughters

        pxt = px1 + px2;//momentum of particle one and two in the x direction equals the total momentum in x direction
        pyt = py1 + py2;
        pzt = pz1 + pz2; 
        //cout << pzt << endl;

        //Calculate the tranverse momentum
        ptt = TMath::Sqrt(pxt*pxt + pyt*pyt); //ptt is total transverse momentum
        // cout << px1 << "\t" << px2 << endl;
        Double_t p = TMath::Sqrt(pxt*pxt + pyt*pyt + pzt*pzt); //momentum in the x,y,z plane
        // Fill histogram
        h_pt->Fill(ptt); 

        // calculate the invariant mass
        //	cout <<"\n"<<"InvMass  = "<< InvMass << "\t"  << endl;
        me1 = me2 = E_MASS;

        E1 = TMath::Sqrt((me1 *me1) + (px1*px1) + (py1*py1) +(pz1*pz1));
        E2 = TMath::Sqrt((me2 *me2) + (px2*px2) + (py2*py2) +(pz2*pz2));

        //cout <<"\n"<<"Energy = "<< E << "\t" << endl;

        E = E1 + E2;

        v1.SetPxPyPzE(px1, py1, pz1, E1);
        v2.SetPxPyPzE(px2, py2, pz2, E2);

        //mass should always be about 3.1 GeV^2 no matter the frame
        InvMass =TMath::Sqrt((E * E)- (pxt * pxt) -(pyt * pyt) -(pzt*pzt));
        Double_t m = TMath::Sqrt(pxt*pxt + pyt*pyt + pzt*pzt);

        Double_t InvMass_smeared = get_invmass_smeared(v1,v2);

        //Fill Histogram
        h_InvMass->Fill( InvMass );
        h_InvMass_smeared->Fill( InvMass_smeared );
        // cout <<"\n"<<"InvMass  = "<< InvMass << "\t"  << endl;

        //You have to calculate the rapitidy,eta, and transverse momentum for the daughter particles. Eta is pseudorapidity which tells you what "direction" or angle they div           ert at, rapidity is the natural unit for speed of the particle in relative spaces, and the transverse momentum tells you.... idrk tbh.

        // calculate the rapidity
        rapt = 0.5 * log((E + pzt)/(E - pzt)); //rapidity of parent

        // Rapidity of daughters

        rap1 = 0.5 * log((E1 + pz1)/(E1 - pz1));
        rap2 = 0.5 * log((E2 + pz2)/(E2 - pz2));
        h_rap1->Fill( rap1 ); 
        h_rap2->Fill( rap2 );
        h_rapt->Fill( rapt );

        // Pseudorapidity of daughters
        double theta1 = atan2( pt1, pz1 );
        eta1 = -log( tan( 0.5*theta1 ) );

        double theta2 = atan2( pt2, pz2 );
        eta2 = -log( tan( 0.5*theta2 ) );

        h_eta1->Fill( eta1 );
        h_eta2->Fill( eta2 );

        double thetat = atan2( ptt, pzt );
        etat = -log( tan(0.5*thetat ) );

        // Check that both daughters are in sPHENIX acceptance
        if ( fabs(eta1) < 1.1 && fabs(eta2) < 1.1 && pt1>0.3 && pt2>0.3 )
        {
            h_rapt_sphenix->Fill( rapt );
            h_InvMass_sphenix->Fill(InvMass);
            h_InvMass_smeared_sphenix->Fill(InvMass_smeared);
            h_pt_sphenix->Fill( ptt );
        }

        nevents++;

        tree->Fill();
    }

    cout << "Processed " << nevents << endl;

    // Scaling factor for integrated luminosity expected in sPHENIX Run
    Double_t scale = xsection*integ_lumi/nevents;
    cout << "Scale factor is " << endl;
    h_rapt->Sumw2();
    h_rapt_sphenix->Sumw2();
    h_pt->Sumw2();
    h_pt_sphenix->Sumw2();
    h_InvMass->Sumw2();
    h_InvMass_sphenix->Sumw2();
    h_InvMass_smeared->Sumw2();
    h_InvMass_smeared_sphenix->Sumw2();
    h_rapt->Scale( scale );
    h_rapt_sphenix->Scale( scale );
    h_pt->Scale( scale );
    h_pt_sphenix->Scale( scale );
    h_InvMass->Scale( scale );
    h_InvMass_sphenix->Scale( scale );
    h_InvMass_smeared->Scale( scale );
    h_InvMass_smeared_sphenix->Scale( scale );

    cout << "Scale factor = " << scale << endl;
    cout << "Total events = " << h_rapt->Integral() << endl;
    cout << "Total events in sPHENIX acceptance = " << h_rapt_sphenix->Integral() << endl;
    cout << "Acceptance Factor = " << h_rapt_sphenix->Integral()/h_rapt->Integral() << endl;
 
    
    TCanvas *a = new TCanvas("a","Total Parent Momentum of JPsi",550,425);
    h_pt->SetLineColor(kBlue);
    h_pt->SetXTitle("p_{T} (GeV/c)");
    h_pt->SetYTitle("Counts");
    h_pt->Draw();
    a->SaveAs("PTofParent.png");

    TCanvas *b = new TCanvas("b","Invariant Mass of JPsi",550,425);
    h_InvMass_smeared->SetLineColor(kBlack);
    h_InvMass_smeared->SetXTitle("mass (GeV/c^{2})");
    h_InvMass_smeared->SetYTitle("Counts");
    h_InvMass_smeared->Draw("ehist");
    h_InvMass_smeared_sphenix->SetLineColor(kBlue);
    h_InvMass_smeared_sphenix->SetMarkerColor(kBlue);
    h_InvMass_smeared_sphenix->SetXTitle("Mass (GeV/c^{2})");
    h_InvMass_smeared_sphenix->Draw("ehistsame");
    b->SaveAs("MassofUPCJPsi.png");

    TCanvas *c = new TCanvas("c","Rapidity of E1 and E2",550,425);
    h_rap1->SetLineColor(kGreen);
    h_rap1->SetTitle("Rapidity e^{+},e^{-}");
    h_rap1->SetXTitle("Rapidity y");
    h_rap1->SetYTitle("Counts");
    h_rap1->Draw();
    h_rap2->SetLineColor(kRed);
    h_rap2->SetXTitle("Rapidity y");
    h_rap2->SetYTitle("Counts");
    h_rap2->Draw("same");
    c->SaveAs("RapofE1E2.png");

    TCanvas *d = new TCanvas("d","Rapidity of UPC J/Psi",550,425);
    h_rapt->SetLineColor(kBlack);
    h_rapt->SetXTitle("Rapidity y");
    h_rapt->SetYTitle("Counts");
    h_rapt->Draw("ehist");
    h_rapt_sphenix->SetLineColor(kBlue);
    h_rapt_sphenix->SetMarkerColor(kBlue);
    h_rapt_sphenix->SetXTitle("Rapidity y");
    h_rapt_sphenix->Draw("ehistsame");
    d->SaveAs("RapofUPCJPsi.png");

    TCanvas *e = new TCanvas("e","PZ",550,425);
    h_pz->SetLineColor(kBlue);
    h_pz->SetXTitle("p_{Z} GeV/c");
    h_pz->SetYTitle("Counts");
    h_pz->Draw();
    gPad->SetLogy(1);
    e->SaveAs("pz.png");

    // Save histograms and TTree to file
    savefile->Write();

} //closing function
