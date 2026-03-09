#ifndef noiDict__h
#define noiDict__h
// David Stewart, Dec 1 2022
// A way to pass 
// used in conveinece for plotting
struct noiWordIn {
    string val;
    noiWordIn (const char* _) { ostringstream inp; inp << _;  val = inp.str(); };
    noiWordIn (string      _) { ostringstream inp; inp << _;  val = inp.str(); };
    noiWordIn (int         _) { ostringstream inp; inp << _;  val = inp.str(); };
    noiWordIn (double      _) { ostringstream inp; inp << _;  val = inp.str(); };
};
struct noiWordOut {
    string val;
    noiWordOut(string _) : val{_} {};
    operator string () { return val; };
    /* operator int    () { istringstream iss (val); int    rval; iss >> rval; return rval; }; */
    /* operator short  () { istringstream iss (val); short  rval; iss >> rval; return rval; }; */
    operator double () { istringstream iss (val); double rval; iss >> rval; return rval; };
    operator const char* () { return val.c_str(); };
};
struct noiDict {
    /* ostringstream options; */
    vector<string> data {};
    noiDict (vector<noiWordIn> words) {
        for (auto W : words) data.push_back(W.val);
    };
    noiDict() {};
    bool operator[](string key) { return find(data.begin(),data.end(),key) != data.end(); };

    noiWordOut operator()(string key, int offset=1) {
        // default to one-past the key
        auto iter = find(data.begin(),data.end(),key);
        if (iter == data.end()) {
            throw runtime_error(Form("Failed to find key \"%s\" in noiDict",key.c_str()));
            return {""};
        }
        int i_iter = iter-data.begin();
        if (i_iter+offset >= data.size()) {
            throw runtime_error(Form("Failed to find value for key \"%s\" offset by %i in noiDict",key.c_str(),offset));
            return {""};
        }
        return {*(iter+offset)};
    }
    noiDict& operator += (noiDict& _) {
        data.insert(data.begin(),_.data.begin(),_.data.end());
        return *this;
    }
};
ostream& operator<< (ostream& os, const noiDict& opt) {
   for (auto& val : opt.data) cout << val << " "; cout << endl;
   return os;
};
#endif
