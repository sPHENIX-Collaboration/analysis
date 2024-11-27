#ifndef VECTOR_STDDEV_H
#define VECTOR_STDDEV_H

double  vector_average (vector <double> input_vector) {
	return accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
}

double vector_stddev (vector <double> input_vector){
	
	double sum_subt = 0;
	double average  = accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
	
	// cout<<"average is : "<<average<<endl;

	for (int i=0; i<input_vector.size(); i++){ sum_subt += pow((input_vector[i] - average),2); }

	//cout<<"sum_subt : "<<sum_subt<<endl;
	// cout<<"print from the function, average : "<<average<<" std : "<<stddev<<endl;

	return sqrt( sum_subt / double(input_vector.size()-1) );
}	

#endif