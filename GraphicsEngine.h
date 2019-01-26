#ifndef _GRAPHICSENGINE_H
#define _GRAPHICSENGINE_H

#include "Level.h"
#include "PixelMap.h"
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class GraphicsEngine {
    public:
    const float PI = acos(-1.0f);
    
    const PixelMap& getPixelMap() const;
    void init(const Vector2u& pixelSize, const Vector2u& regionSize);
    void redraw(Level& level, const Vector3f& camPosition, const Vector3f& camRotation);
    Vector2f graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, float x0, float y0, float z0);
    
    private:
    PixelMap _pixelMap;
    
    void _horizontalLine(int xLeft, int xRight, int y);
    void _fillTriangleFlatBottom(const Vector2i& a, const Vector2i& b, const Vector2i& c);    // Point a needs to be at the top.
    void _fillTriangleFlatTop(const Vector2i& a, const Vector2i& b, const Vector2i& c);    // Point c needs to be at the bottom.
    void _fillTriangle(Vector2i a, Vector2i b, Vector2i c);
};

#endif