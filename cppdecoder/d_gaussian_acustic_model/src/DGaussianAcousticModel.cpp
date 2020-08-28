#include "DGaussianAcousticModel.h"

#include <Utils.h>

void DGaussianAcousticModel::read_model(const std::string &filename) {
  std::cout << "Reading DGaussianAcoustic model from " << filename << "..."
            << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line;
  const char del = ' ';

  if (fileI.is_open()) {
    getline(fileI, line);       // AMODEL
    getline(fileI, line);       // DGaussian
    getline(fileI, line, del);  // D <dim>
    getline(fileI, line);       // D <dim>
    std::stringstream(line) >> dim;

    getline(fileI, line, del);  // SMOOTH
    getline(fileI, line);       // Value

    smooth = parse_line(line);

    getline(fileI, line, del);  // N
    getline(fileI, line);
    std::stringstream(line) >> n_states;

    std::string name;
    int n_q;
    float temp_trans, temp_mu, temp_var, temp_ivar;

    for (int statesIter = 0; statesIter < n_states; statesIter++) {
      std::vector<float> trans;

      // Name
      getline(fileI, line);
      line.erase(0, 1);
      line.erase(line.size() - 1, line.size());
      std::stringstream(line) >> name;
      states.push_back(name);

      // Q
      getline(fileI, line, del);
      getline(fileI, line);
      std::stringstream(line) >> n_q;

      state_to_num_q[name] = n_q;

      getline(fileI, line);  // Trans
      getline(fileI, line);
      trans = parse_line(line);

      state_to_trans[name] = trans;

      std::vector<std::vector<float>> states_mu;
      std::vector<std::vector<float>> states_var;
      std::vector<std::vector<float>> states_ivar;
      std::vector<float> states_logc;

      for (int i = 0; i < n_q; i++) {
        std::vector<float> mu;
        std::vector<float> var;
        std::vector<float> ivar;
        float logc = 0;

        // MU
        getline(fileI, line, del);  // MU
        getline(fileI, line);       // values

        mu = parse_line(line);

        getline(fileI, line, del);  // VAR
        getline(fileI, line);       // values

        var = parse_line(line);

        for (auto value : var) {
          ivar.push_back(1.0 / value);
          logc += log(value);
        }

        logc += dim * LOG2PI;
        logc = -0.5 * logc;

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
  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
  }
}

void DGaussianAcousticModel::write_model(const std::string &filename) {
  std::ofstream fileO(filename, std::ios::app);

  int n_q;

  if (fileO.is_open()) {
    fileO << "AMODEL\n";
    fileO << "DGaussian\n";
    fileO << "D " << dim << std::endl;
    fileO << "SMOOTH ";

    for (auto &value : smooth) fileO << value << " ";

    fileO << "\n";
    fileO << "N " << n_states << std::endl;

    for (auto &name : states) {
      n_q = state_to_num_q[name];

      fileO << "'" << name << "'" << std::endl;
      fileO << "Q " << n_q << std::endl;

      fileO << "Trans\n";

      for (auto &value : state_to_trans[name]) fileO << value << " ";

      fileO << std::endl;

      std::vector<std::vector<float>> mus = state_to_mus[name];
      std::vector<std::vector<float>> vars = state_to_vars[name];

      for (auto i = 0; i < n_q; i++) {
        fileO << "MU ";
        for (auto &value : mus[i]) fileO << value << " ";

        fileO << std::endl;

        fileO << "VAR ";
        for (auto &value : vars[i]) fileO << value << " ";

        fileO << std::endl;
      }
    }

    fileO.close();

  } else {
    std::cout << "Unable to open file for writing" << std::endl;
  }
}

DGaussianAcousticModel::DGaussianAcousticModel(const std::string &filename) {
  DGaussianAcousticModel::read_model(filename);
}

DGaussianAcousticModel::~DGaussianAcousticModel() {
  std::cout << "Destructor" << std::endl;
}

unsigned int DGaussianAcousticModel::getDim() { return dim; }

unsigned int DGaussianAcousticModel::getNStates() { return n_states; }

float DGaussianAcousticModel::calc_prob(const std::string &state, const int &q,
                                        const std::vector<float> &frame) {
  std::vector<float> mu = state_to_mus[state][q];
  std::vector<float> ivar = state_to_ivars[state][q];
  float logc = state_to_logc[state][q];

  float prob = 0.0;
  float aux = 0.0;

  for (auto i = 0; i < frame.size(); i++) {
    aux = frame[i] - mu[i];
    prob += (aux * aux) * ivar[i];
  }
  return -0.5 * prob + logc;
}

int main() {
  DGaussianAcousticModel amodel(
      "bin/models/dgaussian_monopohoneme_I01.example.model");
  amodel.write_model(
      "bin/models/dgaussian_monopohoneme_I01.example.again.model");

  std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};

  float prob = amodel.calc_prob("aa", 0, frame);

  std::cout << "Prob: " << prob << std::endl;
}
