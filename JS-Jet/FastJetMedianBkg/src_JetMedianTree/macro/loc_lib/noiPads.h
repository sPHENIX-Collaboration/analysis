#ifndef tuClass__h

#ifndef noiPads__h
#define noiPads__h

#include "noiDict.h"
#include "noi_fnc.h"
   const int kLeft  = 2;
   const int kRight = 3;
// David Stewart, Dec 1 2022
// Stripped down version of tuPads
// used in conveinece for plotting
struct noiPadDim {
    // a structure to contain the four coordinates requisite for a TPad (in x or y):
    //    low   : left/bottom edge of TPad (outer edge of margin)
    //    p_low : left/bottom edge of the plot area
    //    p_up  : right/top edge of the plot area
    //    up    : right/top edge of TPad
    //
    //    If initialized with only two arguments (low, up), then set p_low to low and p_up to up
    //    If initialized with three arguments (low, plow, up) set p_up to up
    double low;
    double p_low;
    double p_up;
    double up;

    void check_input() {
        if (   low   < 0. || low   > 1. 
                || p_low < 0. || p_low > 1. 
                || p_up  < 0. || p_up  > 1. 
                || up    < 0. || up    > 1. ) {
            cout << " Fatal error: input coordinates for tuPadDim for pads must all "
                " be in range [0,1] " << endl;
            print();
            exit (2);
        } else if ( low > p_low || p_low > p_up || p_up > up ) {
            cout << " Fatal error: input coordinates must monotonically increase " << endl;
            print();
            exit(2);
        }
    };
    noiPadDim( double _low, double _p_low, double _p_up, double _up ) :
        low{_low}, p_low{_p_low}, p_up{_p_up}, up{_up} { check_input(); };
    noiPadDim( double _low, double _up ) : 
        low{_low}, p_low{_low}, p_up{_up}, up{_up} { check_input(); };
    noiPadDim( double _low, double _p_low, double _up ) : 
        low{_low}, p_low{_p_low}, p_up{_up}, up{_up} { check_input(); };
    noiPadDim( ) :
        low{0.}, p_low{0.}, p_up{1.}, up{1.} { check_input(); };

    void print() const {
        cout << Form(" Four points are: (%.2f, %.2f %.2f, %.2f)",low,p_low,p_up,up) << endl;
    };
    double low_margin () const {
        double margin { (p_low - low) / (up - low) };
        if (margin < 0) margin = 0;
        return margin;
    };
    double up_margin () const {
        // use to get set the lower margin
        double margin { (up - p_up) / (up - low) };
        if (margin < 0) margin = 0;
        return margin;
    };
    bool operator==(noiPadDim& B) const {
        return low == B.low
            && p_low == B.p_low
            && p_up  == B.p_up
            && up    == B.up;
    };
};
struct noiPadDimSet {
    /* noiPadDimSet(int _nPads, // make negative is wish to flip directtuns */
            /* vector<double> _lefts={}, */ 
            /* vector<double> _rights={}); */
    vector<double> lefts;
    vector<double> rights;
    int            nPads;

    static noiPadDim make_pad(double left, 
                double left_margin, double pad_width, 
                double right_margin) 
    {
        return noiPadDim{ left, 
                         left+left_margin,
                         left+left_margin+pad_width,
                         left+left_margin+pad_width+right_margin };
    };

