#pragma once
#include <math.h>

float inf = INFINITY;

class LeastSquare
{
    /*!
      @class LeastSquare
      @brief It calculates slope and intercept of the liner function y = a x + b from given data by least square
     */
private:
    //! A TGraph as a data container
    TGraph *g_;

    //! vector variables for data
    vector<double> x_;
    vector<double> y_;
    vector<double> yerr_;

    //! variables
    double slope_ = 0;
    double intercept_ = 0;
    bool debug_ = false;

    // void SetValues()
    // {
    //     /*!
    //       @brief Data is taken from g_ and stored in x_ and y_
    //      */
    //     x_.erase(x_.begin(), x_.end());
    //     y_.erase(y_.begin(), y_.end());

    //     for (int i = 0; i < g_->GetN(); i++)
    //     {
    //         double x, y;
    //         g_->GetPoint(i, x, y);
    //         x_.push_back(x);
    //         y_.push_back(y);
    //     }
    // }

    //! Mean of argument is returned
    double GetMean(vector<double> &values)
    {
        return accumulate(values.begin(), values.end(), 0.0) / values.size();
    }

    //! Variance of argument is returned
    double GetVariance(vector<double> &values)
    {
        double rtn = 0;
        double mean = this->GetMean(values);

        for (auto &val : values)
            rtn += pow(val - mean, 2);

        return rtn / values.size();
    }

    double Getval_err2(vector<double> values)//x/err^2
    {
        double sum_ = 0;
        for(unsigned int i=0; i<values.size(); i++)
        {
            sum_ += values[i]/(yerr_[i]*yerr_[i]);
        }
        // for (auto &val : values)
        //     sum_+=(val)/(yerr_[i]*yerr_[i]);

        return sum_;
    }

    double Get1_err2()// 1/err^2
    {
        double sum_=0;
        for(unsigned int i=0; i<yerr_.size(); i++){
            sum_ +=1 / (yerr_[i]*yerr_[i]);
        }
        return sum_;
    }

    double Getxy_err2()//xy/err^2
    {
        double sum_=0;
        for(unsigned int i=0; i<yerr_.size(); i++){
            sum_ +=(x_[i]*y_[i])/(yerr_[i]*yerr_[i]);
        }
        return sum_;
    }

    double Getx2_err2()//y/err^2
    {
        double sum_ = 0;
        for(unsigned int i=0; i<yerr_.size(); i++){
            sum_ +=(x_[i] * x_[i])/(yerr_[i]*yerr_[i]);
        }
        return sum_;
    }

    //! Covariance of x_ and y_ is returned
    double GetCovariance()
    {
        double rtn = 0;
        double x_mean = this->GetMean(x_);
        double y_mean = this->GetMean(y_);
        for (int i = 0; i < (int)x_.size(); i++)
        {
            rtn += (x_[i] - x_mean) * (y_[i] - y_mean);
            // cout<<"x : "<<x_[i]<<"  y : "<<y_[i]<<endl;
        }
        // cout <<"x_mean : "<<x_mean<< "  covariance : " << rtn / x_.size() << endl;


        return rtn / x_.size();
    }

public:
    //! The default constructor
    LeastSquare(){};

    //! A constructor which takes a TGraph as data input
    LeastSquare(TGraph *g) : g_(g){};

    //! It returns the slope. It should to be executed afte Calc()
    double GetSlope() { return slope_; };
    //! It returns the intercept. It should to be executed afte Calc()
    double GetIntercept() { return intercept_; };

    void Setdatax(vector<double> &x)
    {
        x_.erase(x_.begin(), x_.end());
        x_ = x;
    }

    void Setdatay(vector<double> &y)
    {
        y_.erase(y_.begin(), y_.end());
        y_ = y;
    }

    void Setdatayerr(vector<double> &yerr)
    {
        yerr_.erase(yerr_.begin(), yerr_.end());
        yerr_ = yerr;
    }

    // double GetAngle(){
    //     double midx = (x_[0] + x_[1]) /2;
    //     double midy = (y_[0] + y_[1]) /2;
    //     phi_ = atan2(midy, midx)
    //         return phi_;
    // }

    // void doRotate(double phi)
    // {
    //     for(unsigned int i=0; i<v1.size(); i++)
    //     {
    //         x_[i] = cos(phi) * x_[i] + sin(phi) * y_[i];
    //         y_[i] = cos(phi) * y_[i] - sin(phi) * x_[i];
    //     }
    // }

