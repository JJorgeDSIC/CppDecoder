#include "TiedStatesAcousticModel.hpp"
#include "Utils.hpp"

TiedStatesAcousticModel::TiedStatesAcousticModel(){
  cout << "Constructor" << endl;
}

void TiedStatesAcousticModel::read_model(const string &filename){

  cout << "Reading TiedStatesAcousticModel from " << filename << "..." << endl;

  ifstream fileI (filename, ifstream::in);
  string line, name, trans_id;
  int i, statesIter;
  const char del = ' ';

  int components, n_q;

  if(fileI.is_open()){
    
    getline(fileI, line); //AMODEL
    getline(fileI, line); //TiedStates
    getline(fileI, line); //Mixture
    getline(fileI, line); //DGaussian    
    getline(fileI, line, del); //D
    getline(fileI, line); //DGaussian
    stringstream(line) >> dim;

    getline(fileI, line, del); //SMOOTH
    getline(fileI, line); //Value
    
    smooth = parse_line(line);

    getline(fileI, line, del); //N
    getline(fileI, line); //Value
    stringstream(line) >> n_states;

    getline(fileI, line); //States

    for(auto i = 0; i < n_states; i++){

      vector <float> pmembers;

      getline(fileI, line); //state_id
      stringstream(line) >> name;
      senones.push_back(name);

      getline(fileI, line, del); //I
      getline(fileI, line); //value
      stringstream(line) >> components;

      getline(fileI, line, del); //PMembers
      getline(fileI, line); //value
      pmembers = parse_line(line);

      getline(fileI, line); //Members

      vector<vector<float>> states_mu;
      vector<vector<float>> states_var;
      vector<vector<float>> states_ivar;
      vector<float> states_logc;

      for(auto i = 0; i < components; i++){
	
	vector<float> mu;
	vector<float> var;
	vector<float> ivar;
	float logc = 0;

	//MU
	getline(fileI, line,del); //MU
	getline(fileI, line); //values
	
	mu = parse_line(line);

	getline(fileI, line,del); //VAR
	getline(fileI, line); //values
	
	var = parse_line(line);
	
	for(const auto &value: var){
	  ivar.push_back(1.0/value);
	  logc+= log(value);
	}

	logc+= dim * LOG2PI;

	logc = -0.5 * logc;
	
	states_mu.push_back(mu);
	states_var.push_back(var);
	states_ivar.push_back(ivar);
        states_logc.push_back(logc);	
      }

      senone_to_mus[name] = states_mu;
      senone_to_vars[name] = states_var;
      senone_to_ivars[name] = states_ivar;
      senone_to_logc[name] = states_logc;
      senone_to_pmembers[name] = pmembers;
      
    }

    getline(fileI, line, del); //N
    getline(fileI, line); //Value
    stringstream(line) >> n_trans;

    string symbol;
    
    for(auto i = 0; i < n_trans; i++){
      
      getline(fileI, line); //
      line.erase(0,1);
      line.erase(line.size()-1,line.size());
      stringstream(line) >> symbol;
      symbols.push_back(symbol);
      
      getline(fileI, line, del); //Q
      getline(fileI, line); //Value
      stringstream(line) >> n_q;

      getline(fileI, line); //Trans / TransP

      vector<float> trans;
      vector<string> senones;


      if(line.find("TransP")==string::npos){
	
	getline(fileI, line);
	
	stringstream strans(line);

	float x;

	while(strans >> x)
	  trans.push_back(x);
	
	getline(fileI, line);

	stringstream ssenone(line);

	string s;
		
	while(ssenone >> s)
	  senones.push_back(s);

	symbol_to_transitions[symbol] = trans;
	symbol_to_senones[symbol] = senones;
	symbol_to_symbol_transitions[symbol] = symbol;
	
      }else{
	
	string token;
	vector<string> result;
	istringstream iss(line);
	iss >> token; //TransP
	iss >> token; //Symbol with transitions for this symbol
	
	getline(fileI, line);
	stringstream ssenone(line);

	string s;
		
	while(ssenone >> s)
	  senones.push_back(s);
	
	symbol_to_transitions[symbol] = symbol_to_transitions[token];
	symbol_to_senones[symbol] = senones;
	symbol_to_symbol_transitions[symbol] = token;

      }
	
      
    }
    fileI.close();
  }else cout << "Unable to open file for reading" << endl;
}


void TiedStatesAcousticModel::write_model(const string &filename){

  ofstream fileO(filename, ios::app);

  int n_q;
  
  if(fileO.is_open()){

    fileO << "AMODEL\n";
    fileO << "TiedStates\n";
    fileO << "Mixture\n";
    fileO << "DGaussian\n";
    fileO << "D " << dim << endl;
    fileO << "SMOOTH ";
    for(const auto &value: smooth){
      fileO << value << " ";
    }
    fileO << endl;

    fileO << "N ";
    fileO << n_states << endl;

    fileO << "States" << endl;

    for(auto i = 0; i < n_states; i++){

      fileO << senones[i] << endl;
      vector<float> pmembers = senone_to_pmembers[senones[i]];
      int components = pmembers.size();

      fileO << "I " << components << endl;
      fileO << "PMembers ";
      for(const auto &value: pmembers){
	fileO << value << " ";
      }
      fileO << endl;

      fileO << "Members" << endl;

      for(auto j = 0; j < components; j++){
	vector<float> mu = senone_to_mus[senones[i]][j];
	fileO << "MU ";
	for(const auto &value: mu)
	  fileO << value << " ";
	fileO << endl;

	vector<float> var = senone_to_vars[senones[i]][j];
	fileO << "VAR ";
	for(const auto &value: var)
	  fileO << value << " ";
	fileO << endl;
	
      }
      

    }

    fileO << "N " << n_trans << endl;

    for(auto i = 0; i < n_trans; i++){

      string symbol = symbols[i];

      fileO << "'" << symbol << "'" << endl;

      fileO << "Q " << symbol_to_senones[symbol].size() << endl;

      string symbol_equivalent = symbol_to_symbol_transitions[symbol];

      if(symbol.compare(symbol_equivalent)==0){
	fileO << "Trans" << endl;
	vector<float> trans = symbol_to_transitions[symbol];
	for(const auto &value: trans)
	  fileO << value << " ";
	fileO << endl;
	
      }else{
	fileO << "TransP " << symbol_equivalent << endl;
      }

      vector<string> senones = symbol_to_senones[symbol];

      for(const auto &value: senones)
	fileO << value << " ";
      fileO << endl;

    }
    
  }else cout << "Unable to open file for writing." << endl;

}

TiedStatesAcousticModel::TiedStatesAcousticModel(const string &filename){
  TiedStatesAcousticModel::read_model(filename);
}

TiedStatesAcousticModel::~TiedStatesAcousticModel(){
  cout << "Destructor" << endl;
}

int main(){
  TiedStatesAcousticModel amodel("../models/tiedphoneme_I04.example.model");
  amodel.write_model("../models/tiedphoneme_I04.example.again.model");
}
