#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <random>
#include <cmath>


using namespace std;


class SearchGraphLanguageModelState
{

    public:
        int state_id;
        string symbol;
        string word;
        int edge_begin, edge_end;

        SearchGraphLanguageModelState(int state_id, string symbol, string word, int edge_begin, int edge_end);

};


class SearchGraphLanguageModelEdge
{

    public:
        int edge_id;
        int edge_dst;
        float edge_weight;

        SearchGraphLanguageModelEdge(int edge_id, int edge_dst, float edge_weight);

};

class SearchGraphLanguageModel
{
    unordered_map<string, int> symbol_to_id;
    unordered_map<int, string> id_to_symbol;

    unordered_map<string, int> word_to_id;
    unordered_map<int,string> id_to_word;

    int symbol_id = 0;
    int word_id = 0;

    int nstates, nedges, start, final;

    vector<SearchGraphLanguageModelState> sg_lm_states;
    vector<SearchGraphLanguageModelEdge> sg_lm_edges;

    public:

    void read_model(const string &filename);
    void write_model(const string &filename);



};