    vector<noiPadDim> calc_pads() {
        int npads = nPads;
        bool flip_direction = false;
        if (npads < 0) { 
            npads = -npads; 
            flip_direction=true;
        };
        vector<noiPadDim> pads (npads) ;

        double first_left = (lefts.size() > 0) ? lefts[0] : 0.2;
        double inner_left = (lefts.size() > 1) ? lefts[1] : 0.0001;
        double page_left  = (lefts.size() > 2) ? lefts[2] : 0.01;

        double last_right  = (rights.size() > 0) ? rights[0] : 0.0001;
        double inner_right = (rights.size() > 1) ? rights[1] : 0.0;
        double page_right  = (rights.size() > 2) ? rights[2] : 0.01;

        if (npads == 0) throw std::runtime_error(
            "fatal in noiPadDimSet must request at least one pad");
        if (npads == 1) {
            double pad_width = 1.-first_left-page_left-last_right-page_right;
            if (pad_width<=0) throw std::runtime_error(
                    "fatal in noiPadDimSet margins have consumed more than 100\% of TCanvas");
            pads[0] = make_pad( page_left, first_left, pad_width, last_right );
            return pads;
        } 

        double pad_width { (1.-(first_left+page_left+last_right+page_right+
                (inner_left+inner_right)*(npads-1)))/npads };
        if (pad_width<=0) throw std::runtime_error(
                "fatal in noiPadDimSet margins have consumed more than 100\% of TCanvas");

        int index = flip_direction ? npads-1 : 0;
        pads[index] = make_pad(page_left, first_left, pad_width, inner_right);
        double left = pads[index].up;

        for (int i=1;i<npads-1;++i) {
            int index = flip_direction ? npads-i-1 : i;
            pads[index] = make_pad(left, inner_left, pad_width, inner_right);
            left = pads[index].up;
        }
        pads[flip_direction ? 0 : npads-1] = make_pad(left, inner_left, pad_width, last_right);
        return pads;
    };


    noiPadDimSet(vector<double> _lefts, vector<double> _rights ) :
              rights{_rights} 
    {
        if (_lefts.size() == 0) nPads = 1;
        else if (_lefts[0] >= 1.) {
            nPads = (int) _lefts[0];
            for (int i{0}; i<(int)_lefts.size()-1; ++i) lefts.push_back(_lefts[i+1]);
        } else {
            nPads = 1;
            lefts = _lefts;
        }
    };
    /* operator vector<noiPadDim> (); */
};

struct noiPads {
    // data
    string prefix="";
    int i_prefix=1;
    TCanvas* canvas = nullptr;
    vector<pair<noiPadDim,noiPadDim>> pad_dimensions;
    vector<TPad*> pads; // all the generated smaller pads
    TPad* canvas_pad;   // a single big pad the size of the canvas
    int nRow{1};
    int nCol{1};
    int canvas_width         { 1200 };
    int canvas_height        {  800 };
    
    noiPads ( int nYpads=1, vector<double> dimensions={}, int nXpads=1 ) {
        // build the noiPadDimSet out of dimensions
        noiPadDimSet xPads{ {0.2, 0.0001, 0.01}, {0.0001,0.0,0.01 }};
        noiPadDimSet yPads{ {0.2, 0.0001, 0.01}, {0.0001,0.0,0.01 }};

        int which = 0;
        int cnt   = 0;
        canvas_width  = -1;
        canvas_height = -1;
        for (auto val : dimensions) {
            if (val > 6) { // set first dimensions
                if (canvas_width == -1) canvas_width  = val;
                else                    canvas_height = val;
                continue;
            } else if (val > 1) { 
                which = (int) val;
                cnt = 0;
                continue;
            } else if (which == 0) {
                which = kLeft;
            }
            switch (which) {
                case 6:  // kTop
                    yPads.rights[cnt] = val;
                    break;
                case 5: // kBottom
                    yPads.lefts[cnt] = val;
                    break;
                case kLeft:
                    xPads.lefts[cnt] = val;
                    break;
                case kRight:
                    xPads.rights[cnt] = val;
                    break;
                default:
                    throw std::runtime_error(Form("fatal error: noiPads::noiPads: Error in selection of pad dimensions: was %i but must be (2,3,5,6:kLeft,Right,Bottom,Top)",
                                which));
            }
            ++cnt;
        }
        yPads.nPads = -nYpads;
        xPads.nPads =  nXpads;
        if (canvas_width  == -1) canvas_width = 1200;
        if (canvas_height == -1) canvas_height =  800;
        nCol = TMath::Abs(nXpads);
        nRow = TMath::Abs(nYpads);
        for (auto x_pad : xPads.calc_pads())
            for (auto y_pad : yPads.calc_pads())
                pad_dimensions.push_back( {y_pad, x_pad} );
    };

                    
    noiPads ( vector<pair<noiPadDim, noiPadDim>> _pad_dimensions={{{},{}}}, int _canvas_width=0, int _canvas_height=0) :
        pad_dimensions{ _pad_dimensions }
    {
        if (_canvas_width)  canvas_width  = _canvas_width;
        if (_canvas_height) canvas_height = _canvas_height;
    };

