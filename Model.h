#ifndef _MODEL_H
#define _MODEL_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

struct Triangle {
    unsigned int a, b, c;
    Vector3f normal;
    // more needed
};

class Model {
    public:
    Model();
    ~Model();
    Model(const string& name, unsigned int pointsSize, Vector3f* points, unsigned int trianglesSize, Triangle* triangles);
    unsigned int getPointsSize() const;
    unsigned int getTrianglesSize() const;
    const Vector3f* getPoints() const;
    const Triangle* getTriangles() const;
    
    private:
    string _name;
    unsigned int _pointsSize, _trianglesSize;
    Vector3f* _bounds;
    Vector3f* _points;
    Triangle* _triangles;
    
    friend ostream& operator<<(ostream& out, const Model& model);
};

#endif