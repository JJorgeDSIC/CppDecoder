#include "MixtureAcousticModel.h"

#include <Utils.h>

void DGaussianState::addPMembers(const std::string &line) {
  pmembers = parse_line(line);
}

void DGaussianState::addMu(const std::string &line) {
  mus.push_back(parse_line(line));
}

void DGaussianState::addVar(const std::string &line) {
  vars.push_back(parse_line(line));

  std::vector<float> ivar;

  float lgc = 0.0f;

  for (auto value : vars[vars.size() - 1]) {
    ivar.push_back(1.0 / value);
    lgc += log(value);
  }
  ivars.push_back(ivar);

  lgc += vars[vars.size() - 1].size() * LOG2PI;

  lgc = -0.5 * lgc;

  logc.push_back(lgc);
}

MixtureAcousticModel::MixtureAcousticModel() {
  std::cout << "Constructor" << std::endl;
}

void MixtureAcousticModel::read_model(const std::string &filename) {
  std::cout << "Reading MixtureAcousticModel model from " << filename << "..."
            << std::endl;

  std::ifstream fileI(filename, std::ifstream::in);
  std::string line;
  int i, statesIter;
  float value;
  const char del = ' ';

  if (fileI.is_open()) {
    std::cout << "Reading..." << std::endl;
    getline(fileI, line);  // AMODEL
    getline(fileI, line);  // Mixture
    getline(fileI, line);  // DGaussian

    getline(fileI, line, del);  // D
    getline(fileI, line);
    std::stringstream(line) >> dim;

    getline(fileI, line, del);  // SMOOTH
    getline(fileI, line);
    smooth = parse_line(line);

    getline(fileI, line, del);  // N
    getline(fileI, line);
    std::stringstream(line) >> n_states;

    std::string name;
    int n_q;
    float temp_trans;

    for (statesIter = 0; statesIter < n_states; statesIter++) {
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

      if (line == "Trans") {
        state_to_type[name] = "Trans";
        if (n_q == 1) {
          getline(fileI, line, del);  // Trans
          getline(fileI, line);       // actual value
          std::stringstream(line) >> temp_trans;
          trans.push_back(temp_trans);

        } else {
          getline(fileI, line);  // Trans
          std::istringstream iss(line);
          while (iss >> value) trans.push_back(value);
        }
        state_to_trans[name] = trans;
      } else if (line == "TransL") {
        state_to_type[name] = "TransL";
        std::string state_src, state_dst;
        float value;
        std::unordered_map<std::string, std::vector<TransValue>> transL;
        for (auto j = 0; j < n_q + 1; j++) {
          getline(fileI, line);  // State name
          std::stringstream(line) >> state_src;
          for (auto s = 0; s < n_q + 1; s++) {
            getline(fileI, line, del);  //
            getline(fileI, line, del);  //
            getline(fileI, line, del);  //
            std::stringstream(line) >> state_dst;
            getline(fileI, line);  // actual value
            std::stringstream(line) >> value;
            TransValue tv(state_dst, value);
            transL[state_src].push_back(tv);
          }
          getline(fileI, line);
        }
        state_to_transL[name] = transL;
      }

      std::vector<DGaussianState> state_dgaussians;

      for (i = 0; i < n_q; i++) {
        DGaussianState dg_state;

        int components;
        std::vector<float> state_pmembers;

        getline(fileI, line, del);  // I
        getline(fileI, line);
        std::stringstream(line) >> components;

        dg_state.setComponents(components);
        getline(fileI, line, del);  // PMembers
        getline(fileI, line);       //

        dg_state.addPMembers(line);

        for (auto j = 0; j < components; j++) {
          // MU
          getline(fileI, line, del);  // MU
          getline(fileI, line);       // values

          dg_state.addMu(line);

          getline(fileI, line, del);  // VAR
          getline(fileI, line);       // values

          dg_state.addVar(line);
        }

        state_dgaussians.push_back(dg_state);
      }

      symbol_to_states[name] = state_dgaussians;
    }

    fileI.close();
  } else {
    std::cout << "Unable to open the file " << filename << " for reading."
              << std::endl;
  }
}

