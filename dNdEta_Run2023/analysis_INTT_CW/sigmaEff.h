#ifndef SIGMAEFF_H
#define SIGMAEFF_H

vector<float> sigmaEff(vector<float> v, float threshold)
{

  std::sort(v.begin(),v.end());

  int total = v.size();
  int max = (int)(threshold * total);

  vector<float>  start;
  vector<float>  stop;
  vector<float>  width;

  unsigned i = 0 ;
  while (i != v.size()-1){

    int count = 0;
    unsigned j = i;
    while (j != v.size()-1 && count < max){

      ++count;
      ++j;

    }

    if ( j != v.size()-1){
      start.push_back(v[i]);
      stop .push_back(v[j]);
      width.push_back(v[j] - v[i]);

    }
    ++i;
  }

  float minwidth = *min_element(width.begin(), width.end());

  unsigned pos = min_element(width.begin(), width.end()) - width.begin();

  float xmin = start[pos];
  float xmax = stop [pos];

  // cout<<"sigEffi test return width : "<<minwidth<<" edgel - edger : "<<xmin-xmax<<endl;

  return {minwidth, xmin, xmax};

}

float  vector_average (vector <float> input_vector)
{
	return accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
}

vector<float> sigmaEff_avg (vector<float> v, float threshold)
{
  
  vector<float> sigmaEff_vec = sigmaEff(v,threshold);

  vector<float> v_range; v_range.clear(); 

  for (int i = 0; i < v.size(); i++){
    if (v[i] >= sigmaEff_vec[1] && v[i] <= sigmaEff_vec[2]){
      v_range.push_back( v[i] );
    }
  }

  // note : return the
  // note : 0. the avg among the sigeff range 
  // note : 1. xmin
  // note : 2. xmax
  return {vector_average(v_range),sigmaEff_vec[1],sigmaEff_vec[2]};

}

#endif