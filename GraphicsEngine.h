#ifndef _GRAPHICSENGINE_H
#define _GRAPHICSENGINE_H

#include "Level.h"
#include "Model.h"
#include "PixelMap.h"
#include "TextureRect.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
using namespace sf;

struct ScreenPoint {    // A point transformed from the 3D world into a 2D point on screen. May or may not be visible.
    int x, y;
    unsigned int z;
    bool tooNear, tooFar;
    
    ScreenPoint() {}
    ScreenPoint(int x, int y, unsigned int z, bool tooNear, bool tooFar) : x(x), y(y), z(z), tooNear(tooNear), tooFar(tooFar) {}
};

struct ShaderPoint {    // Similar to a ScreenPoint but has color and texture attributes.
    int x, y;
    unsigned int z;
    Color color;
    float xTex, yTex;
    
    ShaderPoint() {}
    ShaderPoint(int x, int y, unsigned int z, Color color, float xTex, float yTex) : x(x), y(y), z(z), color(color), xTex(xTex), yTex(yTex) {}
};

class GraphicsEngine {
    public:
    const float PI = acos(-1.0f), NEAR_PLANE = 1.0f, FAR_PLANE = 20.0f;
    
    const PixelMap& getPixelMap() const;
    void init(const Vector2u& pixelSize, const Vector2u& regionSize);
    void redraw(Level& level, const Vector3f& camPosition, const Vector3f& camRotation);
    ScreenPoint graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, const Vector3f& point);
    ScreenPoint graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, Vector3f pointA, Vector3f pointB, float zTarget);
    void fillTriangle(const ShaderPoint* a, const ShaderPoint* b, const ShaderPoint* c, const TextureRect* texture);
    
    private:
    PixelMap _pixelMap;
    vector<ScreenPoint> _screenPoints;
    
    Vector3f _rotationTransform(const Vector3f& camRotation, Vector3f point);
    ScreenPoint _scalingTransform(const Vector2u& screenSize, Vector3f point);
    void _horizontalLine(const ShaderPoint& a, const ShaderPoint& b, const ShaderPoint& c, const TextureRect* texture, int x0, int x1, int y);
    void _fillTriangleFlatBottom(const ShaderPoint& a, const ShaderPoint& b, const ShaderPoint& c, const TextureRect* texture);    // Point a needs to be at the top.
    void _fillTriangleFlatTop(const ShaderPoint& a, const ShaderPoint& b, const ShaderPoint& c, const TextureRect* texture);    // Point c needs to be at the bottom.
};

#endif