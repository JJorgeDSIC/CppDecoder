all: DGaussianAcousticModel MixtureAcousticModel TiedStatesAcousticModel SearchGraphLanguageModel

MixtureAcousticModel: MixtureAcousticModel.cpp
	g++ MixtureAcousticModel.cpp -o MixtureAcousticModel -std=c++11

TiedStatesAcousticModel: TiedStatesAcousticModel.cpp
	g++ TiedStatesAcousticModel.cpp -o TiedStatesAcousticModel -std=c++11

DGaussianAcousticModel: DGaussianAcousticModel.cpp
	g++ DGaussianAcousticModel.cpp -o DGaussianAcousticModel -std=c++11

SearchGraphLanguageModel: SearchGraphLanguageModel.cpp
	g++ SearchGraphLanguageModel.cpp -o SearchGraphLanguageModel -std=c++11

clean:
	rm DGaussianAcousticModel TiedStatesAcousticModel MixtureAcousticModel SearchGraphLanguageModel ../models/*again*
