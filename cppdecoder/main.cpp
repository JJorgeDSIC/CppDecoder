#include <DGaussianAcousticModel.h>
#include <MixtureAcousticModel.h>
#include <Sample.h>
#include <SearchGraphLanguageModel.h>
#include <TiedStatesAcousticModel.h>
#include <Utils.h>

int main() {
  std::cout << "Testing SearchGraphLanguageModel..." << std::endl;

  std::vector<float> pprobs = {0.2, 0.3, 0.1};
  float max = 0.3;
  float components = 3;

  std::cout << robust_add(pprobs, max, components) << std::endl;

  std::vector<float> frame = {
      -0.392699, -2.06331,  0.0109949,  0.0630278, 0.713447,    -0.557419,
      1.46355,   0.809983,  0.990555,   0.682074,  -1.62765,    0.60225,
      0.493882,  1.55829,   -0.59736,   -1.34481,  -0.0268113,  -0.0561324,
      0.536304,  1.16865,   0.753556,   -0.813899, -0.370601,   -0.346987,
      -1.12761,  0.0755082, -1.127,     -1.23163,  0.717646,    -0.20932,
      0.515273,  0.0881923, 0.00711961, 0.294303,  -0.00440401, -0.600391,
      -0.627719, 0.292688,  0.360419,   -0.443323, -0.189734,   0.420539,
      0.881978,  0.19503,   -0.93659,   -0.414377, 0.544633,    0.00430982};

  Frame feas(frame);

  feas.show_content();

  std::cout << "Testing SearchGraphLanguageModel..." << std::endl;

  SearchGraphLanguageModel sgraph;
  sgraph.read_model("bin/models/2.gram.graph");
  sgraph.write_model("bin/models/2.gram.graph.again");

  // All this will be moved to testing soon...
  std::vector<float> mu = {2.0, 3.0, 5.0, 6.0};
  std::vector<float> var = {0.5, 0.4, 0.6, 0.8};

  std::string lineMu = {"2.0, 3.0, 5.0, 6.0"};
  std::string lineVar = {"0.5, 0.4, 0.6, 0.8"};

  GaussianState gstate;

  gstate.addMu(lineMu);

  DGaussianAcousticModel dgaussianmodel(
      "bin/models/dgaussian_monopohoneme_I01.example.model");
  dgaussianmodel.write_model(
      "bin/models/dgaussian_monopohoneme_I01.example.again.model");

  float prob = dgaussianmodel.calc_prob("aa", 0, frame);

  std::cout << "Prob: " << prob << std::endl;

  MixtureAcousticModel mixturemodel(
      "bin/models/mixture_monophoneme_I32.example.model");
  mixturemodel.write_model(
      "bin/models/mixture_monophoneme_I32.example.again.model");

  std::cout << "Log prob : " << mixturemodel.calc_prob("a", 0, frame)
            << std::endl;

  TiedStatesAcousticModel tiedmodel("bin/models/tiedphoneme_I04.example.model");
  tiedmodel.write_model("bin/models/tiedphoneme_I04.example.again.model");

  std::cout << "Log prob : " << tiedmodel.calc_prob("aa_B+l_E", 0, frame)
            << std::endl;
}
