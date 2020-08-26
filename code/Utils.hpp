#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

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


std::vector<float> parse_line( const std::string &line )
{
  std::istringstream stm(line) ;
  return { std::istream_iterator<float>(stm), std::istream_iterator<float>() } ;
}
