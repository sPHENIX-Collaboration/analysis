#include "sPhenixStyle.C"


int basic(int num ,int run_number )
{
//SetsPhenixStyle();
//TFile *file = TFile::Open("../test602_v11_20869_all.root");
string File_name= Form("../test602_v%d_%d_all.root",num,run_number);

string outfile= Form("result/out%d_all_v2_67.root",run_number);

TFile *file = TFile::Open(File_name.c_str());
TNtuple* tapple = (TNtuple*)file->Get("tapple");

TFile* tf_out = new TFile( outfile.c_str(), "RECREATE" ) ;


TH1D* hist = new TH1D( "hist", "", 120, -12, 12 );

TH1D* hist_Zsize_all = new TH1D( "hist_Zsize_all", "Z size", 9, 1, 10 );
tapple->Draw("Z_size>>hist_Zsize_all", "" );
tf_out->WriteTObject( hist_Zsize_all, "hist_Zsize_all" );

TH1D* hist_PHYsize = new TH1D( "hist_PHYsize", "PHY size", 128, 1, 129 );
tapple->Draw("PHY_size>>hist", "" );
tf_out->WriteTObject( hist_PHYsize, "hist_PHYsize" );

TH1D* hist_X_all = new TH1D( "hist_X_all", "x position", 120, -12, 12 );
tapple->Draw("x>>hist_X_all", "" );
tf_out->WriteTObject( hist_X_all, "hist_X_all" );

TH1D* hist_Y_all = new TH1D( "hist_Y_all", "y position", 120, -12, 12 );
tapple->Draw("y>>hist_Y_all", "" );
tf_out->WriteTObject( hist_Y_all, "hist_Y_all" );


TH1D* hist_Z_all = new TH1D( "hist_Z_all", "Z position", 200, -25, 25 );
tapple->Draw("z>>hist_Z_all", "" );
tf_out->WriteTObject( hist_Z_all, "hist_Z_all" );


Double_t event = tapple->GetMaximum("ievt");

TH1D* hist_Zsize_all_scale = new TH1D( "hist_Zsize_all_scale", "Z size", 9, 1, 10 );
tapple->Draw("Z_size>>hist_Zsize_all_scale", "" );
hist_Zsize_all_scale->Scale(1/event);
tf_out->WriteTObject( hist_Zsize_all_scale, "hist_Zsize_all_scale" );
hist_Zsize_all_scale->Draw("HIST");

for(int i=1;i<9;i++)
{

auto hist_name = Form("histx_inner_Zsize_%d", i);
TH1D* hist = new TH1D(hist_name, "xposition", 120, -12, 12);
cout<<"i="<<i<<",hist_name="<<hist_name<<endl;
TString draw_command = Form("x>>%s", hist_name);
TString condition = Form("Z_size==%d && (layer==1 || layer==0)", i);
tapple->Draw(draw_command, condition, "");

auto hist_nameY = Form("histy_inner_Zsize_%d", i);
TH1D* histY = new TH1D(hist_nameY, "yposition", 120, -12, 12);
cout<<"i="<<i<<",hist_name="<<hist_name<<endl;
TString draw_commandY = Form("y>>%s", hist_nameY);
TString conditionY = Form("Z_size==%d && (layer==1 || layer==0)", i);
tapple->Draw(draw_commandY, conditionY, "");


tf_out->WriteTObject( hist, hist_name );

tf_out->WriteTObject( histY, hist_nameY );




}

for(int ladder = 0 ;ladder < 3; ladder++)
{

auto hist_name_Zsize = Form("hist_Zsize_layyer_0_ladder_%d", ladder);
TH1D* hist_Zsize = new TH1D(hist_name_Zsize, "Zsize", 9, 1, 10);
TString draw_command_Zsize = Form("Z_size>>%s", hist_name_Zsize);
TString condition_Zsize = Form("ladder==%d && layer==0", ladder);
tapple->Draw(draw_command_Zsize, condition_Zsize, "");

tf_out->WriteTObject( hist_Zsize, hist_name_Zsize );


auto hist_name_z = Form("hist_z_layyer_0_ladder_%d", ladder);
TH1D* hist_z = new TH1D(hist_name_z, "z", 200, -25, 25);
TString draw_command_z = Form("z>>%s", hist_name_z);
TString condition_z = Form("ladder==%d && layer==0", ladder);
tapple->Draw(draw_command_z, condition_z, "");
cout<<"ladder="<<ladder<<"comand"<<draw_command_z<<","<<condition_z<<endl;
tf_out->WriteTObject( hist_z, hist_name_z );


hist_Zsize->Scale(1/event);
auto hist_name_Zsize_Scale = Form("hist_Zsize_Scale_layyer_0_ladder_%d", ladder);
tf_out->WriteTObject( hist_Zsize, hist_name_Zsize_Scale );


}



tf_out->Close() ;
file->Close();
return 0;

}
