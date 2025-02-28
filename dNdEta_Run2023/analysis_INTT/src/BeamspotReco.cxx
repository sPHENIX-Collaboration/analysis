#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TTree.h"

#include <Eigen/Dense>

#include "Beamspot.h"
#include "Hit.h"
#include "Utilities.h"

Beamspot fit_PCAD0Phi0(std::vector<std::pair<std::shared_ptr<Hit>, std::shared_ptr<Hit>>> tracklets, Beamspot BS_init, double d0_cut)
{
    Beamspot reco_BS;

    int Ntracklets = 0;

    std::cout << "processing " << tracklets.size() << " tracklets..." << std::endl;

    for (auto &tracklet : tracklets)
    {
        double x1 = tracklet.first->posX();
        double y1 = tracklet.first->posY();
        double z1 = tracklet.first->posZ();
        double x2 = tracklet.second->posX();
        double y2 = tracklet.second->posY();
        double z2 = tracklet.second->posZ();

        int size1 = tracklet.first->PhiSize();
        int size2 = tracklet.second->PhiSize();

        // std::cout << "tracklet: (" << x1 << ", " << y1 << ", " << z1 <<") -> (" << x2 << ", " << y2 << ", " << z2 << ")" << std::endl;

        // cluster position uncertainties
        double stripwidth = 0.0078; // cm
        double rphierr1 = stripwidth * size1;
        double rphierr2 = stripwidth * size2;
        double r1 = sqrt(x1 * x1 + y1 * y1);
        double r2 = sqrt(x2 * x2 + y2 * y2);
        double phi1 = atan2(y1, x1);
        double phi2 = atan2(y2, x2);
        double sigma2_x1 = pow(rphierr1 * cos(phi1), 2.);
        double sigma2_y1 = pow(rphierr1 * sin(phi1), 2.);
        double sigma2_x2 = pow(rphierr2 * cos(phi2), 2.);
        double sigma2_y2 = pow(rphierr2 * sin(phi2), 2.);

        // phi0 (angle of momentum vector at PCA) and uncertainty
        double px = x2 - x1;
        double py = y2 - y1;
        double sigma2_px = sigma2_x2 + sigma2_x1;
        double sigma2_py = sigma2_y2 + sigma2_y1;
        double phi0 = atan2(py, px);
        double sigma2_phi0 = (sigma2_px * py * py + sigma2_py * px * px) / pow(px * px + py * py, 2.); // the aftermath of a bit of algebra
        // std::cout << "phi0: " << phi0 << " +- " << sqrt(sigma2_phi0) << std::endl;

        // PCA and uncertainty
        double pca_x = x1 + ((BS_init.x - x1) * cos(phi0) + (BS_init.y - y1) * sin(phi0)) * cos(phi0);
        double pca_y = y1 + ((BS_init.x - x1) * cos(phi0) + (BS_init.y - y1) * sin(phi0)) * sin(phi0);
        // std::cout << "PCA: (" << pca_x << " +- " << sqrt(sigma2_pca_x) << ", " << pca_y << " +- " << sqrt(sigma2_pca_y) << std::endl;

        double dca_origin = sqrt(pca_x * pca_x + pca_y * pca_y);
        double dzdr = (z2 - z1) / (r2 - r1);
        double z0 = z1 - dzdr * (r1 - dca_origin);

        double d0 = sqrt(pow(pca_x - BS_init.x, 2.) + pow(pca_y - BS_init.y, 2.));
        if (d0 > d0_cut)
            continue;
        double phi_pca = atan2(pca_y - BS_init.y, pca_x - BS_init.x);
        double oppositephi_pca = phi_pca + M_PI;
        if (oppositephi_pca > M_PI)
            oppositephi_pca -= 2 * M_PI;

        reco_BS.d0phi0dist->Fill(phi_pca, d0);
        reco_BS.d0phi0dist->Fill(oppositephi_pca, -d0);

        reco_BS.pcadist->Fill(pca_x, pca_y);
        reco_BS.z0dist->Fill(z0);

        Ntracklets++;
    }

    TF1 z_f("gaus", "gaus", -60., 60.);

    // Remove "background"
    reco_BS.d0phi0dist_bkgrm = (TH2F *)reco_BS.d0phi0dist->Clone("d0phi0dist_bkgrm");
    for (int i = 0; i < reco_BS.d0phi0dist->GetNbinsX(); i++)
    {
        TH1D *slice = reco_BS.d0phi0dist->ProjectionY("slice", i, i + 1);
        int maxbin = slice->GetMaximumBin();
        float max = slice->GetBinContent(maxbin);
        for (int j = 0; j < slice->GetNbinsX(); j++)
        {
            float content = slice->GetBinContent(j + 1);
            if (slice->GetBinContent(j + 1) < 0.995 * max)
                reco_BS.d0phi0dist_bkgrm->SetBinContent(i + 1, j + 1, 0.);
        }
    }

    reco_BS.d0phi0dist_bkgrm_prof = reco_BS.d0phi0dist_bkgrm->ProfileX("d0phi0dist_bkgrm_prof");

    TGraphErrors *g = new TGraphErrors();
    for (int i = 0; i < reco_BS.d0phi0dist_bkgrm_prof->GetNbinsX(); i++)
    {
        double d0 = reco_BS.d0phi0dist_bkgrm_prof->GetBinContent(i + 1);
        double phi = -M_PI + i * (2. * M_PI) / 200.;
        g->SetPoint(i, phi, d0);
    }

    TF1 d0cos("d0cos", "[0]*TMath::Cos(x-[1])", -M_PI, M_PI);
    g->Fit(&d0cos);
    g->Write("maxgraph_prermoutl");

    vector<float> phi_2;
    vector<float> d0_2;
    TF1 d0cos_rmoutl("d0cos_rmoutl", "[0]*TMath::Cos(x-[1])", -M_PI, M_PI);
    for (int i = 0; i < g->GetN(); i++)
    {
        double x, y;
        g->GetPoint(i, x, y);
        double y_fit = d0cos.Eval(x);
        // cout << "x: " << x << " y: " << y << " y_fit: " << y_fit << "abs(y-y_fit): " << fabs(y - y_fit) << endl;
        if (fabs(y - y_fit) < 0.2)
        {
            phi_2.push_back(x);
            d0_2.push_back(y);
        }
    }

    TGraphErrors *g2 = new TGraphErrors(phi_2.size(), &phi_2[0], &d0_2[0]);
    // fit the graph again
    g2->Fit(&d0cos_rmoutl);
    g2->Write("maxgraph_rmoutl");

    reco_BS.z0dist->Fit(&z_f);

    // build reconstructed beamspot: x and y first
    double BS_d0 = d0cos_rmoutl.GetParameter(0);
    double BS_phi0 = d0cos_rmoutl.GetParameter(1);
    double BS_sigmad0 = d0cos_rmoutl.GetParError(0);
    double BS_sigmaphi0 = d0cos_rmoutl.GetParError(1);

    reco_BS.x = BS_d0 * cos(BS_phi0) + BS_init.x;
    reco_BS.y = BS_d0 * sin(BS_phi0) + BS_init.y;
    reco_BS.sigma_x = sqrt(pow(cos(BS_phi0), 2.) * pow(BS_sigmad0, 2.) + pow(BS_d0 * sin(BS_phi0), 2.) * pow(BS_sigmaphi0, 2.));
    reco_BS.sigma_y = sqrt(pow(sin(BS_phi0), 2.) * pow(BS_sigmad0, 2.) + pow(BS_d0 * cos(BS_phi0), 2.) * pow(BS_sigmaphi0, 2.));

    reco_BS.Ntracklets = Ntracklets;

    reco_BS.z = z_f.GetParameter("Mean");
    reco_BS.sigma_z = z_f.GetParameter("Sigma");

    return reco_BS;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cout << "Usage: ./BeamspotReco [infile] [outfile] [dphi_cut]" << std::endl;
        return 0;
    }

    std::string infile = argv[1];
    std::string outfile = argv[2];
    double dphi_cut = atof(argv[3]);

    TFile *inf = TFile::Open(infile.c_str());
    TTree *events = (TTree *)inf->Get("EventTree");

    TFile *outf = new TFile(outfile.c_str(), "RECREATE");
    TTree *t = new TTree("reco_beamspot", "reco_beamspot");

    uint64_t intt_bco;
    int Nclusters;
    std::vector<double> *clusters_x = 0;
    std::vector<double> *clusters_y = 0;
    std::vector<double> *clusters_z = 0;
    std::vector<int> *clusters_layer = 0;
    std::vector<int> *clusters_phisize = 0;
    std::vector<unsigned int> *clusters_adc = 0;
    bool InttBco_IsToBeRemoved;

    events->SetBranchAddress("INTT_BCO", &intt_bco);
    if (events->GetListOfBranches()->FindObject("InttBco_IsToBeRemoved"))
    {
        events->SetBranchAddress("InttBco_IsToBeRemoved", &InttBco_IsToBeRemoved);
    }
    else
    {
        InttBco_IsToBeRemoved = false;
    }
    events->SetBranchAddress("NClus", &Nclusters);
    events->SetBranchAddress("ClusX", &clusters_x);
    events->SetBranchAddress("ClusY", &clusters_y);
    events->SetBranchAddress("ClusZ", &clusters_z);
    events->SetBranchAddress("ClusLayer", &clusters_layer);
    events->SetBranchAddress("ClusPhiSize", &clusters_phisize);
    events->SetBranchAddress("ClusAdc", &clusters_adc);

    std::vector<std::pair<std::shared_ptr<Hit>, std::shared_ptr<Hit>>> tracklets;
    std::vector<uint64_t> bcos;

    // assemble tracklets that pass dphi cut
    for (int i = 0; i < events->GetEntries(); i++)
    {
        std::vector<std::shared_ptr<Hit>> layer1_clusters;
        std::vector<std::shared_ptr<Hit>> layer2_clusters;

        events->GetEntry(i);

        bcos.push_back(intt_bco);

        std::cout << "event " << i << " INTT_BCO " << intt_bco << std::endl;

        if (InttBco_IsToBeRemoved == true)
            continue;

        // use low-multiplicity events
        if (Nclusters < 20 || Nclusters > 350)
            continue;

        for (int j = 0; j < Nclusters; j++)
        {
            if (clusters_adc->at(j) <= 35)
                continue;

            if (clusters_layer->at(j) == 3 || clusters_layer->at(j) == 4)
            {
                layer1_clusters.push_back(std::make_shared<Hit>(clusters_x->at(j), clusters_y->at(j), clusters_z->at(j), 0., 0., 0., 0, clusters_phisize->at(j)));
            }
            if (clusters_layer->at(j) == 5 || clusters_layer->at(j) == 6)
            {
                layer2_clusters.push_back(std::make_shared<Hit>(clusters_x->at(j), clusters_y->at(j), clusters_z->at(j), 0., 0., 0., 0, clusters_phisize->at(j)));
            }
        }

        if (layer1_clusters.size() < 10 || layer2_clusters.size() < 10)
            continue;

        for (auto &cluster1 : layer1_clusters)
        {
            for (auto &cluster2 : layer2_clusters)
            {
                // if (fabs(cluster1->Phi() - cluster2->Phi()) < dphi_cut && fabs(cluster1->posZ() - cluster2->posZ()) < 5.) // bug in calculating delta phi (?);
                if (deltaPhi(cluster1->Phi(), cluster2->Phi()) < dphi_cut)
                {
                    tracklets.push_back(std::make_pair(cluster1, cluster2));
                }
            }
        }
    }

    Beamspot origin;
    // fit_PCAD0Phi0(std::vector<std::pair<std::shared_ptr<Hit>, std::shared_ptr<Hit>>> tracklets, Beamspot BS_init, double d0_cut)
    // Beamspot BS = fit_PCAD0Phi0(tracklets, origin, 100.);
    Beamspot BS = fit_PCAD0Phi0(tracklets, origin, 1.);
    // BS = fit_PCAD0Phi0(tracklets,BS,0.5);
    // BS = fit_PCAD0Phi0(tracklets,BS,0.2);
    // BS = fit_PCAD0Phi0(tracklets,BS,0.1);

    // Get the minimum, median, and maximum values of bcos
    std::sort(bcos.begin(), bcos.end());
    uint64_t min_bco = bcos.front();
    uint64_t median_bco = (bcos.size() % 2 == 0) ? (bcos[bcos.size() / 2] + bcos[bcos.size() / 2 - 1]) / 2 : bcos[bcos.size() / 2];
    uint64_t max_bco = bcos.back();

    t->Branch("x", &BS.x);
    t->Branch("y", &BS.y);
    t->Branch("z", &BS.z);
    t->Branch("sigma_x", &BS.sigma_x);
    t->Branch("sigma_y", &BS.sigma_y);
    t->Branch("sigma_z", &BS.sigma_z);
    t->Branch("xy_correlation", &BS.xy_correlation);
    t->Branch("Ntracklets", &BS.Ntracklets);
    t->Branch("intt_bco_min", &min_bco);
    t->Branch("intt_bco_median", &median_bco);
    t->Branch("intt_bco_max", &max_bco);

    t->Fill();
    t->Write();

    BS.d0phi0dist->Write("d0phi0dist", TObject::kOverwrite);
    BS.d0phi0dist_bkgrm->Write("d0phi0dist_bkgrm", TObject::kOverwrite);
    BS.d0phi0dist_bkgrm_prof->Write("d0phi0dist_bkgrm_prof", TObject::kOverwrite);
    BS.z0dist->Write("z0dist", TObject::kOverwrite);
    BS.pcadist->Write("pcadist", TObject::kOverwrite);

    BS.identify();

    outf->Close();

    return 0;
}
