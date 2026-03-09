struct chan_info{
    int pid;
    int module;
    int chip;
    int chan;
    // int entry;
};

// note : .first=dead channel. .second=hot channel
pair<vector<chan_info>,vector<chan_info>> bad_chan_finder(string folder_direction, string file_name, double hot_ch_cut, int iteration = 2)
{
    TFile * file_in = TFile::Open(Form("%s/%s.root",folder_direction.c_str(),file_name.c_str()));
    TTree * tree = (TTree *)file_in->Get("tree");
    long long N_event = tree -> GetEntries();
    cout<<Form("N_event in file %s : %lli",file_name.c_str(), N_event)<<endl;

    vector<chan_info> dead_ch_vec; dead_ch_vec.clear();
    vector<chan_info> hot_ch_vec; hot_ch_vec.clear();

    for ( int pid_i = 3001; pid_i < 3009; pid_i++ ) // note : from 3001 to 3008
    {
        for ( int module_i = 0; module_i < 14; module_i++ ) // note  from 0 to 13
        {
            // todo : it may be just a test, adc is not used
            for ( int chip_i = 1; chip_i < 27; chip_i++ ) // note : chip 1 to chip 26 
            {
                TH1F * chan_hist = new TH1F("chan_hist","chan_hist",128,0,128);
                tree->Draw("chan_id>>chan_hist", Form("pid == %i && module == %i && chip_id == %i", pid_i, module_i, chip_i));

                // note : find the dead channel
                for (int ele_i = 0; ele_i < 128; ele_i++){
                    if (chan_hist -> GetBinContent(ele_i + 1) == 0){
                        dead_ch_vec.push_back( {pid_i, module_i, chip_i, ele_i} );
                    }
                }

                // note : scan iteration
                // note : find the hot channel
                for ( int ite = 0; ite < iteration; ite++ ){
                    TH1F * chan_hist_nor = (TH1F*)chan_hist -> Clone();
                    chan_hist_nor -> Scale( 1. / chan_hist_nor -> Integral(-1,-1) );
                    
                    for (int ele_i = 0; ele_i < 128; ele_i++){
                        if (chan_hist_nor -> GetBinContent(ele_i + 1) > hot_ch_cut){
                            hot_ch_vec.push_back( {pid_i, module_i, chip_i, ele_i} );
                            chan_hist -> SetBinContent(ele_i + 1, 0);
                        }
                    }

                } 
            } // note : chip
        } // note : module 
    } // note : pid

    return {dead_ch_vec, hot_ch_vec};
    
}

void hot_dead_chan_finder()
{

}