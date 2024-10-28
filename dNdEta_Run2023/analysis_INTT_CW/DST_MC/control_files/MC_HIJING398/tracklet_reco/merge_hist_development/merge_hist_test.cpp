#include <iostream>
#include <fstream>
using namespace std;

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TKey.h>
#include <TObject.h>
#include <THStack.h>

vector<string> read_list(string input_txt_file_directory)
{
    string list_buffer;
    ifstream data_list;
	data_list.open((input_txt_file_directory).c_str());

    vector<string> output_vec; output_vec.clear();

	while (1)
	{
		data_list >> list_buffer;
		if (!data_list.good())
		{
			break;
		}
		output_vec.push_back(list_buffer);
	}
	cout<<"size : "<<output_vec.size()<<endl;

    return output_vec;
}

void TH1F_fake_add(TH1F * hist_base, TH1F * hist_new)
{
    if (
        hist_base -> GetNbinsX() != hist_new -> GetNbinsX() || 
        hist_base -> GetXaxis() -> GetXmin() != hist_new -> GetXaxis() -> GetXmin() || 
        hist_base -> GetXaxis() -> GetXmax() != hist_new -> GetXaxis() -> GetXmax())
    {
        cout<<"TH1F_fake_add : the two histograms have different bin numbers!"<<endl;
        return;
    }

    for (int i = 1; i < hist_base -> GetNbinsX() + 1; i++)
    {
        hist_base -> SetBinContent(i, hist_base -> GetBinContent(i) + hist_new -> GetBinContent(i));
    }
}

void TH2F_fake_add(TH2F * hist_base, TH2F * hist_new)
{
    if (
        hist_base -> GetNbinsX() != hist_new -> GetNbinsX() || 
        hist_base -> GetXaxis() -> GetXmin() != hist_new -> GetXaxis() -> GetXmin() || 
        hist_base -> GetXaxis() -> GetXmax() != hist_new -> GetXaxis() -> GetXmax() || 
        hist_base -> GetNbinsY() != hist_new -> GetNbinsY() || 
        hist_base -> GetYaxis() -> GetXmin() != hist_new -> GetYaxis() -> GetXmin() || 
        hist_base -> GetYaxis() -> GetXmax() != hist_new -> GetYaxis() -> GetXmax())
    {
        cout<<"TH2F_fake_add : the two histograms have different bin numbers!"<<endl;
        return;
    }

    for (int i = 1; i < hist_base -> GetNbinsX() + 1; i++)
    {
        for (int j = 1; j < hist_base -> GetNbinsY() + 1; j++)
        {
            hist_base -> SetBinContent(i, j, hist_base -> GetBinContent(i, j) + hist_new -> GetBinContent(i, j));
        }
    }
}


