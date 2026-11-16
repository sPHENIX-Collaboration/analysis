// Write "run segment entries path" for every outputCollect_<run>_<seg>.root in a Collect directory or in its
// run-number subfolders (<dir>/<run>/, the ana573 layout from 2026-10-08), sorted by run then segment (numeric).
// Called by make_lists.bash; output feeds make_lists.py.
// runs = optional comma-separated run numbers (e.g. "81584,81585,81586"); empty = every run in dir.
void count_entries(const char* dir, const char* out, const char* runs=""){
	TString rsel	= TString(",")+runs+",";
	std::vector<std::tuple<int,int,TString>> v;
	std::vector<TString> dirs	= { dir };
	{
		TSystemDirectory d("d",dir); TList* l = d.GetListOfFiles();
		TIter nx(l); TSystemFile* f;
		while ((f=(TSystemFile*)nx())){ TString n=f->GetName(); if (f->IsDirectory() && n.IsDigit()) dirs.push_back(TString(dir)+"/"+n); }
	}
	for (auto& dd : dirs){
		TSystemDirectory d("d",dd); TList* files = d.GetListOfFiles();
		if (!files) continue;
		TIter next(files); TSystemFile* f;
		while ((f=(TSystemFile*)next())){
			TString n=f->GetName();
			if (!n.BeginsWith("outputCollect_") || !n.EndsWith(".root")) continue;
			int run=0, seg=0;
			if (sscanf(n.Data(),"outputCollect_%d_%d.root",&run,&seg)!=2) continue;
			if (strlen(runs) && !rsel.Contains(Form(",%d,",run))) continue;
			v.push_back(std::make_tuple(run,seg,dd+"/"+n));
		}
	}
	std::sort(v.begin(),v.end());
	FILE* fo=fopen(out,"w");
	long tot=0; int nbad=0;
	for (auto& t : v){
		TFile* tf=TFile::Open(std::get<2>(t).Data());
		long ne=-1;
		if (tf && !tf->IsZombie()){ TTree* tr=(TTree*)tf->Get("outTree"); if (tr) ne=tr->GetEntries(); }
		fprintf(fo,"%d %d %ld %s\n",std::get<0>(t),std::get<1>(t),ne,std::get<2>(t).Data());
		if (ne>0) tot+=ne; else ++nbad;
		if (tf) tf->Close();
	}
	fclose(fo);
	printf("files=%zu total entries=%ld unreadable=%d\n",v.size(),tot,nbad);
}
