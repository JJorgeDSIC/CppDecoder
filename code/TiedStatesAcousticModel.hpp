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

class TiedStatesAcousticModel
{

  unsigned int dim;
  unsigned int n_states;
  unsigned int n_trans;
  vector<float> smooth;
  
  vector<string> symbols;
  vector <string> senones;

  unordered_map<string, vector<string>> symbol_to_senone;
  
  unordered_map<string, vector<vector<float>>> senone_to_mus;
  unordered_map<string, vector<vector<float>>> senone_to_vars;
  unordered_map<string, vector<vector<float>>> senone_to_ivars;
  unordered_map<string, vector<float>> senone_to_logc;
  unordered_map<string, vector<float>> senone_to_pmembers;
  unordered_map<string, string> symbol_to_symbol_transitions;
  unordered_map<string, vector<float>> symbol_to_transitions;
  unordered_map<string, vector<string>> symbol_to_senones;
  
public:

  TiedStatesAcousticModel();
  TiedStatesAcousticModel(const string &filename);
  ~TiedStatesAcousticModel();
  void read_model(const string &filename);
  void write_model(const string &filename);
  
};
