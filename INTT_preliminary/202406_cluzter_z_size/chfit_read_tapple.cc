#include <iostream>
#include <random>

TH1D* nCp(const std::vector<Double_t>& pro_list,int Event);
int chip_reaction(Double_t probability);

int chfit_read_tapple()
{

// TFile *file = TFile::Open("/sphenix/u/tomoya/work/24.05/own_dst_ana/macro/data/all/test602_v14_41620_all.root");//test602_v6_43409.root");
TFile *file = TFile::Open("/sphenix/u/tomoya/work/24.05/own_dst_ana/macro/data/draw_macor/result/out20869_all_v2.root");

// TFile *file = TFile::Open("/sphenix/u/tomoya/work/24.05/own_dst_ana/macro/data/test602_v4.root"); 
TH1D* hist = (TH1D*)file->Get("hist_z_layyer_0_ladder_0");

 TH1D* hist_2 = (TH1D*)file->Get("hist_Zsize_Scale_layyer_0_ladder_0");

  if (!hist)
  {
    std::cerr << "Unable to get ntuple" << std::endl;
    file->Close();
     return 1;
  }
 
int num_chip =0;
std::vector<Double_t>pro_list;
 for(int i=0 ;i <401;i++)
{

  Double_t entry=hist->GetBinContent(i);
  if(entry>300000)
  {

  cout<<"entry="<<entry<<endl;
  Double_t  pro = entry/550108/128;
pro_list.push_back(pro);

   cout<<"pro="<<pro<<endl;
  }
}


 TCanvas *c1 = new TCanvas("c1", "Canvas Example", 800, 600);
//  c1->Divide(3,2);
//  c1->cd(1);
// hist_2->Scale(1/5);
  hist_2->SetLineColor(kRed);
  hist_2->Draw("HIST");
  
 TFile *td_out = new TFile("G609Ghypo_run20869_100000.root","RECREATE") ;
//  cout<<"write"<<endl;
//  histZ_all_layer->Write(); 
//  outfile->Close();

std::vector<Double_t>pro_list_1(26,0.008);

TH1D* hist_m = nCp(pro_list_1,100000);
hist_m->SetLineColor(kGreen);
td_out->WriteTObject( hist_m, "hist_m" );
hist_m->Draw("SAME");

//td_out->WriteTObject( hist_m, "hist_m" );
hist_m->Scale(1.0/100000);

td_out->WriteTObject( hist_m, "hist_m_scale" );
td_out->WriteTObject( hist_2, "hist_scale_Zsize_ladder0_layyer0" ); 
  
 
return 0;
}


int chip_reaction(double probability)
{

        std::random_device seed_gen;
        std::default_random_engine engine(seed_gen());

        std::binomial_distribution<> dist(1, probability);

        int result = dist(engine);

return result;
}



TH1D* nCp(const std::vector<Double_t>& pro_list, int Event)
{
    std::vector<int> chip_list;
    std::vector<int> name_fire_chip;
    TH1D* hist_hypo = new TH1D("hist_hypo", "hist_hypo", 25, 1, 26);

    for (int i = 0; i < Event; i++)
    {
//        cout<<"i="<<i<<endl;
        int b_first = 0;
        int b_count = 0;
        std::vector<std::vector<float>> name_fire_channel(28);

        chip_list.clear();
        name_fire_chip.clear();

        for (int chip = 0; chip < 26; chip++)
        {
            for (int chp = 0; chp < 128; chp++)
            {
                int b = chip_reaction(pro_list[chip]);
//		cout<<"chip"<<chip<<"chp"<<chp<<"b"<<b<<endl;
                chip_list.push_back(b);
                if (b == 1)
                {
                  //  cout<<"chip"<<chip<<"chp"<<chp<<"b"<<b<<endl;
                    b_first = b;
                    b_count++;
                    name_fire_channel[chip].push_back(chp);
                }
            }

            if (b_first == 1)
            {
                name_fire_chip.push_back(chip);
                b_first = 0;
            }
        }

int consecutiveCount = 1;
        int time_fill = 0;

        for (int t = 0; t < name_fire_chip.size() ; t++)
        {
            bool shouldBreak = false;

            if (name_fire_chip[t] == name_fire_chip[t + 1] - 1)
            {
                for (size_t l = 0; l < name_fire_channel[name_fire_chip[t]].size(); l++)
                {
                    for (size_t p = 0; p < name_fire_channel[name_fire_chip[t + 1]].size(); p++)
                    {
                        if (name_fire_channel[name_fire_chip[t]][l] == name_fire_channel[name_fire_chip[t + 1]][p] ||
                            name_fire_channel[name_fire_chip[t]][l] - 1 == name_fire_channel[name_fire_chip[t + 1]][p] ||
                            name_fire_channel[name_fire_chip[t]][l] + 1 == name_fire_channel[name_fire_chip[t + 1]][p])
                        {
                            consecutiveCount++;
                            shouldBreak = true;
                            break;
                        }
                        else if (name_fire_chip[t] == name_fire_chip[t - 1] + 1 && consecutiveCount > 1)
                        {
                            hist_hypo->Fill(consecutiveCount);
                    //        cout<<"consecutiveCount"<<consecutiveCount<<endl;
                            time_fill += consecutiveCount;
                            b_count -= consecutiveCount;
                            consecutiveCount = 1;
                            shouldBreak = true;
                            break;
                        }
                    }
                    if (shouldBreak) break;
                }
            }
            else if (t==name_fire_chip.size()-1)
            {
               hist_hypo->Fill(consecutiveCount);
//     cout<<"consecutiveCount_2"<<consecutiveCount<<endl;

               time_fill= time_fill+consecutiveCount;
               consecutiveCount =1;
            }
            else
            {
                       hist_hypo->Fill(1, name_fire_channel[name_fire_chip[t]].size());
  //                cout<<"consecutiveCount_3,1"<<endl;

                time_fill += name_fire_channel[name_fire_chip[t]].size();
            }
        }
    }
    return hist_hypo;
}
 
