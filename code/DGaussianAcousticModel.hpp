#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class DGaussianAcousticModel
{
  vector<vector<float>> states_mu;
  vector<vector<float>> states_var;
  vector<vector<float>> states_ivar;
  
  vector<float> smooth;
  float logc;
  unsigned int D;
    
public:

  DGaussianAcousticModel();
  DGaussianAcousticModel(const string filename);
  void readModel(const string filename);
  
};
