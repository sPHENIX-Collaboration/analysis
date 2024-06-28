void splitNtuple(TString inputfname = "./Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root", TString outputdir = "./Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey")
{
    system(Form("mkdir -p %s", outputdir.Data()));

    TFile *f = TFile::Open(inputfname.Data(), "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    Long64_t Nevent = t->GetEntries();
    Long64_t NeventPerFile = 1000;
    Long64_t Nfile = ceil(double(Nevent) / double(NeventPerFile));

    cout << "Nevent: " << Nevent << ", Nfile: " << Nfile << endl;

    for (Long64_t i = 0; i < Nfile; i++)
    {
        TString outputfile = Form("%s/ntuple_%05d.root", outputdir.Data(), int(i));
        cout << "outputfile: " << outputfile << endl;
        
        TFile *fout = TFile::Open(outputfile.Data(), "recreate");
        TTree *tnew = t->CloneTree(0);
        for (Long64_t j = 0; j < NeventPerFile; j++)
        {
            Long64_t k = i * NeventPerFile + j;
            if (k >= Nevent)
            {
                break;
            }
            t->GetEntry(k);
            tnew->Fill();
        }
        tnew->Write("", TObject::kWriteDelete);
        fout->Close();
    }
    f->Close();
}