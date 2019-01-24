#include "Level.h"

#include <fstream>
#include <SFML/Graphics.hpp>
#include <stdexcept>

using namespace std;
using namespace sf;

void Level::save(const string& saveFilename) {
    ofstream saveFile(saveFilename, ios::trunc);
    if (!saveFile.is_open()) {
        throw runtime_error(saveFilename + ": Unable to save level data.");
    }
    
    saveFile << "version,1.0\nname," << _levelName << "\n\ndata:\nend\n\nmodels:\n";
    for (auto modelsIter = _models.begin(); modelsIter != _models.end(); ++modelsIter) {
        saveFile << *modelsIter << "\n";
    }
    
    saveFile << "end";
    saveFile.close();
}

void Level::load(const string& loadFilename) {
    ifstream loadFile(loadFilename);
    if (!loadFile.is_open()) {
        throw runtime_error(loadFilename + ": Unable to access level data.");
    }
    _models.clear();
    
    float levelVersion;
    string line, modelName;
    int lineNumber = 0, numEntries = 0, nestedEntries = 0;
    unsigned int pointsSize, trianglesSize, index;
    Vector3f* points = nullptr;
    Triangle* triangles = nullptr;
    auto modelsIter = _models.before_begin();
    try {
        while (getline(loadFile, line)) {
            ++lineNumber;
            vector<string> lineData = _parseCSV(line);
            
            if (lineData.size() == 1 && lineData[0].length() == 0) {
                continue;
            } else if (numEntries == 0 && lineData.size() == 2 && lineData[0] == "version") {
                levelVersion = stof(lineData[1]);
                if (levelVersion != 1.0f) {
                    throw runtime_error("Invalid map version.");
                }
                ++numEntries;
            } else if (numEntries == 1 && lineData.size() == 2 && lineData[0] == "name") {
                _levelName = lineData[1];
                ++numEntries;
            } else if (numEntries == 2 && lineData.size() == 1 && lineData[0] == "data:") {
                ++numEntries;
            } else if (numEntries == 3) {    // Parse data for all custom data.
                if (lineData.size() == 1 && lineData[0] == "end") {
                    ++numEntries;
                } else {
                    throw runtime_error("Unrecognized data.");
                }
            } else if (numEntries == 4 && lineData.size() == 1 && lineData[0] == "models:") {
                ++numEntries;
            } else if (numEntries == 5) {    // Parse data for all models.
                if (lineData.size() == 1 && lineData[0] == "end") {
                    if (nestedEntries == 0) {
                        ++numEntries;
                    } else {
                        ++nestedEntries;
                    }
                } else if (nestedEntries == 0 && lineData.size() == 2 && lineData[1] == "{") {
                    modelName = lineData[0];
                    ++nestedEntries;
                } else if (nestedEntries == 1 && lineData.size() == 2 && lineData[0] == "points:") {
                    pointsSize = stoi(lineData[1]);
                    index = 0;
                    points = new Vector3f[pointsSize];
                    ++nestedEntries;
                } else if (nestedEntries == 2 && lineData.size() == 4) {    // Parse data for the points.
                    if (index >= pointsSize) {
                        throw runtime_error("Point count is incorrect, memory size exceeded.");
                    }
                    points[index].x = stof(lineData[1]);
                    points[index].y = stof(lineData[2]);
                    points[index].z = stof(lineData[3]);
                    ++index;
                } else if (nestedEntries == 3 && lineData.size() == 2 && lineData[0] == "triangles:") {
                    trianglesSize = stoi(lineData[1]);
                    index = 0;
                    triangles = new Triangle[trianglesSize];
                    ++nestedEntries;
                } else if (nestedEntries == 4 && lineData.size() == 4) {    // Parse data for the triangles.
                    if (index >= trianglesSize) {
                        throw runtime_error("Triangle count is incorrect, memory size exceeded.");
                    }
                    triangles[index].a = stoi(lineData[1]);
                    triangles[index].b = stoi(lineData[2]);
                    triangles[index].c = stoi(lineData[3]);
                    ++index;
                } else if (nestedEntries == 5 && lineData.size() == 1 && lineData[0] == "}") {
                    modelsIter = _models.emplace_after(modelsIter, modelName, pointsSize, points, trianglesSize, triangles);
                    nestedEntries = 0;
                } else {
                    throw runtime_error("Invalid data for model.");
                }
            } else {
                throw runtime_error("Invalid level data.");
            }
        }
    } catch (exception& ex) {
        delete[] points;
        delete[] triangles;
        throw runtime_error(loadFilename + " at line " + to_string(lineNumber) + ": " + ex.what());
    }
    
    if (numEntries != 6) {
        throw runtime_error(loadFilename + ": Missing level data, end of file reached.");
    }
    loadFile.close();
}

vector<string> Level::_parseCSV(const string& str) {    // Parse a line of a CSV file. Splits the line and returns as a vector of strings, there will always be at least one string in the vector. Comments must be on their own line and start with a "#".
    vector<string> values;
    if (str.length() > 0 && str[0] == '#') {
        values.push_back("");
        return values;
    }
    
    size_t index = 0;
    while (index < str.length() && str[index] != ',') {
        ++index;
    }
    
    vector<size_t> points;
    points.push_back(index);
    while (index < str.length()) {
        ++index;
        while (index < str.length() && str[index] != ',') {
            ++index;
        }
        points.push_back(index);
    }
    
    values.reserve(points.size() - 1);
    values.push_back(str.substr(0, points[0]));
    for (unsigned int i = 1; i < points.size(); ++i) {
        values.push_back(str.substr(points[i - 1] + 1, points[i] - points[i - 1] - 1));
    }
    return values;
}