#ifndef noiMsgTree__h
#define noiMsgTree__h
class noiMsgTree {
    private:
        string b_msg;
        TTree tree;
    public:
        bool echo_to_cout {false};
        noiMsgTree(bool set_echo) : 
            b_msg{""}, 
            tree{"Messages", "Tree of messages"}
        {
            tree.Branch("messages", &b_msg);
            dash();
            msg(" Start of msg tree ");
            dash();
            echo_to_cout = set_echo;
        };
        void msg(string msg) {
            b_msg = msg;
            if (echo_to_cout) cout << b_msg << endl;
            tree.Fill();
        };
        void msg(vector<string> messages) {
            for (auto& msg : messages) {
                b_msg = msg;
                if (echo_to_cout) cout << b_msg << endl;
                tree.Fill();
            }
        };
        void dash() {
            b_msg = "---------------";
            if (echo_to_cout) cout << b_msg << endl;
            tree.Fill();
        };
        void write(){
            tree.Write();
        };
        static void read_messages(const char* f_name){
            cout << " Reading file: " << f_name << endl;
            /* TTree *tree; */
            TFile* fin  = new TFile(f_name, "read");
            if (!fin->IsOpen()) {
                cout << " Input file: " << f_name << " is not open." << endl;
                delete fin;
                return;
            }
            TTreeReader myReader("Messages",fin);
            TTreeReaderValue<string> msg(myReader, "messages");
            cout << "  Contents of TFile(\""<<f_name<<"\") TTree(\"Messages\"):" << endl;
            while (myReader.Next()) cout << " " << *msg << endl;
            fin->Close();
            delete fin;
            /* } */
    };
    void slurp_file(const char* which_file) {
        // try and read all lines of which_file into the tree
        msg(Form("--Begin contents of file \"%s\"",which_file));
        ifstream f_in {which_file};
        if (!f_in.is_open()) {
            msg(Form("  error: couldn't open file \"%s\"",which_file));
        } else {
            string line;
            while (getline(f_in,line)) msg(line);
        }
        msg(Form("--End contents of file \"%s\"",which_file));
        return;
    };
};
#endif

