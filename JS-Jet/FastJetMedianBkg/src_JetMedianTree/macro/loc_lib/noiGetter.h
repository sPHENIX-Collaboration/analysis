#ifndef noiGetter__h
#define noiGetter__h
struct noiGetter{
    map<string, vector<TObject*>> got {};
    map <string, TFile*> files    {};
    int n_objects {0};
    noiGetter(){};

    TFile* get_file(string f_name) {
        if (f_name.find(".root") == string::npos) f_name += ".root";
        TFile *f = ( files.count(f_name) ? files[f_name] : new TFile(f_name.c_str(), "read") );
        if (!f->IsOpen()) {
            cout << " Fatal error, cannot open file: " << f_name << endl;
            exit (1);
        }
        return f;
    };

    TObject* operator()(string f_name, string object_name) {
        TFile *s_current = gDirectory->GetFile();
        TFile *f = get_file(f_name);
        TObject* obj;
        f->GetObject(object_name.c_str(), obj);
        if (obj == nullptr) {
            cout << " !fatal: failed to get object " << object_name 
                << " from file " << f_name << endl;
            exit(2);
        }
        ++n_objects;
        /* obj->SetName(unique_name()); */
        if (got.count(object_name) != 0) got[object_name].push_back(obj);
        else got[object_name].push_back({obj});

        if (s_current!=nullptr) s_current->cd();
        return obj;
    };

};
#endif