    //! Calculation of the slope and intercept is done by least square. The results are stored in the private variables.
    bool Calc(int mode)
    {
        double slope;
        double intercept;
        if(mode == 0)
        {
        // this->SetValues();
        double x_mean = this->GetMean(x_);
        double y_mean = this->GetMean(y_);

        double x_variance = this->GetVariance(x_);
        double y_variance = this->GetVariance(y_);

        double covariance = this->GetCovariance();

        slope = covariance / x_variance;
        if(x_variance == 0)
        slope = inf;
        intercept = y_mean - slope * x_mean;

        // doRotate(GetAngle());
        }
        else if(mode == 1)
        {

        double slope_num = this->Get1_err2()* this->Getxy_err2() - this->Getval_err2(x_) * this->Getval_err2(y_);

        double slope_den = this->Getx2_err2() * this->Get1_err2() - (this->Getval_err2(x_) * this->Getval_err2(x_));

        double intercept_num = - this->Getval_err2(x_) * this->Getxy_err2() + this->Getx2_err2()* this->Getval_err2(y_);

        slope = slope_num / slope_den;
        if(slope_den == 0)
        slope = inf;
        intercept = intercept_num / slope_den;
        }
        
        // if (debug_)
        // {
        //     int length = 15;
        //     cout << string(length * 4, '-') << endl;
        //     for (int i = 0; i < (int)x_.size(); i++)
        //     {
        //         cout << setw(length) << "p" << i << " ()"
        //              << setw(length) << setprecision(5) << x_[i] << ", "
        //              << setw(length) << setprecision(5) << y_[i] << " )" << endl;
        //     }
        //     cout << setw(length) << "Mean: "
        //          << setw(length) << setprecision(5) << x_mean << "\t"
        //          << setw(length) << setprecision(5) << y_mean << endl;
        //     cout << setw(length) << "Variance: "
        //          << setw(length) << setprecision(5) << x_variance << "\t"
        //          << setw(length) << setprecision(5) << y_variance << endl;
        //     cout << setw(length) << "Covariance: "
        //          << setw(length) << setprecision(5) << covariance << endl;
        //     cout << setw(length) << "Slope: "
        //          << setw(length) << setprecision(5) << slope << endl;
        //     cout << setw(length) << "Intercept: "
        //          << setw(length) << setprecision(5) << intercept << endl;
        // }

        slope_ = slope;
        intercept_ = intercept;
        return true;
    }

    void SetDebugMode(bool flag) { debug_ = flag; };
};

TGraph *GetGraph(TRandom3 *random)
{

    TGraph *g = new TGraph();
    if (true)
    {
        for (int i = 0; i < 3; i++)
        {
            double y = 2 * i * random->Gaus(1, 0.1);
            g->SetPoint(g->GetN(), i, y);
            // hist->Fill( y );
        }
    }
    else
    {

        // example from https://sci-pursuit.com/math/statistics/least-square-method.html
        g->SetPoint(0, 50, 40);
        g->SetPoint(1, 60, 70);
        g->SetPoint(2, 70, 90);
        g->SetPoint(3, 80, 60);
        g->SetPoint(4, 90, 100);
    }

    // TH1* hist = new TH1D( "hist", "title", 100, -10, 10 );
    // g->SetMarkerStyle( 20 );
    //  cout << "Hist StdDev: " << hist->GetStdDev() << endl;
    //  cout << "Hist Variance: " << pow( hist->GetStdDev(), 2 ) << endl;
    //  cout << "Graph Covariance: " << g->GetCovariance() << endl;

    return g;
}

//! A test function with LeastSquare class
int TestMine(int loop_num, int mode)
{
    TRandom3 *random = new TRandom3();
    for (int i = 0; i < loop_num; i++)
    {
        auto g = GetGraph(random);
        LeastSquare *ls = new LeastSquare(g);
        ls->SetDebugMode(true);
        ls->Calc(mode);
    }

    return 0;
}

//! A test function with ROOT fitting
int TestRootFit(int loop_num)
{

    TRandom3 *random = new TRandom3();
    for (int i = 0; i < loop_num; i++)
    {
        auto g = GetGraph(random);
        TF1 *f = new TF1("f", "pol1", -10, 10);
        g->Fit(f, "QN0CF");
    }

    return 0;
}

//! A test function with LeastSquare class
int TestBoth(int loop_num, int mode)
{
    TRandom3 *random = new TRandom3();
    for (int i = 0; i < loop_num; i++)
    {
        auto g = GetGraph(random);
        LeastSquare *ls = new LeastSquare(g);
        ls->SetDebugMode(true);
        ls->Calc(mode);

        TF1 *f = new TF1("f", "pol1", -10, 10);
        g->Fit(f, "QN0CF");
        double length = 15;
        cout << "-- ROOT fitting " << string(length, '-') << endl;
        cout << setw(length) << "Slope: "
             << setw(length) << setprecision(5) << f->GetParameter(1) << endl;
        cout << setw(length) << "Intercept: "
             << setw(length) << setprecision(5) << f->GetParameter(0) << endl;
    }

    return 0;
}

int least_square(int loop_num = 1e5, int test_mode = 0, int mode = 0)
{
    /*!
      @brief This is the main function.
      @param loop_num The number of tests
      @param test_mode 0 tests LeastSquare class. 1 tests ROOT fitting.
      @details How to run:
      Example 1: Testing LeastSquare class for 100 times:
        $ root 'least_square( 100, 0 )'

      Example 2: Testing ROOT fitting for 10000 times:
        $ root 'least_square( 10000, 1 )'
     */

    if (test_mode == 0)
        TestMine(loop_num, mode);
    else if (test_mode == 1)
        TestRootFit(loop_num);
    else
        TestBoth(loop_num, mode);
    return 0;
}