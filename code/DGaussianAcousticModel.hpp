#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <unordered_map>

using namespace std;

class DGaussianAcousticModel
{

  vector<string> states;
  unordered_map<string, vector<vector<float>>> state_to_mus;
  unordered_map<string, vector<vector<float>>> state_to_var;
  unordered_map<string, vector<float>> state_to_trans;
  unordered_map<string, int> state_to_num_q;
  vector<float> smooth;
  unsigned int dim;
  unsigned int n_states;
    
public:

  DGaussianAcousticModel();
  DGaussianAcousticModel(const string &filename);
  ~DGaussianAcousticModel();
  void read_model(const string &filename);
  void write_model(const string &filename);
};
