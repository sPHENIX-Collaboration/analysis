#ifndef noiBinVec__h
#define noiBinVec__h

#ifndef tuClass__h
// David Stewart, Dec 1 2022
// Stripped down version of a convenience class that provide's *double and int for histograms binning
// initialized with vector<double>
struct tuBinVec {
    // data members
    double*        ptr;
    int            size;
    vector<double> vec;

    void build_ptr() {
        size = vec.size();
        ptr = new double[size];
        for (int i{0}; i<size; ++i) ptr[i] = vec[i];
    };

    tuBinVec(vector<double> V) {
        // range repeate will add a range leading to the next number
        // it is triggered by a repeat value of the last number, followed by the number
        // of bins
        //   example:
        //         0, 0, 5, 1. 2. 3. = 0 .2 .4 .6 .8 1.0 2. 3.
        vec.push_back(V[0]);
        int S = V.size();
        int i{1}; 
        while (i<(int)S) {
            if ( V[i] == V[i-1] ) {
                if (i>(S-3)) throw std::runtime_error( "fatal in tuBinVec with range_repeat");
                double step = (V[i+2]-V[i])/V[i+1];
                for (int k{1}; k<=V[i+1]; ++k) vec.push_back(V[i]+k*step);
                i+=3;
            } else {
                vec.push_back(V[i]);
                ++i;
            }
        }
        build_ptr();
    };

    operator int () const { return size-1; };
    operator double* () const { return ptr; };
    operator vector<double> () { return vec; };
    int bin_from_0(double val) const {
        auto loc = upper_bound(vec.begin(), vec.end(), val);
        return loc-vec.begin()-1;
    };
    int bin_from_1(double val) const {
        return bin_from_0(val) + 1;
    };
};
#endif // endif noiclass noibinvec definition

#endif
