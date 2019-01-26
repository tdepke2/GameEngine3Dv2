#include "Model.h"

using namespace std;
using namespace sf;

Model::Model() {
    _pointsSize = 0;
    _trianglesSize = 0;
    _bounds = nullptr;
    _points = nullptr;
    _triangles = nullptr;
}

Model::~Model() {
    delete[] _bounds;
    delete[] _points;
    delete[] _triangles;
}

Model::Model(const string& name, unsigned int pointsSize, Vector3f* points, unsigned int trianglesSize, Triangle* triangles) {
    _name = name;
    _pointsSize = pointsSize;
    _points = points;
    _trianglesSize = trianglesSize;
    _triangles = triangles;
    
    _bounds = nullptr;    // yeah we need that init function back
}

unsigned int Model::getPointsSize() const {
    return _pointsSize;
}

unsigned int Model::getTrianglesSize() const {
    return _trianglesSize;
}

const Vector3f* Model::getPoints() const {
    return _points;
}

const Triangle* Model::getTriangles() const {
    return _triangles;
}

ostream& operator<<(ostream& out, const Model& model) {
    out << model._name << ",{\npoints:," << model._pointsSize << "\n";
    for (unsigned int i = 0; i < model._pointsSize; ++i) {
        out << "[" << i << "]," << to_string(model._points[i].x) << "," << to_string(model._points[i].y) << "," << to_string(model._points[i].z) << "\n";
    }
    
    out << "end\ntriangles:," << model._trianglesSize << "\n";
    for (unsigned int i = 0; i < model._trianglesSize; ++i) {
        out << "[" << i << "]," << model._triangles[i].a << "," << model._triangles[i].b << "," << model._triangles[i].c << "\n";
    }
    out << "end\n}";
    
    return out;
}