int main()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/TrackletReco_condor";
    string input_file_list = "file_list.txt";
    string output_directory = input_directory + "/merged_file";

    vector<string> file_list = read_list((input_directory + "/" + input_file_list).c_str());
    map<string, TFile*> TFile_in_map; TFile_in_map.clear();
    map<string, TH1F*> TH1F_map; TH1F_map.clear();
    map<string, TH2F*> TH2F_map; TH2F_map.clear();

    int file_counting = 0;

    for (auto file_name : file_list)
    {
        cout<<"running the file : "<<file_name.substr(input_directory.size(),file_name.size())<<endl;
        TFile_in_map[file_name.c_str()] = TFile::Open(Form("%s", file_name.c_str()));

        for (TObject* keyAsObj : *TFile_in_map[file_name.c_str()]->GetListOfKeys())
        {
            
            auto key = dynamic_cast<TKey*>(keyAsObj);
            string hist_name  = key->GetName();
            string class_name = key->GetClassName();

            if (class_name == "TH1F")
            {
                if (TH1F_map.find(hist_name) == TH1F_map.end())
                { 
                    TH1F_map[hist_name.c_str()] = new TH1F(
                        (hist_name + "_merge").c_str(), (hist_name+"_merge").c_str(),
                        ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetNbinsX(),
                        ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetXaxis()->GetXmin(),
                        ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetXaxis()->GetXmax()
                    );
                    
                    TH1F_fake_add(TH1F_map[hist_name.c_str()], ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() )));
                }
                else
                { 
                    TH1F_fake_add(TH1F_map[hist_name.c_str()], ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))); 
                }
            } 
            else if (class_name == "TH2F")
            {
                if (TH2F_map.find(hist_name) == TH2F_map.end())
                { 
                    TH2F_map[hist_name.c_str()] = new TH2F(
                        (hist_name + "_merge").c_str(), (hist_name+"_merge").c_str(),
                        ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetNbinsX(),
                        ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetXaxis()->GetXmin(),
                        ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetXaxis()->GetXmax(),
                        ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetNbinsY(),
                        ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetYaxis()->GetXmin(),
                        ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetYaxis()->GetXmax()
                    );

                    TH2F_fake_add(TH2F_map[key->GetName()], ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() )) );
                }
                else
                { 
                    TH2F_fake_add(TH2F_map[key->GetName()], ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() )) ); 
                }
            }
            else 
            {
                cout<<"weird : "<<key->GetClassName()<<endl;
            }

        }

        cout<<"test : "<<((TH2F*) TFile_in_map[file_name.c_str()] -> Get("Eta_Z_reference")) -> GetBinContent(18,8)<<endl;

        // file_counting += 1;
        // if (file_counting == 2) {break;}
    }
    

    TFile * file_out = new TFile(Form("%s/merged_hist.root", output_directory.c_str()), "RECREATE");
    file_out -> cd();
    for (pair<string,TH1F*> hist : TH1F_map)
    {
        hist.second -> Write((hist.first).c_str(), TObject::kOverwrite);
    }

    for (pair<string,TH2F*> hist : TH2F_map)
    {
        hist.second -> Write((hist.first).c_str(), TObject::kOverwrite);
    }
    file_out -> Close();

    cout<<"the merged file is closed"<<endl;

    for (pair<string, TFile*> file : TFile_in_map)
    {
        file.second -> Close();
    }



    // for (auto file_name : file_list)
    // {
    //     cout<<"running the file : "<<file_name.substr(input_directory.size(),file_name.size())<<endl;
    //     TFile_in_map[file_name.c_str()] = TFile::Open(Form("%s", file_name.c_str()));

    //     cout<<"aaaa in preparation : "<<((TH2F *) TFile_in_map[file_name.c_str()] -> Get( "Eta_Z_reference" )) -> GetBinContent(18,8)<<endl;

    //     for (TObject* keyAsObj : *TFile_in_map[file_name.c_str()]->GetListOfKeys()){
    //         auto key = dynamic_cast<TKey*>(keyAsObj);
    //         string hist_name = key->GetName();

    //         cout<<"bbb in preparation : "<<((TH2F *) TFile_in_map[file_name.c_str()] -> Get( "Eta_Z_reference" )) -> GetBinContent(18,8)<<" and key->GetName() : "<<hist_name<<endl;
    //         // cout<<"ccc test : "<<          ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() ))->GetNbinsX()<<endl;

    //         // note : for the TH1F
    //         if (std::string(key->GetClassName()) == "TH1F")
    //         {
    //             if (TH1F_map.find(key->GetName()) == TH1F_map.end())
    //             { 
    //                 TH1F_map[std::string(key->GetName())] = new TH1F(
    //                     key->GetName(),key->GetName(),
    //                     ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() ))->GetNbinsX(),
    //                     ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() ))->GetXaxis()->GetXmin(),
    //                     ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() ))->GetXaxis()->GetXmax()
    //                 );
                    
    //                 TH1F_fake_add(TH1F_map[std::string(key->GetName())], ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() )));
                    
    //                 // TH1F_map[std::string(key->GetName())] -> Add((TH1F*)key->ReadObj());
    //                 // temp_hist = (TH1F*)key->ReadObj();
    //                 // TH1F_map[std::string(key->GetName())] -> Add( (TH1F *)((TH1F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() ))->Clone() );
    //                 // cout<<"in loop test : "<<TH1F_map[std::string(key->GetName())] -> GetNhists()<<endl;
    //                 // cout<<"test here "<<endl;
    //             }
    //             else { TH1F_fake_add(TH1F_map[std::string(key->GetName())], ((TH1F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() ))); }
    //             // cout<<"test : file name : "<<key->GetName()<<" "<<key->GetClassName()<<" Entry : "<<((TH1F*)key->ReadObj())->GetEntries()<<endl;
    //             // cout<<"bro, too slow!"<<endl;
    //         }
    //         else if (std::string(key->GetClassName()) == "TH2F") // note : for the TH2F
    //         {
    //             if (TH2F_map.find(hist_name) == TH2F_map.end())
    //             { 
    //                 TH2F_map[hist_name] = new TH2F(
    //                     "", "",
    //                     ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetNbinsX(),
    //                     ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetXaxis()->GetXmin(),
    //                     ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetXaxis()->GetXmax(),
    //                     ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetNbinsY(),
    //                     ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetYaxis()->GetXmin(),
    //                     ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( hist_name.c_str() ))->GetYaxis()->GetXmax()
    //                 );

    //                 if (std::string(key->GetName()) == "Eta_Z_reference") {cout<<" in preparation : "<<((TH2F *) TFile_in_map[file_name.c_str()] -> Get( "Eta_Z_reference" )) -> GetBinContent(18,8)<<endl;}
    //                 // TH2F_fake_add(TH2F_map[key->GetName()], ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() )) );
    //                 if (std::string(key->GetName()) == "Eta_Z_reference") {cout<<" in preparation : "<<((TH2F *) TFile_in_map[file_name.c_str()] -> Get( "Eta_Z_reference" )) -> GetBinContent(18,8)<<endl;}
    //             }
    //             // else { TH2F_fake_add(TH2F_map[key->GetName()], ((TH2F *) TFile_in_map[file_name.c_str()] -> Get( key->GetName() )) ); }
    //         }
    //         else 
    //         {
    //             cout<<"weird : "<<key->GetClassName()<<endl;
    //         }

    //         // std::cout << "Key name: " << key->GetName() << " Type:----" << key->GetClassName() <<"----"<< std::endl;
    //     }

    //     file_counting += 1;
    //     if (file_counting == 1) {break;}

    //     // file_in -> Close();

    // } // note : end of number of files

    // cout<<"ddd in preparation : "<<((TH2F *) TFile_in_map["/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/TrackletReco_condor/core_0/INTT_final_hist_info.root"] -> Get( "Eta_Z_reference" )) -> GetBinContent(18,8)<<endl;
    // cout<<"test 2 "<< TH2F_map["Eta_Z_reference"]->GetBinContent(18,8)<<endl;
    
    

    // return 0;
}
