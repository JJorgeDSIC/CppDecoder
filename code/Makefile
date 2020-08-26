all: DGaussianAcousticModel MixtureAcousticModel TiedStatesAcousticModel

MixtureAcousticModel: MixtureAcousticModel.cpp
	g++ MixtureAcousticModel.cpp -o MixtureAcousticModel -std=c++11

TiedStatesAcousticModel: TiedStatesAcousticModel.cpp
	g++ TiedStatesAcousticModel.cpp -o TiedStatesAcousticModel -std=c++11

DGaussianAcousticModel: DGaussianAcousticModel.cpp
	g++ DGaussianAcousticModel.cpp -o DGaussianAcousticModel -std=c++11
clean:
	rm DGaussianAcousticModel TiedStatesAcousticModel MixtureAcousticModel ../models/*again*
