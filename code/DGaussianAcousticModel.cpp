#include "DGaussianAcousticModel.hpp"


void printVector(const std::vector<float> &vec){
  
    for (auto it = vec.begin(); it != vec.end(); ++it)
	cout << *it << ' ';
      cout << endl;
      
}

DGaussianAcousticModel::DGaussianAcousticModel(){

  cout << "Constructor" << endl;

}

DGaussianAcousticModel::DGaussianAcousticModel(const string filename){

  cout << "Reading DGaussianAcoustic model from " << filename << "..." << endl;

  ifstream fileO (filename, ifstream::in);
  string line;
  int dim,states,statesIter,i;
  const char del = ' ';

  if(fileO.is_open()){
    cout << "Reading..";
    getline(fileO, line); //AMODEL
    getline(fileO, line); //DGaussian
    getline(fileO, line, del); //D <dim>
    getline(fileO, line); //D <dim>
    stringstream(line) >> dim;
    cout << "Dimension: " << dim << endl;
    
    getline(fileO, line, del); //SMOOTH

    for(i = 0; i < dim; i++){
      getline(fileO, line, del); //Smooth values
      cout << line << endl;
    }

    //N is already consumed
    getline(fileO, line);
    stringstream(line) >> states;
    
    cout << "Total states: " << states << endl;

    string name;
    int n_q;
    float temp_trans,temp_mu,temp_var,temp_ivar;

    for(statesIter = 0; statesIter < states; statesIter++){

      vector<float> trans;

      //Name
      getline(fileO, line);
      stringstream(line) >> name;
      cout << "State: " << name << endl;
      stringstream(line) >> name;
      //Q
      getline(fileO, line,del);
      getline(fileO, line);
      stringstream(line) >> n_q;
      cout << "Q: " << n_q << endl;

      if(n_q == 1){
	getline(fileO, line,del); //Trans
	getline(fileO, line); //actual value

	stringstream(line) >> temp_trans;
	cout << "temp_trans: " << temp_trans << endl;
	trans.push_back(temp_trans);
      }else{
	getline(fileO, line); //Trans

	for(int transIter = 0; transIter < n_q; transIter++){
	  getline(fileO,line);
	  stringstream(line) >> temp_trans;
	  trans.push_back(temp_trans);
	}
      }

      printVector(trans);

      for(i = 0; i < n_q; i++){

	vector<float> mu;
	vector<float> var;
	vector<float> ivar;

	//MU
	getline(fileO, line,del); //MU

	for(i = 0; i < dim; i++){
	  getline(fileO, line,del);
	  stringstream(line) >> temp_mu;
	  mu.push_back(temp_mu);
	}

	printVector(mu);

	for(i = 0; i < dim; i++){
	  getline(fileO, line,del);
	  stringstream(line) >> temp_var;
	  var.push_back(temp_var);
	}

	printVector(var);
	
      }
    }
    
    //TODO
    fileO.close();
  }else cout << "Unable to open the file " << filename << " for reading." << endl;

  

}

int main(){

  DGaussianAcousticModel amodel("../models/model_0");

}
