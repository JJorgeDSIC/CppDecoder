#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <random>
#include <cmath>

const int LOG2PI = 1.83787706641;

using namespace std;

class TransValue
{
  string state;
  float value;
public:
  TransValue(const string &st, const float &val);
  string& getState(){ return state; }
  float& getValue(){ return value; }
};

TransValue::TransValue(const string &st, const float &val){
  state = st;
  value = val;
}

class DGaussianState
{

  vector<vector<float>> mus;
  vector<vector<float>> vars;
  vector<vector<float>> ivars;
  vector<float> pmembers;
  vector<float> logc;
  size_t components;

public:
  //DGaussianState();
  void addPMembers(const string &line);
  void addMu(const string &line);
  void addVar(const string &line);
  void setComponents(const size_t comps){ components = comps;}
  size_t getComponents() { return components; }
  vector<vector<float>>& getMus() { return mus;}
  vector<vector<float>>& getVars() { return vars;}
  vector<float>& getPMembers() { return pmembers;}
  
};

class MixtureAcousticModel
{
  typedef std::tuple<string,float> value_t;
  vector<string> states;
  unordered_map <string, vector<DGaussianState>> symbol_to_states;
  unordered_map <string, vector<float>> state_to_trans;
  unordered_map <string, unordered_map <string, vector<TransValue>>> state_to_transL;
  unordered_map <string, string> state_to_type;
  unordered_map<string, int> state_to_num_q;
  vector<float> smooth;
  unsigned int dim;
  unsigned int n_states;
    
public:

  MixtureAcousticModel();
  MixtureAcousticModel(const string &filename);
  ~MixtureAcousticModel();
  
  unsigned int getDim();
  unsigned int getNStates();
  
  void read_model(const string &filename);
  void write_model(const string &filename);
  float calc_prob(const string &state, const int &q, const vector<float> &frame);
};
