// Write "run segment entries path" for every outputCollect_<run>_<seg>.root in a Collect directory,
// sorted by run then segment (numeric). Called by make_lists.bash; output feeds make_lists.py.
void count_entries(const char* dir, const char* out){
	TSystemDirectory d("d",dir);
	TList* files = d.GetListOfFiles();
	std::vector<std::tuple<int,int,TString>> v;
	TIter next(files); TSystemFile* f;
	while ((f=(TSystemFile*)next())){
		TString n=f->GetName();
		if (!n.BeginsWith("outputCollect_") || !n.EndsWith(".root")) continue;
		int run=0, seg=0;
		if (sscanf(n.Data(),"outputCollect_%d_%d.root",&run,&seg)!=2) continue;
		v.push_back(std::make_tuple(run,seg,TString(dir)+"/"+n));
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
