#ifndef _LEVEL_H
#define _LEVEL_H

#include "Model.h"
#include <forward_list>
#include <string>
#include <vector>

using namespace std;

class Level {
    public:
    void save(const string& saveFilename);
    void load(const string& loadFilename);
    
    private:
    string _levelName;
    forward_list<Model> _models;
    
    static vector<string> _parseCSV(const string& str);    // Parse a line of a CSV file. Splits the line and returns as a vector of strings, there will always be at least one string in the vector. Comments must be on their own line and start with a "#".
};

#endif