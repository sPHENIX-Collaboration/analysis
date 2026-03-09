# Module of INTT vertex Z reconstruction
* The algorithm has been introduced in the [analysis note](https://sphenix-invenio.sdcc.bnl.gov/communities/sphenixcommunity/requests/717c5d51-65dc-4a5b-877b-bc19789f21f7)
* The performance of the INTT vertex Z can be found in page 11 of [the slide on indico](https://indico.bnl.gov/event/25508/contributions/99232/attachments/58441/100349/INTT_2024_11_07_SimulationFix.pdf)
* You will need to compile the module.

# The status of the module
* The module has been tested with run 54280 (AuAu run taken in zero-field in run2024)
* Should be in principle able to handle the p+p data (can simply be mimiced as the low-multiplicity AuAu data)
* Should have the capability to handle even the field-on data
  * Will have to provide two delta phi selections (simply because of the particle bending due the magnetic field)
 
# The input of the module
* All are with the unit of \[cm\]
* This module is in principle designed to be inclusive in the sense of requiring some inputs in the first place, such as the `vertexXY`, `delta_phi cut`, `dca_cut`. Please see the following.
``` C++

InttZVertexFinderTrapezoidal(
    const std::string &name = "InttZVertexFinderTrapezoidal",
    std::pair<double, double> vertexXYIncm_in = {0, 0}, // note : in cm
    bool IsFieldOn_in = false,
    bool IsDCACutApplied_in = true,
    std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree_in = {{-1,1},{-1000.,1000.}}, // note : in degree
    std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm_in = {{-1,1},{-1000.,1000.}}, // note : in cm
    int ClusAdcCut_in = 35,
    int ClusPhiSizeCut_in = 8,
    bool PrintRecoDetails_in = false
  );

```

* For the run 54280, you can simply following the setting below in your macro
``` C++

pair<double, double> vertexXYIncm = {-0.0215087, 0.223197};
bool IsFieldOn = false;
bool IsDCACutApplied = true;
std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree = {{-0.5, 0.5},{-1000.,1000.}}; // note : in degree
std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm = {{-0.1,0.1},{-1000.,1000.}}; // note : in cm
int ClusAdcCut = 35;
int ClusPhiSizeCut = 5;
bool PrintRecoDetails = true;

```

# Data object
* At this moment, it's very simple, it just saves some of the information as the outcome of the INTT vertex Z reconstruction. Please see the followings.
``` C++
class INTTvtxZF4AObj : public PHObject
{
    public:
        INTTvtxZF4AObj();
        
        // void identify(std::ostream& os = std::cout) const override;
        void Reset() override;
        int isValid() const override;

        double INTTvtxZ;
        double INTTvtxZError;
        double NgroupTrapezoidal;
        double NgroupCoarse;
        double TrapezoidalFitWidth;
        double TrapezoidalFWHM;
        long long NClusAll;
        long long NClusAllInner;
    private:
        ClassDefOverride(INTTvtxZF4AObj, 1);
};

```

# Future
* We possibly have to improve the module to make it capable of handling two things
  *  Multiple vertices for each bunch crossing in one INTT F4A event (one strobe length) (basically necessary)
  *  Multiple collisions in single bunch crossing
* We will have to update the data object as well accordingly.
