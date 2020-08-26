#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <cmath>

const float LOGEPS = -36.0437;

void print_vector(const std::vector<float> &vec){
    for (auto it = vec.begin(); it != vec.end(); ++it)
	cout << *it << ' ';
      cout << endl;
}

void print_vector(const std::vector<string> &vec){
    for (auto it = vec.begin(); it != vec.end(); ++it)
	cout << *it << ' ';
      cout << endl;
}

float robust_add(const vector<float> &pprobs, const float &max, const size_t &components){

  size_t n;
  float aux, res= 0.0;
  for ( n= 0; n < components; ++n )
    {
      if ( (aux= pprobs[n]-max) >= LOGEPS )
	res+= exp ( aux );
    }

  return max + log ( res );
}


std::vector<float> parse_line( const std::string &line )
{
  std::istringstream stm(line) ;
  return { std::istream_iterator<float>(stm), std::istream_iterator<float>() } ;
}
