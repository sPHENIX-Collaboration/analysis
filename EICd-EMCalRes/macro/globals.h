/// which calorimeter clusters should be reconstructed?
/// they are all included in the GEANT4 simulation anyway.

enum {
	EEMC,
	CEMC,
	FEMC,
	NCALOS,
};
const char *calo_names[NCALOS] = {
	"EEMC",
	"CEMC",
	"FEMC",
};

/// how should the normalized energies of the clusters be histogrammed?
/// this is real important 'cause fitting the histogram give dE/E.

Int_t ENERGY_SPECTRUM_NBINS = 68;
Float_t ENERGY_SPECTRUM_XMAX = 1.7;

/// the code puts a hard limit on the number of clusters in an event.
/// does not count the clusters with transverse momentum less than 0.2GeV/c.
/// *NOTE!* this constant is independently defined in ClusterPeeler.h.
/// the dang thing would not compile if i #include'd this file.

const int MAX_NCLUSTER = 1024;