    void add_pad(pair<noiPadDim,noiPadDim>& coord){
        canvas->cd();

        if (pads.size()==0) {
            canvas_pad = new TPad(noiUniqueName(i_prefix,prefix.c_str()),"",0.,0.,1.,1.);
            canvas_pad ->SetFillStyle(4000);
            canvas_pad ->SetFrameFillStyle(4000);
            
            canvas_pad->Draw();
            canvas->cd();
        }

        const noiPadDim x { coord.second };
        const noiPadDim y { coord.first  };
        int i{0};
        /* while (gDirectory->FindObjectAny(Form("loc_pad_%i",i))) { ++i; } */
        TPad* p = new TPad(noiUniqueName(),"",x.low,y.low,x.up,y.up);

        // set the boundaries left(l), right(r), top(t), bottom(b)
        p->SetLeftMargin(x.low_margin());
        p->SetRightMargin(x.up_margin());
        p->SetBottomMargin(y.low_margin());
        p->SetTopMargin(y.up_margin());

        p->SetFillStyle(4000);
        p->SetFrameFillStyle(4000);
        p->Draw();
        pads.push_back(p);
    };

    void add_pad(vector<pair<noiPadDim,noiPadDim>> input) {
        for (auto& inp : input) add_pad(inp);
    };

    void init() {
        // make and stylize the TCanvas and pads currently in the list
        /* const char* t_name = Form("canv_%s",noiUniqueName()); */
        canvas = new TCanvas(noiUniqueName(i_prefix,Form("canv_%s",prefix.c_str())), "",canvas_width, canvas_height);
        canvas->SetFillStyle(4000);
        canvas->SetFrameFillStyle(4000);
        canvas->Draw();
        canvas->cd();

        // add all pads
        add_pad(pad_dimensions);

        canvas->cd();
    };

    TPad* operator()(int row=0, int col=0) {
        if (pads.size() == 0) init();
        if (row < 0) {
            row = -row;
            col = row % nCol;
            row = row / nCol;
        }
        int i_pad = row+col*nRow;
        if (i_pad >= (int)pads.size()) {
            i_pad = i_pad % (int) pads.size();
        }
        pads[i_pad]->cd();
        return pads[i_pad];
    };

    void stamp(string msg, noiDict opt={}, noiDict dict = {{ 
        "TextColor", (kGray+2), 
        "TextSize", 16,
        "x-loc", .05,
        "y-loc", .05}} ) 
    {
        dict += opt;
        if (msg.size() > 120) {
            cout << "pads stamp msg size: " << msg.size() << endl;
            noiDict dsize {{ "TextSize", 10 }};
            dict += dsize;
        }
        canvas_pad->cd();
        /* cout << " x: " << dict["x-loc"] << "  " << dict["y-loc"] << endl; */
        noiDrawTLatex(msg.c_str(),dict("x-loc"), dict("y-loc"), dict);
    };
    // save the output; check for *cc or *C ending and save as .pdf if required
    
    void pdf(string string_with_dollar0, vector<string> other={}) {
        string name = gSystem->pwd();
        name += "/";
        istringstream iss;
        iss.str(string_with_dollar0);
        string word;
        while (iss >> word) {
            name += noiStripEnds( noiStripStart(word, "./"), {".C",".cc",".cxx",".pdf"});
        }
        for (auto& words : other) {
            iss.clear();
            iss.str(words);
            while (iss >> word)  {
                name += noiStripEnds( noiStripStart(word, "./"), {".C",".cc",".cxx",".pdf"});
            }
        }
        name += ".pdf";
        canvas->Print(name.c_str());
    };
};


#endif

#endif
