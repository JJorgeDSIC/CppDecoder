#include "DGaussianAcousticModel.hpp"
#include "Utils.hpp"

DGaussianAcousticModel::DGaussianAcousticModel(){
  cout << "Constructor" << endl;
}

void DGaussianAcousticModel::read_model(const string &filename){

  cout << "Reading DGaussianAcoustic model from " << filename << "..." << endl;

  ifstream fileI (filename, ifstream::in);
  string line;
  int i, statesIter;
  const char del = ' ';

  if(fileI.is_open()){
    cout << "Reading..";
    getline(fileI, line); //AMODEL
    getline(fileI, line); //DGaussian
    getline(fileI, line, del); //D <dim>
    getline(fileI, line); //D <dim>
    stringstream(line) >> dim;
    cout << "Dimension: " << dim << endl;
    
    getline(fileI, line, del); //SMOOTH
    getline(fileI, line); //Value
    
    smooth = parse_line(line);

    getline(fileI, line, del); //N
    getline(fileI, line);
    stringstream(line) >> n_states;
    
    cout << "Total states: " << n_states << endl;

    string name;
    int n_q;
    float temp_trans,temp_mu,temp_var,temp_ivar;

    for(statesIter = 0; statesIter < n_states; statesIter++){

      vector<float> trans;

      //Name
      getline(fileI, line);
      line.erase(0,1);
      line.erase(line.size()-1,line.size());
      stringstream(line) >> name;
      cout << "State: " << name << endl;
      states.push_back(name);
      
      //Q
      getline(fileI, line,del);
      getline(fileI, line);
      stringstream(line) >> n_q;
      cout << "Q: " << n_q << endl;

      state_to_num_q[name] = n_q;

      if(n_q == 1){
	getline(fileI, line,del); //Trans
	getline(fileI, line); //actual value
	stringstream(line) >> temp_trans;
	trans.push_back(temp_trans);
      }else{
	getline(fileI, line); //Trans
	
	for(int transIter = 0; transIter < n_q; transIter++){
	  getline(fileI,line);
	  stringstream(line) >> temp_trans;
	  trans.push_back(temp_trans);
	}
      }

      print_vector(trans);

      state_to_trans[name] = trans;

      vector<vector<float>> states_mu;
      vector<vector<float>> states_var;
      vector<vector<float>> states_ivar;
      vector<float> states_logc;
      
      for(i = 0; i < n_q; i++){

	vector<float> mu;
	vector<float> var;
	vector<float> ivar;
	float logc = 0;

	//MU
	getline(fileI, line,del); //MU
	getline(fileI, line); //values
	
	mu = parse_line(line);

	print_vector(mu);

	getline(fileI, line,del); //VAR
	getline(fileI, line); //values
	
	var = parse_line(line);
	
	for(auto value: var){
	  ivar.push_back(1.0/value);
	  logc+= log(value);
	}

	logc+= dim * LOG2PI;

	logc = -0.5 * logc;

	cout << endl;
	print_vector(var);
	cout << endl;
	print_vector(ivar);
	
	states_mu.push_back(mu);
	states_var.push_back(var);
	states_ivar.push_back(ivar);
        states_logc.push_back(logc);	
      }

      state_to_mus[name] = states_mu;
      state_to_vars[name] = states_var;
      state_to_ivars[name] = states_ivar;
      state_to_logc[name] = states_logc;
    }
    
    fileI.close();
  }else cout << "Unable to open the file " << filename << " for reading." << endl;

}

void DGaussianAcousticModel::write_model(const string &filename){

  ofstream fileO(filename, ios::app);

  int n_q;
  
  if(fileO.is_open()){

    fileO << "AMODEL\n";
    fileO << "DGaussian\n";
    fileO << "D " << dim << endl;
    fileO << "SMOOTH ";
    
    for(auto &value: smooth)
      fileO << value << " ";
 
    fileO << "\n";
    fileO << "N " << n_states << endl;

    for(auto &name: states){

      n_q = state_to_num_q[name];
   
      fileO << "'" << name << "'" << endl;
      fileO << "Q " << n_q << endl;

      if(n_q == 1){
	fileO << "Trans ";
      }else{
	fileO << "Trans\n";
      }
    
      for(auto &value: state_to_trans[name])
	fileO << value << "\n";

      vector <vector<float>> mus = state_to_mus[name];
      vector <vector<float>> vars = state_to_vars[name];
      
      for(auto i = 0; i < n_q; i++){
	
	fileO << "MU ";
	for(auto &value: mus[i])
	  fileO << value << " ";
      
	fileO << endl;
      
	fileO << "VAR ";
	for(auto &value: vars[i])
	  fileO << value << " ";
      
	fileO << endl;
      }
    }
    
    fileO.close();
    
  }else cout << "Unable to open file for writing" << endl;
}


DGaussianAcousticModel::DGaussianAcousticModel(const string &filename){
  
  DGaussianAcousticModel::read_model(filename);
}

DGaussianAcousticModel::~DGaussianAcousticModel(){
  cout << "Destructor" << endl;
}

unsigned int DGaussianAcousticModel::getDim(){
  return dim;
}

unsigned int DGaussianAcousticModel::getNStates(){
  return n_states;
}


float DGaussianAcousticModel::calc_prob(const string &state, const int &q, const vector<float> &frame){

  vector<float> mu = state_to_mus[state][q];
  vector<float> ivar = state_to_ivars[state][q];
  float logc = state_to_logc[state][q];

  cout << endl;
  print_vector(mu);
  cout << endl;
  print_vector(ivar);
  cout << endl;

  float prob = 0.0;
  float aux = 0.0;
  
  for(auto i = 0; i< frame.size(); i++){
    aux = frame[i] - mu[i];
    prob = (aux*aux) * ivar[i];
  }

  return -0.5 * prob + logc;
}

int main(){

  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(0, 10);

  DGaussianAcousticModel amodel("../models/model_0");
  amodel.write_model("../models/new_model_librispeech");

  cout << endl;
  cout << dist(e2) << endl;
  vector<float> frame;

  for(auto i = 0; i < amodel.getDim(); i++){
    frame.push_back(dist(e2) / 10.0);
  }

  cout << endl;
  print_vector(frame);

  float prob = amodel.calc_prob("aa",0,frame);

  cout << "Prob: " << prob << endl;

}