void MixtureAcousticModel::write_model(const std::string &filename) {
  std::ofstream fileO(filename, std::ios::app);

  int n_q;

  if (fileO.is_open()) {
    fileO << "AMODEL\n";
    fileO << "Mixture\n";
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

      std::string trans_type = state_to_type[name];

      if (trans_type == "Trans") {
        if (n_q == 1) {
          fileO << "Trans ";
        } else {
          fileO << "Trans\n";
        }

        for (auto &value : state_to_trans[name]) fileO << value << " ";
        fileO << std::endl;

      } else if (trans_type == "TransL") {
        fileO << "TransL" << std::endl;
        std::unordered_map<std::string, std::vector<TransValue>> transL =
            state_to_transL[name];
        fileO << "I" << std::endl;
        for (auto i = 0; i < n_q + 1; i++) {
          fileO << "  " << transL["I"][i].getState();
          fileO << " " << transL["I"][i].getValue() << std::endl;
        }
        fileO << "  ." << std::endl;

        std::string s;
        for (auto i = 1; i < n_q + 1; i++) {
          fileO << i << std::endl;
          std::stringstream ss;
          ss << i;
          ss >> s;
          for (auto j = 0; j < n_q + 1; j++) {
            fileO << "  " << transL[s][j].getState();
            fileO << " " << transL[s][j].getValue() << std::endl;
          }
          fileO << "  ." << std::endl;
        }
      }

      int num_q = state_to_num_q[name];

      for (auto i = 0; i < num_q; i++) {
        DGaussianState dg_states = symbol_to_states[name][i];
        fileO << "I " << dg_states.getComponents() << std::endl;
        fileO << "PMembers ";
        for (auto &value : dg_states.getPMembers()) {
          fileO << value << " ";
        }
        fileO << std::endl;

        fileO << "Members " << std::endl;
        for (auto j = 0; j < dg_states.getComponents(); j++) {
          fileO << "MU ";
          for (auto &value : dg_states.getMus()[j]) {
            fileO << value << " ";
          }
          fileO << std::endl;

          fileO << "VAR ";
          for (auto &value : dg_states.getVars()[j]) {
            fileO << value << " ";
          }
          fileO << std::endl;
        }
      }
    }

    fileO.close();

  } else {
    std::cout << "Unable to open file for writing" << std::endl;
  }
}

MixtureAcousticModel::MixtureAcousticModel(const std::string &filename) {
  MixtureAcousticModel::read_model(filename);
}

MixtureAcousticModel::~MixtureAcousticModel() {
  std::cout << "Destructor" << std::endl;
}

unsigned int MixtureAcousticModel::getDim() { return dim; }

unsigned int MixtureAcousticModel::getNStates() { return n_states; }

float MixtureAcousticModel::calc_prob(const std::string &state, const int &q,
                                      const std::vector<float> &frame) {
  int n_q = state_to_num_q[state];

  if (q > n_q) return -1.0;

  DGaussianState dgstate = symbol_to_states[state][q];

  int components = dgstate.getComponents();

  std::vector<float> pmembers = dgstate.getPMembers();
  std::vector<float> mu, ivar, logcs;

  std::vector<float> pprob;

  logcs = dgstate.getLogcs();

  float max = -HUGE_VAL;

  for (auto i = 0; i < components; i++) {
    float prob = 0.0, aux = 0.0;
    float logc;

    mu = dgstate.getMuByComponent(i);
    ivar = dgstate.getIVarByComponent(i);
    logc = logcs[i];

    for (auto i = 0; i < frame.size(); i++) {
      aux = frame[i] - mu[i];
      prob += (aux * aux) * ivar[i];
    }
    prob = -0.5 * prob + logc;

    aux = pmembers[i] + prob;
    if (aux > max) max = aux;

    pprob.push_back(aux);
  }

  if (max != -HUGE_VAL) {
    float r_add = robust_add(pprob, max, components);
    return r_add;
  } else {
    return -HUGE_VAL;
  }
}

int main() {
  MixtureAcousticModel amodel(
      "bin/models/mixture_monophoneme_I32.example.model");
  amodel.write_model("bin/models/mixture_monophoneme_I32.example.again.model");

  std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};

  std::cout << "Log prob : " << amodel.calc_prob("a", 0, frame) << std::endl;
}
